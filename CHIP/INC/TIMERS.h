/*-----------------------------------------------------------------------------
 * MODULE TIMERS.H
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
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 20, 2000 by GLP)
 *
 * ------------------------------------------------------------------------- */

#ifndef TIMERS_H
#define TIMERS_H

#include "swtchs.h"
#include "types.h"

void Load_Timer_A(void);
void Enable_Timer_A(void);
void Stop_Timer_A(void);
void Load_Timer_B(void);
void Stop_Timer_B(void);
#if !TICKER_1MS
void Load_Timer_PS2(BYTE time);
void Stop_Timer_PS2(void);
#endif // TICKER_1MS

void Timers_Init(void);

#endif /* ifndef TIMERS_H */

