/*-----------------------------------------------------------------------------
 * MODULE PS2.C
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
 * Revision History (Started July 23, 1996 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * 20 Jan 00   Changes for Active PS/2 Multiplexing (APM) support
 *             marked as V21.1.
 * EDH
 * 11 Dec 00 	Support the fourth PS/2 port as port 4. It will be
 * 				recognized as port 0 with Active PS/2 Multiplexing
 *					driver.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * ------------------------------------------------------------------------- */

#define PS2_C

#include <asm.h>

#include "types.h"
#include "regs.h"
#include "ps2.h"
#include "icu.h"
#include "purdat.h"
#include "proc.h"

#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif


/* When using the hardware shift mechanism mode the
   ports_enabled byte keeps track of the PS2 devices
   that have been activated. */
 BITS_8  ports_enabled;
 #define AUX_PORT1_ENABLED bit0
 #define AUX_PORT2_ENABLED bit1
 #define AUX_PORT3_ENABLED bit2

#if AUX_PORT3_SUPPORTED
 AUX_PORT * const aux_pntr_table[3] =
 {
    &Aux_Port1,
    &Aux_Port2,
    &Aux_Port3,
 };
#elif AUX_PORT2_SUPPORTED
 AUX_PORT * const aux_pntr_table[2] =
 {
    &Aux_Port1,
    &Aux_Port2,
 };
#else
 AUX_PORT * const aux_pntr_table[1] =
 {
    &Aux_Port1,
 };
#endif

/* ---------------------------------------------------------------
   Name: Ps2_Init

   Initialize PS2 registers in hardware shift mechanism
   mode or in legacy mode.  Put in receive mode.
   --------------------------------------------------------------- */
void Ps2_Init(BYTE pscon)
{
   /* Inhibit shift mechanism. */
   PSCON = 0;

	MULPX.byte = 0;

   /* Initialize PS2 registers in hardware shift
      mechanism mode.  Put in receive mode. */

   /* Raise all data bits, keep all clocks low to inhibit transactions. */
   SET_BIT(PSOSIG, PSOSIG_WDAT1 | PSOSIG_WDAT2 | PSOSIG_WDAT3);
   /* Setup PSCON with data from pscon.  It is assumed
      that PSCON_XMT and PSCON_EN bits in pscon are clear
      so that PSCON will be set to receive mode and disabled. */
   PSCON = pscon;

   /* Release SM reset (set PSCON.EN bit). */
   SET_BIT(PSCON, PSCON_EN);

   ports_enabled.byte = 0;
}

/* ---------------------------------------------------------------
   Name: Enable_Serial_Irq

   Enable interrupt request so that an interrupt will occur
   when data is being transferred to/from a serial device.

   Input: port is AUX_PORT1_CODE, AUX_PORT2_CODE or AUX_PORT3_CODE.
   --------------------------------------------------------------- */
void Enable_Serial_Irq(SMALL port)
{
   /* When using the hardware shift mechanism mode... */
   switch (port)
   {
      case AUX_PORT1_CODE:
         ports_enabled.bit.AUX_PORT1_ENABLED = 1;
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         ports_enabled.bit.AUX_PORT2_ENABLED = 1;
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         ports_enabled.bit.AUX_PORT3_ENABLED = 1;
         break;
      #endif
   }

   /* Set the interrupt mode.  Set
      Start of Transaction Interrupt Enable and
      End of Transaction Interrupt Enable. */
   SET_BIT(PSIEN, PSIEN_SOTIE | PSIEN_EOTIE);
}

/* ---------------------------------------------------------------
   Name: Disable_Serial_Irq

   Disable interrupt request so that an interrupt will not
   occur when data is being transferred to/from a serial device.

   Input: port is AUX_PORT1_CODE, AUX_PORT2_CODE or AUX_PORT3_CODE.
   --------------------------------------------------------------- */
void Disable_Serial_Irq(SMALL port)
{
   /* When using the hardware shift mechanism mode... */

   WORD psr_value;

   switch (port)
   {
      case AUX_PORT1_CODE:
         ports_enabled.bit.AUX_PORT1_ENABLED = 0;
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         ports_enabled.bit.AUX_PORT2_ENABLED = 0;
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         ports_enabled.bit.AUX_PORT3_ENABLED = 0;
         break;
      #endif
   }

   if (ports_enabled.bit.AUX_PORT1_ENABLED == 0 &&
       #if AUX_PORT3_SUPPORTED
       ports_enabled.bit.AUX_PORT3_ENABLED == 0 &&
       #endif
       ports_enabled.bit.AUX_PORT2_ENABLED == 0
      )
   {
      /* If no PS2 ports are enabled, clear
         Start of Transaction Interrupt Enable and
         End of Transaction Interrupt Enable. */

      _spr_("psr", psr_value);               /* Get state of global
                                                interrupt enable flag. */
      _di_();                                /* Disable interrupts before
                                                clearing bits. */
      CLEAR_BIT(PSIEN, PSIEN_SOTIE | PSIEN_EOTIE); /* Disable SOT and EOT ints. */
      if (psr_value & (1 << (PSR_E_BIT)))
         _ei_();                             /* Put interrupt state back. */
   }
}

/* ---------------------------------------------------------------
   Name: Serial_Irq_Is_Enabled

   Reads a serial device's interrupt enable status.

   Input: port is AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE.

   Returns: 1 if enabled, 0 if not.
   --------------------------------------------------------------- */
FLAG Serial_Irq_Is_Enabled(SMALL port)
{
   /* When using the hardware shift mechanism mode... */
   switch (port)
   {
      case AUX_PORT1_CODE:
         return (ports_enabled.bit.AUX_PORT1_ENABLED);
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         return (ports_enabled.bit.AUX_PORT2_ENABLED);
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         return (ports_enabled.bit.AUX_PORT3_ENABLED);
         break;
      #endif
   }
}

/* ---------------------------------------------------------------
   Name: Parity_Is_Ok

   Check if parity received from auxiliary
   keyboard or auxiliary device (mouse) is OK.

   Returns: 1 if no error, 0 if error.
   --------------------------------------------------------------- */
FLAG Parity_Is_Ok(void)
{
   /* When using the hardware shift mechanism mode,
      the parity bit contains the parity error flag. */
   return (~Aux_Status.Parity_Flag);
}

