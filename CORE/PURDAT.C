/* ----------------------------------------------------------------------------
 * MODULE PURDAT.C
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
 * Revision History (Started June 14, 1996 by GLP)
 *
 * GLP
 * 06 May 97   Added ACPI variables.
 * 06 Mar 98   Added virtual Host InterFace (HIF) variable.
 * 14 Apr 98   Changed Smb_Alarm_xxx into Smb_Alarm_Buffer[].
 * 24 Apr 98   Changed Cmd_Int_Save_Pntr and Cmd_Int_Save_Array
 *             into arrays for the virtual Host interface.
 * 06 May 98   Changed size of Cmd_Int_Save_Pntr and
 *             Cmd_Int_Save_Array arrays.
 * 12 May 98   Added PPK_Cause_Flags.  Removed Ssft_Byte.
 * 14 May 98   Changed Ext_Cb4 to Ps2_Ctrl.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 *             Removed PPK_Cause_Flags.
 * 19 Aug 99   Updated copyright.
 * 07 Sep 99   Removed CMD_BC switch.
 * 20 Jan 00   Support for Active PS/2 Multiplexing (APM) is
 *             marked as V21.1.
 * EDH
 * 11 Dec 00 	Support the fourth PS/2 port as port 4. It will be
 *             recognized as port 0 with Active PS/2 Multiplexing
 *	            driver.
 * 02 Jan 01   Changed the I2C process from direct register to pointer.
 *             Support the secondary SMBus.
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * ------------------------------------------------------------------------- */

#define PURDAT_C

#include "purdat.h"
#include "purxlt.h"
#include "swtchs.h"
#if SMB_FLUPDATE_SUPPORTED
#include "fu_oem.h"
#endif

/* See PURDAT.H for comments on these variables. */

FLAGS    Flag;
INT_VAR  Int_Var;
BITS_8   Ccb42;
BITS_8   Ext_Cb0;

BITS_8	MULPX;

BITS_8   Ext_Cb3;
BITS_8   Ps2_Ctrl;
VBITS_16 Service;
BITS_8   Led_Ctrl;
BITS_8   Led_Data;
BITS_8   Scanner_State;
BITS_8   Gen_Info;
BITS_8   HIF_Response[HIF_CHNLS];
BITS_8   Wakeup1;
BITS_8   Wakeup2;
BITS_8   Wakeup3;
BITS_8   Wakeup4;
BITS_8   Wakeup5;
VBITS_8  Aux_Data;

BYTE     Save_Kbd_State;
//BYTE     Cmd_Byte;
//BYTE     Tmp_Load;
//BYTE     Tmp_Byte[TMP_SIZE];
HIF_VAR Hif_Var[5];
BYTE     Pass_Buff[PASS_SIZE];
BYTE     Pass_Buff_Idx;
BYTE     Pass_On;
BYTE     Pass_Off;
BYTE     Pass_Make1;
BYTE     Pass_Make2;
BYTE     Fn_Make_Key1;
BYTE     Save_Typematic;
VBYTE    Gen_Timer;
BYTE     Mouse_State[3];
BYTE     Mouse_Cmd;


#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#elif RAM_SCAN_TABLE
BYTE     Rc_RAM_Table[1+RC_TABLE_SIZE+COMB_TABLE_SIZE+MAX_CUSTOM_TABLE_SIZE];
#endif

WORD Scanner_Strobe_Delay;

AUX_STATUS Aux_Status;
BYTE       Aux_Port_Ticker;
AUX_PORT   Aux_Port1;
#ifndef AUX_PORT2_SUPPORTED
#error AUX_PORT2_SUPPORTED switch is not defined.
#elif AUX_PORT2_SUPPORTED
AUX_PORT   Aux_Port2;
#endif

#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#elif AUX_PORT3_SUPPORTED
 AUX_PORT  Aux_Port3;
#endif

BYTE Expect_Count;

TIMER     Timer_A;
TIMER     Timer_B;

TIMER     Timer_A3;
TIMER     Timer_A4;
TIMER     Timer_A5;


KEYPRO_STAT Keypro_Stat;
EXT_CB2   Ext_Cb2;
INT_MOUSE Int_Mouse;
//BYTE     *Tmp_Pntr;

BYTE     HIF2_Out_Data_Buffer;

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#if ACPI_SUPPORTED
 BYTE EC_Addr;
 BYTE EC_Space[EC_ADDR_MAX+1];
#endif

#if I2C_SUPPORTED
// BYTE Smb_Alarm_Buffer[SMB_CHNLS][3];
#endif

BYTE Cause_Flags[CAUSE_ARRAY_SIZE];
/* The xxx_Event_Buffer will be used as a ring buffer.  One
   element of the buffer will be unused when the buffer is full. */
BYTE SCI_Event_In_Index;
BYTE SCI_Event_Out_Index;
BYTE SCI_Event_Buffer[EVENT_BUFFER_SIZE];
BYTE SMI_Event_In_Index;
BYTE SMI_Event_Out_Index;
BYTE SMI_Event_Buffer[EVENT_BUFFER_SIZE];

BITS_8 SMB_Status[SMB_CHNLS];
BYTE SMB_Buffer[SMB_CHNLS][SMB_BUFFER_DEPTH];
//BYTE i2c_slave_buffer[SMB_CHNLS][SMB_BUFFER_DEPTH];

AD_VALUE AD_Value;
BYTE DIDLE_Delay;

#if SMB_FLUPDATE_SUPPORTED
BYTE SMB_FL_Rbuf[SMB_FL_Buf_Size];
BYTE SMB_FL_Wbuf[SMB_FL_Buf_Size];
BYTE SMB_Flupdate_Sts;
#endif
#if MOUSE_EMULATION
BYTE    *Aux_Tmp_Pntr;
BYTE    Aux_Tmp_Load1;
BYTE    Aux_Response; // jacob 0419: modify for fix mouse emulation issue
WORD    Aux_Send_Timer; //jacob 0508
BITS_8 Emu_Aux;
#endif // MOUSE_EMULATION
// Jacob add for new SHM RC scan diagnostic mode
BITS_8  SHM_RC_STATUS;
BYTE    SHM_RC_VALUE;
// end of new SHM RC scan diagnostic mode

