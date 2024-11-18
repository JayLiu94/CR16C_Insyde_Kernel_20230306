/* ---------------------------------------------------------------
 * MODULE SMB_TRAN.H
 *
 *       Copyright 2010, all rights reserved
 *       Insyde Software Corp.
 *
 * Revision History (Started March 23, 1999 by GLP)
 *
 * --------------------------------------------------------------- */

#ifndef SMB_TRAN_H
#define SMB_TRAN_H

#include "types.h"

#define SMB0    0
#define SMB1    1
#define SMB2    2
#define SMB3    3

#define ALARM_SMB_CHANNEL   SMB1
#define ACPI_SMB_CHANNEL    SMB1
#define HIF1_SMB_CHANNEL    SMB1
#define HIF2_SMB_CHANNEL    SMB1
#define HIF3_SMB_CHANNEL    SMB1



void Init_SMB_Transfer(void);

void SMB_Transfer_1mS(BYTE Channel);

FLAG SMB_wrWORD(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, WORD wData, FLAG Prtcl_Type /* BYTE RETRY_CNT */);

BYTE * SMB_rdWORD(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type /* BYTE RETRY_CNT */);

FLAG SMB_wrBYTE(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, BYTE wData, FLAG Prtcl_Type /*BYTE RETRY_CNT */);

BYTE * SMB_rdBYTE(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type /* BYTE RETRY_CNT */);

FLAG SMB_wrBLOCK(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, BYTE Send_CNT, BYTE * trans_pntr, FLAG Prtcl_Type /*BYTE RETRY_CNT*/);

BYTE *SMB_rdBLOCK(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type /* BYTE RETRY_CNT */);

BYTE * SMB_RECEIVE(BYTE Channel, BYTE SMB_SLAVE, FLAG Prtcl_Type /*BYTE RETRY_CNT*/);

FLAG SMB_SEND(BYTE Channel, BYTE SMB_SLAVE,  BYTE wData, FLAG Prtcl_Type /*BYTE RETRY_CNT*/);

FLAG Transfer_Finished(BYTE Channel);

BYTE * Get_Result(BYTE Channel);

extern BYTE protocol[];

#endif /* ifndef SMB_TRAN_H */
