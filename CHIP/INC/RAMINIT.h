/*-----------------------------------------------------------------------------
 * MODULE RAMINIT.H
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
 * Definitions for the variables in the RAM initialization table.
 *
 * Revision History (Started August 27, 1998 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "types.h"

/* ------------------------- Ext_Cb3 -------------------------
	Extended scanner control.
	Accessed through Commands 97h and 98h.

	DISABLE_NKEY   1 = Simple Matrix, 0 = Nkey.
	FN_CANCEL      1 = Any shift key clears Fn state.
	NumLockC_OVR   1 = Overlay controlled by NumLock.
	DIAG_MODE      1 = Row/Col output, 0 = scan output. */
#define Ext_Cb3_Init 0x31

/* ------------------------- Ext_Cb2 -------------------------
   Extended debounce control.
   Accessed through Command 99h.

   unused         Bit  0
   Break_Count    Bits 1 through 3
   unused         Bit  4
   Make_Count     Bits 5 through 7 */
#define Ext_Cb2_Init 0x66

/* --------------------- Save_Typematic ---------------------- */
#define Save_Typematic_Init  0x2A
#define Save_Led_State_Init  0  /* The maskLEDS bits are used and are
                           expected to be initialized to 0. */

#define Wakeup1_Init     0x00        /* Irq wakeup mask 1. */
#define Wakeup2_Init     0x00        /* Irq wakeup mask 2. */
#define Wakeup3_Init     0x40        /* Irq wakeup mask 3. */
#define Wakeup4_Init     0x00
#define Wakeup5_Init     0x00

#define Mouse_State0_Init    0
#define Mouse_State1_Init    2
#define Mouse_State2_Init    100

#endif /* ifndef RAMINIT_H */
