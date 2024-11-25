/* ----------------------------------------------------------------------------
 * MODULE PURHOOK.H
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
 * Hooks in the core.
 *
 * Revision History (Started July 23, 1996 by GLP)
 *
 * GLP
 * 12 Dec 96   Corrected Def_Hookc_Check_Ppk_Entry
 *             (was Def_Check_Ppk_Entry).
 * 03 Feb 98   Corrected Def_Hookc_Low_Power_Enter
 *             (was defined as Hookc_Low_Power_Exit).
 * 27 Feb 98   Added Hookc_Get_Hotkey_Pntr, Hookc_Get_Rc_Pntr,
 *             Hookc_Read_EC, Hookc_Write_EC, Hookc_Service_OEM_0,
 *             Hookc_Service_OEM_1, and Hookc_Service_OEM_2.
 * 24 Mar 98   Added Hookc_SMB_Function.  Updated hooks to remove
 *             errors generated when hook is removed.
 * 31 Mar 98   Added Hookc_Service_1mS.
 * 06 Apr 98   Added Hookc_Pc_Cmd1, Hookc_Pc_Dat1, Hookc_Pc_Cmd4,
 *             and Hookc_Pc_Dat4.
 *             Changed Hookc_Pcdat to Hookc_Srvc_Pcdat.
 *             Added Hookc_Srvc_Pccmd2 and Hookc_Srvc_Pcdat2.
 * 14 Apr 98   Added Hookc_Chk_SMB_Alrm_Rdy and Hookc_Signal_SMB_Alrm.
 * 13 May 98   Added Hookc_Signal_SMB_Done.
 * 15 Jun 98   Removed Hookc_SMB_Function (this is handled by
 *             the SMBus descriptor tables).  Removed Hookc_Cmd_Data.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 12 Oct 98   Moved Hookc_Get_Rc_Pntr to CHIPHOOK.H.
 * 17 Aug 99   Added Hookc_Chk_Flashing_Safe.  Updated copyright.
 *             Moved Hookc_Read_EC and Hookc_Write_EC because they
 *             are in PURACPI module.
 * 24 Aug 99   Added Hookc_OEM_Sys_Ctrl_Funct.  Changed
 *             Hookc_Key_Enable to take a parameter and return a
 *             flag.  Removed Hookc_Ext_To_Pc and Hookc_Send_To_Pc.
 * EDH
 * 15 Feb 05  Added to process command/data write from Third Host Interface.
 * ------------------------------------------------------------------------- */

#ifndef PURHOOK_H
#define PURHOOK_H

/* A hook can be removed by changing the definition of the hook
   from 1 to 0 and recompiling the core.  This may be useful in
   generating smaller and/or faster code.  The hook will not be
   called if the definition is not 1.

   For example,
   to remove Hookc_Wonderful_Hook() change the line that reads:

   #define Def_Hookc_Wonderful_Hook() 1

   to:

   #define Def_Hookc_Wonderful_Hook() 0
*/

#include "types.h"

/*********************** Hooks in PURACPI.C ***********************/
#define Def_Hookc_Read_EC              1
#define Def_Hookc_Write_EC             1

/*********************** Hooks in PURINI.C ***********************/
#define Def_Hookc_Warm_Reset_Begin     1
#define Def_Hookc_Warm_Reset_End       1
#define Def_Hookc_Cold_Reset_Begin     1
#define Def_Hookc_Cold_Reset_End       1

/*********************** Hooks in PURMAIN.C **********************/
#define Def_Hookc_Low_Power_Enter      1
#define Def_Hookc_Low_Power_Exit       1
#define Def_Hookc_Exit_Idle            1
#define Def_Hookc_Enter_Service        1
#define Def_Hookc_Get_Multibyte        1
#define Def_Hookc_Key_Enable           1
#define Def_Hookc_Unlock               1
#define Def_Hookc_Service_OEM_0        1
#define Def_Hookc_Service_OEM_1        1
#define Def_Hookc_Service_OEM_2        1
#define Def_Hookc_Service_1mS          1
#define Def_Hookc_Service_Ext_Irq      1
#define Def_Hookc_Service_Scan         1


/*********************** Hooks in PURCMD.C ***********************/
#define Def_Hookc_Srvc_Pccmd           1
#define Def_Hookc_Srvc_Pcdat           1
#define Def_Hookc_Self_Test            1
#define Def_Hookc_All_Others           1
#define Def_Hookc_Pc_Cmd1              1
#define Def_Hookc_Pc_Dat1              1
#define Def_Hookc_Pc_Cmd4              1
#define Def_Hookc_Pc_Dat4              1
#define Def_Hookc_Pc_Cmd5              1
#define Def_Hookc_Pc_Dat5              1

/*********************** Hooks in PURPM.C ***********************/
#define Def_Hookc_Srvc_Pm1cmd          1
#define Def_Hookc_Srvc_Pm1dat          1
#define Def_Hookc_Pmx_Cmd0             1
#define Def_Hookc_Pmx_Cmd1             1
#define Def_Hookc_Pmx_Cmd2             1
#define Def_Hookc_Pmx_Cmd3             1
#define Def_Hookc_Pmx_Cmd4             1
#define Def_Hookc_Pmx_Cmd5             1
#define Def_Hookc_Pmx_Cmd6             1
#define Def_Hookc_Pmx_Cmd7             1
#define Def_Hookc_Pmx_Cmd8             1
#define Def_Hookc_Pmx_Cmd9             1
#define Def_Hookc_Pmx_Cmda             1
#define Def_Hookc_Pmx_Cmdb             1
#define Def_Hookc_Pmx_Cmdc             1
#define Def_Hookc_Pmx_Cmdd             1
#define Def_Hookc_Pmx_Cmde             1
#define Def_Hookc_Pmx_Cmdf             1
#define Def_Hookc_Pmx_Dat0             1
#define Def_Hookc_Pmx_Dat1             1
#define Def_Hookc_Pmx_Dat2             1
#define Def_Hookc_Pmx_Dat3             1
#define Def_Hookc_Pmx_Dat4             1
#define Def_Hookc_Pmx_Dat5             1
#define Def_Hookc_Pmx_Dat6             1
#define Def_Hookc_Pmx_Dat7             1
#define Def_Hookc_Pmx_Dat8             1
#define Def_Hookc_Pmx_Dat9             1
#define Def_Hookc_Pmx_Data             1
#define Def_Hookc_Pmx_Datb             1
#define Def_Hookc_Pmx_Datc             1
#define Def_Hookc_Pmx_Datd             1
#define Def_Hookc_Pmx_Date             1
#define Def_Hookc_Pmx_Datf             1

/*********************** Hooks in PURPM.C ***********************/
#define Def_Hookc_Srvc_Pm2cmd          1
#define Def_Hookc_Srvc_Pm2dat          1


/*********************** Hooks in PUREXT.C ***********************/
#define Def_Hookc_Chk_Flashing_Safe    1

/*********************** Hooks in PURFUNCT.C *********************/
#define Def_Hookc_Sys_Ctrl_Entry       1
#define Def_Hookc_OEM_Sys_Ctrl_Funct   1

/*********************** Hooks in PURSCN.C ***********************/
#define Def_Hookc_Scan_Keys            1

/*********************** Hooks in PURXLT.C ***********************/
#define Def_Hookc_Key_Proc             1
#define Def_Hookc_enable_FnSticky      1

/*****************************************************************/
/* The following are the details for generating */
/* the hooks and the prototypes for the hooks.  */

/* --- Hooks in PURACPI.C --- */

#if Def_Hookc_Read_EC
 #define Gen_Hookc_Read_EC(index)      (Hookc_Read_EC(index))
 WORD Hookc_Read_EC(BYTE index);
#else
 #define Gen_Hookc_Read_EC(index)      ((WORD) ~0)
#endif

#if Def_Hookc_Write_EC
 #define Gen_Hookc_Write_EC(index, data)      (Hookc_Write_EC(index, data))
 FLAG Hookc_Write_EC(BYTE index, BYTE data);
#else
 #define Gen_Hookc_Write_EC(index, data)      (0)
#endif

/* --- Hooks in PURINI.C --- */

#if Def_Hookc_Warm_Reset_Begin
 #define Gen_Hookc_Warm_Reset_Begin()  Hookc_Warm_Reset_Begin()
 void Hookc_Warm_Reset_Begin(void);
#else
 #define Gen_Hookc_Warm_Reset_Begin()
#endif

#if Def_Hookc_Warm_Reset_End
 #define Gen_Hookc_Warm_Reset_End()    Hookc_Warm_Reset_End()
 void Hookc_Warm_Reset_End(void);
#else
 #define Gen_Hookc_Warm_Reset_End()
#endif

#if Def_Hookc_Cold_Reset_Begin
 #define Gen_Hookc_Cold_Reset_Begin()  Hookc_Cold_Reset_Begin()
 void Hookc_Cold_Reset_Begin(void);
#else
 #define Gen_Hookc_Cold_Reset_Begin()
#endif

#if Def_Hookc_Cold_Reset_End
 #define Gen_Hookc_Cold_Reset_End()    Hookc_Cold_Reset_End()
 void Hookc_Cold_Reset_End(void);
#else
 #define Gen_Hookc_Cold_Reset_End()
#endif

/* --- Hooks in PURMAIN.C --- */

#if Def_Hookc_Low_Power_Enter
 #define Gen_Hookc_Low_Power_Enter()   Hookc_Low_Power_Enter()
 void Hookc_Low_Power_Enter(void);
#else
 #define Gen_Hookc_Low_Power_Enter()
#endif

#if Def_Hookc_Low_Power_Exit
 #define Gen_Hookc_Low_Power_Exit()    Hookc_Low_Power_Exit()
 void Hookc_Low_Power_Exit(void);
#else
 #define Gen_Hookc_Low_Power_Exit()    Enable_Irq()
#endif

#if Def_Hookc_Exit_Idle
 #define Gen_Hookc_Exit_Idle()         Hookc_Exit_Idle()
 void Hookc_Exit_Idle(void);
#else
 #define Gen_Hookc_Exit_Idle()
#endif

#if Def_Hookc_Enter_Service
 #define Gen_Hookc_Enter_Service()     Hookc_Enter_Service()
 void Hookc_Enter_Service(void);
#else
 #define Gen_Hookc_Enter_Service()
#endif

#if Def_Hookc_Get_Multibyte
 #define Gen_Hookc_Get_Multibyte()     Hookc_Get_Multibyte()
 WORD Hookc_Get_Multibyte(void);
#else
 #define Gen_Hookc_Get_Multibyte()     ((WORD) 0)
#endif

#if Def_Hookc_Key_Enable
 #define Gen_Hookc_Key_Enable(data)    (Hookc_Key_Enable(data))
 FLAG Hookc_Key_Enable(WORD data);
#else
 #define Gen_Hookc_Key_Enable(data)    (1)
#endif

#if Def_Hookc_Unlock
 #define Gen_Hookc_Unlock()            Hookc_Unlock()
 void Hookc_Unlock(void);
#else
 #define Gen_Hookc_Unlock()
#endif

#if Def_Hookc_Service_OEM_0
 #define Gen_Hookc_Service_OEM_0()     Hookc_Service_OEM_0()
 void Hookc_Service_OEM_0(void);
#else
 #define Gen_Hookc_Service_OEM_0()
#endif

#if Def_Hookc_Service_OEM_1
 #define Gen_Hookc_Service_OEM_1()     Hookc_Service_OEM_1()
 void Hookc_Service_OEM_1(void);
#else
 #define Gen_Hookc_Service_OEM_1()
#endif

#if Def_Hookc_Service_OEM_2
 #define Gen_Hookc_Service_OEM_2()     Hookc_Service_OEM_2()
 void Hookc_Service_OEM_2(void);
#else
 #define Gen_Hookc_Service_OEM_2()
#endif

#if Def_Hookc_Service_1mS
 #define Gen_Hookc_Service_1mS()       Hookc_Service_1mS()
 void Hookc_Service_1mS(void);
#else
 #define Gen_Hookc_Service_1mS()
#endif

#if Def_Hookc_Service_Ext_Irq
 #define Gen_Hookc_Service_Ext_Irq()   Hookc_Service_Ext_Irq()
 void Hookc_Service_Ext_Irq(void);
#else
 #define Gen_Hookc_Service_Ext_Irq()
#endif

#if Def_Hookc_Service_Scan
 #define Gen_Hookc_Service_Scan()   Hookc_Service_Scan()
 FLAG Hookc_Service_Scan(void);
#else
 #define Gen_Hookc_Service_Scan()
#endif

/* --- Hooks in PURCMD.C --- */

#if Def_Hookc_Srvc_Pccmd
 #define Gen_Hookc_Srvc_Pccmd(cmnd)    (Hookc_Srvc_Pccmd(cmnd))
 WORD Hookc_Srvc_Pccmd(BYTE command_num);
#else
 #define Gen_Hookc_Srvc_Pccmd(cmnd)    ((WORD) cmnd)
#endif

#if Def_Hookc_Srvc_Pcdat
 #define Gen_Hookc_Srvc_Pcdat(cmd, data) Hookc_Srvc_Pcdat(cmd, data)
 void Hookc_Srvc_Pcdat(BYTE cmd, BYTE data);
#else
 #define Gen_Hookc_Srvc_Pcdat(cmd, data)
#endif


#if Def_Hookc_Self_Test
 #define Gen_Hookc_Self_Test()         (Hookc_Self_Test())
 WORD Hookc_Self_Test(void);
#else
 #define Gen_Hookc_Self_Test()         ((WORD) 0x55)
#endif

#if Def_Hookc_All_Others
 #define Gen_Hookc_All_Others(data)    Hookc_All_Others(data)
 void Hookc_All_Others(BYTE data);
#else
 #define Gen_Hookc_All_Others(data)
#endif

#if Def_Hookc_Pc_Cmd1
 #define Gen_Hookc_Pc_Cmd1(cmnd)       (Hookc_Pc_Cmd1(cmnd))
 WORD Hookc_Pc_Cmd1(BYTE command_num);
#else
 #define Gen_Hookc_Pc_Cmd1(cmnd)       ((WORD) ~0)
#endif

#if Def_Hookc_Pc_Dat1
 #define Gen_Hookc_Pc_Dat1(cmd, data)  (Hookc_Pc_Dat1(cmd, data))
 WORD Hookc_Pc_Dat1(BYTE cmd, BYTE data);
#else
 #define Gen_Hookc_Pc_Dat1(cmd, data)  ((WORD) ~0)
#endif

#if Def_Hookc_Pc_Cmd4
 #define Gen_Hookc_Pc_Cmd4(hvp, cmd, hif)       ((Hookc_Pmx_Cmd4(hvp, cmd, hif)))
 WORD Hookc_Pmx_Cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pc_Cmd4(cmnd)       ((WORD) ~0)
#endif

#if Def_Hookc_Pc_Dat4
 #define Gen_Hookc_Pc_Dat4(hvp, cmd, data, hif)  (Hookc_Pmx_Dat4(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat4(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pc_Dat4(cmd, data)  ((WORD) ~0)
#endif

#if Def_Hookc_Pc_Cmd5
 #define Gen_Hookc_Pc_Cmd5(hvp, cmd, hif)  (Hookc_Pmx_Cmd5(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pc_Cmd5(cmnd)       ((WORD) ~0)
#endif

#if Def_Hookc_Pc_Dat5
 #define Gen_Hookc_Pc_Dat5(hvp, cmd, data, hif)  (Hookc_Pmx_Dat5(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat5(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pc_Dat5(cmd, data)  ((WORD) ~0)
#endif

/* --- Hooks in PURPM.C --- */

#if Def_Hookc_Pmx_Cmd0
 #define Gen_Hookc_Pmx_Cmd0(hvp, cmd, hif)       (Hookc_Pmx_Cmd0(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd0(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat0
 #define Gen_Hookc_Pmx_Dat0(hvp, cmd, data, hif)  (Hookc_Pmx_Dat0(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat0(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat0(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd1
 #define Gen_Hookc_Pmx_Cmd1(hvp, cmd, hif)       (Hookc_Pmx_Cmd1(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd1(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat1
 #define Gen_Hookc_Pmx_Dat1(hvp, cmd, data, hif)  (Hookc_Pmx_Dat1(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat1(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat1(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd2
 #define Gen_Hookc_Pmx_Cmd2(hvp, cmd, hif)       (Hookc_Pmx_Cmd2(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd2(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat2
 #define Gen_Hookc_Pmx_Dat2(hvp, cmd, data, hif)  (Hookc_Pmx_Dat2(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat2(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat2(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd3
 #define Gen_Hookc_Pmx_Cmd3(hvp, cmd, hif)       (Hookc_Pmx_Cmd3(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd3(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd3(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat3
 #define Gen_Hookc_Pmx_Dat3(hvp, cmd, data, hif)  (Hookc_Pmx_Dat3(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat3(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat3(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd4
 #define Gen_Hookc_Pmx_Cmd4(hvp, cmd, hif)       (Hookc_Pmx_Cmd4(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd4(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat4
 #define Gen_Hookc_Pmx_Dat4(hvp, cmd, data, hif)  (Hookc_Pmx_Dat4(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat4(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat4(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd5
 #define Gen_Hookc_Pmx_Cmd5(hvp, cmd, hif)       (Hookc_Pmx_Cmd5(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd5(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat5
 #define Gen_Hookc_Pmx_Dat5(hvp, cmd, data, hif)  (Hookc_Pmx_Dat5(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat5(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat5(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd6
 #define Gen_Hookc_Pmx_Cmd6(hvp, cmd, hif)       (Hookc_Pmx_Cmd6(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd6(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat6
 #define Gen_Hookc_Pmx_Dat6(hvp, cmd, data, hif)  (Hookc_Pmx_Dat6(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat6(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat6(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd7
 #define Gen_Hookc_Pmx_Cmd7(hvp, cmd, hif)       (Hookc_Pmx_Cmd7(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd7(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd7(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat7
 #define Gen_Hookc_Pmx_Dat7(hvp, cmd, data, hif)  (Hookc_Pmx_Dat7(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat7(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat7(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd8
 #define Gen_Hookc_Pmx_Cmd8(hvp, cmd, hif)       (Hookc_Pmx_Cmd8(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd8(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat8
 #define Gen_Hookc_Pmx_Dat8(hvp, cmd, data, hif)  (Hookc_Pmx_Dat8(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat8(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat8(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmd9
 #define Gen_Hookc_Pmx_Cmd9(hvp, cmd, hif)       (Hookc_Pmx_Cmd9(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmd9(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmd9(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Dat9
 #define Gen_Hookc_Pmx_Dat9(hvp, cmd, data, hif)  (Hookc_Pmx_Dat9(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Dat9(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Dat9(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmda
 #define Gen_Hookc_Pmx_Cmda(hvp, cmd, hif)       (Hookc_Pmx_Cmda(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmda(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Data
 #define Gen_Hookc_Pmx_Data(hvp, cmd, data, hif)  (Hookc_Pmx_Data(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Data(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Data(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmdb
 #define Gen_Hookc_Pmx_Cmdb(hvp, cmd, hif)       (Hookc_Pmx_Cmdb(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmdb(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Datb
 #define Gen_Hookc_Pmx_Datb(hvp, cmd, data, hif)  (Hookc_Pmx_Datb(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Datb(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Datb(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmdc
 #define Gen_Hookc_Pmx_Cmdc(hvp, cmd, hif)       (Hookc_Pmx_Cmdc(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmdc(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Datc
 #define Gen_Hookc_Pmx_Datc(hvp, cmd, data, hif)  (Hookc_Pmx_Datc(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Datc(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Datc(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmdd
 #define Gen_Hookc_Pmx_Cmdd(hvp, cmd, hif)       (Hookc_Pmx_Cmdd(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmdd(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Datd
 #define Gen_Hookc_Pmx_Datd(hvp, cmd, data, hif)  (Hookc_Pmx_Dat0(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Datd(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Datd(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmde
 #define Gen_Hookc_Pmx_Cmde(hvp, cmd, hif)       (Hookc_Pmx_Cmde(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmde(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Date
 #define Gen_Hookc_Pmx_Date(hvp, cmd, data, hif)  (Hookc_Pmx_Date(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Date(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Date(hvp, cmd, data, hif)  ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Cmdf
 #define Gen_Hookc_Pmx_Cmdf(hvp, cmd, hif)       (Hookc_Pmx_Cmdf(hvp, cmd, hif))
 WORD Hookc_Pmx_Cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Cmdf(hvp, cmd, hif)       ((WORD) ~0)
#endif

#if Def_Hookc_Pmx_Datf
 #define Gen_Hookc_Pmx_Datf(hvp, cmd, data, hif)  (Hookc_Pmx_Datf(hvp, cmd, data, hif))
 WORD Hookc_Pmx_Datf(HIF_VAR* hvp, BYTE cmd, BYTE data, BYTE hif);
#else
 #define Gen_Hookc_Pmx_Datf(hvp, cmd, data, hif)  ((WORD) ~0)
#endif


/* --- Hooks in PUREXT.C --- */

#if Def_Hookc_Chk_Flashing_Safe
 #define Gen_Hookc_Chk_Flashing_Safe() Hookc_Chk_Flashing_Safe()
 FLAG Hookc_Chk_Flashing_Safe(void);
#else
 #define Gen_Hookc_Chk_Flashing_Safe() ((FLAG) 1)
#endif

/* --- Hooks in PURFUNCT.C --- */

#if Def_Hookc_Sys_Ctrl_Entry
 #define Gen_Hookc_Sys_Ctrl_Entry(data, event)   (Hookc_Sys_Ctrl_Entry(data, event))
 FLAG Hookc_Sys_Ctrl_Entry(WORD data, SMALL event);
#else
 #define Gen_Hookc_Sys_Ctrl_Entry(data, event)   (0)
#endif

#if Def_Hookc_OEM_Sys_Ctrl_Funct
 #define Gen_Hookc_OEM_Sys_Ctrl_Funct(data, event) (Hookc_OEM_Sys_Ctrl_Funct(data, event))
 void Hookc_OEM_Sys_Ctrl_Funct(WORD data, SMALL event);
#else
 #define Gen_Hookc_OEM_Sys_Ctrl_Funct(data, event)
#endif

/* --- Hooks in PURSCN.C --- */

#if Def_Hookc_Scan_Keys
 #define Gen_Hookc_Scan_Keys()         Hookc_Scan_Keys()
 void Hookc_Scan_Keys(void);
#else
 #define Gen_Hookc_Scan_Keys()
#endif

/* --- Hooks in PURXLT.C --- */

#if Def_Hookc_Key_Proc
 #define Gen_Hookc_Key_Proc()          Hookc_Key_Proc()
 void Hookc_Key_Proc(void);
#else
 #define Gen_Hookc_Key_Proc()
#endif

#if Def_Hookc_enable_FnSticky
 #define Gen_Hookc_enable_FnSticky()          Hookc_enable_FnSticky()
 int Hookc_enable_FnSticky(void);
#else
 #define Gen_Hookc_enable_FnSticky() 0
#endif


#endif /* ifndef PURHOOK_H */

