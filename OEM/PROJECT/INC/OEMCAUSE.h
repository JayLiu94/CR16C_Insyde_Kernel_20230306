/* ----------------------------------------------------------------------------
 * MODULE OEMCAUSE.H
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
 * Contains SCI and SMI cause numbers.
 *
 * Revision History (Started June 29, 1998 by GLP)
 *
 * GLP
 * 05 Oct 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef OEMCAUSE_H
#define OEMCAUSE_H

#include "purconst.h"

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

/* An array of bytes holds the flags that are used to keep track
   of SCI and SMI causes.  The array holds the CORE and OEM
   cause flags.  Each byte in the array can hold eight flags. */

#if !ACPI_SUPPORTED
#define NUM_SCI_FLAGS_BYTES 0
#else
/* ----------------------- SCI Causes ------------------------
   Define the OEM SCI cause numbers here.
   Update LAST_SCI_CAUSE when a cause is added.

   The cause numbers run from 1 through (NUM_SCI_FLAGS_BYTES * 8).
   The OEM is allowed to use SCI causes numbered starting at
   FIRST_OEM_SCI_CAUSE. */

/* Bit 0 of FIRST_BITS_INDEX + 1 changed. */
#define FIRST_BITS_CAUSE   (FIRST_OEM_SCI_CAUSE+0)
/* . */
/* . */
/* . */
/* Bit 7 of FIRST_BITS_INDEX + 1 changed. */
#define LAST_BITS_CAUSE    (FIRST_OEM_SCI_CAUSE+7)

#define STANDBY            (FIRST_OEM_SCI_CAUSE+8)
#define PWR_SWITCH         (FIRST_OEM_SCI_CAUSE+9)
#define SUSPEND_2_RAM      (FIRST_OEM_SCI_CAUSE+10)
#define SUSPEND_2_DISK     (FIRST_OEM_SCI_CAUSE+11)
/* define                  (FIRST_OEM_SCI_CAUSE+12) */
/* define                  (FIRST_OEM_SCI_CAUSE+13) */
/* define                  (FIRST_OEM_SCI_CAUSE+14) */
/* define                  (FIRST_OEM_SCI_CAUSE+15) */

 #define LAST_SCI_CAUSE    (FIRST_OEM_SCI_CAUSE+15)

   /* The following is the number of bytes used by the SCI cause flags. */
   #define NUM_SCI_FLAGS_BYTES (((LAST_SCI_CAUSE-1)>>3)+1)

#endif /* if ACPI_SUPPORTED */

/* ----------------------- SMI Causes ------------------------
   Define the SMI cause numbers here.
   Update LAST_SMI_CAUSE when a cause is added.

   The SMI cause numbers run from FIRST_SMI_CAUSE through
   FIRST_SMI_CAUSE + (NUM_SMI_FLAG_BYTES * 8) - 1.

   The OEM can set the FIRST_SMI_CAUSE to any number from 1
   to 255.  The normal case is to set it to one more than the
   last available SCI number.  This is done so that there will
   be no overlapping SCI and SMI cause numbers and is handled
   automatically here. */
   #define FIRST_SMI_CAUSE ((NUM_SCI_FLAGS_BYTES * 8) + 1)

#define VIDEO_TOGGLE       (FIRST_SMI_CAUSE+0)
#define AUDIO_UP           (FIRST_SMI_CAUSE+1)
#define AUDIO_DOWN         (FIRST_SMI_CAUSE+2)
#define AUDIO_MUTE         (FIRST_SMI_CAUSE+3)
#define BATT_ALARM         (FIRST_SMI_CAUSE+4)
#define LEGACY_SMB_DONE    (FIRST_SMI_CAUSE+5)
#define LEGACY_PWR_CHNG    (FIRST_SMI_CAUSE+6)

#define LAST_SMI_CAUSE     (FIRST_SMI_CAUSE+6)

   /* The following is the number of bytes used by the SMI cause flags. */
   #define NUM_SMI_FLAGS_BYTES (((LAST_SMI_CAUSE-FIRST_SMI_CAUSE)>>3)+1)

   /* The following is the size of the cause flags array. */
   #if (NUM_SCI_FLAGS_BYTES+NUM_SMI_FLAGS_BYTES)==0
    /* If no cause flags are used, set array size
       to 1 to allow code to compile correctly. */
    #define CAUSE_ARRAY_SIZE 1
   #else
    #define CAUSE_ARRAY_SIZE (NUM_SCI_FLAGS_BYTES+NUM_SMI_FLAGS_BYTES)
   #endif

/* SCIs and SMIs can be generated with certain commands and
   downloaded hotkeys.  The cause values for these are held
   in SMI_Event_Buffer and SCI_Event_Buffer.  NUM_SCI_SMI_EVENTS
   is used to help define the size of these buffers. */
#define NUM_SCI_SMI_EVENTS 3

#endif /* ifndef OEMCAUSE_H */
