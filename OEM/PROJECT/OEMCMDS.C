/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMCMDS.C - OEM commands from KBC or PM channels
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#define OEMCMDS_C

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "swtchs.h"
#include "types.h"
#include "proc.h"
#include "regs.h"
#include "oem.h"
#include "host_if.h"
#include "purhook.h"
#include "purdat.h"
#include "pindef.h"
//

/*********************** Hooks in PURCMD.C ***********************/
/* ---------------------------------------------------------------
   Name: Hookc_Srvc_Pccmd

   This function is called before the incoming command from
   Host write to port 64h is handled.  The value returned by
   this routine is used as the input command to the handler.

   Input: "command_num" is the controller command.

   Return: To skip the handler, return a value greater than 255.
           Otherwise, return the controller command in the
           lower byte of the returned WORD.
   --------------------------------------------------------------- */
WORD Hookc_Srvc_Pccmd(BYTE command_num)
{
    return (WORD) command_num;
}

/* ---------------------------------------------------------------
   Name: Hookc_Srvc_Pcdat

   This function is called upon entry to the routine used
   to handle incoming data byte from Host write to port 60h.

   Input: "data" is the data received from Host.
          if command_num == 0 "data" is for the keyboard.
          otherwise, command_num is the controller command
          and        "data" is the data part of the command.
   --------------------------------------------------------------- */
void Hookc_Srvc_Pcdat(BYTE command_num, BYTE data)
{
   ;
}

/*********************** Hooks in PURPM.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd0

   This function is called before the incoming command from
   Host write "0x0X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

   Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd0
WORD Hookc_Pmx_Cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
    switch (hif)
    {
        case HIF_KBC:   // Port 60h/64h
            break;
        case HIF_PM1:   // Port 62h/66h
            break;
        case HIF_PM2:   // Port 68h/6Ch
            break;
    }
    //hvp->Cmd_Byte = command_num;

   return (WORD) ~0;
}
#endif
/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat0

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x0X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */

#if Def_Hookc_Pmx_Dat0
WORD Hookc_Pmx_Dat0(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif
/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd1

   This function is called before the incoming command from
   Host write "0x1X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

   Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */

#if Def_Hookc_Pmx_Cmd1
WORD Hookc_Pmx_Cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat1

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x1X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat1
WORD Hookc_Pmx_Dat1(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif
/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd2

   This function is called before the incoming command from
   Host write "0x2X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

   Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */

#if Def_Hookc_Pmx_Cmd2
WORD Hookc_Pmx_Cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat2

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x2X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat2
WORD Hookc_Pmx_Dat2(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif
/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd3

   This function is called before the incoming command from
   Host write "0x3X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

   Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */

#if Def_Hookc_Pmx_Cmd3
WORD Hookc_Pmx_Cmd3(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat3

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x3X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat3
WORD Hookc_Pmx_Dat3(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif
/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd4

   This function is called before the incoming command from
   Host write "0x4X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */

#if Def_Hookc_Pmx_Cmd4
WORD Hookc_Pmx_Cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat4

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x4X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat4
WORD Hookc_Pmx_Dat4(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd5

   This function is called before the incoming command from
   Host write "0x5X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd5
WORD Hookc_Pmx_Cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat5

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x5X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat5
WORD Hookc_Pmx_Dat5(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd6

   This function is called before the incoming command from
   Host write "0x6X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd6
WORD Hookc_Pmx_Cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat6

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x6X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat6
WORD Hookc_Pmx_Dat6(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd7

   This function is called before the incoming command from
   Host write "0x7X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd7
WORD Hookc_Pmx_Cmd7(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat7

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x7X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat7
WORD Hookc_Pmx_Dat7(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd8

   This function is called before the incoming command from
   Host write "0x8X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd8
WORD Hookc_Pmx_Cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat8

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x8X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat8
WORD Hookc_Pmx_Dat8(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmd9

   This function is called before the incoming command from
   Host write "0x9X" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmd9
WORD Hookc_Pmx_Cmd9(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Dat9

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0x9X" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Dat9
WORD Hookc_Pmx_Dat9(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmda

   This function is called before the incoming command from
   Host write "0xAX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmda
WORD Hookc_Pmx_Cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Data

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xAX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Data
WORD Hookc_Pmx_Data(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmdb

   This function is called before the incoming command from
   Host write "0xBX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmdb
WORD Hookc_Pmx_Cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Datb

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xBX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Datb
WORD Hookc_Pmx_Datb(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmdc

   This function is called before the incoming command from
   Host write "0xCX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmdc
WORD Hookc_Pmx_Cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Datc

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xCX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Datc
WORD Hookc_Pmx_Datc(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmdd

   This function is called before the incoming command from
   Host write "0xDX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmdd
WORD Hookc_Pmx_Cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Datd

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xDX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Datd
WORD Hookc_Pmx_Datd(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmde

   This function is called before the incoming command from
   Host write "0xEX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmde
WORD Hookc_Pmx_Cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif)

{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Date

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xEX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Date
WORD Hookc_Pmx_Date(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Cmdf

   This function is called before the incoming command from
   Host write "0xFX" to PM channels is handled.  The value returned by
   this routine is used as the input command to the handler.

      Input: "command_num" is the controller command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface


   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Cmdf
WORD Hookc_Pmx_Cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return (WORD) ~0;
}
#endif

/* ---------------------------------------------------------------
   Name: Hookc_Pmx_Datf

   This function is called upon entry to the routine used
   to handle incoming data byte for command "0xFX" from Host write to PM channels.

   Input: "data" is the data received from Host.
          "command_num" is the controller command
          and        "data" is the data part of the command.
          "hvp" is Host interface variales.
          "hif" is Host channel
             - 0 : KBC interface
             - 1 : PM1 interface
   --------------------------------------------------------------- */
#if Def_Hookc_Pmx_Datf
WORD Hookc_Pmx_Datf(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif)
{
   return (WORD) ~0;
}
#endif



/* ---------------------------------------------------------------
   Name: Hookc_Self_Test

   This function is called before the
   self test (with interrupts disabled).

   After the hook, the self test may be performed (see "Return"
   below), the interrupts are enabled, the keyboard and auxiliary
   device interfaces are disabled, and Gate A20 is forced high.

   Return: 0x55 to signify that the test passed.
           otherwise, return the data to be sent to the Host (This is
               normally 0 to signify the error and cannot be > 255).
               If 0x55 is not returned, the self test is skipped.
   --------------------------------------------------------------- */
WORD Hookc_Self_Test(void)
{
   return 0x55;
}

/* ---------------------------------------------------------------
   Name: Hookc_All_Others

   F7-FD "set keys" commands and other non-defined
   commands to be passed to external keyboard.

   Input: "data" is the data to be sent to kbd.
   --------------------------------------------------------------- */
void Hookc_All_Others(BYTE data)
{
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Cmd1

   Command Parser for 10 series commands.

   Input: "command_num" is the controller command.

   At this point:
      Cmd_Byte == 0
      Tmp_Load == 0

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Cmd1(BYTE command_num)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Dat1

   Command Parser for 10 series commands - data byte.

   Input: command_num is the controller command.
          "data" is the data for the command.

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Dat1(BYTE command_num, BYTE data)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Cmd4

   Command Parser for 40 series commands.

   Input: "command_num" is the controller command.

   At this point:
      Cmd_Byte == 0
      Tmp_Load == 0

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Cmd4(BYTE command_num)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Dat4

   Command Parser for 40 series commands - data byte.

   Input: command_num is the controller command.
          "data" is the data for the command.

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Dat4(BYTE command_num, BYTE data)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Cmd5

   Command Parser for 50 series commands.

   Input: "command_num" is the controller command.

   At this point:
      Cmd_Byte == 0
      Tmp_Load == 0

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Cmd5(BYTE command_num)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Pc_Dat5

   Command Parser for 50 series commands - data byte.

   Input: command_num is the controller command.
          "data" is the data for the command.

   Return: a value with all bits set if no data is to be sent to the Host;
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Pc_Dat5(BYTE command_num, BYTE data)
{
   return (WORD) ~0;
}


