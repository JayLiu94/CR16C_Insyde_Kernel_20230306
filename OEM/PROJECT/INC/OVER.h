/* ----------------------------------------------------------------------------
 * MODULE OVER.H
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
 * This is the OEM version number (uses Translated key codes).
 *
 * 29 Jun 98   Added definitions for Version Control Block (VCB)
 *             to allow VCB.EXE utility to display Keyboard
 *             Controller's version from a binary file.
 * 06 Aug 99   Added definitions for crisis module and updated
 *             copyright.
 * ------------------------------------------------------------------------- */

/* Change OEM_VER1, OEM_VER2, and (optionally) OEMDATE for different versions. */

/* A letter may be placed at the end of the OEM version during
   development.  The letter is to be removed when a release
   is made to the customer or to the Version Control System. */

/*                '[' */
#define PROJECT_1 '2'
#define PROJECT_2 '0'
#define PROJECT_3 '1'
#define PROJECT_4 '2'
/*                '-' */
#define OEM_VER1  '0'
#define OEM_VER2  '0'
/*                ']' */

#define OEMDATE '0', '3', '/', '1', '2', '/', '1', '8'

#define OEMPRODUCT 'N', 'P', 'C', 'E', '3', '8', '5', 'x'

/* --- The following is for the crisis module. --- */

/* Change CRISIS_OEM_VER1, CRISIS_OEM_VER2, and (optionally)
   CRISIS_OEMDATE for different versions. */

/* A letter may be placed at the end of the OEM version during
   development.  The letter is to be removed when a release
   is made to the customer or to the Version Control System. */

/*                       '['
                  PROJECT_1      The crisis
                  PROJECT_2      module uses the
                  PROJECT_3      project number
                  PROJECT_4      defined above.
                         '-' */
#define CRISIS_OEM_VER1  '0'
#define CRISIS_OEM_VER2  '3'
/*                       ']' */

#define CRISIS_OEMDATE '0', '3', '/', '0', '1', '/', '1', '3'

#define CRISIS_OEMPRODUCT 'K', 'B', 'C', 'R', '_', 'O', 'E', 'M'

/* --------- */

#ifdef TEMP_LETTER
 #define ENGNUM  TEMP_LETTER
#else
 #define ENGNUM  '1'
#endif

#ifdef CRISIS_TEMP_LETTER
 #define CRISIS_ENGNUM  CRISIS_TEMP_LETTER
#else
 #define CRISIS_ENGNUM  '1'
#endif
