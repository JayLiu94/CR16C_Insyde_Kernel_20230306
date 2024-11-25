/* ----------------------------------------------------------------------------
 * MODULE OEMCRSIS.C
 *
 * Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * This file contains code used to perform initialization
 * after a reset.
 *
 * Revision History (Started August 5, 1999 by GLP)
 *
 * GLP
 * 08/17/99 Added CRISIS_CMD_HANDLER switch.
 * 2005/12/26 remove Initialize the clock generator for Nuvoton EC
 * ------------------------------------------------------------------------- */

#define OEMCRSIS_C

#include <asm.h>

#include "swtchs.h"
#include "regs.h"
#include "ini.h"
#include "types.h"
#include "oemcrsis.h"
#include "oem.h"
#include "crisis.h"
#include "purcrver.h"

#ifndef INCLUDE_CRISIS
#error INCLUDE_CRISIS switch is not defined.
#endif

#ifndef CRISIS_CMD_HANDLER
#error CRISIS_CMD_HANDLER switch is not defined.
#endif

#if INCLUDE_CRISIS
/* ---------------------------------------------------------------
   Name: OEM_Crisis_Reset

   Control has been transferred from the RESET.S file.
   The stack pointers and interrupt base register have been set.
   --------------------------------------------------------------- */
void OEM_Crisis_Reset(void)
{
   /*
   Control is transferred to Crisis_Reset().  There is no return from
   Crisis_Reset() and there will be no return from OEM_Crisis_Reset().
   Any unused stack space will be taken care of at Crisis_Reset. */

   Crisis_Reset();
}

/* ---------------------------------------------------------------
   Name: core_vcb, oem_vcb

   Version data.

   Change OVER.H file to change the OEM version number.
   --------------------------------------------------------------- */

/* Version Control Block (VCB) to allow VCB.EXE utility to
   display Keyboard Controller's version from a binary file. */
static const BYTE core_vcb[] =
{
   ID1, ID2,
   CRISISPRODUCT, 0x00,
   CRISISMAJ, '.',
   CRISISMID_H, CRISISMID_L, '.',
   CRISISMIN_H, CRISISMIN_L, 0x00,
   CRISISDATE, 0x00,
   '0', '0', '0', '0', 0x00
};

/* Version Control Block (VCB) to allow VCB.EXE utility to
   display Keyboard Controller's version from a binary file. */
static const BYTE oem_vcb[] =
{
   ID1, ID2,
   CRISIS_OEMPRODUCT, 0x00,
   '0', '.', CRISIS_OEM_VER1, CRISIS_OEM_VER2, '.', '0', CRISIS_ENGNUM, 0x00,
   CRISIS_OEMDATE, 0x00,
   PROJECT_1, PROJECT_2,
   PROJECT_3, PROJECT_4, 0x00
};

/* ---------------------------------------------------------------
   Name: OEM_crisis_check

   This routine is called by the crisis code before checking the
   main keyboard controller code.  This allows the OEM to force
   the system to go into crisis mode regardless of the condition
   of the main keyboard controller code.

   Returns with:
      0 for normal code.
      non-zero to run the crisis code.
   --------------------------------------------------------------- */
WORD OEM_crisis_check(void)
{
   return(0);
}

/* ---------------------------------------------------------------
   Name: OEM_entering_crisis

   This routine is called by the crisis code after it has
   determined that the main keyboard controller code is bad (or
   because OEM_crisis_check has returned a non-zero value).
   --------------------------------------------------------------- */
void OEM_entering_crisis(void)
{
   #if !CRISIS_CMD_HANDLER
   /*
   If the crisis command handler is not included, do not return
   from this routine.  There is no command handler to return to.
   In fact, when the crisis command handler is not included, this
   routine in not called, it is jumped to. */

   while (1)
   {
      ;
   }
   #endif
}

/* ---------------------------------------------------------------
   Name: OEM_Hookc_Crisis_Init_Cmd_Hndlr

   Do any needed initializations just before entering the main
   crisis loop , which calls every 1ms the OEM_Hookc_Crisis_1ms()
   --------------------------------------------------------------- */
void OEM_Hookc_Crisis_Init_Cmd_Hndlr(void)
{

}

void OEM_Hookc_Handle_PM_Data(void)
{

}

/* ---------------------------------------------------------------
   Name: OEM_Crisis_1ms

   This routine is called evry 1ms from the crisis main loop .
   --------------------------------------------------------------- */
void OEM_Hookc_Crisis_1ms(void)
{

}

/* ---------------------------------------------------------------
   Name: OEM_Hookc_controller_cmd

   Processing for Keyboard Controller commands and associated
   data bytes.

   If the current byte is a command byte
      host_byte = the command byte
      bCrisis_Cmd_Num = the command byte
      bCrisis_Cmd_Index = 0

   If the current byte is a data byte
      host_byte = the data byte
      bCrisis_Cmd_Num = the previously stored command byte
      bCrisis_Cmd_Index = the data byte index

   Return
      0 - Command was not handled buy OEM code
      1 - Command was handled buy OEM code
   --------------------------------------------------------------- */
int OEM_Hookc_controller_cmd(BYTE host_byte)
{
    bool res = 0;
    return res;
}

#endif

/* ---------------------------------------------------------------
   Name: OEM_detect_crisis

   This routine is called by the Boot Code.  Return
   a non-zero to cause the crisis code to be run.

   The Boot Code is the code that runs in the ROM inside the
   PC87570.  From the Boot Code definition this routine must:

   1. Save RA register on entry and use its contents as a return address.
   2. Preserve all configuration registers xxCFG.
   3. Preserve Boot Program variables in RAM F000h - F0010h.
   4. Return with Interrupts disabled: PFAIL.EN=0, PSR.I=0.
   5. Return with PSR.Z = 1 if NO crisis detected.

   Returns with:
      0 and Z-flag set for normal code.
      non-zero and Z-flag clear to run the crisis code.
   --------------------------------------------------------------- */
WORD OEM_detect_crisis(void)
{

   /* This could be a "return (x)" instruction but because the
      Z-flag must be set or cleared, the C instruction is split
      into its two assembly instructions.  The return value is
      put into r0 and the Z-flag is set or cleared.  The "jump ra"
      instruction will be added by the C compiler. */

   __asm__("movw $0:s,r0");
   __asm__("cmpw $0:s,r0");
}
