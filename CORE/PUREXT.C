/* ----------------------------------------------------------------------------
 * MODULE PUREXT.C
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
 * This file contains extended Keyboard Controller commands.  These
 * commands are Insyde Software defined commands that provide additional
 * functionality above and beyond the standard 8042 controller commands.
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 20, 2000 by GLP)
 *
 * functions written using switch/case constructs. I checked: the
 * compiler is smart enough to put together jump tables from that.
 * --crb3 03Oct95
 *
 * EDH
 * 15 Feb l05  Added to process command/data write from Third Host Interface.
 * 13 Apr 1. modify command B7 for write word data register.
 *        2. modify command B8 for read word data register.
 * ------------------------------------------------------------------------- */

#define PUREXT_C

#include "swtchs.h"
#include "defs.h"
#include "purext.h"
#include "purdat.h"
#include "purini.h"
#include "purcmd.h"
#include "purconst.h"
#include "proc.h"
#include "ps2.h"
#include "host_if.h"
#include "i2c.h"
#include "d_to_a.h"
#include "oem.h"
#include "purmain.h"
#include "timers.h"
#include "puracpi.h"
#include "a_to_d.h"
#include "purxlt.h"
#include "purfunct.h"
#include "dev.h"
#include "staticfg.h"
#include "purhook.h"
//#include "extmem.h"
#include "smb_tran.h"

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#ifndef CRISIS_CMD_HANDLER
#error CRISIS_CMD_HANDLER switch is not defined.
#endif

const BYTE HIF_To_SMBusChannel[] = {HIF1_SMB_CHANNEL, HIF2_SMB_CHANNEL, HIF3_SMB_CHANNEL};
static void enter_crisis_program(void);

/* ---------------------------------------------------------------
   Name: HIF2_Cmd

   At this point, Cmd_Byte == 0, Tmp_Load == 0.

   Handle commands that are only accessable
   through the secondary Host interface.
   --------------------------------------------------------------- */
WORD HIF2_Cmd(BYTE command_num)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: HIF3_Cmd

   At this point, Cmd_Byte == 0, Tmp_Load == 0.

   Handle commands that are only accessable
   through the third Host interface.
   --------------------------------------------------------------- */
WORD HIF3_Cmd(BYTE command_num)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: HIF2_Dat

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of commands that are only
   accessable through the secondary Host interface.
   --------------------------------------------------------------- */
WORD HIF2_Dat(BYTE command_num, BYTE data)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: HIF3_Dat

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of commands that are only
   accessable through the third Host interface.
   --------------------------------------------------------------- */
WORD HIF3_Dat(BYTE command_num, BYTE data)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: HIF4_Dat

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of commands that are only
   accessable through the third Host interface.
   --------------------------------------------------------------- */
WORD HIF4_Dat(BYTE command_num, BYTE data)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Pc_Cmd0

   At this point, Cmd_Byte == 0, Tmp_Load == 0.

   Handle extended commands 00h through 0Fh.  Any command
   that requires data will be dispatched to "default".

   --------------------------------------------------------------- */
WORD Ext_Cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Ext_Dat0

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 00h to 0Fh extended commands.
   --------------------------------------------------------------- */
WORD Ext_Dat0(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Ext_Cmd8

   At this point, Cmd_Byte == 0, Tmp_Load == 0.

   Handle extended commands 80h through 9Fh.  Any command
   that requires data will be dispatched to "default".
   --------------------------------------------------------------- */
WORD Ext_Cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   FLAG done;
   BYTE temp;
   SMALL index;
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0x92:                 /* Read SMI Cause Value. */
         /*
         The Host has issued this command because an SMI signal
         was generated and/or the SMI_EVT bit in the secondary
         Host interface status register is set.

         If in Legacy mode, the SCI and SMI cause flags are checked.
         If in ACPI mode, only the SMI cause flags are checked.

         For the SCI cause flags...
         Bit 0 of first  byte is cause value 1,
         Bit 1 of first  byte is cause value 2,
             .
             .
             .
         Bit 7 of first  byte is cause value 8,
         Bit 0 of second byte is cause value 9,
         etc.

         For the SMI cause flags...
         The first byte of the SMI cause flags is at
         offset NUM_SCI_FLAGS_BYTES in the cause flags array.
         Bit 0 of first  byte is cause value FIRST_SMI_CAUSE,
         Bit 1 of first  byte is cause value FIRST_SMI_CAUSE + 1,
             .
             .
             .
         Bit 7 of first  byte is cause value FIRST_SMI_CAUSE + 7,
         Bit 0 of second byte is cause value FIRST_SMI_CAUSE + 8,
         etc.

         SMIs can be generated with certain commands and
         downloaded hotkeys.  The cause values for these
         are held in SMI_Event_Buffer. */

         /* Initialize cause number to zero.  If there are
            no events pending, return a zero to the Host. */
         rval = 0;

         #if ACPI_SUPPORTED
         if (Ext_Cb3.bit.ACPI_ENABLED == 0)
         {
         #endif
            /* In Legacy mode, check SCI and SMI cause flags. */
            index = 0;
         #if ACPI_SUPPORTED
         }
         else
         {
            /* In ACPI mode, skip SCI cause flags.
               Only check SMI cause flags. */
            index = NUM_SCI_FLAGS_BYTES;
         }
         #endif

         done = FALSE;
         do
         {
            if (Cause_Flags[index] != 0)
            {
               /* There is at least one event pending. */

               hvp->Tmp_Load = Cause_Flags[index];

               while ( (hvp->Tmp_Load & 1) == 0 )
               {
                  hvp->Tmp_Load = hvp->Tmp_Load >> 1;
                  rval++;
               }

               /* rval has the bit number of the event. */

               /* Clear the cause bit. */
               Cause_Flags[index] &= Cpl_Byte_Mask((SMALL) rval);

               /* Convert bit number to cause number. */
               if (index < NUM_SCI_FLAGS_BYTES)
               {
                  /* SCI cause number. */
                  rval += (index * 8) + 1;
               }
               else
               {
                  /* SMI cause number. */
                  rval += ((index - NUM_SCI_FLAGS_BYTES) * 8)
                          + FIRST_SMI_CAUSE;
               }

               done = TRUE;
            }
            else
            {
               index++;
               if (index == CAUSE_ARRAY_SIZE)
               {
                  /* No SCI or SMI event was found.  rval still = 0. */
                  done = TRUE;
               }
            }
         }
         while(!done);

         if (rval == 0)
         {
            /* Check buffered SMIs. */

            if (SMI_Event_Out_Index != SMI_Event_In_Index)
            {
               /* There is at least one event pending. */

               /* Get the SMI cause number. */
               rval = SMI_Event_Buffer[SMI_Event_Out_Index];

               /* Update the index. */
               SMI_Event_Out_Index++;
               if (SMI_Event_Out_Index >= EVENT_BUFFER_SIZE)
                  SMI_Event_Out_Index = 0;
            }
            else
            {
               /* No SMI event was found.  rval still = 0. */
               ;
            }
         }

         if (rval != 0)
         {
            /* Found flag, check if another flag exists. */

            if (index < CAUSE_ARRAY_SIZE)
            {
               done = FALSE;
               do
               {
                  if (Cause_Flags[index] != 0)
                  {
                     /* There is at least one event pending. */
                     done = TRUE;
                  }
                  else
                  {
                     index++;
                     if (index == CAUSE_ARRAY_SIZE)
                     {
                        /* No pending events. */
                        done = TRUE;
                     }
                  }
               }
               while(!done);
            }

            if (index == CAUSE_ARRAY_SIZE)
            {
               if (SMI_Event_Out_Index != SMI_Event_In_Index)
               {
                  /* There is at least one event pending.  Set index
                     to a value that is not equal to CAUSE_ARRAY_SIZE. */
                  index = 0;
               }
            }
         }

         if ( (rval == 0) || (index == CAUSE_ARRAY_SIZE) )
         {
            /* Either there were no events or there
               are no more events pending after this.
               Clear the SMI_EVT bit. */
            Write_Host_Status_Reg2(Read_Host_Status_Reg2() &
                                   ~maskSTATUS_PORT2_SMIEVT);

            switch ((Cfg0E >> shiftSMI_OP_SELECT) & maskSMI_OP_SELECT)
            {
               /* If the SMI signal was set high or low,
                  deactivate the SMI signal now. */
               /* <<< V5.1 2001/2/13 Modified for SMI control methold. */
               case SXI_SET_HI:
                  Extended_Write_Port(SMI_Pin & ~MASK(bit3), 0);
                  break;
               case SXI_SET_LO:
                  /* Toggle the SMI pin. */
                  #if SXI_SUPPORTED
                  if ((SMI_Pin & 0xF0) == (HW_CODE << 4))
                     Extended_Write_Port(SMI_Pin & ~(MASK(bit3)), 0);
                  else
                  Extended_Write_Port(SMI_Pin | MASK(bit3), 0);
                  #else
                  Extended_Write_Port(SMI_Pin | MASK(bit3), 0);
                  #endif
                  break;
               /* >>> V5.1 2001/2/13 Modified. */
            }
         }
         break;

      case 0x95:                 /* Turn LEDs Off. */
         /* OEM_Write_Leds modifies Led_Data.  Save it. */
         temp = Led_Data.all;

         /* Turn on Flag.LED_ON to allow LEDs to be turned off. */
         Flag.LED_ON = 1;

         /* Turn off LEDs. */
         Led_Data.all = 0;
         OEM_Write_Leds(Led_Data);

         /* Restore Led_Data. */
         Led_Data.all = temp;

         /* Turn off Flag.LED_ON. */
         Flag.LED_ON = 0;
         break;

      case 0x96:                 /* Turn LEDs On. */
         Flag.LED_ON = 1;
         OEM_Write_Leds(Led_Data);
         break;

      case 0x98:                 /* Read Scanner Control Byte. */
         rval = (WORD) Ext_Cb3.byte;
         break;

      case 0x9D:                 /* Read Overlay State. */
         rval = (WORD) Ext_Cb3.bit.OVL_STATE;
         break;

      case 0x9F:                 /* Read Extended LED Control. */
         rval = (WORD) Led_Ctrl.all;
         break;

      //case 0x80:
      //case 0x81:
      case 0x82:
      case 0x83:
      case 0x84:
         /* Handled by ACPI module. */

      case 0x85:
      case 0x86:
      case 0x87:
      case 0x88:
      case 0x89:
      case 0x8A:
      case 0x8B:
      case 0x8C:
      case 0x8D:
      case 0x8E:
      case 0x8F:
         /* Reserved for future ACPI. */

      case 0x93:
      case 0x94:
      case 0x9A:
      case 0x9B:
      case 0x9C:
      case 0x9E:
         break;

      default:
         /* These commands require data.      */

         /* 0x80  Legacy Read EC Space.       */
         /* 0x81  Legacy Write EC Space.      */
         /* 0x90  Legacy Read EC Space.       */
         /* 0x91  Legacy Write EC Space.      */
         /* 0x97  Write Scanner Control Byte. */
         /* 0x99  Write Debounce Control Byte.*/
         /* 0x9C  Write Overlay State.        */

         /* Signal to wait for data from Host.*/
         hvp->Cmd_Byte = command_num;
   }

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Ext_Dat8

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 80h to 9Fh extended commands.
   --------------------------------------------------------------- */
WORD Ext_Dat8(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   BYTE index;
   WORD operation;
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0x90:                 /* Legacy Read EC Space. */
      case 0x80:
         #if ACPI_SUPPORTED
         rval = Read_EC_Space(data);
         #endif
         break;

      case 0x91:                 /* Legacy Write EC Space. */
      case 0x81:
         #if ACPI_SUPPORTED
         Write_EC_Space(command_num, data, hif);
         #endif
         break;

      case 0x97:                 /* Write Scanner Control Byte. */
         Ext_Cb3.byte = data;

         OEM_Update_Ovl_Led();

         if (Ext_Cb3.bit.NumLockC_OVR)
         {
            /* If Overlay controlled by NumLock... */
            Led_Data = Set_Overlay_Mask(Led_Data, Led_Data.led.NUM);
         }

         OEM_Write_Leds(Led_Data);

         break;

      case 0x99:                 /* Write Debounce Control Byte. */
         /* Bits 7, 6, and 5 are used for the "make" count and
            bits 3, 2, and 1 are used for the "break" count. */
         Ext_Cb2.byte &= 0x11;
         data &= 0xEE;
         Ext_Cb2.byte |= data;
         break;

      case 0x9C:                 /* Write Overlay State. */
         Ext_Cb3.bit.OVL_STATE = data & 1;
         OEM_Update_Ovl_Led();
         OEM_Write_Leds(Led_Data);
         break;


      default:
         break;
   }

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Ext_Cmdb

   At this point, hvp->Cmd_Byte == 0, hvp->Tmp_Load == 0.

   Handle extended commands B0h through BFh.
   --------------------------------------------------------------- */
WORD Ext_Cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0xB7:                 /* Write Any Location. */
      case 0xB8:                 /* Read Any Location. */
      case 0xB9:                 /* Extended Write Port. */
      case 0xBA:                 /* Extended Read Port. */
      case 0xBE:                 /* Write Byte to Any Location. */
      case 0xBF:                 /* Read Byte from Any Location. */
         /* These commands require data. */
         hvp->Cmd_Byte = command_num;
         break;

      case 0xB5:                 /* Read Typematic Save Byte. */
         rval = (WORD) Save_Typematic;
         break;

      case 0xB6:                 /* Read LED Save Byte. */
         rval = (WORD) (Save_Kbd_State & maskLEDS);
         break;

      #ifndef CMD_BC
      #error CMD_BC switch is not defined.
      #elif CMD_BC
      case 0xBC:                 /* Read Saved Mouse State. */
         /*
         Get a status report on the current mouse configuration.
         Routine "save_mouse_state" intercepts all commands going
         to the mouse and maintains a configuration array.  BCh is
         similar to the mouse E9h command.  The difference is that
         E9h gets the configuration from the device, while BCh gets
         it from RAM.  BCh is needed when doing hot insert and
         removal of auxiliary devices.

         After receiving BCh, the Keyboard Controller
         returns 3 bytes as follows:

         Byte 1 - bit 0 - 3 : reserved
                  bit 4     : 0 = scaling 1:1, 1 = scaling 2:1
                  bit 5     : 0 = disabled, 1 = enabled
                  bit 6     : 0 = stream mode, 1 = remote mode
                  bit 7     : reserved

         Byte 2 - Resolution Setting
         Byte 3 - Sampling Rate  */

         HIF_Response[hif].byte = respARRAY;
         hvp->Tmp_Pntr = Mouse_State; /* Pointer to array. */
         hvp->Tmp_Load = 3;           /* Number of bytes in array. */
         break;
      #endif

      default:
         /* 0xBB */
         /* 0xBD */
         /* 0xBE */
         /* 0xBF */
         break;
   }
   return (rval);
}

/* ---------------------------------------------------------------
   Name: Ext_Datb

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of B0h to BFh extended commands.
   --------------------------------------------------------------- */
WORD Ext_Datb(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0xB7:                 /* Write Any Location. */
         /* 0st byte is high high byte of address.
            1st byte is high byte of address.
            2nd byte is low byte of address.
            3rd byte is Low byte data to write.
            4rd byte is high byte data to write. */
         if (hvp->Tmp_Load < 4)
         {
            hvp->Tmp_Byte[hvp->Tmp_Load] = data;
            hvp->Tmp_Load++;             /* Increment data byte counter. */
            hvp->Cmd_Byte = command_num; /* Signal to wait for data from Host. */
         }
         else
         {
             *(WORD *)(((DWORD)hvp->Tmp_Byte[0] << 16) + ((WORD)hvp->Tmp_Byte[1] << 8)
                      + hvp->Tmp_Byte[2]) = (((WORD)data << 8 & 0xFF00) + hvp->Tmp_Byte[3]);
         }
         break;

      case 0xB8:                 /* Read Any Location. */
         /* 0st byte is high high byte of address.
            1st byte is high byte of address.
            2nd byte is low byte of address.
            3rd byte is read low byte or high byte.
                0x00~0x01 OR 0x03~0xff: Low byte
                0x02: High byte
            Returns data read. */

         if (hvp->Tmp_Load < 3)
         {
            hvp->Tmp_Byte[hvp->Tmp_Load] = data;
            hvp->Tmp_Load++;             /* Increment data byte counter. */
            hvp->Cmd_Byte = command_num; /* Signal to wait for data from Host. */
         }
         else
         {
            if (data == 0x02)
            {
            	rval = (((*(WORD *) (((DWORD)hvp->Tmp_Byte[0] << 16)
            	        + ((WORD)hvp->Tmp_Byte[1] << 8) + hvp->Tmp_Byte[2])) & 0xFF00) >> 8);
            }
            else
            {
                rval = ((*(WORD *) (((DWORD)hvp->Tmp_Byte[0] << 16)
                        + ((WORD)hvp->Tmp_Byte[1] << 8) + hvp->Tmp_Byte[2])) & 0x00FF);
            }
         }
         break;

      case 0xB9:                 /* Extended Write Port. */
         Extended_Write_Port(data, 0);
         break;

      case 0xBA:                 /* Extended Read Port. */
         rval = Extended_Read_Port(data, 0);
         break;

     case 0xBE:   /* Write Byte to Any Location. */
                  /* 0th byte is high high byte of address.
                     1st byte is high byte of address.
                     2nd byte is low byte of address.
                     3rd byte is data to write. */

        if (hvp->Tmp_Load < 3)
        {
           hvp->Tmp_Byte[hvp->Tmp_Load] = data;
           hvp->Tmp_Load++;             /* Increment data byte counter. */
           hvp->Cmd_Byte = command_num; /* Signal to wait for data from Host. */
        }
        else
        {
             *(BYTE *)(((DWORD)hvp->Tmp_Byte[0] << 16)
                    + ((WORD)hvp->Tmp_Byte[1] << 8) + hvp->Tmp_Byte[2]) = data;
        }
        break;

     case 0xBF:   /* Read Byte from Any Location. */
                  /* 0th byte is high high byte of address.
                     1st byte is high byte of address.
                     2nd byte is low byte of address.
                     Returns data read. */

        if (hvp->Tmp_Load < 2)
        {
           hvp->Tmp_Byte[hvp->Tmp_Load] = data;
           hvp->Tmp_Load++;             /* Increment data byte counter. */
           hvp->Cmd_Byte = command_num; /* Signal to wait for data from Host. */
        }
        else
        {
           rval = *(BYTE *) (((DWORD)hvp->Tmp_Byte[0] << 16) + ((WORD)hvp->Tmp_Byte[1] << 8) + data);
        }
        break;

      default:
         break;
   }

   return (rval);
}
