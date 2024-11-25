/* ----------------------------------------------------------------------------
 * MODULE PURSCN.H
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
 * Revision History (Started June 14, 1996 by GLP)
 *
 * GLP
 * 17 Mar 98   Added prototype for Get_Buffer.
 * 23 Mar 98   Added comment about interrupts for Unlock_Scan.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 *
 * EDH
 * 12 May 15   Moved Set_Overlay_Mask from PURCMD.C
 *
 * ------------------------------------------------------------------------- */

#ifndef PURSCN_H
#define PURSCN_H

#include "types.h"
#include "purdat.h"

/* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
#if MATRIX_18
union KEY
{
   WORD word;

   struct
   {
      #if BIT0_FIRST
      FIELD input:            4; /* Bit 0 - 3. Bit address of the KI bit. */
      FIELD debounce_status:  1; /* Bit 4.     Debounce status. */
      FIELD output:           5; /* Bit 5 - 9. Bit address of the KO bit. */
      #else
      BIT_PADDING
      FIELD output:           5; /* Bit 9 - 5. Bit address of the KO bit. */
      FIELD debounce_status:  1; /* Bit 4.     Debounce status. */
      FIELD input:            4; /* Bit 3 - 0. Bit address of the KI bit. */
      #endif
   } field;
};
#else
union KEY
{
   BYTE byte;

   struct
   {
      #if BIT0_FIRST
      FIELD input:            3; /* Bit 0 - 2. Bit address of the KI bit. */
      FIELD debounce_status:  1; /* Bit 3.     Debounce status. */
      FIELD output:           4; /* Bit 4 - 7. Bit address of the KO bit. */
      #else
      BIT_PADDING
      FIELD output:           4; /* Bit 7 - 4. Bit address of the KO bit. */
      FIELD debounce_status:  1; /* Bit 3.     Debounce status. */
      FIELD input:            3; /* Bit 2 - 0. Bit address of the KI bit. */
      #endif
   } field;
};
#endif
/* >>> V5.1 2001/2/5 Modified. */
void Lock_Scan(void);
void Start_Scan_Transmission(void);
FLAG Check_Scan_Transmission(void);
void Init_Kbd(void);
void Setup_Diode_Key(void);
void Clear_Key(void);
void Clear_Typematic(void);
void Set_Typematic(WORD type_rate);
void Handle_Scan(void);
WORD Get_Buffer(void);
void Buffer_Mark(void);
FLAG Buffer_Key(BYTE row_column);
SMALL Buffer_String(const BYTE *pntr);
BITS_8 Set_Overlay_Mask(BITS_8 data, FLAG bit_state);

/* Pointer to current scanner tables. */
extern const BYTE *Scanner_Table_Pntr;
extern BYTE bfn_matrix[];
/* ---------------------------------------------------------------
   Name: Unlock_Scan

   Doesn't do much, just clears a bit to re-enable scanner.  Using
   a function allows the unlock of the scanner to be monitored
   easier than if it was done with "inline" code. That's
   the only reason a function is used to do such a simple task.

   Enter with interrupts disabled.

   C prototype:
      void Unlock_Scan(void);
   --------------------------------------------------------------- */
#define Unlock_Scan() Int_Var.Scan_Lock = 0

/* ---------------------------------------------------------------
   Name: Int_Lock_Scan

   Lock out the scanner via "Scan_Lock"
   and clear pending scanner "SEND" events.

   The Int_ prefix signifies that this routine is called
   from an interrupt routine.  The interrupts are disabled.
   --------------------------------------------------------------- */
#define Int_Lock_Scan() Int_Var.Scan_Lock = 1; Timer_B.bit.SEND_ENABLE = 0

#if MATRIX_18
#define MAX_SCAN_LINES 18
#else
#define MAX_SCAN_LINES 16
#endif

#endif /* ifndef PURSCN_H */

