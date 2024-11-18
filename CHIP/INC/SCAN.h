/*-----------------------------------------------------------------------------
 * MODULE SCAN.H
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
 * Internal keyboard (scanner).
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 20, 2000 by GLP)
 *
 * ------------------------------------------------------------------------- */

#ifndef SCAN_H
#define SCAN_H

#include "swtchs.h"
#include "types.h"
#include "ps2.h"
#include "icu.h"
#include "oem.h"

void Enable_Any_Key_Irq(void);
void Enable_Any_Key_Wakeup(void);
void Disable_Any_Key_Wakeup(void);
void Write_Strobe(SMALL scan_line_num);
BYTE Read_Scan_Lines(void);
void Scan_Init(void);
void Setup_Scanner_Pntr(void);
FLAG Get_Kbd_Type(void);
SMALL Get_Custom_Table_Size(void);
void Setup_Ext_Hotkey_Pntr(void);

/* Pointer to end of current external keyboard hotkey table. */
extern const BYTE *Ext_Hotkey_Pntr;
/* ---------------------------------------------------------------
   Name: Disable_Any_Key_Irq

   Disable IRQ from internal keyboard (scanner) key presses.

   C prototype:
      void Disable_Any_Key_Irq(void);
   --------------------------------------------------------------- */
#define Disable_Any_Key_Irq() ICU_Disable_Irq(ICU_EINT_KBD_SCAN)



#endif /* ifndef SCAN_H */

