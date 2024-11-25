/* ----------------------------------------------------------------------------
 * MODULE PURDAT.H
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Mask definitions for the variables in PURDAT.C as well as other
 * definitions.  This file should be included by any file that uses
 * variables defined in PURDAT.C.
 *
 * Revision History (Started April 18, 1995 by GLP)
 *
 * GLP
 * 13 Dec 96   Changed SBIA (SMBus timeout bit in Service) from
 *             bit9 to bit10.  It had the same definition as
 *             EXT_IRQ_SVC (External interrupt).
 * 17 Dec 96   Moved K_AUTO_SWITCH in Ext_Cb4 from bit2 to bit4.
 * 06 May 97   Added ACPI variables.
 * 27 Feb 98   Added comments to ACPI_ENABLED.  Added OEM_SRVC_0,
 *             OEM_SRVC_1, and OEM_SRVC_2 to Service.
 * 17 Mar 98   Changed SBIA to SMB_SVC.  Added DENY_OVERRIDE and
 *             SELECTOR_LOADED flags.
 * 19 Mar 98   Changed MS_10 to MS_1.  Changed SMB_SVC to CORE_10.
 * 14 Apr 98   Added ACPI_IN_PROGRESS, ACPI_SMB_ALARM, and
 *             LEGACY_SMB_ALARM.  Changed Smb_Alarm_xxx into
 *             Smb_Alarm_Buffer[].
 * 24 Apr 98   Changed Cmd_Int_Save_Pntr and Cmd_Int_Save_Array
 * 06 May 98   Removed Flag.SCAN_LOAD, Flag.PPKT_LOAD, and
 *             Flag.PASS_LOAD.  Changed size of Cmd_Int_Save_Pntr
 *             and Cmd_Int_Save_Array arrays.
 * 12 May 98   Added PPK_Cause_Flags.  Removed GHOSTCMD and
 *             Ssft_Byte.
 * 14 May 98   Changed Ext_Cb4 to Ps2_Ctrl and redefined bits.
 *             Moved ENABLE_PPKT from Ext_Cb0, bit 5
 *                               to   Ext_Cb3, bit 1.
 *             Moved ACPI_ENABLED from Flag variable
 *                                to   Ext_Cb3, bit 3.
 *             Moved EN_AUX_PORT3 from Ext_Cb3, bit 2
 *                                to   Ext_Cb0, bit 3.
 * 04 Jun 98   Added SMB_DEV_DENIED and SMB_CMD_DENIED definitions.
 * 15 Jun 98   Moved DENY_OVERRIDE and SELECTOR_LOADED flags to
 *             PURSMB module.
 * 08 Jul 98   Added SENDING_SCAN_RC flag.
 * 09 Jul 98   Changed flag positions in Service to change
 *             priorities.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 *             Removed PPK_Cause_Flags.
 * 09 Oct 98   Added comment to Scanner_State variable.
 * 19 Aug 99   Updated copyright.
 * 24 Aug 99   Added ALT_PRESSED and CTRL_PRESSED to Flag.
 * 02 Sep 99   Added HIF2_SAFETY to Timer_A.
 * 07 Sep 99   Removed CMD_BC switch.
 * 20 Jan 00   Support to disable internal mouse if external mouse
 *             exists is marked as V21.3.
 * EDH
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * 13 May 04   Implement SMBus 1.1 and 2.0 for PEC and Write Block process call.
 * ------------------------------------------------------------------------- */

#ifndef PURDAT_H
#define PURDAT_H

#include "swtchs.h"
#include "types.h"
#include "purconst.h"
#include "oemcause.h"
#include "oem_data.h"
#include "ini.h"


#if RAM_BASED_FLASH_UPDATE
#define FLASH_jbb *((volatile unsigned short *)0x10FDE)
#else
#define FLASH_jbb *((volatile unsigned short *)0x10FBE)
#endif



/* Last address possible for EC_Addr. */
#define EC_ADDR_MAX LAST_OEM_EC_INDEX

/* SMBus levels.  Refer to SMB_xxx for comments. */
//#define SMB_LEVELS (HIF2_LEVELS + 1)
#define SMB_CHNLS 4

/* The maximum number of bytes that can be sent to an SMBus
   device is a byte to indicate the number of bytes + the
   bytes themselves = a byte of value 32 + the 32 bytes = 33.

   The input buffer needs that plus bytes to hold the protocol
   type, the write byte count, the address, and command bytes.
   This is 4 bytes so the input buffer needs 4+33. */
#define MAX_SMB_INPUT_BYTES (4+33)

/* The maximum number of bytes that can be received from an
   SMBus device is a byte to indicate the number of bytes + the
   bytes themselves = a byte of value 32 + the 32 bytes = 33.

   The output buffer needs that plus one more byte to hold
   the number of bytes in the SMB Buffer.  So the output
   buffer needs 1+33. */
#define MAX_SMB_OUTPUT_BYTES (1+33)

/* The SMB_Buffer is used for input and output.
   Set the buffer depth to the maximum needed. */
#if (MAX_SMB_INPUT_BYTES > MAX_SMB_OUTPUT_BYTES)
 #define SMB_BUFFER_DEPTH MAX_SMB_INPUT_BYTES
#else
 #define SMB_BUFFER_DEPTH MAX_SMB_OUTPUT_BYTES
#endif

/* The xxx_Event_Buffer will be used as a ring buffer.  One
   element of the buffer will be unused when the buffer is full. */
#define EVENT_BUFFER_SIZE (NUM_SCI_SMI_EVENTS+1)

#ifndef PURDAT_C
/* Only define the following if PURDAT.C is not being compiled. */

/* The variables Ccb42, Pass_On, Pass_Off, Pass_Make1, and
   Pass_Make2 are 8042 compatibility data bytes read by commands
   20, 33, 34, 36, 37 and written by commands 60, 73, 74, 76, 77. */

/* -------------------------- Flag --------------------------- */
extern FLAGS Flag;

#define SCAN_INH         bit0 /* 1 = Scanner transmission inhibited. */
#define SENDING_SCAN_RC  bit1 /* For diagnostic mode. */
#define PASS_READY       bit2 /* Password loaded. */
#define NEW_PRI_K        bit3 /* Switch to new primary keyboard. */
#define NEW_PRI_M        bit4 /* Switch to new primary mouse. */
#define VALID_SCAN       bit5
#define LED_ON           bit6
#define ENTER_LOW_POWER  bit7 /* Go into low power mode. */
#define ACPI_IN_PROGRESS bit8
#define ACPI_SMB_ALARM   bit9
#define LEGACY_SMB_ALARM bit10
#define ALT_PRESSED      bit11/* External keyboard ALT key state. */
#define CTRL_PRESSED     bit12/* External keyboard CTRL key state. */
#define Aux_Not_RESP        bit15
/* ------------------------- Int_Var -------------------------
   Variables that can be modified by interrupts. */
extern INT_VAR Int_Var;

/* -------------------------- Ccb42 --------------------------
   The Controller Command Byte.  Accessed through Commands 20h
   and 60h.  First byte in the 0 through 1F space in an 8042. */
extern BITS_8 Ccb42;

#define INTR_KEY     bit0
#define INTR_AUX     bit1
#define SYS_FLAG     bit2

#define DISAB_KEY    bit4
#define DISAB_AUX    bit5
#define XLATE_PC     bit6

/* ------------------------- Ext_Cb0 -------------------------
   Extended control byte.  Accessed through Commands E9h and EAh. */
extern BITS_8 Ext_Cb0;

#define ENABLE_SCAN  bit0  /* 1 = Enable Internal Keyboard. */
#define EN_AUX_PORT1 bit1  /* 1 = Enable Auxiliary Port 1. */
#define EN_AUX_PORT2 bit2  /* 1 = Enable Auxiliary Port 2. */
#define EN_AUX_PORT3 bit3  /* 1 = Enable Auxiliary Port 3. */
#define ITP_DISABLED bit4	/* 1 = Enable Auxiliary Port 4. */
#define PS2_AT       bit5
#define PORT_CONTROL bit6
#define AUTO_SWITCH  bit7

/* >>> Vincent 2003/3/10 Add. */
/* ------------------------- Keypro_Stat ---------------------
   This Byte for Custom function. */
extern KEYPRO_STAT Keypro_Stat;
/* <<< End Add. */

/* ------------------------- Ext_Cb1 -------------------------
   This has been replaced by Led_Ctrl. */

/* ------------------------- Ext_Cb2 -------------------------
   Extended debounce control.
   Accessed through Command 99h. */
extern EXT_CB2 Ext_Cb2;

/* ------------------------- Int_Mouse -------------------------

*/
extern INT_MOUSE Int_Mouse;



/* ------------------------- Ext_Cb3 -------------------------
   Extended scanner control.
   Accessed through Commands 97h and 98h. */
extern BITS_8 Ext_Cb3;

#define DISABLE_NKEY bit0  /* 1 = Simple Matrix, 0 = Nkey. */
#define OVL_STATE    bit1  /* 1 = Overlay state on. */
#define DISABLE_TB 	bit2  /* 1 = Internal pointing device Disabled. */
#define ACPI_ENABLED bit3  /* 1 = ACPI mode, 0 = Legacy mode. */
#define FN_CANCEL    bit4  /* 1 = Any shift key clears Fn state. */
#define NumLockC_OVR bit5  /* 1 = Overlay controlled by NumLock. */
#define DIAG_MODE    bit6  /* 1 = Row/Col output, 0 = scan output. */
#define ENABLE_HRDW  bit7  /* 1 = Enable Auxiliary Port 4. */

/* ------------------------- MULPX ---------------------------
	Active PS/2 Mulplexing control byte. */
extern BITS_8 MULPX;

#define PREFIX0         bit0	/* save aux port number (0~3) */
#define PREFIX1	        bit1	/* by current prefix command */
#define PREFIX_CMD      bit2	/* save current prefix */
#define Mx_state2       bit3	/* state2 flag for multiplex */
#define Mx_state3       bit4	/* state3 flag for multiplex */
#define Multiplex       bit5	/* Multiplex flag */
#define Poll_Switch		bit6    /* Port1 and port2 polling switch. */
#define Mx_hotplug		bit7	/* Multiplex hotplug flag */


/* ------------------------- Ps2_Ctrl ------------------------
   Accessed through Commands EBh and ECh. */
extern BITS_8 Ps2_Ctrl;

/*                    bit0  (EN_10KEY)   1 = 10-key support enabled. */

#define K_AUTO_SWITCH bit1  /* 1 = Primary keyboard can be switched. */
/* Bits 2 and 3 are used to designate the primary keyboard port.

      Bit
      3 2 Primary Aux Kbd
      --- ----------------
      0 0 Not used
      0 1 Auxiliary Port 1
      1 0 Auxiliary Port 2
      1 1 Not used */

/*                    bit4  (DUAL_MOUSE) 1 = Dual mouse support enabled. */

#define M_AUTO_SWITCH bit5  /* 1 = Primary mouse can be switched. */
/* Bits 6 and 7 are used to designate the primary mouse port.

      Bit
      7 6  Primary Mouse
      --- ----------------
      0 0 Not used
      0 1 Auxiliary Port 1
      1 0 Auxiliary Port 2
      1 1 Auxiliary Port 3 */

/* shiftPRI_KBD is used to shift the bits for the primary keyboard
   port designation to the lower part of the byte.  maskPRI_KBD
   is then used to isolate those bits. */
#define shiftPRI_KBD   2
#define maskPRI_KBD    0x03

/* shiftPRI_MOUSE is used to shift the bits for the primary mouse
   port designation to the lower part of the byte.  maskPRI_MOUSE
   is then used to isolate those bits. */
#define shiftPRI_MOUSE 6
#define maskPRI_MOUSE  0x03

/* ------------------------- Service -------------------------
   Events that require service in main. */
extern VBITS_16 Service;

#define UNLOCK       bit0  /* Device unlock service. */
#define PCI          bit1  /* Host interface service. */
#define AUX_PORT_SND bit2  /* Auxiliary port send to Host. */
#define SEND         bit3  /* Internal device (e.g. scanner request). */
#define KEY          bit4  /* Scanner service. */
#define MS_1         bit5  /* 1 mS elapsed. */
#define PCI3         bit6  /* Third Host interface service. */
#define CORE_7       bit7  /* Reserved for CORE. */
#define PCI2         bit8  /* Secondary Host interface service. */
#define EXT_IRQ_SVC  bit9  /* External interrupt function. */
#define UPD_FLASH    bit10 /* Reserved for CORE. */
#define CORE_11      bit11 /* Reserved for CORE. */
#define CORE_12      bit12 /* Reserved for CORE. */
#define OEM_SRVC_0   bit13 /* OEM Service 0. */
#define OEM_SRVC_1   bit14 /* OEM Service 1. */
#define OEM_SRVC_2   bit15 /* OEM Service 2. */


/* ------------ Led_Ctrl, Led_Data, Save_Kbd_State ----------- */
extern BITS_8 Led_Ctrl; /* Extended LED Control Byte.
                           Accessed through Commands 9Eh and 9Fh. */
extern BITS_8 Led_Data; /* State of local keyboard LEDs. */

#define all byte
#define led bit

#define SCROLL       bit0
#define NUM          bit1
#define CAPS         bit2
#define OVL          bit3
#define LED4         bit4
#define LED5         bit5
#define LED6         bit6
#define LED7         bit7

/* In Save_Kbd_State:

   Bit 0 through 2 are used for LEDs.
   Bit 3 and 4 are used for Scan Code Set number.
   Bit 5 is used for external keyboard enabled flag.

   The bits defined by maskLEDS are the saved state of the LED
   indicators for the external keyboard modified by Keyboard Command EDh.

   The bits defined by maskCODESET are the saved Scan Code Set number
   for the external keyboard modified by Keyboard Command F0h.

   The bit defined by maskAUXENAB is the enabled state of the
   external keyboard modified by Keyboard Command F4h/F5h. */
extern BYTE Save_Kbd_State;

/* maskLEDS is used by Keyboard Command EDh to qualify the data.
   If any bits outside of the mask are attempted to be modifed
   by Keyboard Command EDh, the command is rejected. */
#define maskLEDS     0x07

/* shiftCODESET is used to shift the bits for the saved Scan Code
   Set number to the lower part of the byte.  maskCODESET is then
   used to isolate those bits. */
#define shiftCODESET 3
#define maskCODESET  0x03

/* The enabled state of the external keyboard
   modified by Keyboard Command F4h/F5h. */
#define maskAUXENAB  (1 << 5)

/* ---------------------- Scanner_State ---------------------- */
extern BITS_8 Scanner_State;

#define NUM_LOCK     bit0
#define LEFT         bit1
#define RIGHT        bit2
#define ALT          bit3
#define CONTROL      bit4
#define OVERLAY      bit5  /* Bit is actually in Ext_Cb3.bit.OVL_STATE. */
#define FN           bit6
#define FN_SILENT    bit7  /* bitFN_LATCHED ??? ; key states */

/* ------------------------- Gen_Info ------------------------ */
extern BITS_8 Gen_Info;

#define LED_CHANGE   bit0  /* 1 = LED change in progress. */
#define TYPM_CHANGE  bit1  /* 1 = Typmatic change in progress. */
#define CODE_CHANGE  bit2  /* 1 = Scan code change. */
#define SCAN_LOCKED  bit3  /* 1 = Scanner disabled. */
#define PASS_ENABLE  bit4  /* 1 = Password is activated. */
#define BREAK_AUXD   bit5  /* Used when keypad is on mouse port. */
#define BREAK_AUXK   bit6
#define BREAK_SCAN   bit7  /* Break states for scanner/serial. */

/* ---------------------- HIF_Response -----------------------
   Response code from keyboard.  0 = no response. */
extern BITS_8 HIF_Response[];
#define Kbd_Response HIF_Response[HIF_KBC]

/* Bit 0 through 5 used for code. */
#define maskKBD_RESPONSE_CODE 0x03F
#define SECOND_ACK   bit6  /* Set to indicate a second
                              ACK is to be xmitted. */
#define CMD_RESPONSE bit7  /* Set indicates multi byte response. */

/* NOTE: The following are not "mask" values, they are "absolute" values. */

#define respBAT_OK   0x03  /* BAT completion. */
#define respBAT_FAIL 0x04  /* BAT failure. */
#define respECHO     0x05  /* Echo. */
#define respACK      0x06  /* Transmit acknowledge. */
#define respRESEND   0x07  /* Resend request. */

/* Multibyte response codes. */
#define respCMD_AC   0x80  /* Diagnostic dump.  Sends version string. */
#define respCMD_CFG  0x81  /* Configuration Table. */
#define respARRAY    0x82  /* Sends bytes in an array. */
/* Added for PS/2 mouse emulation. */
#define respCMD_AUX  0x83
/* End of Added. */
/* Codes 83h through 8Fh are reserved for the core code. */
#define respOEM_CODE 0x90  /* First code that may be used by the OEM. */

/* ---------------- Wakeup1, Wakeup2, Wakeup3 ---------------
   Recovery from low power mode - valid wakeup events. */

extern BITS_8 Wakeup1;

#define WAKE1_ANYK      bit0
#define WAKE1_AUX_PORT1 bit1
#define WAKE1_AUX_PORT2 bit2
#define WAKE1_AUX_PORT3 bit3
#define WAKE1_MSWC      bit4
#define WAKE1_HOST_IF   bit5
#define WAKE1_GPIO06    bit6
#define WAKE1_GPIO07    bit7

extern BITS_8 Wakeup2;

#define WAKE2_GPIO44    bit0  /* GPIO44. */
#define WAKE2_GPIO01    bit1  /* GPIO01. */
#define WAKE2_GPIO46    bit2  /* GPIO46. */
#define WAKE2_GPIO03    bit3  /* GPIO03. */
#define WAKE2_GPIO04    bit4  /* GPIO04. */
#define WAKE2_GPIO05    bit5  /* GPIO05. */
#define WAKE2_GPIO06    bit6  /* GPIO06. */
#define WAKE2_GPIO07    bit7  /* GPIO07. */

   /* Wakeup 3 is an edge select for Wakeup2.
      It is also used to select the scanner table.

      0 = falling edge
      1 = rising edge */

extern BITS_8 Wakeup3;

#define WAKE3_GPIO44    bit0  /* GPIO44. */
#define WAKE3_GPIO01    bit1  /* GPIO01. */
#define WAKE3_GPIO46    bit2  /* GPIO46. */
#define WAKE3_GPIO03    bit3  /* GPIO03. */
#define WAKE3_GPIO04    bit4  /* GPIO04. */
#define WAKE3_GPIO05    bit5  /* GPIO05. */
/* Used for scan select bit6 */
/* Used for scan select bit7 */

/* Bits 7 and 6 are used to select the scanner table.

   Bit
   7 6  Selection
   --- -----------
   0 0 RAM Table
   0 1 ROM Table 1
   1 0 ROM Table 2
   1 1 ROM Table 3

   shiftSCAN_SELECT is used to shift the bits for the scanner
   selection designation to the lower part of the byte.
   maskSCAN_SELECT is then used to isolate those bits. */
#define shiftSCAN_SELECT 6
#define maskSCAN_SELECT  0x03

extern BITS_8 Wakeup4;
#define WAKE4_GPIO10    bit0  /* GPIO10. */
#define WAKE4_GPIO11    bit1  /* GPIO11. */
#define WAKE4_GPIO12    bit2  /* GPIO12. */
#define WAKE4_GPIO13    bit3  /* GPIO13. */
#define WAKE4_GPIO14    bit4  /* GPIO14. */
//#define WAKE4_GPIO15    bit5  /* GPIO15. */
#define WAKE4_GPIO16    bit6  /* GPIO16. */
//#define WAKE4_GPIO17    bit7  /* GPIO17. */

extern BITS_8 Wakeup5;
   /* Wakeup 5 is an edge select for Wakeup4.

      0 = falling edge
      1 = rising edge */

#define WAKE5_GPIO10    bit0  /* GPIO10. */
#define WAKE5_GPIO11    bit1  /* GPIO11. */
#define WAKE5_GPIO12    bit2  /* GPIO12. */
#define WAKE5_GPIO13    bit3  /* GPIO13. */
#define WAKE5_GPIO14    bit4  /* GPIO14. */
#define WAKE5_GPIO15    bit5  /* GPIO15. */
#define WAKE5_GPIO16    bit6  /* GPIO16. */
#define WAKE5_GPIO17    bit7  /* GPIO17. */



/* ------------------------ Cmd_Byte ------------------------

   Holds a command byte. */


extern HIF_VAR Hif_Var[];
//extern BYTE Cmd_Byte;

/* --------------- Tmp_Load, Tmp_Byte, Tmp_Pntr --------------
   Temporary byte, pointer and byte array for multibyte responses
   and commands that require more than one data byte. */
//extern BYTE Tmp_Load;
//extern BYTE Tmp_Byte[];
//extern BYTE *Tmp_Pntr;

/* ------------------------ Pass_xxx ------------------------- */
extern BYTE Pass_Buff[];            /* Password buffer. */
extern BYTE Pass_Buff_Idx;          /* Password buffer index
                                       (0 to PASS_SIZE-1). */
extern BYTE Pass_On;                /* Security Code On byte. */
extern BYTE Pass_Off;               /* Security Code Off byte. */
extern BYTE Pass_Make1;             /* Reject make code 1. */
extern BYTE Pass_Make2;             /* Reject make code 2. */

/* ---------------------- Fn_Make_Key1 -----------------------
   Used for local keyboard Fn "binding" processing. */
extern BYTE Fn_Make_Key1;

/* --------------------- Save_Typematic ----------------------
   Save of typematic data for suspend/resume.
   Can be read by Command B5h. */
extern BYTE Save_Typematic;

/* ---------- Aux_Data, Aux_Status, Aux_Port_Ticker ----------
   Aux_Data is byte sent to or received from device on an auxiliary port. */
extern VBITS_8 Aux_Data;
extern AUX_STATUS Aux_Status;
extern BYTE Aux_Port_Ticker;

/* ---------------- Aux_Portx, Aux_Port_Ticker ---------------
   Auxiliary Port 1 through 3.
   The auxiliary keyboard is usually connected to Auxiliary Port 1.
   The external mouse is usually connected to Auxiliary Port 2.
   The internal (non-removable) mouse is connected to Auxiliary Port 3.

   Aux_Port_Ticker is the inactivity timer in resolution of 10 mS.

   Byte in Aux_Portx       For keyboard same as    For mouse same as
   ---------------------   ----------------------  -----------------
   Aux_Portx.state_byte0      Save_Kbd_State         Mouse_State[0]
   Aux_Portx.state_byte1      Save_Typematic         Mouse_State[1]
   Aux_Portx.state_byte2      not used               Mouse_State[2] */

extern AUX_PORT Aux_Port1;
#ifndef AUX_PORT2_SUPPORTED
#error AUX_PORT2_SUPPORTED switch is not defined.
#elif AUX_PORT2_SUPPORTED
extern AUX_PORT Aux_Port2;
#endif
#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#elif AUX_PORT3_SUPPORTED
extern AUX_PORT Aux_Port3;
#endif
typedef enum _SEQUENCE
{
   IDLE,
   ACTIVE,
   TYPE_S1, TYPE_S2, TYPE_S3,
   AA_RCVD,
   RESET1, CMD1,
   IGNORE,
   HOTPLGK, RESETK, WAITRK, CMDK,
   HOTPLGM, RESETM, CMDM
} SEQUENCE;

extern BYTE Expect_Count;

/* -------------------- Timer_A, Timer_B --------------------- */
/* Bits 0 through 3 of TIMER are reserved for "count". */

/* Timer A.  Timer A has a time base of 1 mS. */
extern TIMER Timer_A;
#define HIF2_SAFETY  count /* Secondary host interface timer. */
#define TMR_SCAN     bit4  /* Scanner interval enabled. */
#define TMR_GENTMO   bit5  /* General timeouts. */
/*                   bit6 */
/*                   bit7 */

extern TIMER Timer_A3;
#define HIF3_SAFETY  count /* Third host interface timer. */

extern TIMER Timer_A4;
#define HIF4_SAFETY  count /* Fourth host interface timer. */

extern TIMER Timer_A5;
#define HIF5_SAFETY  count /* Fifth host interface timer. */

/* Timer B.    Timer A has a time base of 150 uS. */
extern TIMER Timer_B;
#define SEND_DELAY   count /* Internal device send delay count used with SEND_ENABLE. */
#define SEND_ENABLE  bit4  /* Internal device send request. */
#define INH_CLEAR    bit5  /* Delay to enable serial clock lines. */
#define UNLOCK_TMO   bit6  /* Delay for unlock request. */
/*                   bit7 */


/* ------------------------ Gen_Timer ------------------------ */
extern VBYTE Gen_Timer;

/* ----------------------- Rc_RAM_Table ----------------------
   Downloadable internal keyboard (scanner) table.

   Includes:
      A byte to hold the number of external keyboard hotkeys
      The bytes for Row/Column Table
      The bytes for Combination Table
      The bytes for Custom Key Table */
#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#elif RAM_SCAN_TABLE
extern BYTE Rc_RAM_Table[];
#endif

/* --------- Config_Table_Offset, Config_Data_Length ---------
   Used for commands that write to and
   read from the Configuration Table. */
extern WORD Config_Table_Offset;
extern BYTE Config_Data_Length;

/* ------------------ Scanner_Strobe_Delay -------------------
   Delay to allow the lines to settle after
   the Write_Strobe() routine was called. */
extern WORD Scanner_Strobe_Delay;

/* ------------------------ Mouse_xxx ------------------------
   Support for Command BCh, Read Saved Mouse Status,
   and for hotplugging. */

extern BYTE Mouse_State[];
extern BYTE Mouse_Cmd;

/* Mouse_State[0] */
#define SCALING_2_1  (1 << 4)
#define ENABLED      (1 << 5)
#define REMOTE_MODE  (1 << 6)

extern BYTE HIF2_Out_Data_Buffer;

/* -------------------------- ECxxx --------------------------
   Embedded Controller Variables for ACPI. */

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#elif ACPI_SUPPORTED

/* Address from Host. */
extern BYTE EC_Addr;

/* Embedded Controller space array. */
extern BYTE EC_Space[];

#endif /* if ACPI_SUPPORTED */

/* ----------------------- Cause_Flags -----------------------
   Arrays for Query Embedded Controller command, QR_EC (0x84)
   and Read SMI cause value command (0x92).  Used to hold the
   cause flags for SCI and SMI events.  Each entry in the array
   holds eight flags.

   The SCI cause numbers run from 1 through
   (NUM_SCI_FLAG_BYTES * 8).

   Byte 0, Bit 0 is cause value  1,
   Byte 0, Bit 1 is cause value  2, ...
   Byte 0, Bit 7 is cause value  8,
   Byte 1, Bit 0 is cause value  9, ...
   Byte 1, Bit 7 is cause value 16...

   The SMI cause numbers start with FIRST_SMI_CAUSE and
   the SMI cause flags are stored in the array starting
   at offset NUM_SCI_FLAG_BYTES.

   Byte x,   Bit 0 is cause value FIRST_SMI_CAUSE,
   Byte x,   Bit 1 is cause value FIRST_SMI_CAUSE+ 1, ...
   Byte x,   Bit 7 is cause value FIRST_SMI_CAUSE+ 7,
   Byte x+1, Bit 0 is cause value FIRST_SMI_CAUSE+ 8, ...
   Byte x+1, Bit 7 is cause value FIRST_SMI_CAUSE+15...



   SCIs and SMIs can be generated with certain commands
   and downloaded hotkeys.  The cause values for these
   are held in SMI_Event_Buffer and SCI_Event_Buffer. */

extern BYTE Cause_Flags[];
extern BYTE SCI_Event_In_Index;
extern BYTE SCI_Event_Out_Index;
extern BYTE SCI_Event_Buffer[];
extern BYTE SMI_Event_In_Index;
extern BYTE SMI_Event_Out_Index;
extern BYTE SMI_Event_Buffer[];

/* -------------------- Smb_Alarm_Buffer ---------------------
   Storage for SMBus originated alarm message.
   Holds the last alarm received.

   Smb_Alarm_Buffer[0] = Address.
   Smb_Alarm_Buffer[1] = Low byte.
   Smb_Alarm_Buffer[2] = High byte. */

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#elif I2C_SUPPORTED
 extern BYTE Smb_Alarm_Buffer[][3];
#endif


extern  BITS_8 SMB_Status[];
#define SMB_OBF         bit0
#define SMB_IBF         bit1
#define SMB_ALARM       bit2
#define SMB_ERROR       bit3
#define SMB_DEV_DENIED  bit4
#define SMB_CMD_DENIED  bit5
#define SMB_PEC_ERROR   bit6
#define SMB_TMO_ERROR   bit7
extern BYTE SMB_Buffer[][SMB_BUFFER_DEPTH];
extern BYTE i2c_slave_buffer[][SMB_BUFFER_DEPTH];

extern WORD T_Count;
extern AD_VALUE AD_Value;

extern BYTE DIDLE_Delay;    // Deep IDLE delay.

#if MOUSE_EMULATION
extern BYTE    *Aux_Tmp_Pntr;
extern BYTE    Aux_Tmp_Load1;
extern BYTE    Aux_Response; // jacob 0419: modify for fix mouse emulation issue
extern WORD    Aux_Send_Timer; //jacob 0508
extern BITS_8 Emu_Aux;
#endif // MOUSE_EMULATION

// Jacob add for new SHM RC scan diagnostic mode
extern BITS_8  SHM_RC_STATUS;
#define DIA_MODE_ACTIVE     bit7
#define NEW_KEY_PRESSED     bit0

extern BYTE    SHM_RC_VALUE;
// end of new SHM RC scan diagnostic mode


extern BYTE DBG[];

#endif /* ifndef PURDAT_C */
#endif /* ifndef PURDAT_H */
