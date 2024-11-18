/* ----------------------------------------------------------------------------
 * MODULE PURCMD.C
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
 * Contains standard 8042 keyboard controller commands.
 *
 * Revision History (Started May 8, 1995 by GLP)
 *
 * GLP
 * 11 Dec 96   Added reference to selftest.h.
 * 12 Dec 96   Changed pc_cmdf so that Command FFh will not
 *             change any signal nor will it use the delay.
 * 16 Dec 96   Changed Command C0h so that BFh is returned
 *             when Ext_Cb0.bit.PORT_CONTROL is 1.  (It
 *             returned BFh when the bit was 0.)
 * 16 Jan 97   Changed sndto_kbd to use Led_Data instead of
 *             local variable to handle LEDs correctly for
 *             keyboard command EDh.  Added Lock_Scan() to
 *             pcxad().
 * 17 Jan 97   Changed pcxa7 (Disable Aux Device Interface)
 *             to set Aux_Port3.inh instead of clearing it.
 * 05 May 97   Added to Srvc_Pccmd2 and Srvc_Pcdat2 for ACPI
 *             Embedded Controller Commands.
 * 09 Feb 98   Removed instruction that sets Ccb42.bit.DISAB_AUX
 *             in pcxad (Command ADh, Disable Keyboard Interface).
 *             This was disabling the mouse when Command ADh was used.
 * 26 Feb 98   Added to Srvc_Pccmd2 and Srvc_Pcdat2 for commands
 *             that are exclusive to the secondary Host interface.
 * 20 Mar 98   Added include for puracpi.h file.
 * 06 Apr 98   Added pc_cmd1 with Gen_Hookc_Pc_Cmd1 and pc_cmd4
 *             with Gen_Hookc_Pc_Cmd4.  Added Gen_Hookc_Pc_Dat1
 *             and Gen_Hookc_Pc_Dat4
 *             Hookc_Srvc_Pccmd is called only for commands from
 *             the primary Host interface.
 *             Changed Hookc_Pcdat to Hookc_Srvc_Pcdat and called
 *             it only for data from the primary Host interface.
 *             Added Hookc_Srvc_Pccmd2 and Hookc_Srvc_Pcdat2.
 * 08 Apr 98   In process_cmd_data, set rval to return data from
 *             Pc_Dat8 so Commands 80h - 9Fh can send a byte to
 *             the Host.
 * 17 Apr 98   Hotplug did not always work.  For example, a mouse is
 *             on Port 1 and a keyboard is on Port 2.  The mouse is
 *             locked and the keyboard is unlocked.  If the keyboard
 *             is swapped to the mouse port, it will not be detected
 *             as a Hotplug because the port is locked.  This was
 *             corrected by removing the Aux_Portx.inh flag.  The
 *             auxiliary devices are not locked under Host control.
 *             When the Host locks the auxiliary keyboard or mouse
 *             devices, the data is simply ignored.
 * 28 Apr 98   Put {} around if statements using Data_To_Host2.
 *             Data_To_Host2 is a macro and if statement was
 *             failing without the {}.
 * 06 May 98   Removed Flag.SCAN_LOAD, Flag.PPKT_LOAD, and
 *             Flag.PASS_LOAD.
 * 14 May 98   Updated code because Ext_Cb4 has been changed to
 *             Ps2_Ctrl.  Changed Srvc_Pccmd to allow ACPI commands.
 * 15 Jun 98   Removed Hookc_Cmd_Data.  Changed Srvc_Pcdat to
 *             handle data from ACPI commands.
 * 23 Jun 98   Added to command D4h to set the valid flag.  If a
 *             mouse has returned an error, the valid flag is
 *             clear.  If the Host has sent a command to the
 *             Keyboard Controller for the primary mouse, the
 *             valid flag is set to allow the command through to
 *             the mouse.
 * 10 Jul 98   Added code to call Load_Timer_B() and set
 *             Timer_B.bit.INH_CLEAR flag when an auxiliary device
 *             is locked.
 * 30 Jul 98   Changed from static WORD pcxa6(void)
 *                     to          WORD Pcxa6(void).
 * 11 Aug 98   Changed pc_cmdf to use Calculate_Microsecond_Delay.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 30 Sep 98   Removed line that updates Scanner_State.bit.OVERLAY
 *             in Set_Overlay_Mask.  Ext_Cb3.bit.OVL_STATE is used
 *             for the overlay state.
 * 17 Aug 99   Added Pc_Cmd0 to pccmd_table and Pc_Dat0 to
 *             process_cmd_data.  Updated copyright.
 * 20 Jan 00   Support for Active PS/2 Multiplexing (APM) is
 *             marked as V21.1.
 *             Support to allow the system to detect an external
 *             mouse even if there is no internal mouse is marked
 *             as V21.2.
 *
 * EDH
 * 11 Dec 00   Support the fourth PS/2 port as port 4. It will be
 *             recognized as port 0 with Active PS/2 Multiplexing
 *             driver.
 * 21 DEC 00   Fixed KBC will send timeout error to Host if the
 *             primary Aux is port4.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * 15 Feb 05   Added to process command/data write from Third Host Interface.
 * 12 May 15   Moved Keyboard command/data to PURSCN.C
 * 12 May 15   Moved PM1, PM2 command/data to PURPM1.C and PURPM2.C
 * ------------------------------------------------------------------------- */

#define PURCMD_C

#include "swtchs.h"
#include "purcmd.h"
#include "purext.h"
#include "defs.h"
#include "types.h"
#include "purdat.h"
#include "host_if.h"
#include "proc.h"
#include "ps2.h"
#include "purscn.h"
#include "purxlt.h"
#include "purhook.h"
#include "purdev.h"
#include "oem.h"
#include "ini.h"
#include "selftest.h"
#include "dev.h"
#include "puracpi.h"
#include "timers.h"
#include "staticfg.h"
#include "cr_uart.h"


#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#ifndef CRISIS_CMD_HANDLER
#error CRISIS_CMD_HANDLER switch is not defined.
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static WORD pc_cmddum(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pc_cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pcxdum(void);
static WORD pcxa4(void);
static WORD pcxa5(void);
static WORD pcxa7(void);
static WORD pcxa9(void);
static WORD pcxaa(void);
static WORD pcxab(void);
static WORD pcxac(void);
static WORD pcxad(void);
static WORD process_cmd_data(HIF_VAR* hvp, BYTE command, BYTE data, BYTE hif);
static void pc_dat6(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
void sndto_kbd(BYTE data);

/* ---------------------------------------------------------------
   Name: Srvc_Pccmd

   Handle incoming command from Host write to port 64h Host interface.
   Dispatch command to one of 16 functions based on the command #.

   Input: Command #

   On return, "Cmd_Byte" will be non-zero if the command
   requires more data to complete processing of the command.
   --------------------------------------------------------------- */

   /* For the right shift to work, "command_num" MUST be unsigned.
      (Right shifting an unsigned quantity fills vacated bits with
      zeros.  Right shifting a signed quantity will fill vacated
      bits with sign bits on some machines and zero bits on others.) */

   /* The functions in the table take an WORD
      (the command #) as a parameter.

      They also return a WORD value with all bits set if
      no data is to be sent to the host.  Otherwise, the low
      byte of the WORD that is returned is the data to
      be sent to the host (the high bits are zero). */

  const FUNCT_PTR_W_HPBB pccmd_table[16] =
  {
      #if EXTEND_KBC_SUPPORTED
      Ext_Cmd0,    /* [0x] Extended commands */
      #else // EXTEND_KBC_SUPPORTED
      pc_cmddum,  /* [0x] Dummy commands */
      #endif // EXTEND_KBC_SUPPORTED
      pc_cmd1,    /* [1x] OEM custom commands */
      pc_cmd2,    /* [2x] Read RAM byte 20-3F */
      pc_cmd2,    /* [3x] */
      pc_cmd4,    /* [4x] OEM custom commands */
      pc_cmd5,    /* [5x] OEM custom commands */
      pc_cmd6,    /* [6x] Write RAM byte 60-7F */
      pc_cmd6,    /* [7x] */
      #if EXTEND_KBC_SUPPORTED
      Ext_Cmd8,    /* [8x] Extended commands */
      Ext_Cmd8,    /* [9x] Extended commands */
      #else // EXTEND_KBC_SUPPORTED
      pc_cmddum,  /* [8x] Dummy commands */
      pc_cmddum,  /* [8x] Dummy commands */
      #endif // EXTEND_KBC_SUPPORTED
      pc_cmda,    /* [Ax] */
      #if EXTEND_KBC_SUPPORTED
      Ext_Cmdb,    /* [Bx] Extended commands */
      #else // EXTEND_KBC_SUPPORTED
      pc_cmddum,  /* [0x] Dummy commands */
      #endif // EXTEND_KBC_SUPPORTED
      pc_cmdc,    /* [Cx] */
      pc_cmdd,    /* [Dx] */
      pc_cmde,    /* [Ex]  */
      pc_cmdf     /* [Fx] */
   };

void Srvc_Pccmd(BYTE command_num)
{
   WORD data;

    DEBUG_PRINTF("KBC Cmd: 0x%02x\n", command_num);


   Hif_Var[HIF_KBC].Cmd_Byte = 0;  /* Clear any previous command. */
   Hif_Var[HIF_KBC].Tmp_Load = 0;  /* Initialize Tmp_Load. */

   /* The hook will return a word with a value larger than 255
      if the command handler is to be skipped.  Otherwise,
      the lower part of the word will contain the command. */
   data = Gen_Hookc_Srvc_Pccmd(command_num);

   if (data < 256)
   {
      #if ACPI_SUPPORTED
      if ( (data >= FIRST_ACPI_CMD) && (data <= LAST_ACPI_CMD) )
      {
         /* For ACPI Embedded Controller Commands. */
         data = ACPI_Cmd((BYTE) data);
         if (data != (WORD) ~0)
         {
            Data_To_Host((BYTE) data); /* Send data to Host. */
            ACPI_Gen_Int();            /* Generate interrupt. */
         }
      }
      else
      #endif
      {
         /* Dispatch to command handler. */
         data = (pccmd_table[data >> 4])(&Hif_Var[HIF_KBC], (BYTE) data, HIF_KBC);

         if (data != (WORD) ~0)
         {
            Data_To_Host((BYTE) data); /* Send data to host. */
         }
      }
   }
}

/* ---------------------------------------------------------------
   Name: pc_cmddum

   Dummy function to handle unsupported controller commands.

   Input: Command #

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pc_cmddum(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pc_datdum

   Input: command == command number.
          data == data byte.

   At this point, Hif_Var[HIF_KBC].Cmd_Byte == 0.

   Handle the data part of 00h to 0Fh extended commands.
   --------------------------------------------------------------- */
WORD pc_datdum(BYTE command_num, BYTE data)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   return (rval);
}

/* ---------------------------------------------------------------
   Name: pc_cmd1

   Handle "1x" controller commands from HOST.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pc_Cmd1(command_num));
}

/* ---------------------------------------------------------------
   Name: pc_cmd2

   Handle "2x" and "3x" controller commands from Host.  These
   commands are the "Read Controller RAM" commands. Supported
   commands are 20, 33, 34, 36, and 37.  "0" is the default data
   returned for the commands that are not specifically supported.

   Input: Command #

   Returns: data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   BYTE data;

   switch(command_num)
   {
      case 0x20:
         /* Read Controller Command Byte. */
         data = Ccb42.byte;
         break;

      case 0x33:
         /* Read Security Code On byte. */
         data = Pass_On;
         break;

      case 0x34:
         /* Read Security Code Off byte. */
         data = Pass_Off;
         break;

      case 0x36:
         /* Read Password Code Discard 1. */
         data = Pass_Make1;
         break;

      case 0x37:
         /* Read Password Code Discard 2. */
         data = Pass_Make2;
         break;

      default:
         data = 0;
         break;
   }

   return((WORD) data);
}

/* ---------------------------------------------------------------
   Name: pc_cmd4

   Handle "4x" controller commands from HOST.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pc_Cmd4(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pc_cmd5

   Handle "5x" controller commands from HOST.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pc_Cmd5(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pc_cmd6

   Handle "6x" and "7x" controller commands from Host.  These
   commands are the "Write Controller RAM" commands.  Supported
   commands are 60, 73, 74, 76, and 77. (see "pc_dat6" also)

   Input: Command # (range 60-7F)

   Returns: a value with all bits set (no data is to be sent to the host)
   --------------------------------------------------------------- */
static WORD pc_cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   /* For these commands, a data byte is needed from
      the host.  Save the command and wait for the data. */

   hvp->Cmd_Byte = command_num;
   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pc_cmda

   Handle "Ax" controller commands from Host.  Supported commands
   are A4, A5, A6, A7, A8, A9, AA, AB, AC, AD, and AE. Invalid
   commands are ignored.

   Input: Command # (range A0-AF)

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */

   /* The functions in the table return a WORD value with all
      bits set if no data is to be sent to the host.  Otherwise,
      the low byte of the WORD that is returned is the data
      to be sent to the host (the high bits are zero). */
   const FUNCT_PTR_W_V pccmda_table[16] =
   {
      pcxdum,     /* A0 */
      pcxdum,     /* A1 */
      pcxdum,     /* A2 */
      pcxdum,     /* A3 */
      pcxa4,      /* A4 - Test password */
      pcxa5,      /* A5 - Load Password */
      Pcxa6,      /* A6 - Enable Password */
      pcxa7,      /* A7 - Disable Aux. Dev. Interface */
      Pcxa8,      /* A8 - Enable Auxiliary Device Interface */
      pcxa9,      /* A9 - Aux. Dev. Line Test */
      pcxaa,      /* AA - Controller Self-Test */
      pcxab,      /* AB - Aux. Keyboard Line Test */
      pcxac,      /* AC - Dump Copyright Message */
      pcxad,      /* AD - Disable Keyboard Interface */
      Pcxae,      /* AE - Enable Keyboard Interface */
      pcxdum      /* AF */
   };

static WORD pc_cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   /* Dispatch to command handler. */
   return( (pccmda_table[command_num & 0x0F])());
}

/* ---------------------------------------------------------------
   Name: pcxdum

   Dummy handler for unsupported "Ax" commands.

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pcxdum(void)
{
   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pcxa4

   Handle command A4 - Test Password Installed.

   Returns: data to send to HOST (FA or F1)
   --------------------------------------------------------------- */
static WORD pcxa4(void)
{
   if (Flag.PASS_READY)
      return(0xFA);  /* Return FA if password is loaded. */
   else
      return(0xF1);  /* Return F1 if password not loaded. */
}

/* ---------------------------------------------------------------
   Name: pcxa5

   Handle command A5 - Load Password

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pcxa5(void)
{
   Hif_Var[HIF_KBC].Cmd_Byte = 0xA5;  /* Need more data for this
                        command, save command byte. */

   /* Tmp_Load will be used as the password buffer index. */

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: Pcxa6

   Handle command A6 - Enable Password

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
WORD Pcxa6(void)
{
   WORD data;

   if (Flag.PASS_READY)
   {
      /* At this point, a password is loaded. */

      Write_Inhibit_Switch_Bit(0);  /* Enable inhibit switch. */
      Gen_Info.bit.PASS_ENABLE = 1; /* Enable password. */
      Pass_Buff_Idx = 0;            /* Clear password buffer index. */

      Flag.SCAN_INH = 0;            /* Clear internal keyboard inhibit. */

      data = (WORD) Pass_On;        /* Ready to send Pass_On byte to host. */
      if (data == 0)
          data = (WORD) ~0;         /* If Pass_On byte is zero, do not send. */
   }
   else
   {
      /* At this point, a password is not loaded. */
      data = (WORD) ~0;
   }

   return(data);
}

/* ---------------------------------------------------------------
   Name: pcxa7

   Handle command A7 - Disable Aux Device Interface

   The CLK line will be driven low here so that the interrupt
   handler will not have to drive it low if an interrupt is
   generated by the device.  However, driving the line low
   may generate an interrupt which the interrupt handler will
   have to ignore.

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pcxa7(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   Disable_Irq();

   if (MULPX.bit.PREFIX_CMD)
   {
      MULPX.bit.PREFIX_CMD = 0;
      switch (MULPX.byte & 0x03)
      {
         case AUX_PORT1_CODE:
               if (Aux_Port1.kbd == 0)
               {
                  /* A mouse compatible device is on Auxiliary Port 1. */
                  Aux_Port1.disable = 1;

                  //Disable_Irq();
                  Int_Lock_Aux_Port1();
                  //Enable_Irq();
               }
               break;

         case AUX_PORT2_CODE:
               #if AUX_PORT2_SUPPORTED
               if (Aux_Port2.kbd == 0)
               {
                  /* A mouse compatible device is on Auxiliary Port 1. */
                  Aux_Port2.disable = 1;

                  //Disable_Irq();
                  Int_Lock_Aux_Port2();
                  //Enable_Irq();
               }
               #endif
               break;
         case AUX_PORT3_CODE:
               #if AUX_PORT3_SUPPORTED
               if (Aux_Port3.kbd == 0)
               {
                  /* A mouse compatible device is on Auxiliary Port 1. */
                  Aux_Port3.disable = 1;

                  //Disable_Irq();
                  Int_Lock_Aux_Port3();
                  //Enable_Irq();
               }
               #endif
               break;
      }
   }
   else
   {
      Ccb42.bit.DISAB_AUX = 1;   /* Disable auxiliary device (mouse). */

      if (Aux_Port1.kbd == 0)
      {
         /* A mouse compatible device is on Auxiliary Port 1. */

         //Disable_Irq();
         Int_Lock_Aux_Port1();
         //Enable_Irq();
      }

      #if AUX_PORT2_SUPPORTED
      if (Aux_Port2.kbd == 0)
      {
         /* A mouse compatible device is on Auxiliary Port 2. */

         //Disable_Irq();
         Int_Lock_Aux_Port2();
         //Enable_Irq();
      }
      #endif

      #if AUX_PORT3_SUPPORTED
      if (Aux_Port3.kbd == 0)
      {
         /* A mouse compatible device is on Auxiliary Port 3. */

         //Disable_Irq();
         Int_Lock_Aux_Port3();
         //Enable_Irq();
      }
      #endif
   }

      /* Start inhibit delay timer. */
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   Enable_Irq();

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: Pcxa8

   Handle command A8 - Enable Auxiliary Device Interface

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
WORD Pcxa8(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif


   if (MULPX.bit.PREFIX_CMD)
   {  /* 90~3h command flag is set. */
      MULPX.bit.PREFIX_CMD = 0;  /* clear prefix flag. */
      switch (MULPX.byte & 0x03)
      {
         case AUX_PORT1_CODE:
               if ((Aux_Port1.kbd == 0) && (Ext_Cb0.bit.EN_AUX_PORT1))
               {
                  /* A mouse compatible device is on Auxiliary Port 1. */
                  Aux_Port1.disable = 0;
               }
               break;

         case AUX_PORT2_CODE:
               #if AUX_PORT2_SUPPORTED
               if (Aux_Port2.kbd == 0)
               {
                  /* A mouse compatible device is on Auxiliary Port 2. */
                  Aux_Port2.disable = 0;
               }
               #endif
               break;
         case AUX_PORT3_CODE:
               #if AUX_PORT3_SUPPORTED
               if (Aux_Port3.kbd == 0)
               {
                  /* A mouse compatible device is on Auxiliary Port 3. */
                  Aux_Port3.disable = 0;
               }
               #endif
               break;
      }
   }
   else
   {
      Ccb42.bit.DISAB_AUX = 0;   /* Enable auxiliary device (mouse). */
   }

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pcxa9

   Handle command A9 - Test Aux Device Interface

   Returns: test error code:
               0 = no error
               1 = Clock line stuck low
               2 = Clock line stuck high
               3 = Data line stuck low
               4 = Data line stuck high
   --------------------------------------------------------------- */
static WORD pcxa9(void)
{
   MULPX.bit.Multiplex = 0;
   #if AUX_PORT2_SUPPORTED
   return(Do_Line_Test(AUX_PORT2_CODE));
   #else // AUX_PORT2_SUPPORTED
   return(Do_Line_Test(AUX_PORT1_CODE));
   #endif // AUX_PORT2_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: pcxaa

   Handle command AA - Self Test
   Test registers, flags, and RAM.

   Returns: 0x55 to signify that the test passed;
            otherwise, returns 0 or return value from hook.
   --------------------------------------------------------------- */
static WORD pcxaa(void)
{
   WORD data;

   Disable_Irq();

   data = Gen_Hookc_Self_Test();
//   if (data == 0x55)
//   {
//      /* At this point there was no error from hook.  Do self test. */
//      data = Self_Test();
//   }
   Enable_Irq();
   pcxad();          /* Disable keyboard interface. */
   pcxa7();          /* Disable auxiliary device interface. */

   OEM_A20_High();   /* Force Gate A20 signal high.
                        (Always leave high after self test.) */
   Flag.PASS_READY = 0;

   return(data);
}

/* ---------------------------------------------------------------
   Name: pcxab

   Handle command AB - Test Keyboard Interface

   Returns: test error code:
               0 = no error
               1 = Clock line stuck low
               2 = Clock line stuck high
               3 = Data line stuck low
               4 = Data line stuck high
   --------------------------------------------------------------- */
static WORD pcxab(void)
{
   #if AUX_PORT2_SUPPORT
   return(Do_Line_Test(AUX_PORT2_CODE));
   #elif AUX_PORT3_SUPPORT
   return(Do_Line_Test(AUX_PORT3_CODE));
   #else
   return(0);
   #endif
}

/* ---------------------------------------------------------------
   Name: pcxac

   Handle command AC - Diagnostic Dump

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pcxac(void)
{
   Kbd_Response.byte = respCMD_AC;  /* Send multibyte sequence. */

   /* Tmp_Byte[0] will be used as the index for data. */
   Hif_Var[HIF_KBC].Tmp_Byte[0] = 0;

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pcxad

   Handle command AD - Disable Keyboard Interface

   The CLK line will be driven low here so that the interrupt
   handler will not have to drive it low if an interrupt is
   generated by the device.  However, driving the line low
   may generate an interrupt which the interrupt handler will
   have to ignore.

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pcxad(void)
{
   Ccb42.bit.DISAB_KEY = 1;   /* Disable auxiliary keyboard. */
   Flag.SCAN_INH = 1;         /* Inhibit scanner (internal keyboard). */
   Lock_Scan();               /* Lock scanner. */

   Disable_Irq();

   #if AUX_PORT2_SUPPORTED
   if (Aux_Port2.kbd)
   {
      /* A keyboard compatible device is on Auxiliary Port 1. */

      //Disable_Irq();
      Int_Lock_Aux_Port2();
      //Enable_Irq();
   }
   #endif

   #if AUX_PORT3_SUPPORTED
   if (Aux_Port3.kbd)
   {
      /* A keyboard compatible device is on Auxiliary Port 2. */

      //Disable_Irq();
      Int_Lock_Aux_Port3();
      //Enable_Irq();
   }
   #endif

   /* Start inhibit delay timer. */
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   Enable_Irq();

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: Pcxae

   Handle command AE - Enable Keyboard Interface

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
WORD Pcxae(void)
{
   Ccb42.bit.DISAB_KEY = 0;   /* Enable auxiliary keyboard. */
   Flag.SCAN_INH = 0;         /* Enable scanner (internal keyboard). */

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: pc_cmdc

   Handle "Cx" controller commands from HOST.  Only the
   "C0" command is supported.  Other commands will be ignored.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   WORD data;
   data = (WORD) ~0;

   if (command_num == 0xC0)
   {
      /* Read input port command. */

      if (Ext_Cb0.bit.PORT_CONTROL)
      {
         /* Input port is not wired, return compatibility value. */
         data = 0xBF;
      }
      else
      {
         data = (WORD) Read_Input_Port_1();
      }
   }

   return (data);
}

/* ---------------------------------------------------------------
   Name: pc_cmdd

   Handle "Dx" controller commands from HOST.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pc_cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   WORD data;

   switch(command_num)
   {
      case 0xD0:       /* Read Output Port. */
         data = (WORD) Read_Output_Port_2();
         break;

      case 0xD1:  /* Write Output Port 2. */
      case 0xD2:  /* Write Keyboard Output Buffer. */
      case 0xD3:  /* Write Auxiliary Device Output Buffer. */
         Hif_Var[HIF_KBC].Cmd_Byte = command_num;    /* Save command number
                               and wait for data byte. */
         data = (WORD) ~0;
         break;

      case 0xD4:  /* Write to Auxiliary Device. */
         Hif_Var[HIF_KBC].Cmd_Byte = command_num;    /* Save command number
                                       and wait for data byte. */
         MULPX.bit.Multiplex = 0;

         /* >>> Vincent 2003/6/18 Modify for After Install Synaptics Driver,
                System Restart cause T/P No function. */
         Aux_Port1.disable = 0;
         #if AUX_PORT2_SUPPORTED
         Aux_Port2.disable = 0;
         #endif
         #if AUX_PORT3_SUPPORTED
         Aux_Port3.disable = 0;
         #endif
         /* <<< End Add. */

         #if MOUSE_EMULATION
         Aux_Response = 0;
         Aux_Tmp_Load1 = 0;
         #endif // MOUSE_EMULATION


         data = (WORD) ~0;
         break;

      default:
         data = (WORD) ~0;
         break;
   }

   return (data);
}

/* ---------------------------------------------------------------
   Name: pc_cmde

   Handle "Ex" controller commands from Host.

   Read Test Inputs Command E0h.
   Input: Command #

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pc_cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
    WORD rval = ~0;
    if (command_num == 0xE0)              /* Read Test Inputs. */
    {
        if (Ext_Cb0.bit.PS2_AT)
        {
            /* PS2 mode. */
            rval = (WORD) PS2_Read_Test();
        }
        else
        {
            /* AT mode. */
            rval = (WORD) AT_Read_Test();
        }
    }

   return(rval);
}

/* ---------------------------------------------------------------
   Name: pc_cmdf

   Handle "Fx" controller commands from Host.

   Pulse Output Port Commands F0 through FF.  The low nibble
   of this command is used to signify which line(s) to pulse.
   A value of 0 in the bit causes the pulse to occur.
   The bits are defined as:

      Bit      AT Function      PS/2 Function
      ---      -----------      -------------
       0       System reset     System reset
       1       Gate A20         Gate A20
       2                        Auxiliary Data
       3                        Clock Data

   It is assumed that the CPU reset and Gate A20 pins handled by
   these functions are configured as outputs on initialization,
   and that they are initialized to "high" state.

   Input: Command #

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
static WORD pc_cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   WORD delay_value;

   /* Command FFh is a nop. */
   if (command_num != 0xFF)
   {

      if ((command_num & 1) == 0) OEM_Host_Reset_Low();
      if ((command_num & 2) == 0) OEM_A20_Low();
      Microsecond_Delay(Rst_A20_Pulse);

      if ((command_num & 1) == 0) OEM_Host_Reset_High();
      if ((command_num & 2) == 0) OEM_A20_High();
   }

   return((WORD) ~0);
}

/* ---------------------------------------------------------------
   Name: Srvc_Pcdat, Srvc_Pcdat2, Srvc_Pcdat3

   Handle incoming data byte from Host write to port 60h, 62h or from
   write to third Host interface.  The byte is either data
   that is required to complete processing of a controller command
   from the Host, or it is data that should be passed on to the
   keyboard (i.e., it is a keyboard command).

   Input: Data byte
            Cmd_Byte is non-zero if the data
            is part of a controller command.
   --------------------------------------------------------------- */
void Srvc_Pcdat(BYTE data)
{
   BYTE command;
   WORD rval;

   Gen_Hookc_Srvc_Pcdat(Hif_Var[HIF_KBC].Cmd_Byte, data);

   command = Hif_Var[HIF_KBC].Cmd_Byte;  /* Get the command byte. */
   Hif_Var[HIF_KBC].Cmd_Byte = 0;        /* Clear out the command byte. */

   if (command == 0)
   {
      DEBUG_PRINTF("KBD Cmd/Data: 0x%02x\n", data);

      /* At this point, "data" is actually a command for the keyboard. */

      /* Any command sent to the keyboard should clear keyboard
         disable bit in Ccb42.  This fixes the UNIX problem - it
         disables the keyboard, then sends ED (Update LEDs) command
         to the keyboard and expects that the keyboard will get enabled. */

      Pcxae();          /* Do AE command to enable keyboard. */
      sndto_kbd(data);  /* Send data (command) to keyboard. */
   }
   else
   {
      /* At this point, "data" is the data part of a controller command. */
      DEBUG_PRINTF("KBC Data: 0x%02x\n", data);

      #if ACPI_SUPPORTED
      if ( (command >= FIRST_ACPI_CMD) && (command <= LAST_ACPI_CMD) )
      {
         /* For ACPI Embedded Controller Commands. */
         rval = ACPI_Dat(command, data);
         if (rval != (WORD) ~0)
         {
            Data_To_Host2((BYTE) rval);/* Send data to Host. */
            ACPI_Gen_Int();            /* Generate interrupt. */
         }
      }
      else
      #endif
      {
         rval = process_cmd_data(&Hif_Var[HIF_KBC], command, data, HIF_KBC);
         if (rval != (WORD) ~0)
         {
            Data_To_Host((BYTE) rval);
         }
      }
   }
}


static WORD process_cmd_data(HIF_VAR* hvp, BYTE command, BYTE data, BYTE hif)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   AUX_PORT *aux_pntr;
   SMALL port;
   WORD rval;
   FLAG current;
   #if MOUSE_EMULATION
   WORD em_data;
   #endif // MOUSE_EMULATION

   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   if ((command & 0xE0) == 0x60) /* Check high 3 bits of command byte. */
   {
      pc_dat6(hvp, command, data, hif);          /* Handle 60-7F commands. */
   }
   else if ((command & 0xE0) == 0x80)  /* Check high 3 bits. */
   {
      rval = Ext_Dat8(hvp, command, data, hif);   /* Handle 80-9F extended cmds. */
   }
   else if ((command & 0xF0) == 0x00)  /* Check high 4 bits for 0x command. */
   {
      rval = Ext_Dat0(hvp, command, data, hif);   /* Handle 00-0F extended cmds. */
   }
   else if ((command & 0xF0) == 0x10)  /* Check high 4 bits for 1x command. */
   {
      rval = Gen_Hookc_Pc_Dat1(command, data);  /* Hook for OEM commands. */
   }
   else if ((command & 0xF0) == 0x40)  /* Check high 4 bits for 4x command. */
   {
      rval = Gen_Hookc_Pc_Dat4(hvp, command, data, hif);  /* Hook for OEM commands. */
   }
   else if ((command & 0xF0) == 0x50)  /* Check high 4 bits for 5x command. */
   {
      rval = Gen_Hookc_Pc_Dat5(hvp, command, data, hif);  /* Hook for OEM commands. */
   }
   else if ((command & 0xF0) == 0xB0)  /* Check high 4 bits for Bx command. */
   {
      rval = Ext_Datb(hvp, command, data, hif);   /* Handle B0-BF cmds. */
   }
   else
   {
      switch(command)
      {
         case 0xD1:
            /* Write this data to 8042 Output Port 2. */

            Write_Output_Port_2(data);

         break;

         case 0xD2:
            /* Write this data to output buffer.
               Send data to Host. */
            rval = (WORD) data;
            break;

         case 0xD3:
            /* Write this data to auxiliary output buffer. */
            current = 1;
            if (data == 0xF0)
            {  /* Data from Host is F0h. */
               MULPX.bit.Mx_state2 = 1;
               MULPX.bit.Mx_state3 = 0;
               current = 0;   /* Don't clear State2 and State3. */
            }
            else if (data == 0x56)
            { /* Data from Host is 56h. */
               if (MULPX.bit.Mx_state2)
               { /* Current is State2. */
                  MULPX.bit.Mx_state2 = 0;
                  MULPX.bit.Mx_state3 = 1;
                  current = 0;
               }
            }
            else if (data == 0xA4)
            { /* Data from Host is A4h. */
               if (MULPX.bit.Mx_state3)
               { /* Current is State3. */
                  MULPX.bit.Multiplex = 1;   /* Enable Activate Multiplex mode. */
                  MULPX.byte &= ~(0x03);
                  Ext_Cb0.byte |= 0x0F;   /* Enable all PS/2 Port. */
                  data = 0x11;      /* Return Version number. */
               }
            }
            else if (data == 0xA5)
            { /* Data from Host is A5h. */
               if (MULPX.bit.Mx_state3)
               { /* Current is State3. */
                  MULPX.bit.Multiplex = 0; /* Reveret to Legacy Mode. */
                  data = 0x11;   /* Return version number. */
               }
            }
            if (current)
            {
               MULPX.byte &= ~(0x18); /* Clear State2 and State3. */
            }

            Aux_Data_To_Host(data);
            break;

         case 0xD4:
            /* Write this data to auxiliary device. */

            /* Maintain mouse state for Command BCh. */

            if (!MULPX.bit.Multiplex)
            {
               #if MOUSE_EMULATION
               em_data = Save_Mouse_State(data);
               #else // MOUSE_EMULATION
               Save_Mouse_State(data);
               #endif // MOUSE_EMULATION
            }

            Expect_Count = 1;
            if (data == 0xEB) Expect_Count = 4;
            else if (data == 0xF2) Expect_Count = 2;
            else if (data == 0xFF) Expect_Count = 3;

            /* There can be a mouse on Auxiliary Port 1, 2, and/or
               3.  The primary mouse is the mouse that is to accept
               commands.  Any other mouse on another Auxiliary Port
               is a secondary mouse.  Only allow D4 data to go to
               the primary mouse at this time.  Any secondary mouse
               will be updated later. */

            /* Reset inactivity timer (whether or
               not data will be sent to a mouse). */

            Aux_Port_Ticker = 0;

            /* Get the port number of the primary mouse. */
            if (MULPX.bit.PREFIX_CMD)
            {
               port = MULPX.byte & 0x03;
               //MULPX.bit.PREFIX_CMD = 0;  /* Clear prefix command flag */
               if (port == 0) port = 3;
            }
            else
            {
               port = (Ps2_Ctrl.byte >> shiftPRI_MOUSE) & maskPRI_MOUSE;
            }
            if (MULPX.bit.Multiplex)
            {
               if ((port == 1) && !Ext_Cb0.bit.EN_AUX_PORT1) port = 0;
            }
            #if MOUSE_EMULATION
            else if ((port == 1) && !Ext_Cb0.bit.EN_AUX_PORT1) port = 0;
            #endif // MOUSE_EMULATION

            if (port == 0)
            {
               #if MOUSE_EMULATION
               if (em_data != ~0)
               {
                  Pcxa8(); /* Enable Auxiliary Device Interface.
                              Makes it compatible with IBM. */
                  Aux_Data_To_Host(em_data);
               }
               #else // MOUSE_EMULATION
               Gen_Timer = (BYTE) ~0;
               Aux_Error_To_Host(0xFE, 0);
               #endif // MOUSE_EMULATION
            }
            else
            {
               port &= 0x03;
               aux_pntr = Make_Aux_Pntr(port);

               if (aux_pntr->kbd == 0)
               {  /*
                  This is set as a mouse port.  Set the valid flag to
                  allow the command from the Host to reach the mouse.

                  If the valid flag is clear, an error must have been
                  detected previously on this port.  With this flag clear,
                  the command can not be sent to the mouse.  Since this
                  is the primary mouse, the flag will be set to allow the
                  command to reach the mouse.  It is possible that the
                  error was just a transient error and not the removal
                  of the mouse. */
                  ;
               }
               else if (MULPX.bit.Multiplex)
               {
                  Gen_Timer = (BYTE) ~0;
                  Aux_Error_To_Host(0xFE, 0x40);
               }

               if ( (aux_pntr->kbd == 0) && (Check_Transmit(port)) )
               {
                  /* OK to send data to mouse that is on the Auxiliary Port. */

                  Pcxa8(); /* Enable Auxiliary Device Interface.
                              Makes it compatible with IBM. */

                  /* Command being sent to primary mouse. */
                  Aux_Cmd_Setup(aux_pntr, data);
                  if (!Send_To_Aux(aux_pntr, port, data))
                  {
                     #if MOUSE_EMULATION
                     Aux_Response = 0;
                     Aux_Tmp_Load1 = 0;
                     #endif // MOUSE_EMULATION

                     /* At this point, no error occurred. */
                     aux_pntr->ack = 1;/* Flag to wait for acknowledge. */
                  }
                  else
                  {  /*
                     An error occurred.  If feature is enabled,
                     signal that an attempt should be made to
                     find a new primary mouse. */

                     Gen_Timer = (BYTE) ~0;
                     if (!MULPX.bit.Multiplex)
                     {  /* Multiplex mode not be enable. */
                        if (Ps2_Ctrl.bit.M_AUTO_SWITCH) Flag.NEW_PRI_M = 1;
                        Disable_Irq();
                        Service.bit.AUX_PORT_SND = 0;
                        Enable_Irq();
                        aux_pntr->ack = 0;/* Flag to not wait for acknowledge. */
                        aux_pntr->valid = 0;
                        Aux_Error_To_Host(0xFE, 0x40);
                     }
                     else
                     {
                        Aux_Error_To_Host(0xFE, 0);
                     }
                  }
               }
               else
               {
                  /* A keyboard compatible device is on the Auxiliary
                     Port or the port cannot accept data. */

                  /* If feature is enabled, signal that an attempt
                     should be made to find a new primary mouse. */
                  Gen_Timer = (BYTE) ~0;
                  if (!MULPX.bit.Multiplex)
                  {  /* Multiplex mode not be enable. */
                     if (Ps2_Ctrl.bit.M_AUTO_SWITCH) Flag.NEW_PRI_M = 1;
                     Aux_Error_To_Host(0xFE, 0x40);
                  }
                  else
                  {
                     Aux_Error_To_Host(0xFE, 0);
                  }
               }
            }

            MULPX.bit.PREFIX_CMD = 0;  /* Clear prefix command flag */

            break;

         case 0xA5:
            /* Load Password. */

            if ( (hvp->Tmp_Load == 0) && (data == 0) )
            {  /*
               If the first byte is NULL, then
               clear password ready flag and exit. */
               Flag.PASS_READY = 0;

               /*
               Cmd_Byte has already been set to 0.
               So the code will NOT wait for next byte */
            }
            else
            {
               if (hvp->Tmp_Load < PASS_SIZE)
               {
                  /* If there is room in the buffer... */

                  Pass_Buff[hvp->Tmp_Load] = data;   /* Store scan code. */
                  hvp->Tmp_Load++;                   /* Increment password
                                                   buffer index. */
               }

               hvp->Cmd_Byte = 0xA5;  /* Set to keep waiting for next byte. */

               if (data == 0)
               {
                  /* The last byte (null terminated string) has been stored. */

                  hvp->Cmd_Byte = 0;        /* Clear out the command byte. */

                  Flag.PASS_READY = 1; /* Set password ready bit. */
               }
            }
            break;
      }
   }

   return (rval);
}

/* ---------------------------------------------------------------
   Name: pc_dat6

   Writes data sent from HOST with commands 60-7F ("Write Controller
   RAM" commands) to the appropriate data variable.  Our on-chip RAM
   does not store data at the same locations as the original 8042,
   so we just write the data to the correct variable - not to the
   explicit RAM address implied with the command #.  Supported
   commands are 60, 73, 74, 76, and 77.

   Input: command_num = command number
          data = data byte sent with command
   --------------------------------------------------------------- */
static void pc_dat6(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   BITS_8 temp;

   switch(command_num)
   {
      case 0x60:
         /* Write controller command byte. */


         Ccb42.byte = data;   /* Write the data. */

         /* Reconfigure variables/registers that
            are dependent on Command Byte value. */

         if (Ext_Cb0.bit.PS2_AT)
         {
            /* For PS/2 style 8042 interface. */

            if (Ccb42.bit.DISAB_AUX)
               pcxa7(); /* Disable aux device interface. */
            else
               Pcxa8(); /* Enable Auxiliary Device Interface. */
         }

         if (Ccb42.bit.DISAB_KEY)
            pcxad(); /* Disable keyboard interface. */
         else
            Pcxae(); /* Enable keyboard interface. */

         /* Put system flag bit in Status Reg. */
         Write_System_Flag(Ccb42.bit.SYS_FLAG);
         break;

      case 0x73:
         /* Write Security Code On byte. */
         Pass_On = data;
         break;

      case 0x74:
         /* Write Security Code Off byte. */
         Pass_Off = data;
         break;

      case 0x76:
         /* Write Password Code Discard 1. */
         Pass_Make1 = data;
         break;

      case 0x77:
         /* Write Password Code Discard 2. */
         Pass_Make2 = data;
         break;
   }
}


