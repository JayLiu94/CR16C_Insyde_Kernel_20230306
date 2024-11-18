/* ----------------------------------------------------------------------------
 * MODULE PURCRSIS.C
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
 * Contains the command handler portion of the crisis section.
 *
 * Revision History (Started June 4, 1996 by GLP)
 *
 * GLP
 * 07/25/96 Changed so that a more complete version number will be generated
 *          for Command ACh for crisis mode.
 *          The first version generated 1.02.00.  The version is now sent as
 *          x.xx.xx[xxxx-xx]-x.  The data for the version is from header files.
 * 09/11/96 Changed include file from chipdef.h to swtchs.h and changed "ifdef
 *          MESSAGES" to "if MESSAGES".
 * 08/19/99 Removed debug comment in controller_cmd.  Added BOOTABLE_CRISIS and
 *          CRISIS_CMD_0FH switches.  Added Cmd0F.  Updated comments and
 *          copyright.
 * ------------------------------------------------------------------------- */

#define PURCRSIS_C

/* ----------------------------------------------------------------------------

   The Keyboard Controller BIOS consists of a section of memory which
   is to remain fixed and a section of memory which may be updated.
   The fixed portion will be referred to as the crisis section.  This
   section could be put into the "boot block" section of a Flash ROM
   or could be a section of memory that is artificially called the
   boot block.  The section of memory which may be updated could be a
   battery-backed RAM, a Flash ROM, or another part which may be
   updated.  This section will be referred to as the Flash section.

   This file contains the command handler portion of the crisis
   section.  This command handler is operated in polled mode using no
   interrupts.

   A subset of the standard commands are supported; enough to allow
   the system to boot to DOS with a featureless BIOS.  Also, Flash
   section update commands are supported in the 0xh block.  The Flash
   section update commands may be used by an update utility to update
   the Flash section.

   This command handler is run when the system is booted and it is
   determined that the Flash section does not contain a valid program
   or that the program needs to be updated.  The program running in
   the Flash section can also force entry to crisis mode via the
   standard command handler with commands 06h and 07h.  This is done
   when the user wants to update the Flash section after first doing
   a normal boot.

   Crisis mode is entered via a fixed jump vector in the crisis
   section.  The location of this vector is implemention dependent
   but must remain at a fixed location.  This allows the Flash
   section code to access the crisis entry, even if the crisis code
   evolves over time.  Once the Flash section code jumps to the
   crisis entry in the crisis section, the update commands are then
   supported, so updating can begin.  To eliminate duplication, the
   update commands are NOT supported in the standard command handler.

   Once crisis mode is entered, the only return to Flash section code
   is via a downloaded RAM program or system reset.

   Above all, the crisis section must be COMPLETELY independant of
   the Flash section.  This means there should be no INCLUDE files
   used which could evolve with the Flash section code.  Keeping this
   rule involves some duplication of code, but not much.  Similarly,
   the Flash section code should not depend on any of the crisis
   section code.  The only link between the two worlds is done
   through the jump table vector.  This vector location must remain
   fixed!
   --------------------------------------------------------------- */

/* ---------------------------------------------------------------
   BE CAREFUL WHAT YOU INCLUDE HERE !
   --------------------------------------------------------------- */

#include "swtchs.h"
#include "types.h"
#include "defs.h"
#include "purcrsis.h"
#include "crisis.h"
#include "host_if.h"
#include "purcrver.h"
#include "id.h"
#include "icu.h"
#include "timers.h"
#include "regs.h"
#include "oemcrsis.h"

#if RAM_BASED_FLASH_UPDATE
extern void FLASH_UPDATE_handle(void);
#else
#include "fu.h"
#endif

#ifndef MESSAGES
#error MESSAGES switch is not defined.
#endif

#ifndef BOOTABLE_CRISIS
#error BOOTABLE_CRISIS switch is not defined.
#endif

#ifndef CRISIS_CMD_0FH
#error CRISIS_CMD_0FH switch is not defined.
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void controller_cmd(BYTE host_byte);
void Load_Crisis_1ms(void);
/* ---------------------------------------------------------------
   Name: cmdxx - Unused Command

   Use this handler for any commands that are not used.

   --------------------------------------------------------------- */
#define cmdxx() bCrisis_Cmd_Index = 0;  /* Clear the index. */

/* ---------------------------------------------------------------
   Name: Crisis_Command_Handler

   Process an incoming command or data byte.

   If the byte arrives via port 64h, store the byte in
   bCrisis_Cmd_Num, clear bCrisis_Cmd_Index, and process
   the byte as a Keyboard Controller command.

   bCrisis_Cmd_Index is used to determine which data byte is
   currently being sent from the host.  The scheme is:

      bCrisis_Cmd_Index = 0 means this is the command byte
      bCrisis_Cmd_Index = 1 means this is the first data byte
      bCrisis_Cmd_Index = 2 means this is the second data byte
      bCrisis_Cmd_Index = 3 means this is the third data byte
      etc.

   Each command updates bCrisis_Cmd_Index.  If the command needs no
   more data bytes, the index is set to 0.  If more data bytes are
   needed, the index is incremented for the next data byte.

   If the byte arrives via port 60h, then:

   If bCrisis_Cmd_Index is nonzero, then this is a data byte which is
   part of a Keyboard Controller command in progress.

   If bCrisis_Cmd_Num is zero, then this is a keyboard command.
   --------------------------------------------------------------- */
void Crisis_Command_Handler(void)
{
   WORD host_byte;

   /* Enable the keyboard (set the inhibit switch bit to
      off, disable the password) to allow the Host to run.
      Also, perform other needed initialization such as
      turning on Fast A20 support. */
   Crisis_Init_Cmd_Hndlr();

   #if BOOTABLE_CRISIS
   /* Initialize the Keyboard Controller command byte. */
   Crisis_Command_Byte.byte = CRISIS_COMMAND_BYTE_INIT;
   #endif

   Crisis_Flags.byte = 0;
   Crisis_Flags.bit.GEN_CRISIS = 1; /* Flag for crisis mode. */

   bCrisis_Cmd_Index = 0;           /* Get ready for first command. */

   /* Here is the actual crisis mode command interface.  Keep polling
      for new input from port 64h or 60h here.  Process the command or
      data, and keep coming back here for the next byte.

      There is no escape from here except for a system reset or if
      control is taken away with a RAM routine from the Host. */

   #if MESSAGES
    Message("Crisis_Command_Handler\n", 0);
   #endif

   do
   {
      // check if need to do Flash Update - call FLASH_UPDATE_handle() (ROM or RAM)
      FLASH_UPDATE_handle();

      /* Wait until data is available from Host. */
      while (!Input_Buffer_Full())
      {
         if (ICU_Irq_Is_Pending(ICU_EINT_T0OUT))
         {
            // oem hook for crisis 1msec
            OEM_Hookc_Crisis_1ms();

            Load_Crisis_1ms();
         }

         // check if need to do Flash Update - call FLASH_UPDATE_handle() (ROM or RAM)
         FLASH_UPDATE_handle();

         // oem hook for getting data on PM channels
         if (OEM_Hookc_PM_Buffer_Full())
         {
            OEM_Hookc_Handle_PM_Data();
         }
      }
      /* Then, process the byte. */

      host_byte = Get_Host_Byte();

      if (host_byte > 255)
      {  /*
         Byte was from port 64h.  Process as Keyboard Controller
         command. */

         #if MESSAGES
          if ((host_byte & 0xFF) != 0xD1)
             Message("Cmnd 0x%X\n", (int) host_byte & 0xFF);
         #endif

         bCrisis_Cmd_Num = (BYTE) host_byte; /* Store the command. */
         bCrisis_Cmd_Index = 0;              /* Clear the byte index counter. */

         controller_cmd((BYTE) host_byte);   /* Dispatch the command. */
      }
      else if (bCrisis_Cmd_Index)
      {  /*
         Byte was from port 60h and bCrisis_Cmd_Index is not zero.  This
         is a data byte which is part of a Keyboard Controller command in
         progress. */

         #if MESSAGES
          if (bCrisis_Cmd_Num != 0xD1)
             Message("Data 0x%X\n", (int) host_byte);
         #endif

         controller_cmd((BYTE) host_byte);   /* Dispatch the command. */
      }
      else
      {  /*
         Byte was from port 60h and bCrisis_Cmd_Num is zero.  This
         is a keyboard command (not a Keyboard Controller command). */

         #if BOOTABLE_CRISIS
         /*
         Make sure to return with bCrisis_Cmd_Index set to 0 so the
         next byte byte will not be seen as part of a Keyboard
         Controller command.

         The only time that keyboard commands are received in crisis
         mode is during POST.  Just fool the Host into thinking there is
         a real keyboard out there.  This is done by always returning
         FAh (ACK) for any keyboard command that comes in.  Keyboard
         commands are not actually processed.

         There are, however, two exceptions:

         If the keyboard command if F2h (read ID),
         FAh / ABh / 41h is returned.

         If the keyboard command is FFh (reset keyboard),
         FAh / AAh is returned. */

         #if MESSAGES
          Message("Keyboard command 0x%X\n", (int) host_byte);
         #endif

         Crisis_Data_To_Host(0xFA); /* Reply with ACK to make the Host
                                       think it is really a good keyboard. */

         if (host_byte == 0xF2)
         {
            /* For read ID command, reply with AB / 41 to make
               the Host think that it a keyboard exists. */
            Crisis_Data_To_Host(0xAB);
            Crisis_Data_To_Host(0x41);
         }
         else if (host_byte == 0xFF)
         {
            /* For reset command, reply with AA to make the
               Host think that it is a good keyboard reset. */
            Crisis_Data_To_Host(0xAA);
         }
         #endif

         /* Set bCrisis_Cmd_Index to 0. */
         bCrisis_Cmd_Index = 0;
      }
   }
   while (1);
}

/* ---------------------------------------------------------------
   Name: controller_cmd

   Here is processing for Keyboard Controller commands and associated
   data bytes.

   If the current byte is a command byte
      host_byte = the command byte
      bCrisis_Cmd_Num = the command byte
      bCrisis_Cmd_Index = 0

   If the current byte is a data byte
      host_byte = the data byte
      bCrisis_Cmd_Num = the previously stored command byte
      bCrisis_Cmd_Index = the data byte index

   Only the minimum number of 8042 commands are supported to allow
   the machine to get through POST into DOS.  In almost all cases,
   the Host is being fooled into thinking that real commands are
   being processed.  A "fake" Keyboard Controller command byte is
   kept so that reads and writes to it (20h and 60h) agree.

   The Flash section update commands in the 0x block are supported
   as described later.
   --------------------------------------------------------------- */
static void controller_cmd(BYTE host_byte)
{
   #if BOOTABLE_CRISIS
   /* Version data for Command ACh. */
   static const BYTE version[] =
   {
      TRANS(CRISISMAJ), 0x34,
      TRANS(CRISISMID_H), TRANS(CRISISMID_L), 0x34,
      TRANS(CRISISMIN_H), TRANS(CRISISMIN_L),
      0x1A,
      TRANS(PROJECT_1),       TRANS(PROJECT_2),
      TRANS(PROJECT_3),       TRANS(PROJECT_4), 0x0C,
      TRANS(CRISIS_OEM_VER1), TRANS(CRISIS_OEM_VER2),
      0x1B,
      CRISIS_TMP_LET
      0x0C,             /* - */
      ID_MARK,
      0xAA              /* Terminator. */
   };
   #endif

   SMALL index;
   BYTE data;

   if (OEM_Hookc_controller_cmd(host_byte))
   {
        return; // command was handled by OEM crisis code
   }

   switch (bCrisis_Cmd_Num)
   {
      #if BOOTABLE_CRISIS
      case 0x20:
         /* ------------------------------------------------------
            Read Command Byte

            8042 command to read the command byte.  Make the Host
            think this works by reading a fake command byte.
            ------------------------------------------------------ */
         Crisis_Data_To_Host(Crisis_Command_Byte.byte);
         bCrisis_Cmd_Index = 0;
         break;

      case 0x60:
         /* ------------------------------------------------------
            Write Command Byte

            8042 command to write the command byte.  Make the Host
            think this works by writing a fake command byte.

            Input:
               if bCrisis_Cmd_Index = 0, host_byte = command byte
               else host_byte = data byte.

            Returns:
               updated bCrisis_Cmd_Index
            ------------------------------------------------------ */
         if (bCrisis_Cmd_Index == 0)
            bCrisis_Cmd_Index++; /* Increment the index when the
                                    command byte is received. */
         else
         {
            /* Otherwise, this is the data byte. */

            /* Update the Keyboard Controller command byte. */
            Crisis_Command_Byte.byte = host_byte;

            /* Store the system flag in the status register
               so Host thinks the command byte is real. */
            Write_System_Flag(Crisis_Command_Byte.bit.SYS_FLAG);

            /* Enable or disable IRQ1 generation to Host. */
            if (Crisis_Command_Byte.bit.INTR_KEY)
               Enable_Host_IRQ1();
            else
               Disable_Host_IRQ1();

            bCrisis_Cmd_Index = 0;  /* Clear the index. */
         }
         break;

      case 0xA7:
         /* ------------------------------------------------------
            Disable Aux Device

            8042 command to disable the aux device.
            Simulate by just updating the fake command byte
            so Host thinks aux device is really disabled.
            ------------------------------------------------------ */
         Crisis_Command_Byte.bit.DISAB_AUX = 1;
         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xAA:
         /* ------------------------------------------------------
            Self Test

            8042 self test command.
            Just send 55h so Host thinks it is a real selftest.
            ------------------------------------------------------ */
            Crisis_Data_To_Host(0x55);
            bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xAC:
         /* ------------------------------------------------------
            Send Version String

            Insyde Software Send Version String command.
            Report the version string.
            ------------------------------------------------------ */

         index = 0;
         do
         {
            data = version[index++];
            Crisis_Data_To_Host(data);
         }
         while (data != 0xAA);

         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xAD:
         /* ------------------------------------------------------
            Disable Keyboard

            8042 command to disable the keyboard.
            Simulate by just updating the fake command byte
            so Host thinks keyboard is really disabled.
            ------------------------------------------------------ */
         Crisis_Command_Byte.bit.DISAB_KEY = 1;
         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xAE:
         /* ------------------------------------------------------
            Enable Keyboard

            8042 command to enable the keyboard.
            Simulate by just updating the fake command byte
            so Host thinks keyboard is really enabled.
            ------------------------------------------------------ */
         Crisis_Command_Byte.bit.DISAB_KEY = 0;
         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xC0:
         /* ------------------------------------------------------
            Read Input Port

            8042 command to read compatible port.

            Bit 7 = Keyboard not inhibited via switch.
            Bit 6 = Reserved.
            Bit 5 = Reserved.
            Bit 4 = Reserved.
            Bit 3 =
            Bit 2 =
            Bit 1 = Auxiliary Device (mouse) data in line (PS/2 only).
            Bit 1 = Auxiliary Keyboard data in line (PS/2 only).

            Just return BFh - seems to be what host wants!
            ------------------------------------------------------ */
         Crisis_Data_To_Host(0xBF);
         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;

      case 0xD1:
         CmdD1(host_byte);
         break;

      case 0xD4:
         /* ------------------------------------------------------
            Write to Aux Device

            8042 command to write to aux device.
            Get data and return FEh to make host think no
            aux device exists.

            Input:
               if bCrisis_Cmd_Index = 0, host_byte = command byte
               else host_byte = data byte.

            Returns:
               updated bCrisis_Cmd_Index
            ------------------------------------------------------ */
         if (bCrisis_Cmd_Index == 0)
            bCrisis_Cmd_Index++; /* Increment the index when the
                                    command byte is received. */
         else
         {
            /* Otherwise, this is the data byte. */

            /* Send FEh no matter what so Host
               thinks aux device does not exist. */
            Crisis_Aux_Data_To_Host(0xFE);
            bCrisis_Cmd_Index = 0;  /* Clear the index. */
         }
         break;
      #endif

      default:
         if (bCrisis_Cmd_Num & 0xF0)
         {
            /* Not a valid command. */
            cmdxx();
         }
         else
         {  /*
            Commands 00h through 0Fh (Flash section update
            commands).

            These commands (all in the 0x block), are actually
            handled in three different code regions:

            1) In the RAM program, commands 00h - 05h are used to
               perform the actual update operation.  Commands 00h -
               05h are not supported anywhere else.

            2) In the main keyboard controller code, commands 06h and
               07h are used to enter the crisis mode command handler.
               Commands 06h and 07h are not supported anywhere else.

            3) In the crisis module (here), commands 08h - 0Bh and
               command 0Fh are reserved for use in downloading the RAM
               program and other flash support functions.  Commands
               08h - 0Bh and 0Fh are not supported anywhere else.

            One command, 0Ch, is supported both in the main keyboard
            controller code and the crisis module.  It returns the
            crisis status flags and therefore it must be supported
            in both places.  There is no overlap of command numbers,
            even though all but 0Ch are used in separate code sections.

            A Flash Section Update Disk utility could conceptually
            work as follows:

            1) If the system booted in normal mode, issues command 06h
               or 07h to put the Keyboard Controller into the crisis
               command handler.

            2) Loads the RAM program via command 08h.

            3) Executes the RAM program via command 09h.

            4) Procedes to update the device using the
               commands supported in the RAM program. */

            switch (bCrisis_Cmd_Num)
            {
               case 0x08:
                  Cmd08(host_byte);
                  break;

               case 0x09:
                  Cmd09(host_byte);
                  break;

               case 0x0A:
                  Cmd0A();
                  break;

               case 0x0B:
                  Cmd0B();
                  break;

               case 0x0C:
                  /* ------------------------------------------------------
                     Get Critical Status

                     Return the critical status flags.  These
                     may be defined at a later time.  For now:
                        bit 7 : 0 = NOT in crisis mode
                                1 = IN crisis mode
                     ------------------------------------------------------ */
                  Crisis_Data_To_Host(Crisis_Flags.byte);
                  bCrisis_Cmd_Index = 0;  /* Clear the index. */
                  break;

               #if CRISIS_CMD_0FH
               case 0x0F:
                  Cmd0F();
                  bCrisis_Cmd_Index = 0;  /* Clear the index. */
                  break;
               #endif

               default:
                  /* 00 - 05 Supported in RAM program.
                     06 - 07 Supported in main keyboard controller code.
                     0D - 0E Unused. */
                  cmdxx();
                  break;
            }
         }
         break;
   }
}

void Load_Crisis_1ms (void)
{
   /* The timer from the TWD (Timer and Watchdog
      Module) is used for Timer A.  The pre-scale and
      TWDT0 registers have already been initialized. */

   SET_BIT(T0CSR, T0CSR_RST);                    /* Cause the timer to reload
                                             on the next input clock. */

   ICU_Clr_Pending_Irq(ICU_EINT_T0OUT);   /* Clear any pending interrupt. */
}

