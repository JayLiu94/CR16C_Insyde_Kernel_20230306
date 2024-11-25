/* ----------------------------------------------------------------------------
 * MODULE PURFUNCT.H
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started June 17, 1998 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * 23 Aug 99   Added Y_OEM_FUNCT.
 * ------------------------------------------------------------------------- */

#ifndef PURFUNCT_H
#define PURFUNCT_H

#include "types.h"

void System_Control_Function(WORD data, SMALL event);
void Sys_Funct_Init(void);
void Handle_Sys_Funct(void);
void Gen_SMI(void);

/* ------------------ Control Function Word ------------------
   The Control Function Word is setup as follows:

      0xxy,yyyy,zzzz,zzzz

   The following are definitions for y,yyyy. */

#define Y_NOP            0x00  /* NOP                                */
#define Y_SET_HIGH       0x01  /* Set pin high                       */
#define Y_SET_LOW        0x02  /* Set pin low                        */
#define Y_PULSE_HIGH     0x03  /* Pulse pin high                     */
#define Y_PULSE_LOW      0x04  /* Pulse pin low                      */
#define Y_TOGGLE         0x05  /* Toggle pin                         */
#define Y_CON_FLASHER    0x06  /* Connect pin to Flasher             */
#define Y_DISCON_FLASHER 0x07  /* Disconnect pin from Flasher        */
#define Y_INT_CONTROL    0x08  /* Internal control events            */
#define Y_INT_SEL_CHG    0x09  /* Internal Selector & Charger events */
#define Y_GEN_SCAN_CODE  0x0A  /* Generate scan code as if from kbd  */
#define Y_ENT_CHAIN      0x0B  /* Enter control function chain       */
#define Y_HOST_TIMEOUT   0x0C  /* Wait for Host command timeout      */
#define Y_OEM_FUNCT      0x10  /* Calls Hookc_OEM_Sys_Ctrl_Funct     */

   /*
   y,yyyy set to 1,0tuv is for SMI/SCI generation (when tuv != 0).

   t: 1 = Generate SMI in Legacy mode with query value = ZZh.
   u: 1 = Generate SMI in ACPI mode   with query value = ZZh.
   v: 1 = Generate SCI in ACPI mode   with query value = ZZh. */

#define Y_SMI_SCI        0x10
#define Y_SMI_LEGACY_BIT bit2
#define Y_SMI_ACPI_BIT   bit1
#define Y_SCI_ACPI_BIT   bit0

/* ---------------------- Timed_Function ---------------------
   System Control Function for timed functions.
   Setup with Command 9Ah. */
typedef struct _TIMED_FUNCT
{
   BYTE timer;
   WORD operation;
} TIMED_FUNCT;
extern TIMED_FUNCT Timed_Function[];



#endif /* ifndef PURFUNCT_H */
