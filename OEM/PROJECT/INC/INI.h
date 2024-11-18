/* ----------------------------------------------------------------------------
 * MODULE INI.H
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
 * Contains OEM defined constants for
 * initializing variables to known values on reset.
 *
 * Revision History (Started June 17, 1996 by GLP)
 *
 * GLP
 * 16 Dec 96   Changed EXT_CB0_INIT to also set the PORT_CONTROL bit.
 * 15 May 97   Added TYPE_ADB3 definitions.
 * 13 Feb 98   Changed MCFG_INIT from 0x0F to 0x09 (Bit 1 is setup
 *             by the Boot Code).  Changed SZCFG2_INIT from
 *             0x0A80 to 0x0280 (Bit 11 must not be set).
 * 29 Jun 98   Changed SZCFG0_INIT from 0x0A80 to 0x0A00.
 *             Updated for use with CORE 1.01.20.
 * 05 Oct 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef INI_H
#define INI_H

#include "defs.h"
#include "swtchs.h"


#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif

/* ---------------------------------------------------------------
 * OEM DEFINED CONSTANTS TO INITIALIZE CORE VARIABLES.
 * --------------------------------------------------------------- */

/* ------------------------- Ext_Cb0 -------------------------
   Extended control byte.  Accessed through Commands E9h and EAh.

   ENABLE_SCAN  1 = Scanner (internal keyboard) enabled.
   EN_AUX_PORT1 1 = Auxiliary Port 1 enabled.
   EN_AUX_PORT2 1 = Auxiliary Port 2 enabled.
   EN_AUX_PORT3 1 = Auxiliary Port 3 enabled.
   ENABLE_HRDW  1 = Fast Gate A20 and Fast Reset enabled.
   PS2_AT       1 = PS/2 style.
   PORT_CONTROL 1 = Return BFh for Command C0h.
   AUTO_SWITCH  1 = Auto switch to scanner on aux kbd error. */

#if AUX_PORT3_SUPPORTED
  #if AUX_PORT2_SUPPORTED
      #define AUX_SETTING (MASK(EN_AUX_PORT3) | MASK(EN_AUX_PORT2) | MASK(EN_AUX_PORT1))
  #else
      #define AUX_SETTING (MASK(EN_AUX_PORT3) | MASK(EN_AUX_PORT1))
  #endif
#else
  #if AUX_PORT2_SUPPORTED
 #define AUX_SETTING (MASK(EN_AUX_PORT2) | MASK(EN_AUX_PORT1))
  #else
 #define AUX_SETTING (MASK(EN_AUX_PORT1))
  #endif
#endif

#define EXT_CB0_INIT (MASK(AUTO_SWITCH) | MASK(PORT_CONTROL) | MASK(ENABLE_SCAN) | AUX_SETTING | MASK(PS2_AT))

/* ------------------------- Ps2_Ctrl ------------------------
   Accessed through Commands EBh and ECh.

   K_AUTO_SWITCH  1 = Primary keyboard can be switched.
   M_AUTO_SWITCH  1 = Primary mouse can be switched.

   shiftPRI_KBD   is used to shift the bits for the primary keyboard
                  port designation to/from the lower part of the byte.
   maskPRI_KBD    is then used to isolate those bits.

   shiftPRI_MOUSE is used to shift the bits for the primary mouse
                  port designation to/from the lower part of the byte.
   maskPRI_MOUSE  is then used to isolate those bits.

   If three auxiliary ports are supported, initialize primary mouse
   to port 3 with no autoswitching.  Otherwise, initialize primary
   mouse to port 2 with autoswitching.

   Initialize primary keyboard to port 1 with autoswitching. */

/* Mark Lin modify 2006/10/14
   KBD_SETUP is setup KBD from AUX_PORT1 OR AUX_PORT2 OR AUX_PORT3
   Mouse_setup is set AUX from AUX_PORT1 OR AUX_PORT2 OR AUX_PORT3
   but if AUX_PORT2 and AUX_PORT3 turn off, you need use AUX_PORT1 (KBD),
   Please set KBD_SETUP == AUX_PORT1_CODE.
   if you need use AUX_PORT1 (Mouse), please MOUSE_SETUP == AUX_PORT1_CODE
   PS. KBD_SETUP and Mouse_SETUP can't use same prot */
#define KBD_SETUP ((AUX_PORT2_CODE << shiftPRI_KBD) + MASK(K_AUTO_SWITCH))
#define PS2_CTRL_INIT ((AUX_PORT1_CODE << shiftPRI_MOUSE) + MASK(M_AUTO_SWITCH) + KBD_SETUP)

/* ------------------------- Led_Ctrl ------------------------
   Initialization for the Led_Ctrl byte.

   If bit is 0, the LED setting is modified by Command 88h
   (Write Port Pin), Command 94h (Write LED), and Port Pin
   Key Table events.

   If bit is 1, LED setting is modified by:

   SCROLL       Keyboard Command EDh.
   NUM          Keyboard Command EDh.
   CAPS         Keyboard Command EDh.
   OVL          Command 9Ch and Fn or Overlay states of scanner.
   LED4
   LED5
   LED6
   LED7

   Keyboard Command EDh is the Set/Reset Status Indicators command.

   Command 9Ch is the Write Overlay State command. */
#define LED_CTRL_INIT (MASK(SCROLL) | MASK(NUM) | MASK(CAPS) | MASK(OVL))

/* --------------------- Save_Kbd_State ----------------------
   Initialization for the Save_Kbd_State byte.

   maskLEDS     is for the LEDs on the auxiliary keyboard.

   shiftCODESET is used to shift the bits for the Scan Code
                Set number to/from the lower part of the byte.
   maskCODESET  is then used to isolate those bits.

   maskAUXENAB  is the enabled state of the auxiliary keyboard
                modified by Keyboard Command F4h/F5h.

   Initialize to enabled, Scan Code Set 2, and LEDs off. */
#define SAVE_KBD_STATE_INIT (maskAUXENAB + (2 << shiftCODESET))

/* -------------------------- Ccb42 --------------------------
   The Controller Command Byte.  Accessed through Commands 20h
   and 60h.  First byte in the 0 through 1F space in an 8042.

   INTR_KEY     1 =
   INTR_AUX     1 =
   SYS_FLAG     1 =
   DISAB_KEY    1 =
   DISAB_AUX    1 =
   XLATE_PC     1 = */
#define CCB42_INIT (MASK(DISAB_KEY))

/* ---------------------- MAX_BATTERIES ----------------------
   The maximum number of batteries in the system. */

#define MAX_BATTERIES 1

#define HIF2_SAFETY_TIME 2
#define HIF3_SAFETY_TIME 2

#define HIF_CHNLS   3

#define HIF_KBC 0
#define HIF_PM1 1
#define HIF_PM2 2
#define HIF_PM3 3
#define HIF_PM4 4

#define HIF_PCI     HIF_KBC
#define HIF_PCI2    HIF_PM1
#define HIF_PCI3    HIF_PM2
#define HIF_PCI4    HIF_PM3
#define HIF_PCI5    HIF_PM4


#endif /* ifndef INI_H */

