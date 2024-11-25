/* ----------------------------------------------------------------------------
 * MODULE PURDEV.H
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
 * For auxiliary keyboard and auxiliary device (mouse)
 * serial send/receive and lock/unlock code.
 *
 * Revision History (Started August 20, 1996 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * 07 Sep 99   Removed CMD_BC switch.
 * ------------------------------------------------------------------------- */

#ifndef PURDEV_H
#define PURDEV_H

#include "swtchs.h"
#include "types.h"

void Lock_Line(SMALL port);
void Lock_Aux_Devs(void);
void Aux_Cmd_Setup(AUX_PORT *aux_pntr, BYTE data);
FLAG Send_To_Aux (AUX_PORT *aux_pntr, SMALL port, BYTE data);
FLAG Check_Transmit(SMALL port);
void Enable_Gentmo(WORD time);
void Disable_Gentmo(void);
void Inhibit_Delay(void);
#if MOUSE_EMULATION
WORD Save_Mouse_State(BYTE data);
#else // MOUSE_EMULATION
void Save_Mouse_State(BYTE data);
#endif // MOUSE_EMULATION
void Init_SXI(void);

#endif /* ifndef PURDEV_H */


