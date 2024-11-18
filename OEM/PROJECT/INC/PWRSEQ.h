/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     PWRSEQ.H - OEM power sequence control function.
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef PWRSEQ_H
#define PWRSEQ_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#define ResumeS0    0
#define InS0        0
#define EntryS3     3
#define InS3        3
#define EntryS4     4
#define InS4        4
#define EntryS5     5
#define InS5        5
#define EntryG3     6
#define InG3        6

void PwrSeq_Init(FUNCT_PTR_V_V power_sequence_handle, BYTE ticker);
void HandlePwrSeq(void);
void SeqNullHandle(void);
void Seq_Step1(void);
void Seq_Step2(void);
void Seq_Step3(void);
void Seq_Active(void);
void Seq_Monitor_SysState(void);


#endif /* ifndef PWRSEQ_H */
