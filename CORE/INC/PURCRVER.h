/*-----------------------------------------------------------------------------
 * MODULE PURCVER.H
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
 * This is the core crisis version number (uses Translated key codes).
 *
 * Revision History (Started August 6, 1999 by GLP)
 *
 * ------------------------------------------------------------------------- */

#include "over.h"

/* Major, middle, and minor portions of the core crisis version. */
#define CRISISMAJ   '4'
/*                '.' */
#define CRISISMID_H '0'
#define CRISISMID_L '0'
/*                '.' */
#define CRISISMIN_H '0'
#define CRISISMIN_L '4'

/* Month, day, and year of the core crisis version. */
#define CRISISDATE '0', '7', '/', '1', '3', '/', '0', '6'

#define CRISISPRODUCT 'K', 'B', 'C', 'R', 'I', 'S', 'I', 'S'

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

#if   (CRISIS_TEMP_LETTER=='A')||(CRISIS_TEMP_LETTER=='a')
 #define CRISIS_TMP_LET  0x1E,
#elif (CRISIS_TEMP_LETTER=='B')||(CRISIS_TEMP_LETTER=='b')
 #define CRISIS_TMP_LET  0x30,
#elif (CRISIS_TEMP_LETTER=='C')||(CRISIS_TEMP_LETTER=='c')
 #define CRISIS_TMP_LET  0x2E,
#elif (CRISIS_TEMP_LETTER=='D')||(CRISIS_TEMP_LETTER=='d')
 #define CRISIS_TMP_LET  0x20,
#elif (CRISIS_TEMP_LETTER=='E')||(CRISIS_TEMP_LETTER=='e')
 #define CRISIS_TMP_LET  0x12,
#elif (CRISIS_TEMP_LETTER=='F')||(CRISIS_TEMP_LETTER=='f')
 #define CRISIS_TMP_LET  0x21,
#elif (CRISIS_TEMP_LETTER=='G')||(CRISIS_TEMP_LETTER=='g')
 #define CRISIS_TMP_LET  0x22,
#elif (CRISIS_TEMP_LETTER=='H')||(CRISIS_TEMP_LETTER=='h')
 #define CRISIS_TMP_LET  0x23,
#elif (CRISIS_TEMP_LETTER=='I')||(CRISIS_TEMP_LETTER=='i')
 #define CRISIS_TMP_LET  0x17,
#elif (CRISIS_TEMP_LETTER=='J')||(CRISIS_TEMP_LETTER=='j')
 #define CRISIS_TMP_LET  0x24,
#elif (CRISIS_TEMP_LETTER=='K')||(CRISIS_TEMP_LETTER=='k')
 #define CRISIS_TMP_LET  0x25,
#elif (CRISIS_TEMP_LETTER=='L')||(CRISIS_TEMP_LETTER=='l')
 #define CRISIS_TMP_LET  0x26,
#elif (CRISIS_TEMP_LETTER=='M')||(CRISIS_TEMP_LETTER=='m')
 #define CRISIS_TMP_LET  0x32,
#elif (CRISIS_TEMP_LETTER=='N')||(CRISIS_TEMP_LETTER=='n')
 #define CRISIS_TMP_LET  0x31,
#elif (CRISIS_TEMP_LETTER=='O')||(CRISIS_TEMP_LETTER=='o')
 #define CRISIS_TMP_LET  0x18,
#elif (CRISIS_TEMP_LETTER=='P')||(CRISIS_TEMP_LETTER=='p')
 #define CRISIS_TMP_LET  0x19,
#elif (CRISIS_TEMP_LETTER=='Q')||(CRISIS_TEMP_LETTER=='q')
 #define CRISIS_TMP_LET  0x10,
#elif (CRISIS_TEMP_LETTER=='R')||(CRISIS_TEMP_LETTER=='r')
 #define CRISIS_TMP_LET  0x13,
#elif (CRISIS_TEMP_LETTER=='S')||(CRISIS_TEMP_LETTER=='s')
 #define CRISIS_TMP_LET  0x1F,
#elif (CRISIS_TEMP_LETTER=='T')||(CRISIS_TEMP_LETTER=='t')
 #define CRISIS_TMP_LET  0x14,
#elif (CRISIS_TEMP_LETTER=='U')||(CRISIS_TEMP_LETTER=='u')
 #define CRISIS_TMP_LET  0x16,
#elif (CRISIS_TEMP_LETTER=='V')||(CRISIS_TEMP_LETTER=='v')
 #define CRISIS_TMP_LET  0x2F,
#elif (CRISIS_TEMP_LETTER=='W')||(CRISIS_TEMP_LETTER=='w')
 #define CRISIS_TMP_LET  0x11,
#elif (CRISIS_TEMP_LETTER=='X')||(CRISIS_TEMP_LETTER=='x')
 #define CRISIS_TMP_LET  0x2D,
#elif (CRISIS_TEMP_LETTER=='Y')||(CRISIS_TEMP_LETTER=='y')
 #define CRISIS_TMP_LET  0x15,
#elif (CRISIS_TEMP_LETTER=='Z')||(CRISIS_TEMP_LETTER=='z')
 #define CRISIS_TMP_LET  0x2C,
#else
 #define CRISIS_TMP_LET
#endif
 
#define TRANS(x) x=='0'?0x0B:x-'1'+2
