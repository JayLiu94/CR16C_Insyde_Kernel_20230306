/* ----------------------------------------------------------------------------
 * MODULE OEM_DATA.H
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
 * Contains Data and signal definitions for OEM.
 *
 * Revision History (Started June 29, 1998 by GLP)
 *
 * GLP
 * 05 Oct 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef OEM_DATA_H
#define OEM_DATA_H

#include "swtchs.h"
#include "purconst.h"
#include "defs.h"

/* The EC_Space array is used to hold the Embedded Controller
   (EC) data.  The first offset available to the OEM is
   OEM_EC_START.  This makes the first byte in EC space
   for the OEM EC_Space[OEM_EC_START].

   LAST_OEM_EC_INDEX must be set to the last address used by the OEM.
   Set it to OEM_EC_START-1 if no OEM EC space is needed. */


/* Set LAST_OEM_EC_INDEX to OEM_EC_START-1 if no OEM EC space is needed. */
 #define LAST_OEM_EC_INDEX  (OEM_EC_START-1)

#endif /* ifndef OEM_DATA_H */
