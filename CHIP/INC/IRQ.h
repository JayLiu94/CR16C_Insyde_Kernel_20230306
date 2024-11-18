/*-----------------------------------------------------------------------------
 * MODULE IRQ.H
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
 * Revision History (Started August 15, 1996 by GLP)
 *
 * GLP
 * 17 Mar 98   Added prototypes.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * 10 Nov 00   Added the fourth PS/2 channel.
 * ------------------------------------------------------------------------- */

#ifndef IRQ_H
#define IRQ_H

#ifdef IRQ_C
#if KERNEL_TICKER_T0
#pragma interrupt(Irq_Timer_A)
#endif // KERNEL_TICKER_T0
#pragma interrupt(Irq_Anykey)
#pragma interrupt(Irq_Ibf1)
#pragma interrupt(Irq_Ibf2)
#pragma interrupt(Irq_Shm)
#pragma interrupt(Irq_Obe1)
#endif

void Irq_Aux_Port1(void);
void Irq_Aux_Port2(void);
void Irq_Aux_Port3(void);
void Irq_Aux_Port4(void);
void Irq_Ibf1(void);
void Irq_Ibf2(void);
void Irq_Obe1(void);
void Irq_Timer_A(void);
void Irq_Timer_B(void);
#if !TICKER_1MS
void Irq_Timer_PS2(void);
#endif // TICKER_1MS
void Irq_Anykey(void);
void Irq_Shm(void);
#endif /* ifndef IRQ_H */

