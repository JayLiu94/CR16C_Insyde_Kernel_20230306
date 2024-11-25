/*-----------------------------------------------------------------------------
 * MODULE PROC.C
 *
 * Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Contains processor specific code.
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 15, 2000 by GLP)
 *
 * EDH
 * 11 Dec 00 	Support the fourth PS/2 port as port 4. It will be
 * 				recognized as port 0 with Active PS/2 Multiplexing
 *					driver.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * ------------------------------------------------------------------------- */

#define PROC_C

#include <asm.h>

#include "swtchs.h"
#include "types.h"
#include "proc.h"
#include "regs.h"
#include "ps2.h"
#include "purdat.h"
#include "oem.h"
#include "host_if.h"
#include "purdev.h"
#include "scan.h"


// delay between wakeup edge setting and clear pending
#define WKEDG_WKPCL_DELAY   {__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");}
static void Write_To_MIWU(BYTE Group, BYTE enbale, BYTE edge, BYTE aedge);

/* ---------------------------------------------------------------
   Name: Chip_Low_Power_Enter

   Setup before the processor is put in low power mode.

   Input: Wakeup1.byte and Wakeup2.byte contain the wakeup flags.
          Wakeup3.byte contain the edge select flags for Wakeup2.

            0 = do NOT wake up via this event.
            1 = wake up via this event.

          Wakeup1.bit.WAKE1_ANYK:      Irq_Anykey,    Any Key Resume.
          Wakeup1.bit.WAKE1_AUX_PORT1: Irq_Aux_Port1, Auxiliary device 1.
          Wakeup1.bit.WAKE1_AUX_PORT2: Irq_Aux_Port2, Auxiliary device 2.
          Wakeup1.bit.WAKE1_AUX_PORT3: Irq_Aux_Port3, Auxiliary device 3.
          Wakeup1.bit.WAKE1_AUX_PORT4: Irq_Aux_Port4, Auxiliary device 4.
          Wakeup1.bit.WAKE1_HOST_IF1:  Irq_Ibf1,      Primary Host interface.
          Wakeup1.bit.WAKE1_HOST_IF2:  Irq_Ibf2,      Secondary Host interface.
          Wakeup1.bit.WAKE1_BIT7:

GLP NOTE: Change 0, 10, 11, 15 to 20, 21, 22, 23?

          Wakeup2.bit.WAKE2_GPIO00  GPIO00.
          Wakeup2.bit.WAKE2_GPIO01  GPIO01.
          Wakeup2.bit.WAKE2_GPIO02  GPIO02.
          Wakeup2.bit.WAKE2_GPIO03  GPIO03.
          Wakeup2.bit.WAKE2_GPIO04  GPIO04.

            0 = falling edge
            1 = rising edge

          Wakeup3.bit.WAKE3_GPIO00  GPIO00.
          Wakeup3.bit.WAKE3_GPIO01  GPIO01.
          Wakeup3.bit.WAKE3_GPIO02  GPIO02.
          Wakeup3.bit.WAKE3_GPIO03  GPIO03.
          Wakeup3.bit.WAKE3_GPIO04  GPIO04.

   On entry, interrupts are disabled.

   Enable internal keyboard (scanner) if needed, setup for
   external interrupt if used, and lock devices not used.
   --------------------------------------------------------------- */
void Chip_Low_Power_Enter(void)
{

    /* Disable all the wakeup sources. */
    WKEN1 = 0;
    WKEN2 = 0;
    WKEN3 = 0;
    WKEN4 = 0;
    WKEN5 = 0;
    WKEN6 = 0;
    WKEN7 = 0;
    WKEN8 = 0;
    WKEN9 = 0;
    WKENA = 0;

    Enable_Any_Key_Wakeup();

    Write_To_MIWU(2, 0x03, 0x00, 03);        // PM_SLP_S4#, PM_SLP_S3#
    ICU_Clr_Pending_Irq(ICU_EINT_GPIO44);
    ICU_Enable_Irq(ICU_EINT_GPIO44);
    ICU_Clr_Pending_Irq(ICU_EINT_GPIO1);
    ICU_Enable_Irq(ICU_EINT_GPIO1);

    Write_To_MIWU(1, 0x1B, 0x09, 0x12);        // Vol_Up, Vol_Down, PS/2 Start bit.
    Write_To_MIWU(4, 0x21, 0, 0x21);        // PWRSW, Hold
    ICU_Clr_Pending_Irq(ICU_EINT_MIWU2);
    ICU_Enable_Irq(ICU_EINT_MIWU2);

    //if (LOW_TEST)
    Write_To_MIWU(5, 0x51, 0x00, 0x11);        // Host Access, Lreset, Left_Click, Right_Click
    Write_To_MIWU(7, 0x02, 0, 0x02);           // PM_SLP_S5#
    Write_To_MIWU(8, 0x01, 0, 0x01);           // Nikiski Button
    ICU_Clr_Pending_Irq(ICU_EINT_MIWU3);
    ICU_Enable_Irq(ICU_EINT_MIWU3);

}

/* ---------------------------------------------------------------
   Name: Chip_Low_Power_Exit

   Cleanup after the processor comes from low power mode.

   Input: Wakeup1.byte is the byte of wakeup flags.
          Interrupts are disabled.
   --------------------------------------------------------------- */
void Chip_Low_Power_Exit(void)
{
   /* Disable all the wakeup sources. */
    WKEN1 = 0;
    WKEN2 = 0;
    WKEN3 = 0;
    WKEN4 = 0;
    WKEN5 = 0;
    WKEN6 = 0;
    WKEN7 = 0;
    WKEN8 = 0;
    WKEN9 = 0;
    WKENA = 0;

   Disable_Any_Key_Wakeup();

   Enable_Any_Key_Irq();   /* Enable Any Key interrupt. */

   /* Multi-input wakeup 2 */
   ICU_Disable_Irq(ICU_EINT_GPIO44);
   /* Multi-input wakeup 2 */
   ICU_Disable_Irq(ICU_EINT_GPIO1);

   /* Multi-input wakeup 2 */
   ICU_Disable_Irq(ICU_EINT_MIWU2);
   /* Multi-input wakeup 3 */
   ICU_Disable_Irq(ICU_EINT_MIWU3);
}

/* ---------------------------------------------------------------
   Name: Get_Irq_Enable_Flag

   Get the state of the global interrupt enable flag.

   Returns: 1 if interrupts are enabled, otherwise 0.
   --------------------------------------------------------------- */
FLAG Get_Irq_Enable_Flag(void)
{
   WORD psr_value;

   _spr_("psr", psr_value);   /* Get state of global
                                 interrupt enable flag. */

   if (psr_value & (1 << (PSR_E_BIT))) return(1);
   else                                return(0);
}

/* ---------------------------------------------------------------
   Name: Read_Input_Port_1

   Read 8042 Input Port 1 and return its contents.

   Returns: Byte from input port.
      Bit 7 = Keyboard not inhibited via switch.
      Bit 6 = Monochrome adapter (0 = Color/Graphics adapter).
      Bit 5 = Manufacturing jumper not installed.
      Bit 4 = Enable 2nd 256K of system board RAM.
      Bit 3 =
      Bit 2 =
      Bit 1 = Auxiliary data in line (PS/2 only).
      Bit 0 = Keyboard data in line (PS/2 only).
   --------------------------------------------------------------- */
BYTE Read_Input_Port_1(void)
{
   /* Port 3 bit 7   bit 7
      Port 3 bit 6   bit 6
      Port 3 bit 5   bit 5
      1              bit 4 through 0 */

   return (P3DIN | 0x1F);
}

/* ---------------------------------------------------------------
   Name: Read_Output_Port_2

   Read 8042 output port 2.

   Returns: Byte from output port.
      Bit 7 = Keyboard data output line (inverted on PS/2).
      Bit 6 = Keyboard clock output line.
      Bit 5 = Input buffer empty (auxiliary interrupt IRQ12 on PS/2).
      Bit 4 = Output buffer full (Generates IRQ1).
      Bit 3 = Reserved (inverted auxiliary clock output line on PS/2).
      Bit 2 = Reserved (inverted auxiliary data output line on PS/2).
      Bit 1 = Gate A20.
      Bit 0 = System reset.
   --------------------------------------------------------------- */
BYTE Read_Output_Port_2(void)
{
   BYTE data;

   /* "data" will hold bits read from different inputs.
      Start with bit 0 set and set bits 2 and 3 for AT mode.
      If PS2 mode is being used, bits 2 and 3 will be changed. */
   data = (1 << 3) | (1 << 2) | (1 << 0);

   /* Put Gate A20 bit value into bit 1. */
   data |= OEM_Get_A20() << 1;

   data |= Get_Aux_Port1_Clk_Out() << 6;
   data |= Get_Aux_Port1_Dat_Out() << 7;

   if (Ext_Cb0.bit.PS2_AT)
   {
      /* If PS2 mode is enabled... */

      /* Invert auxiliary keyboard data line (bit 7)
         and clear bits 2 and 3.  (Since bits 2 and 3
         were set before, they can be cleared by inverting.) */
      data ^= 0x8C;

      /* Put inverted auxiliary device (mouse) clock line in bit 3. */
      data |= (!Get_Aux_Port2_Clk_Out()) << 3;

      /* Put inverted auxiliary device (mouse) data line in bit 2. */
      data |= (!Get_Aux_Port2_Dat_Out()) << 2;
   }

   return (data);
}

/* ---------------------------------------------------------------
   Name: Write_Output_Port_2

   Write to 8042 output port 2.

   Input: data that was received with D1h command.
      Bit 7 = Keyboard data output line (inverted on PS/2).
      Bit 6 = Keyboard clock output line.
      Bit 5 = Input buffer empty (auxiliary interrupt IRQ12 on PS/2).
      Bit 4 = Output buffer full (Generates IRQ1).
      Bit 3 = Reserved (inverted auxiliary clock output line on PS/2).
      Bit 2 = Reserved (inverted auxiliary data output line on PS/2).
      Bit 1 = Gate A20.
      Bit 0 = System reset.

   Only bit 1 is used to write to Gate A20 bit.

   NOTE: If fast A20 is enabled, then the data byte will be intercepted
   by the hardware so this function will never be invoked in that case.
   --------------------------------------------------------------- */
void Write_Output_Port_2(BYTE data)
{
   if (data & 0x02) OEM_A20_High(); /* Force Gate A20 high. */
   else OEM_A20_Low();  /* Force Gate A20 low. */
}

/* ---------------------------------------------------------------
   Name: port_write_table

   The table used by Extended_Read_Port and Extended_Write_Port.

   The port_write_table holds the read shadow location (first byte),
   register location (second byte), and write shadow location (third
   byte) for up to 16 ports.  Entries for ports with invalid numbers
   or that should not be written to are filled with 0's.  If the port
   is a read/write port, the read shadow location is the port itself
   and the write shadow location is set to 0.  If the port is a write
   only port, the read shadow location is a RAM location to be read
   and the write shadow location is the same RAM location to keep
   a copy of the port data.  If the port can be read at a different
   location than the port itself, the read shadow location will
   contain this port address and the write shadow location will
   be set to 0.
   --------------------------------------------------------------- */

struct PORT_WR_DEF
{
   PORT_BYTE_PNTR shadow_rd;
   PORT_BYTE_PNTR port;
   PORT_BYTE_PNTR shadow_wr;
};

static const struct PORT_WR_DEF port_write_table[] =
{
   { (PORT_BYTE_PNTR) PWT_0_SHADOW_RD, /* Port code 0. */
     (PORT_BYTE_PNTR) PWT_0_PORT,
     (PORT_BYTE_PNTR) PWT_0_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_1_SHADOW_RD, /* Port code 1. */
     (PORT_BYTE_PNTR) PWT_1_PORT,
     (PORT_BYTE_PNTR) PWT_1_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_2_SHADOW_RD, /* Port code 2. */
     (PORT_BYTE_PNTR) PWT_2_PORT,
     (PORT_BYTE_PNTR) PWT_2_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_3_SHADOW_RD, /* Port code 3. */
     (PORT_BYTE_PNTR) PWT_3_PORT,
     (PORT_BYTE_PNTR) PWT_3_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_4_SHADOW_RD, /* Port code 4. */
     (PORT_BYTE_PNTR) PWT_4_PORT,
     (PORT_BYTE_PNTR) PWT_4_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_5_SHADOW_RD, /* Port code 5. */
     (PORT_BYTE_PNTR) PWT_5_PORT,
     (PORT_BYTE_PNTR) PWT_5_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_6_SHADOW_RD, /* Port code 6. */
     (PORT_BYTE_PNTR) PWT_6_PORT,
     (PORT_BYTE_PNTR) PWT_6_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_7_SHADOW_RD, /* Port code 7. */
     (PORT_BYTE_PNTR) PWT_7_PORT,
     (PORT_BYTE_PNTR) PWT_7_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_8_SHADOW_RD, /* Port code 8. */
     (PORT_BYTE_PNTR) PWT_8_PORT,
     (PORT_BYTE_PNTR) PWT_8_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_9_SHADOW_RD, /* Port code 9. */
     (PORT_BYTE_PNTR) PWT_9_PORT,
     (PORT_BYTE_PNTR) PWT_9_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_A_SHADOW_RD, /* Port code A. */
     (PORT_BYTE_PNTR) PWT_A_PORT,
     (PORT_BYTE_PNTR) PWT_A_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_B_SHADOW_RD, /* Port code B. */
     (PORT_BYTE_PNTR) PWT_B_PORT,
     (PORT_BYTE_PNTR) PWT_B_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_C_SHADOW_RD, /* Port code C. */
     (PORT_BYTE_PNTR) PWT_C_PORT,
     (PORT_BYTE_PNTR) PWT_C_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_D_SHADOW_RD, /* Port code D. */
     (PORT_BYTE_PNTR) PWT_D_PORT,
     (PORT_BYTE_PNTR) PWT_D_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_E_SHADOW_RD, /* Port code E. */
     (PORT_BYTE_PNTR) PWT_E_PORT,
     (PORT_BYTE_PNTR) PWT_E_SHADOW_WR },

   { (PORT_BYTE_PNTR) PWT_F_SHADOW_RD, /* Port code F. */
     (PORT_BYTE_PNTR) PWT_F_PORT,
     (PORT_BYTE_PNTR) PWT_F_SHADOW_WR }
};

/* ---------------------------------------------------------------
   Name: Extended_Read_Port

   Read port selected by the data byte.

   Input: Bits 3 through 0 of data byte contains the port code.
          If output flag is set, an output register is read
             (or the shadow register of an output register).

   Returns: Data byte from port or 0 if invalid.
   --------------------------------------------------------------- */
BYTE Extended_Read_Port(BYTE data, FLAG output)
{  /*
   The port_read_table holds the register location for
   up to 16 ports.  Entries for ports with invalid numbers
   or that should not be read are filled with 0's. */

   static const PORT_BYTE_PNTR port_read_table[] =
   {
      (PORT_BYTE_PNTR) P0DIN_ADDR,  /* Port code 0. */
      (PORT_BYTE_PNTR) P1DIN_ADDR,  /* Port code 1. */
      (PORT_BYTE_PNTR) P2DIN_ADDR,  /* Port code 2. */
      (PORT_BYTE_PNTR) P3DIN_ADDR,  /* Port code 3. */
      (PORT_BYTE_PNTR) P4DIN_ADDR,  /* Port code 4. */
      (PORT_BYTE_PNTR) P5DIN_ADDR,  /* Port code 5. */
      (PORT_BYTE_PNTR) P6DIN_ADDR,  /* Port code 6. */
      (PORT_BYTE_PNTR) P7DIN_ADDR,  /* Port code 7. */
      (PORT_BYTE_PNTR) P8DIN_ADDR,  /* Port code 8. */
      (PORT_BYTE_PNTR) P9DIN_ADDR,  /* Port code 9. */
      (PORT_BYTE_PNTR) PADIN_ADDR,  /* Port code A. */
      (PORT_BYTE_PNTR) PBDIN_ADDR,  /* Port code B. */
      (PORT_BYTE_PNTR) PCDIN_ADDR,  /* Port code C. */
      (PORT_BYTE_PNTR) PDDIN_ADDR,  /* Port code D. */
      (PORT_BYTE_PNTR) PEDIN_ADDR,  /* Port code E. */
      (PORT_BYTE_PNTR) PFDIN_ADDR,  /* Port code F. */
   };

   BYTE rval;
   PORT_BYTE_PNTR address;

   if (output)
      address = port_write_table[data & 0x0F].shadow_rd;
   else
      address = port_read_table[data & 0x0F];

   if (address) rval = *address;
   else         rval = 0;

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Extended_Write_Port

   Modify one port pin to HI or LO.

   Input: Data byte is configured as follows:
            bit 7 - 4 : Port code
            bit     3 : 0 = modify to LO, 1 = modify to HI
            bit 2 - 0 : Port Pin # (0 - 7)

          Option argument is used to modify the operation
            If 0x0000,          bit 3 of data is used to modify pin.
            If 0x0001 - 0xFFFE, pin is pulsed.
            If 0xFFFF,          bit 3 of data is ignored and pin is toggled.

            For pulse, bit 3 of data is used to modify pin
                       and option is amount of delay.
                       After delay, pin is restored.
   --------------------------------------------------------------- */
void Extended_Write_Port(BYTE data, WORD option)
{
   SMALL index;
   BYTE mask, port_data;
   PORT_BYTE_PNTR shadow_rd_address;
   PORT_BYTE_PNTR port_address;
   PORT_BYTE_PNTR shadow_wr_address;
   FLAG interrupt_enable;

   index = data >> 4;
   mask = Byte_Mask((SMALL) (data & 7));
   shadow_rd_address = port_write_table[index].shadow_rd;

   if (shadow_rd_address)
   {
      port_address = port_write_table[index].port;
      shadow_wr_address = port_write_table[index].shadow_wr;

      interrupt_enable = Get_Irq_Enable_Flag();
      Disable_Irq();

      port_data = *shadow_rd_address;
      #if SXI_SUPPORTED
      if ((data & 0xF0) == (HW_CODE << 4))
         port_data &= ~0x03;
      #endif
      if (option == 0xFFFF) port_data ^=  mask;
      else if (data & 8)    port_data |=  mask;
      else                  port_data &= ~mask;

      if (shadow_wr_address) *shadow_wr_address = port_data;

      /* The test for pulse is done before the pin is modified in an
         attempt to generate an accurate pulse.  The time it takes for
         the if statement interferes with the delay when large values
         of ROM wait states are used or when the clock speed is low. */
      if ( (option > 0x0000) && (option < 0xFFFF) )
      {
         /* Pulse the port pin. */

         /* Modify the pin. */
         *port_address = port_data;

         if (interrupt_enable) Enable_Irq();

         /* Delay. */
         Microsecond_Delay(option);

         Disable_Irq();
         #if SXI_SUPPORTED
         if ((data & 0xF0) != (HW_CODE << 4))
            port_data = *shadow_rd_address;
         #else
         port_data = *shadow_rd_address;
         #endif
         port_data ^=  mask;

         /* Restore the pin. */
         *port_address = port_data;

         if (shadow_wr_address) *shadow_wr_address = port_data;

         if (interrupt_enable) Enable_Irq();
      }
      else
      {
         *port_address = port_data;

         if (interrupt_enable) Enable_Irq();
      }
   }
}
/* ---------------------------------------------------------------
   Name: Write_To_Miwu

   Setting interrupt, wake up source and edge type.

   Input: Group byte is configured as follows:
            bit 7 - 4 : Group code # (1 - A)
            bit     3 : Reserved.
            bit 2 - 0 : Input Number # (0 - 7)

          Type argument is used to configure edge type.
            bit 7     : MIWU input enable.
            bit 6     : Pending clear is required.
            bit 5 - 2 : Reserved.
            bit 1 - 0 : edge type.
            If 00b,     Low-to-high transition
            If 01b,     High-to-low transition
            If 1xb,     Any transition is selected.

   --------------------------------------------------------------- */
struct PORT_MIWU_DEF
{
   PORT_BYTE_PNTR WKEDGx;
   PORT_BYTE_PNTR WKENx;
   PORT_BYTE_PNTR WKPCLx;
};

static const struct PORT_MIWU_DEF miwu_table[] =
{
   { (PORT_BYTE_PNTR) WKEDG1_ADDR, /* Group code 1. */
     (PORT_BYTE_PNTR) WKEN1_ADDR,
     (PORT_BYTE_PNTR) WKPCL1_ADDR },

   { (PORT_BYTE_PNTR) WKEDG2_ADDR, /* Group code 2. */
     (PORT_BYTE_PNTR) WKEN2_ADDR,
     (PORT_BYTE_PNTR) WKPCL2_ADDR },

   { (PORT_BYTE_PNTR) WKEDG3_ADDR, /* Group code 3. */
     (PORT_BYTE_PNTR) WKEN3_ADDR,
     (PORT_BYTE_PNTR) WKPCL3_ADDR },

   { (PORT_BYTE_PNTR) WKEDG4_ADDR, /* Group code 4. */
     (PORT_BYTE_PNTR) WKEN4_ADDR,
     (PORT_BYTE_PNTR) WKPCL4_ADDR },
   { (PORT_BYTE_PNTR) WKEDG5_ADDR, /* Group code 5. */
     (PORT_BYTE_PNTR) WKEN5_ADDR,
     (PORT_BYTE_PNTR) WKPCL5_ADDR },
   { (PORT_BYTE_PNTR) WKEDG6_ADDR, /* Group code 6. */
     (PORT_BYTE_PNTR) WKEN6_ADDR,
     (PORT_BYTE_PNTR) WKPCL6_ADDR },
   { (PORT_BYTE_PNTR) WKEDG7_ADDR, /* Group code 7. */
     (PORT_BYTE_PNTR) WKEN7_ADDR,
     (PORT_BYTE_PNTR) WKPCL7_ADDR },
   { (PORT_BYTE_PNTR) WKEDG8_ADDR, /* Group code 8. */
     (PORT_BYTE_PNTR) WKEN8_ADDR,
     (PORT_BYTE_PNTR) WKPCL8_ADDR },
   { (PORT_BYTE_PNTR) WKEDG9_ADDR, /* Group code 9. */
     (PORT_BYTE_PNTR) WKEN9_ADDR,
     (PORT_BYTE_PNTR) WKPCL9_ADDR },
   { (PORT_BYTE_PNTR) WKEDGA_ADDR, /* Group code A. */
     (PORT_BYTE_PNTR) WKENA_ADDR,
     (PORT_BYTE_PNTR) WKPCLA_ADDR },
   { (PORT_BYTE_PNTR) WKEDGB_ADDR, /* Group code B. */
     (PORT_BYTE_PNTR) WKENB_ADDR,
     (PORT_BYTE_PNTR) WKPCLB_ADDR },
   { (PORT_BYTE_PNTR) WKEDGC_ADDR, /* Group code C. */
     (PORT_BYTE_PNTR) WKENC_ADDR,
     (PORT_BYTE_PNTR) WKPCLC_ADDR }
};
static void Write_To_MIWU(BYTE Group, BYTE enable, BYTE edge, BYTE aedge)
{
    SMALL index;
    BITS_8 temp;
    PORT_BYTE_PNTR miwu_edge_pntr;
    PORT_BYTE_PNTR miwu_en_pntr;
    PORT_BYTE_PNTR miwu_pcl_pntr;

    index = Group - 1;
    miwu_edge_pntr = miwu_table[index].WKEDGx;
    miwu_en_pntr = miwu_table[index].WKENx;
    miwu_pcl_pntr = miwu_table[index].WKPCLx;

    *(miwu_edge_pntr + 1) = aedge;
    *miwu_edge_pntr = edge;
    WKEDG_WKPCL_DELAY;
    // Clear pending flag.
//    *miwu_pcl_pntr = enable;
    // Enable wake up.
    *miwu_en_pntr = enable;
}

