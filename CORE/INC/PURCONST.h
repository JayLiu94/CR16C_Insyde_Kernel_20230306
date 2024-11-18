/*-----------------------------------------------------------------------------
 * MODULE PURCONST.H
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
 * Constants.
 *
 * Revision History (Started August 14, 1996 by GLP)
 *
 * GLP
 * 22 Nov 96   Added support for I2C bus (ACCESS.bus).
 * 06 May 97   Added ACPI definitions.
 * 25 Feb 98   Added SCI and SMI definitions.
 * 06 May 98   Changed TMP_SIZE from 35 to 3.
 * 15 May 98   Added to EC_Space.
 * 29 Jul 98   Moved RC_TABLE_SIZE to the PURXLT.H file.
 * 19 Aug 99   Updated copyright.
 * 20 Jan 00   Support for Active PS/2 Multiplexing (APM) is
 *             marked as V21.1.
 * ------------------------------------------------------------------------- */

#ifndef PURCONST_H
#define PURCONST_H

#include "swtchs.h"

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

/* Temporary buffer (Tmp_Byte[]) size. */
#define TMP_SIZE 4

/* For ACPI. */

   /* ------------------------- SMB_xxx -------------------------
      SMBus Variables for ACPI. */

   /* This is the array for ACPI SMBus transactions.  It is
      defined as the start of Embedded Controller (EC) space. */

#if I2C_SUPPORTED
#if ACPI_SUPPORTED
 #define SMB_ACPI EC_Space
#endif
#endif

#define ACPI_SMB_BASE    0x20

   /*
	The following are offsets into the SMB_ACPI array.  They
	are to be kept in order so that the Read and Write Embedded
	Controller commands (80h/81h) can access the correct locations. */

#define SMB_PRTCL_INDX     0  /* Protocol Register.            */
#define SMB_STS_INDX       1  /* Status register.              */
#define SMB_ADDR_INDX      2  /* Address register.             */
#define SMB_CMD_INDX       3  /* Command register.             */
#define SMB_DATA_INDX      4  /* Data register 0 through 31.   */
#define SMB_BCNT_INDX      36 /* Block Count register.         */
#define SMB_ALRM_ADDR_INDX 37 /* Alarm address.                */
#define SMB_ALRM_DATA_INDX 38 /* Alarm data register 0 and 1.  */

#define SMB_BCNT_MAX 32 /* Maximum number of bytes in SMB_DATA. */

/* ACPI SMBus request done and SMBus alarm received cause values.
   The EC_Space for these variables are both initialized to
   SMB_PROTOCOL_DONE.  They can be modified to cause other cause
   values to be returned for SMBus request done and SMBus alarm received
   events. */
#define SMB_DNE_CAUSE_INDX  40 /* ACPI SMBus request done cause value. */
#define SMB_ALRM_CAUSE_INDX 41 /* ACPI SMBus alarm cause value. */

/* The ACPI Timer done cause value.  The EC_Space for this variable
   is initialzice to ACPI_TMR_DONE.  It can be modified to cause
   other cause values to be returned when the ACPI timer times out. */
#define ACPI_TMR_CAUSE_INDX 42 /* ACPI Timer cause value. */

#define ACPI_TMR_LO_INDX    43 /* ACPI Timer low byte. */
#define ACPI_TMR_HI_INDX    44 /* ACPI Timer high byte. */


/* First address available for OEM (48. = 0x30). */
#define OEM_EC_START 48

   /* Cause numbers for Cause_Flags
      (Embedded Controller SCI Cause). */

#define SMB_PROTOCOL_DONE     1  /* SMBus protocol done. */
#define SMB_ALRM_RECEIVED     2  /* SMBus alarm received. */
#define ACPI_TMR_DONE         3  /* ACPI Timer timed out. */

#if ACPI_SUPPORTED
 #define FIRST_OEM_SCI_CAUSE  9  /* First available SCI cause for OEM. */
#else
 #define FIRST_OEM_SCI_CAUSE  0  /* First available SCI cause for OEM. */
#endif

/* Password buffer size. */
#define PASS_SIZE 8

/* Auxiliary device constants. */
#define AUX_SEND_ERROR    0xFE
#define AUX_RECEIVE_ERROR 0xFF
#define KBD_CMD_RESEND    0xFE

#define AUX_REMOVED	0xFD

#endif /* ifndef PURCONST_H */

