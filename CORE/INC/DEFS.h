/*-----------------------------------------------------------------------------
 * MODULE DEFS.H
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
 * Revision History (Started June 7, 1996 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef DEFS_H
#define DEFS_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
/* TRUE can be defined as !FALSE but defining
   it as 1 allows it to fit into a bitfield. */
#define TRUE 1
#endif

#define MASK_bit0 (1 << 0)
#define MASK_bit1 (1 << 1)
#define MASK_bit2 (1 << 2)
#define MASK_bit3 (1 << 3)
#define MASK_bit4 (1 << 4)
#define MASK_bit5 (1 << 5)
#define MASK_bit6 (1 << 6)
#define MASK_bit7 (1 << 7)
#define CONCAT(a, b) a ## b
#define MASK(x) CONCAT(MASK_, x)

#endif

