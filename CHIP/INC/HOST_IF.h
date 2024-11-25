/*-----------------------------------------------------------------------------
 * MODULE HOST_IF.H
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Host interface.
 *
 * Revision History (Started July 10, 1996 by GLP)
 *
 * GLP
 * 26 Nov 96   Added Read_Host_Status_Reg2 and Write_Host_Status_Reg2.
 * 06 May 97   Added status register definitions for secondary  Host interface
 *             for ACPI.
 * 27 Feb 98   Removed references to EMUL.
 * 17 Mar 98   Added prototype.
 * 20 Mar 98   Removed definitions for maskSTATUS_PORT2_ALARM,
 *             maskSTATUS_PORT2_BUSY, and maskSTATUS_PORT2_SMI.
 * 24 Apr 98   Updated Data_To_Host2 to save a copy of the data.
 * 19 Aug 99   Updated copyright.
 * EDH
 * 15 Feb 05   Added Read_Host_Status_Reg3 and Write_Host_Status_Reg3.
 *             Added status register definitions for third Host interface
 *             for SMI.
 * ------------------------------------------------------------------------- */

#ifndef HOST_IF_H
#define HOST_IF_H

#include "swtchs.h"
#include "types.h"
#include "regs.h"
#include "cr_uart.h"

void Host_If_Init(void);
void Aux_Data_To_Host(BYTE data);
void Aux_Error_To_Host(BYTE data, BYTE error_bits);

/* ---------------------------------------------------------------
   Name: Input_Buffer_Full, Input_Buffer2_Full, Input_Buffer3_Full

   Checks to see if the Primary Host Interface (Input_Buffer_Full),
   Secondary Host Interface (Input_Buffer2_Full) input buffer or
   Third Host Interface (Input_Buffer3_Full) is full.
   Checks if data is available from the Host interface.

   C prototypes:
      FLAG Input_Buffer_Full(void);
      FLAG Input_Buffer2_Full(void);
      FLAG Input_Buffer3_Full(void);

   Returns: 0 if input buffer is not full, 1 if it is full.
   --------------------------------------------------------------- */
#define Input_Buffer_Full()  (IS_BIT_SET(STATUS_PORT,  maskSTATUS_PORT_IBF))
#define Input_Buffer2_Full() (IS_BIT_SET(STATUS_PORT2, maskSTATUS_PORT_IBF))
#define Input_Buffer3_Full() (IS_BIT_SET(STATUS_PORT3, maskSTATUS_PORT_IBF))
#define Input_Buffer4_Full() (IS_BIT_SET(STATUS_PORT4, maskSTATUS_PORT_IBF))
#define Input_Buffer5_Full() (IS_BIT_SET(STATUS_PORT5, maskSTATUS_PORT_IBF))
/* ---------------------------------------------------------------
   Name: Get_Host_Byte, Get_Host_Byte2, Get_Host_Byte3

   Gets a byte from the Primary Host Port (Get_Host_Byte),
   from the Secondary Host Port (Get_Host_Byte2) or from the Third Host
   Port (Get_Host_Byte3) and checks if the byte is a command or data.

   C prototypes:
      WORD Get_Host_Byte(void);
      WORD Get_Host_Byte2(void);
      WORD Get_Host_Byte3(void);

   Returns: byte value in lower portion of word returned.
            if a command was received, at least 1 bit of higher
            portion of word is set.  In other words, if the value
            returned is > 255, the byte returned is a command.
            Otherwise, it is data.
   --------------------------------------------------------------- */
#define Get_Host_Byte()  ( ((STATUS_PORT  & maskSTATUS_PORT_CMD) << 8) | HIKMDI )
#define Get_Host_Byte2() ( ((STATUS_PORT2 & maskSTATUS_PORT_CMD) << 8) | HIPM1DI )
#define Get_Host_Byte3() ( ((STATUS_PORT3 & maskSTATUS_PORT_CMD) << 8) | HIPM2DI )
#define Get_Host_Byte4() ( ((STATUS_PORT4 & maskSTATUS_PORT_CMD) << 8) | HIPM3DI )
#define Get_Host_Byte5() ( ((STATUS_PORT5 & maskSTATUS_PORT_CMD) << 8) | HIPM4DI )
/* ---------------------------------------------------------------
   Name: Write_System_Flag

   Updates the system flag bit in the Host status port.

   C prototype:
      void Write_System_Flag(FLAG bit);
   --------------------------------------------------------------- */
#define Write_System_Flag(x) (x ? SET_BIT(STATUS_PORT, maskSTATUS_PORT_SYSTEM) \
                                 : CLEAR_BIT(STATUS_PORT, maskSTATUS_PORT_SYSTEM))

/* ---------------------------------------------------------------
   Name: Output_Buffer_Full, Output_Buffer2_Full, Output_Buffere3_Full

   Checks to see if the Primary Host Interface (Output_Buffer_Full),
   Secondary Host Interface (Output_Buffer2_Full) or Third Host Interface
   (Output_Buffer3_Full) output buffer is full.
   Checks if data has been taken by the Host.

   C prototypes:
      FLAG Output_Buffer_Full(void);
      FLAG Output_Buffer2_Full(void);
      FLAG Output_Buffer3_Full(void);

   Returns: 0 if output buffer is not full, 1 if it is full.
   --------------------------------------------------------------- */
#define Output_Buffer_Full()  (IS_BIT_SET(STATUS_PORT, maskSTATUS_PORT_OBF))
#define Output_Buffer2_Full() (IS_BIT_SET(STATUS_PORT2, maskSTATUS_PORT_OBF))
#define Output_Buffer3_Full() (IS_BIT_SET(STATUS_PORT3, maskSTATUS_PORT_OBF))
#define Output_Buffer4_Full() (IS_BIT_SET(STATUS_PORT4, maskSTATUS_PORT_OBF))
#define Output_Buffer5_Full() (IS_BIT_SET(STATUS_PORT5, maskSTATUS_PORT_OBF))
/* ---------------------------------------------------------------
   Name: Data_To_Host

   Clears error bits in the Primary Host Interface status port
   and sends a byte of data from a command response or keyboard
   data to the Host.  Generates Host IRQ1 if keyboard interrupts
   are enabled in controller command byte.

   C prototype:
      void Data_To_Host(BYTE data);

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
void Data_To_Host(BYTE data);

/* ---------------------------------------------------------------
   Name: Data_To_Host2

   Sends a byte of data from a command response to the Host.
   Normally, no Host IRQ is generated.

   C prototype:
      void Data_To_Host2(BYTE data);

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
#define Data_To_Host2(data) DEBUG_PRINTF("PM1 Data: 0x%02x>Host\n", data); HIF2_Out_Data_Buffer = data; HIPM1DO = data


/* ---------------------------------------------------------------
   Name: Data_To_Host3

   Sends a byte of data from a command response to the Host.
   Normally, no Host IRQ is generated.

   C prototype:
      void Data_To_Host3(BYTE data);

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
#define Data_To_Host3(data) DEBUG_PRINTF("PM2 0x%02x>Host\n", data); HIPM2DO = data

#define Data_To_Host4(data) DEBUG_PRINTF("PM3 0x%02x>Host\n", data); HIPM3DO = data

#define Data_To_Host5(data) DEBUG_PRINTF("PM4 0x%02x>Host\n", data); HIPM4DO = data

/* ---------------------------------------------------------------
   Name: Error_To_Host

   Sets appropriate error bits in the Host Interface status port and
   sends a byte of data to the Host.  The error bits for the status
   register will be modified before writing them if the controller is
   in PS/2 compatible mode.  Generates Host IRQ1 if keyboard
   interrupts are enabled in controller command byte.

   C prototype:
      void Error_To_Host(BYTE data, BYTE error_bits);

   Input:
      error bits for status register, data to send to Host.
   --------------------------------------------------------------- */
void Error_To_Host(BYTE data, BYTE error_bits);

/* ---------------------------------------------------------------
   Name: Enable_Host_IRQ1

   Enable IRQ1 generation so Host will be sent
   an IRQ1 with data through the Host interface.

   C prototype:
      void Enable_Host_IRQ1(void);
   --------------------------------------------------------------- */
#define Enable_Host_IRQ1() SET_BIT(HICTRL, HICTRL_OBFKIE)

/* ---------------------------------------------------------------
   Name: Disable_Host_IRQ1

   Disable IRQ1 generation so Host will not be sent an IRQ1
   with data through the Host interface.  It is assumed that
   when the interrupt is disabled, the interrupt signal stays low.

   C prototype:
      void Disable_Host_IRQ1(void);
   --------------------------------------------------------------- */
#define Disable_Host_IRQ1() CLEAR_BIT(HICTRL, HICTRL_OBFKIE)

/* ---------------------------------------------------------------
   Name: Enable_Host_IBF_Irq, Enable_Host_IBF2_Irq, Enable_Host_IBF3_Irq

   Enable Host Input Buffer Full Interrupt request for the
   Primary Host Interface (Enable_Host_IBF_Irq), the Secondary
   Host Interface (Enable_Host_IBF2_Irq) or the Third Host Interface
   (Enable_Host_IBF3_Irq) so that an interrupt
   will occur when the Host puts data into the input buffer.

   C prototypes:
      void Enable_Host_IBF_Irq(void);
      void Enable_Host_IBF2_Irq(void);
      void Enable_Host_IBF3_Irq(void);
   --------------------------------------------------------------- */
/* Add Turn off AUX_PORT3_SUPPORTED and AUX_PORT2_SUPPORTED Switch*/
#if AUX_PORT3_SUPPORTED
  #if AUX_PORT2_SUPPORTED
#define Enable_Host_IBF_Irq()  if (!Aux_Port1.ack && !Aux_Port2.ack && !Aux_Port3.ack) \
                                        SET_BIT(HICTRL, HICTRL_IBFCIE)
  #else  // added
#define Enable_Host_IBF_Irq()  if (!Aux_Port1.ack && !Aux_Port3.ack) \
                                        SET_BIT(HICTRL, HICTRL_IBFCIE)
  #endif // added
#else
#if  AUX_PORT2_SUPPORTED
#define Enable_Host_IBF_Irq()  if (!Aux_Port1.ack && !Aux_Port2.ack) \
                                    SET_BIT(HICTRL, HICTRL_IBFCIE)
#else
#define Enable_Host_IBF_Irq()  if (!Aux_Port1.ack) SET_BIT(HICTRL, HICTRL_IBFCIE)
#endif
#endif

#define Enable_Host_IBF2_Irq() SET_BIT(HICTRL, HICTRL_PMICIE)
#define Enable_Host_IBF3_Irq() SET_BIT(HIPM2CTL, HIPMnCTL_IBFIE)
#define Enable_Host_IBF4_Irq() SET_BIT(HIPM3CTL, HIPMnCTL_IBFIE)
#define Enable_Host_IBF5_Irq() SET_BIT(HIPM4CTL, HIPMnCTL_IBFIE)

#define Enable_Host_OBE1_Irq() HICTRL |= HICTRL_OBECIE
#define Disable_Host_OBE1_Irq() HICTRL &= ~(HICTRL_OBECIE)

/* ---------------------------------------------------------------
   Name: Disable_Host_IBF_Irq, Disable_Host_IBF2_Irq, Disable_Host_IBF3_Irq

   Disable Host Input Buffer Full Interrupt request for the
   Primary Host Interface (Disable_Host_IBF_Irq), the Secondary
   Host Interface (Disable_Host_IBF2_Irq) or the Third Host Interface
   (Disable_Host_IBF3_Irq) so that an interrupt
   will not occur when the Host puts data into the input buffer.

   C prototypes:
      void Disable_Host_IBF_Irq(void);
      void Disable_Host_IBF2_Irq(void);
      void Disable_Host_IBF3_Irq(void);
   --------------------------------------------------------------- */
#define Disable_Host_IBF_Irq()  CLEAR_BIT(HICTRL, HICTRL_IBFCIE)
#define Disable_Host_IBF2_Irq() CLEAR_BIT(HICTRL, HICTRL_PMICIE)
#define Disable_Host_IBF3_Irq() CLEAR_BIT(HIPM2CTL, HIPMnCTL_IBFIE)
#define Disable_Host_IBF4_Irq() CLEAR_BIT(HIPM3CTL, HIPMnCTL_IBFIE)
#define Disable_Host_IBF5_Irq() CLEAR_BIT(HIPM4CTL, HIPMnCTL_IBFIE)
/* ---------------------------------------------------------------
   Name: Read_Host_IBF2_Irq, Read_Host_IBF3_Irq, Read_Host_IBF4_Irq,
         Read_Host_IBF5_Irq

   Read Host Input Buffer Full Interrupt request for the
   Secondary Host Interface (Read_Host_IBF2_Irq) or the Third Host Interface
   (Read_Host_IBF3_Irq) or the Fourth Host Interface
   (Read_Host_IBF4_Irq)or the Fifth Host Interface
   (Read_Host_IBF5_Irq) to get host interface interrupt is eanble or
   disable.

   C prototypes:
      void Read_Host_IBF2_Irq(void);
      void Read_Host_IBF3_Irq(void);
      void Read_Host_IBF4_Irq(void);
      void Read_Host_IBF5_Irq(void);
   --------------------------------------------------------------- */

#define Read_Host_IBF2_Irq() ((HICTRL & HICTRL_PMICIE) ? 1 : 0)
#define Read_Host_IBF3_Irq() ((HIPM2CTL & HIPMnCTL_IBFIE) ? 1 : 0)
#define Read_Host_IBF4_Irq() ((HIPM3CTL & HIPMnCTL_IBFIE) ? 1 : 0)
#define Read_Host_IBF5_Irq() ((HIPM4CTL & HIPMnCTL_IBFIE) ? 1 : 0)


/* ---------------------------------------------------------------
   Name: Read_Inhibit_Switch_Bit

   Returns the state of the inhibit
   switch bit in the status register.

   C prototype:
      FLAG Read_Inhibit_Switch_Bit(void);

   Returns: 0 if bit is 0, 1 if bit is 1.
   --------------------------------------------------------------- */
#define Read_Inhibit_Switch_Bit() (IS_BIT_SET(STATUS_PORT, maskSTATUS_PORT_INHIBIT))

/* ---------------------------------------------------------------
   Name: Write_Inhibit_Switch_Bit

   Modifies the state of the inhibit
   switch bit in the status register.

   C prototype:
      void Write_Inhibit_Switch_Bit(FLAG bit);
   --------------------------------------------------------------- */
#define Write_Inhibit_Switch_Bit(x) (x ? SET_BIT(STATUS_PORT, maskSTATUS_PORT_INHIBIT) \
                                    : CLEAR_BIT(STATUS_PORT, maskSTATUS_PORT_INHIBIT))

/* ---------------------------------------------------------------
   Name: Enable_Fast_A20

   Enable hardware intercept of Gate A20 commands.  Commands to
   modify the Gate A20 signal will be intercepted by the hardware.
   The Keyboard Controller will not react to these commands.

   This processor has only firmare control of the Gate A20
   signal so this proceedure is defined as nothing.

   C prototype:
      void Enable_Fast_A20(void);
   --------------------------------------------------------------- */
#define Enable_Fast_A20()

/* ---------------------------------------------------------------
   Name: Disable_Fast_A20

   Disable hardware intercept of Gate A20 commands.
   The Keyboard Controller will respond to commands
   to modify the Gate A20 signal.

   This processor has only firmare control of the Gate A20
   signal so this proceedure is defined as nothing.

   C prototype:
      void Disable_Fast_A20(void);
   --------------------------------------------------------------- */
#define Disable_Fast_A20()

/* ---------------------------------------------------------------
   Name: Enable_Fast_Reset

   Enable hardware intercept of Reset Host commands.  Commands to
   modify the Reset Host signal will be intercepted by the hardware.
   The Keyboard Controller will not react to these commands.

   This processor has only firmare control of the Reset Host
   signal so this proceedure is defined as nothing.

   C prototype:
      void Enable_Fast_Reset(void);
   --------------------------------------------------------------- */
#define Enable_Fast_Reset()

/* ---------------------------------------------------------------
   Name: Disable_Fast_Reset

   Disable hardware intercept of Reset Host commands.
   The Keyboard Controller will respond to commands
   to modify the Reset Host signal.

   This processor has only firmare control of the Reset Host
   signal so this proceedure is defined as nothing.

   C prototype:
      void Disable_Fast_Reset(void);
   --------------------------------------------------------------- */
#define Disable_Fast_Reset()

/* ---------------------------------------------------------------
   Name: Read_Host_Status_Reg2

   Reads the status register for Host interface 2.

   C prototype:
      BYTE Read_Host_Status_Reg2(void);

   Returns: data from status register.
   --------------------------------------------------------------- */
#define Read_Host_Status_Reg2() STATUS_PORT2

/* ---------------------------------------------------------------
   Name: Read_Host_Status_Reg3

   Reads the status register for Host interface 3.

   C prototype:
      BYTE Read_Host_Status_Reg3(void);

   Returns: data from status register.
   --------------------------------------------------------------- */
#define Read_Host_Status_Reg3() STATUS_PORT3

#define Read_Host_Status_Reg4() STATUS_PORT4
#define Read_Host_Status_Reg5() STATUS_PORT5
/* ---------------------------------------------------------------
   Name: Write_Host_Status_Reg2

   Writes to the status register for Host interface 2.

   C prototype:
      void Write_Host_Status_Reg2(BYTE data);

   Input:
      data to send to status register.
   --------------------------------------------------------------- */
#define Write_Host_Status_Reg2(x) STATUS_PORT2 = x

/* ---------------------------------------------------------------
   Name: Write_Host_Status_Reg3

   Writes to the status register for Host interface 3.

   C prototype:
      void Write_Host_Status_Reg3(BYTE data);

   Input:
      data to send to status register.
   --------------------------------------------------------------- */
#define Write_Host_Status_Reg3(x) STATUS_PORT3 = x

#define Write_Host_Status_Reg4(x) STATUS_PORT4 = x
#define Write_Host_Status_Reg5(x) STATUS_PORT5 = x

#endif /* ifndef HOST_IF_H */

