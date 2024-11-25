/* ----------------------------------------------------------------------------
 * MODULE PUREXT.H
 *
 * Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started May 15, 1995 by GLP)
 *
 * GLP
 * 05 May 97   Added FIRST_ACPI_CMD, LAST_ACPI_CMD, ACPI_Cmd,
 *             and ACPI_Dat.
 * 26 Feb 98   Added FIRST_HIF2_CMD, LAST_HIF2_CMD, HIF2_Cmd,
 *             and HIF2_Dat.
 * 20 Mar 98   Moved ACPI references to PURACPI module.
 * 20 Mar 98   Changed FIRST_HIF2_CMD from 85h to 1.
 *             Changed LAST_HIF2_CMD  from 86h to 0.
 * 30 Jul 98   Changed enter_power_down to Enter_Power_Down.
 * 17 Aug 99   Added prototypes for Pc_Cmd0 and Pc_Dat0.  Updated
 *             copyright.
 * EDH
 * 15 Feb 05  Added to process command/data write from Third Host Interface.
 * ------------------------------------------------------------------------- */

#ifndef PUREXT_H
#define PUREXT_H

#include "types.h"
#include "purdat.h"

/* Set FIRST_HIF2_CMD to the first command number that will
   work only through the secondary Host interface.

   Set LAST_HIF2_CMD to the last command number that will
   work only through the secondary Host interface.

   If there are no commands that are exclusively for the secondary
   Host interface, set LAST_HIF2_CMD less than FIRST_HIF2_CMD. */
#define FIRST_HIF2_CMD 0x01
#define LAST_HIF2_CMD  0x00
#define FIRST_HIF3_CMD 0x01
#define LAST_HIF3_CMD  0x00
#define FIRST_HIF4_CMD 0x01
#define LAST_HIF4_CMD  0x00

WORD HIF2_Cmd(BYTE command_num);
WORD HIF2_Dat(BYTE command_num, BYTE data);
WORD HIF3_Cmd(BYTE command_num);
WORD HIF3_Dat(BYTE command_num, BYTE data);
WORD HIF4_Dat(BYTE command_num, BYTE data);
WORD Ext_Cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif);
WORD Ext_Dat0(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
WORD Ext_Cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif);
WORD Ext_Dat8(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
WORD Ext_Cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif);
WORD Ext_Datb(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);

/* ---------------------------------------------------------------
   Name: Enter_Power_Down

   This function simply sets a bit.  It is a
   function only to allow a place for comments.

   Let the main loop enable interrupts in the path where the
   processor is put into low power mode.  If interupts are
   enabled here, an active irq may prevent the processor from
   ever going into low power mode.  The host expects that the
   processor will go into low power mode if told to do so, even
   if it wakes back up immediately.  If the low power mode is
   bypassed by an active irq, the low power exit hook is never
   executed, which might be necessary to wake up the system!
   This is why there are two flags to handle the low power mode.
   There is an ENTER_LOW_POWER flag and a Low_Power_Mode flag.

   The main loop will also setup for the recovery from power
   down mode before entering that mode.

   C prototype:
      void Enter_Power_Down(void);
   --------------------------------------------------------------- */
#define Enter_Power_Down() Flag.ENTER_LOW_POWER = 1



#endif /* ifndef PUREXT_H */

