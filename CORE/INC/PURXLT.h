/* ----------------------------------------------------------------------------
 * MODULE PURXLT.H
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
 * Contains prototypes for NATXLT.C
 *
 * Revision History (Started May 8, 1995 by GLP)
 *
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef PURXLT_H
#define PURXLT_H

#include "types.h"
#include "purscn.h"

void Send_Scan2(union KEY key, SMALL event);
void Send_Key(BYTE table_entry, SMALL event);
void Clear_Fn_Keys(void);

#define MAKE_EVENT   0
#define BREAK_EVENT  1
#define REPEAT_EVENT 2

/* ---------------------------------------------------------------
   Internal keyboard (scanner) tables.

   A scanner table consists of four parts:
      The Row/Column Table takes up 128 bytes.
      The Combination Table takes up 16 bytes.
      The Custom Key Table is a variable size up to 128 bytes.
      The internal keyboard type and number of external keyboard
          hotkeys takes up 1 byte.

   The tables can be in RAM or ROM.
   They can be added to the ROM as a binary module.

   When they are in ROM, the tables are proceeded by one byte.
   This byte has the size of the Custom Key Table.  This allows
   the internal keyboard type and hotkey definition byte and
   the second scanner table to be found.

      Start of tables:

         1 byte, sizeof(Custom_Table_1)
      Scanner Table 1
         128 bytes for Row/Column Table 1
         16 bytes for Combination Table 1
         bytes for Custom_Table_1
         1 byte, internal keyboard type and
                 number of external keyboard hotkeys

         1 byte, sizeof(Custom_Table_2)
      Scanner Table 2
         128 bytes for Row/Column Table 2
         16 bytes for Combination Table 2
         bytes for Custom_Table_2
         1 byte, internal keyboard type and
                 number of external keyboard hotkeys

      Scanner Table 2 is located at
         Start of tables + 1 + 128 + 16 + sizeof(Custom_Table_1) + 1 + 1

   Word entries in the Custom Key Table are stored in big-endian order.
   Most entries in this table are words.  There is an exception for
   strings stored in the table but only an even number of bytes can
   be stored (or the final byte can be ignored).  The high byte of
   the word is the first byte.  The low byte of the word is next.
   --------------------------------------------------------------- */

/* Table sizes. */

/* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
#if MATRIX_18
#define RC_TABLE_SIZE         144
#define COMB_TABLE_SIZE        18
#else
#define RC_TABLE_SIZE         128
#define COMB_TABLE_SIZE        16
#endif
/* >>> V5.1 2001/2/5 Modified. */

#define MAX_CUSTOM_TABLE_SIZE 128

/* Start of the different tables within the scanner table. */
#define RC_TABLE_INDEX         0
#define COMB_TABLE_INDEX       (RC_TABLE_INDEX + RC_TABLE_SIZE)
#define CUSTOM_KEY_TABLE_INDEX (COMB_TABLE_INDEX + COMB_TABLE_SIZE)
extern const BYTE Rc_ROM_Tables[];



#endif /* ifndef PURXLT_H */
