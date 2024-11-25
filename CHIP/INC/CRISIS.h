/*-----------------------------------------------------------------------------
 * MODULE CRISIS.H
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
 * Revision History (Started June 5, 1996 by GLP)
 *
 * GLP
 * 08/18/99 Added Cmd0F prototype.
 * ------------------------------------------------------------------------- */

#ifndef CRISIS_H
#define CRISIS_H

#define RAM_DISPATCH_SIZE 16*3
extern void (* const _dispatch_table[])(void);

void Crisis_Reset(void);
void Do_Crisis_Recovery(void);
void Crisis_Init_Cmd_Hndlr(void);
void CmdD1(BYTE host_byte);
void Cmd08(BYTE host_byte);
void Cmd09(BYTE host_byte);
void Cmd0A(void);
void Cmd0B(void);
void Cmd0F(void);
void Crisis_Data_To_Host(BYTE data);
void Crisis_Aux_Data_To_Host(BYTE data);

#ifndef CRISIS_C
extern BYTE bCrisis_Cmd_Num;
extern BYTE bCrisis_Cmd_Index;
extern BITS_8 Crisis_Command_Byte;
extern BITS_8 Crisis_Flags;
extern WORD wCrisis_Prog_Index;
extern WORD wCrisis_Prog_Start;
extern WORD wCrisis_Prog_Len;
#endif

/* ------------------- Crisis_Command_Byte ------------------- */
#define INTR_KEY  bit0
#define INTR_AUX  bit1
#define SYS_FLAG  bit2
#define DISAB_KEY bit4
#define DISAB_AUX bit5
#define XLATE_PC  bit6

#define CRISIS_COMMAND_BYTE_INIT 0x10

/* ----------------------- Crisis_Flags ---------------------- */
#define GEN_CRISIS bit7

#endif

