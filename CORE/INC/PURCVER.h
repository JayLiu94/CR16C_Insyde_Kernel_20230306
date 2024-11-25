/* ----------------------------------------------------------------------------
 * MODULE PURCVER.H
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
 * This is the core version number (uses Translated key codes).
 *
 * 01 Jun 98   Added definitions for Version Control Block (VCB)
 *             to allow VCB.EXE utility to display Keyboard
 *             Controller's version from a binary file.
 * 12 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#include "over.h"

/* Major, middle, and minor portions of the core version. */
#define COREMAJ   '4'
/*                '.' */
#define COREMID_H '0'
#define COREMID_L '0'
/*                '.' */
#define COREMIN_H '0'
#define COREMIN_L '8'

/* Month, day, and year of the core version. */
#define COREDATE '1', '0', '/', '1', '4', '/', '0', '6'

#define PRODUCT 'K', 'B', 'C', ' ', 'C', 'O', 'R', 'E'

/* --------------------------------------------------------------- */

#define ID1 0xDD
#define ID2 0x88

/* Keyboard Controller firmware version (uses Translated key codes).

1-02, 2-03, 3-04, 4-05  5-06, 6-07, 7-08, 8-09, 9-0A, 0-0B

.-34, [-1A, ]-1B, --0C

A letter may be placed at the end of the OEM version during
development.  The letter is to be removed when a release
is made to the customer or to the Version Control System.
a-1E, b-30, c-2E, d-20, e-12, f-21, g-22, h-23, i-17, j-24,
k-25, l-26, m-32, n-31, o-18, p-19, q-10, r-13, s-1F, t-14,
u-16, v-2F, w-11, x-2D, y-15, z-2C */

#if   (TEMP_LETTER=='A')||(TEMP_LETTER=='a')
 #define TMP_LET  0x1E,
#elif (TEMP_LETTER=='B')||(TEMP_LETTER=='b')
 #define TMP_LET  0x30,
#elif (TEMP_LETTER=='C')||(TEMP_LETTER=='c')
 #define TMP_LET  0x2E,
#elif (TEMP_LETTER=='D')||(TEMP_LETTER=='d')
 #define TMP_LET  0x20,
#elif (TEMP_LETTER=='E')||(TEMP_LETTER=='e')
 #define TMP_LET  0x12,
#elif (TEMP_LETTER=='F')||(TEMP_LETTER=='f')
 #define TMP_LET  0x21,
#elif (TEMP_LETTER=='G')||(TEMP_LETTER=='g')
 #define TMP_LET  0x22,
#elif (TEMP_LETTER=='H')||(TEMP_LETTER=='h')
 #define TMP_LET  0x23,
#elif (TEMP_LETTER=='I')||(TEMP_LETTER=='i')
 #define TMP_LET  0x17,
#elif (TEMP_LETTER=='J')||(TEMP_LETTER=='j')
 #define TMP_LET  0x24,
#elif (TEMP_LETTER=='K')||(TEMP_LETTER=='k')
 #define TMP_LET  0x25,
#elif (TEMP_LETTER=='L')||(TEMP_LETTER=='l')
 #define TMP_LET  0x26,
#elif (TEMP_LETTER=='M')||(TEMP_LETTER=='m')
 #define TMP_LET  0x32,
#elif (TEMP_LETTER=='N')||(TEMP_LETTER=='n')
 #define TMP_LET  0x31,
#elif (TEMP_LETTER=='O')||(TEMP_LETTER=='o')
 #define TMP_LET  0x18,
#elif (TEMP_LETTER=='P')||(TEMP_LETTER=='p')
 #define TMP_LET  0x19,
#elif (TEMP_LETTER=='Q')||(TEMP_LETTER=='q')
 #define TMP_LET  0x10,
#elif (TEMP_LETTER=='R')||(TEMP_LETTER=='r')
 #define TMP_LET  0x13,
#elif (TEMP_LETTER=='S')||(TEMP_LETTER=='s')
 #define TMP_LET  0x1F,
#elif (TEMP_LETTER=='T')||(TEMP_LETTER=='t')
 #define TMP_LET  0x14,
#elif (TEMP_LETTER=='U')||(TEMP_LETTER=='u')
 #define TMP_LET  0x16,
#elif (TEMP_LETTER=='V')||(TEMP_LETTER=='v')
 #define TMP_LET  0x2F,
#elif (TEMP_LETTER=='W')||(TEMP_LETTER=='w')
 #define TMP_LET  0x11,
#elif (TEMP_LETTER=='X')||(TEMP_LETTER=='x')
 #define TMP_LET  0x2D,
#elif (TEMP_LETTER=='Y')||(TEMP_LETTER=='y')
 #define TMP_LET  0x15,
#elif (TEMP_LETTER=='Z')||(TEMP_LETTER=='z')
 #define TMP_LET  0x2C,
#else
 #define TMP_LET
#endif

#define TRANS(x) x=='0'?0x0B:x-'1'+2

