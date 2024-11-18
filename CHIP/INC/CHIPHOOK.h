/*-----------------------------------------------------------------------------
 * MODULE CHIPHOOK.H
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
 * Hooks in the core chip-level.
 *
 * Revision History (Started August 22, 1996 by GLP)
 *
 * 28 Sep 98   Removed Hookc_Irq_Ext and Hookc_Irq_Nmi.
 * 12 Oct 98   Added Hookc_Get_Rc_Pntr and Hookc_Get_Kbd_Type.
 * 19 Aug 99   Updated copyright.
 * 24 Aug 99   Added Hookc_Get_Ext_Hotkey_Pntr.
 * 07 Sep 99   Added Hookc_Get_Custom_Table_Size.
 * ------------------------------------------------------------------------- */

#ifndef CHIPHOOK_H
#define CHIPHOOK_H

/* A hook can be removed by changing the definition of the hook
   from 1 to 0 and recompiling the chip core.  This may be useful
   in generating smaller and/or faster code.  The hook will not
   be called if the definition is not 1.

   For example,
   to remove Hookc_Wonderful_Hook() change the line that reads:

   #define Def_Hookc_Wonderful_Hook() 1

   to:

   #define Def_Hookc_Wonderful_Hook() 0
*/

/*********************** Hooks in IRQ.C ***********************/
#define Def_Hookc_Irq_Timer_A          1

/************************ Hooks in SCAN.C ************************/
#define Def_Hookc_Get_Rc_Pntr          1
#define Def_Hookc_Get_Kbd_Type         1
#define Def_Hookc_Get_Custom_Table_Size 1
#define Def_Hookc_Get_Ext_Hotkey_Pntr  1
#define Def_Gen_Hookc_Timer_A_Check 1


/*****************************************************************/
/* The following are the details for generating */
/* the hooks and the prototypes for the hooks.  */

/* --- Hooks in IRQ.C --- */

#if Def_Hookc_Irq_Timer_A
 #define Gen_Hookc_Irq_Timer_A()  Hookc_Irq_Timer_A()
 void Hookc_Irq_Timer_A(void);
#else
 #define Gen_Hookc_Irq_Timer_A()
#endif

/* --- Hooks in SCAN.C --- */

#if Def_Hookc_Get_Rc_Pntr
 #define Gen_Hookc_Get_Rc_Pntr()       Hookc_Get_Rc_Pntr()
 BYTE *Hookc_Get_Rc_Pntr(void);

#else
 #define Gen_Hookc_Get_Rc_Pntr()       ((void *)0)
#endif

#if Def_Hookc_Get_Kbd_Type
 #define Gen_Hookc_Get_Kbd_Type()      Hookc_Get_Kbd_Type()
 BYTE Hookc_Get_Kbd_Type(void);
#else
 #define Gen_Hookc_Get_Kbd_Type()      (0xFF)
#endif

#if Def_Hookc_Get_Custom_Table_Size
 #define Gen_Hookc_Get_Custom_Table_Size() Hookc_Get_Custom_Table_Size()
 SMALL Hookc_Get_Custom_Table_Size(void);
#else
 #define Gen_Hookc_Get_Custom_Table_Size() (0xFF)
#endif

#if Def_Hookc_Get_Ext_Hotkey_Pntr
 #define Gen_Hookc_Get_Ext_Hotkey_Pntr() Hookc_Get_Ext_Hotkey_Pntr()
 BYTE *Hookc_Get_Ext_Hotkey_Pntr(void);
#else
 #define Gen_Hookc_Get_Ext_Hotkey_Pntr() ((void *)0)
#endif

#if Def_Gen_Hookc_Timer_A_Check
 #define Gen_Hookc_Timer_A_Check() Hookc_Timer_A_Check()
 FLAG Hookc_Timer_A_Check(void);
#else
 #define Hookc_Timer_A_Check()      (1)
#endif


#endif /* ifndef CHIPHOOK_H */
