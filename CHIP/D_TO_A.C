/*-----------------------------------------------------------------------------
 * MODULE D_TO_A.C
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
 * This file contains code to handle the D/A converter.
 *
 * Revision History (Started February 10, 1998 by GLP)
 *
 * 02 Jul 98   Added code to set D/A output to 0 before enabling
 *             (per Nuvoton's instructions).
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#define D_TO_A_C

#include "d_to_a.h"
#include "purdat.h"

/* ---------------------------------------------------------------
   Name: Write_DtoA

   Write data to D/A converter.

   Input: First byte = converter channel.
          Second byte = data for converter.

   Returns: a value with all bits set.
   --------------------------------------------------------------- */
WORD Write_DtoA(BYTE command_num, BYTE data)
{
   return((WORD) ~0);/* Return value: "nothing to send". */
}
