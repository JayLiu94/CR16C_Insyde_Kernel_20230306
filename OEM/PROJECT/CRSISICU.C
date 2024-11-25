/* ----------------------------------------------------------------------------
 * MODULE CRSISICU.C
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
 * Interrupt Control Unit (ICU) for the crisis module.  This file
 * contains the interrupt dispatch (or interrupt vector) table.
 *
 * Revision History (Started August 6, 1999 by GLP)
 *
 * ------------------------------------------------------------------------- */

#define CRSISICU_C

#include "icu.h"
#include "host_if.h"

/* ----------------------------------------------------------------------------
   Name: Irq_Nmi

   Non-Maskable interrupt handler.
   ------------------------------------------------------------------------- */
void Irq_Nmi(void)
{
    ;
}

/* ---------------------------------------------------------------
   Name: _dispatch_table[]

   This is the interrupt dispatch table.  This table contains
   pointers to handlers for all the traps and interrupts, including
   debug specific interrupts.  These handlers may be changed or
   even replaced with null handlers, but if you want debugging
   support be sure to retain the call to _tmon() in the Nuvoton handlers.
   See Nuvoton's handlers' specific files for more information.  If you
   want to retain the ability to debug your production code to some extent,
   be sure to retain Nuvoton's ISE, TRC and BPT handlers in your production
   code.  The code size cost is 70 bytes but you gain an ability to partialy
   debug production code.
   --------------------------------------------------------------- */
void (*const _dispatch_table[])(void) =
{
   0,             /* Reserved. */
   Irq_Nmi,       /* Non-Maskable interrupt handler. */
   0,             /* Reserved. */
   0,             /* Reserved. */
   0,             /* Reserved. */
   0,//Svc,           /* Supervisor call trap handler. */
   0,//Dvz,           /* Divide by zero trap handler. */
   0,//Flg,           /* Flag trap handler. */
   0,//Bpt,           /* Breakpoint trap handler. */
   0,//Trc,           /* Trace trap handler. */
   0,//Und,           /* Undefined instruction trap. */
   0,             /* Reserved. */
   0,             /* Reserved. */
   0,             /* Reserved. */
   0,             /* Reserved. */
   0,//Ise,           /* In-System emulator interrupt. */

   0,             /* Interrupt 16 (0x10) through... */
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,             /* interrupt 31 (0x1F) are not available. */

   0,             /* Interrupt 32 (0x20). */
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0              /* Interrupt 47 (0x2F). */
};
