/*-----------------------------------------------------------------------------
 * MODULE PURCMD.H
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
 * Contains prototypes for PURCMD.C.
 *
 * Revision History (Started April 18, 1995 by GLP)
 *
 * 30 Jul 98   Added prototype for Pcxa6 and
 *             added Enable_Password definition.
 * 19 Aug 99   Updated copyright.
 * EDH
 * 15 Feb 05    Added to process command/data write from Third Host Interface.
 * 12 May 15    Moved Set_Overlay_Mask to PURSCN.C.
 * ------------------------------------------------------------------------- */

#ifndef PURCMD_H
#define PURCMD_H

#include "types.h"

void Srvc_Pccmd(BYTE command_num);
//void Srvc_Pccmd2(BYTE command_num);
//void Srvc_Pccmd3(BYTE command_num);
void Srvc_Pcdat(BYTE command_num);
//void Srvc_Pcdat2(BYTE data);
//void Srvc_Pcdat3(BYTE data);
WORD Pcxa8(void);
WORD Pcxae(void);
WORD Pcxa6(void);

#define Enable_Password Pcxa6

#endif /* ifndef PURCMD_H */

