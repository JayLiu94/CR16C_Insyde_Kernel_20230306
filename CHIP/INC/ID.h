/*-----------------------------------------------------------------------------
 * MODULE ID.H
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
 * This is the manufacturer identification (uses Translated key codes)
 * and the controller type.
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * 10 Sep 99   Added CONTROLLER_TYPE_HIGH and CONTROLLER_TYPE_LOW.
 * ------------------------------------------------------------------------- */

#ifndef ID_H
#define ID_H

#define ID_MARK 0x31
/*                 n <- ID mark.

   a-1E, b-30, c-2E, d-20, e-12, f-21, g-22, h-23, i-17, j-24,
   k-25, l-26, m-32, n-31, o-18, p-19, q-10, r-13, s-1F, t-14,
   u-16, v-2F, w-11, x-2D, y-15, z-2C
*/

/* The controller type for the PC87570 */
                                 /* Configuration Table Offset */
                                 /* -------------------------- */
#define CONTROLLER_TYPE_FFF0 6   /*           0xFFF0           */
#define CONTROLLER_TYPE_FFF1 0   /*           0xFFF1           */

#endif

