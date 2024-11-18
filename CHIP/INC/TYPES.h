/*-----------------------------------------------------------------------------
 * MODULE TYPES.H
 *
 * Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * C Language implementation dependant definitions, global
 * definitions, and processor dependent preprocessor macros
 * for CompactRISC CPU and VMX Toolset.
 *
 * Revision History (Started April 18, 1995 by GLP)
 *
 * GLP
 * 06 May 97   Added SMB_Busy to INT_VAR.
 * 27 Feb 98   Removed references to EMUL.
 * 09 Mar 98   Added SMB_Service_Request to INT_VAR.
 * 24 Mar 98   Added FUNCT_PTR_S_V definition.
 * 17 Apr 98   Removed the AUX_PORT inh flag.
 * 08 May 98   Added "ticker" to AUX_PORT.
 * 09 Jun 98   Removed Int_Var.SMB_Busy.
 * 22 Sep 98   Added definitions for the Static-Base relative
 *             (sbrel) access mode optimization of the National
 *             Semiconductor compiler.
 * 19 Aug 99   Updated copyright.
 * 01 Sep 99   Removed debug comments.
 * 20 Jan 00   Support to disable internal mouse if external mouse
 *             exists is marked as V21.3.
 * EDH
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * ------------------------------------------------------------------------- */

#ifndef TYPES_H
#define TYPES_H

#include "swtchs.h"

/* --------------------------------------------------------------- */

/* FIELD is used when defining bit-fields within a structure.

   To force the Nuvoton CompactRISC C compiler to allocate one byte
   instead of two in structures (and unions) "unsigned char" is used
   instead of "unsigned" in bit fields.
*/
#define FIELD unsigned char

/* Define a name for a volatile bit in a bitfield. */
typedef volatile FIELD VFLAG;

/* For a variable that is used as a flag (TRUE, FALSE) use FLAG.
   This will be set to the processor's "natural" bit size for
   bit testing to allow the "best" code to be generated.
   For example:

   FLAG done;
   done = FALSE;
   do
   {
      something();
      if (something_or_other) done = TRUE;
   }
   while (!done); */

typedef unsigned int FLAG;

/* For a variable that must be at least 8 bits but it is permissible
   to be larger use SMALL or SSMALL.  SMALL (unsigned) and
   SSMALL (signed) will be set to the processor's "natural"
   variable size (an int).  This is to allow parameters to be passed
   without the overhead of changing from a char to an int.  It should
   also be used for array indices where possible. */

typedef unsigned int SMALL;
typedef signed int SSMALL;

/* If it is important that a memory type is 8 bits, use "BYTE".  A
   byte is 8 bits.  In the VMX implementation a character is 8 bits.
   Use the 'V' prefix term to refer to hardware objects:
        V == VOLATILE.
*/

typedef unsigned char BYTE;
typedef volatile unsigned char VBYTE;
/* <<< 2001/4/27 Added for upper 64k. */
typedef unsigned char FBYTE;
/* >>> 2001/4/27 Added. */

/* If it is important that a memory type is 16 bits, use "WORD" or
   or SWORD, depending on whether it is to be unsigned or signed.
   A word is 16 bits.

   In National's CompactRISC CR16 implementation
   an integer or short integer is 16 bits.

   Use the 'V' prefix term to refer to hardware objects and to
   variables which are subject to change by interrupts.
*/

typedef short SWORD;
typedef volatile short VSWORD;
typedef unsigned short WORD;
typedef volatile unsigned short VWORD;
typedef unsigned long DWORD;
typedef volatile unsigned long VDWORD;

/* The union of a byte and a structure of bits is used
   to allow a byte to be operated on as well as the
   bits.  Initialization may be done by operating directly
   on the byte instead of setting or clearing each bit.
   The union must be constructed CAREFULLY so as to
   enable the bits to line up correctly within the byte.
   The byte may be sent to the Host which expects certain
   bits to be in certain places within the byte.

   Some C compilers assign bits starting with the least
   significant bit.  This is implementation dependant.
   Other compilers may swap the bits.  There would be no
   error generated, but the code would not be correct.

   If bit 0 (the least significant bit in a byte) is allocated
   first, then define BIT0_FIRST as 1.  Otherwise, define it as 0.
   When BIT0_FIRST is defined as 0, the bit-fields in a structure
   are switched to define the higher significant bits first.
   BIT_PADDING is then used to force bit 0 into place.

   For example, if a machine defines the higher bits first
   and the bits are aligned into a word instead of into a byte
   use the following:

   define BIT0_FIRST 0
   define BIT_PADDING FIELD : 8;

   If no bit padding is required do not set it to 0, set it to blank.
*/
#define BIT0_FIRST 1
#define BIT_PADDING

typedef union _BITS_8
{
   BYTE byte;

   struct
   {
      #if BIT0_FIRST
      FIELD bit0: 1;
      FIELD bit1: 1;
      FIELD bit2: 1;
      FIELD bit3: 1;
      FIELD bit4: 1;
      FIELD bit5: 1;
      FIELD bit6: 1;
      FIELD bit7: 1;
      #else
      BIT_PADDING
      FIELD bit7: 1;
      FIELD bit6: 1;
      FIELD bit5: 1;
      FIELD bit4: 1;
      FIELD bit3: 1;
      FIELD bit2: 1;
      FIELD bit1: 1;
      FIELD bit0: 1;
      #endif
   } bit;
} BITS_8;

typedef union _BITS_16
{
   WORD word;

   struct
   {
      #if BIT0_FIRST
      FIELD bit0:  1;
      FIELD bit1:  1;
      FIELD bit2:  1;
      FIELD bit3:  1;
      FIELD bit4:  1;
      FIELD bit5:  1;
      FIELD bit6:  1;
      FIELD bit7:  1;
      FIELD bit8:  1;
      FIELD bit9:  1;
      FIELD bit10: 1;
      FIELD bit11: 1;
      FIELD bit12: 1;
      FIELD bit13: 1;
      FIELD bit14: 1;
      FIELD bit15: 1;
      #else
      FIELD bit15: 1;
      FIELD bit14: 1;
      FIELD bit13: 1;
      FIELD bit12: 1;
      FIELD bit11: 1;
      FIELD bit10: 1;
      FIELD bit9:  1;
      FIELD bit8:  1;
      FIELD bit7:  1;
      FIELD bit6:  1;
      FIELD bit5:  1;
      FIELD bit4:  1;
      FIELD bit3:  1;
      FIELD bit2:  1;
      FIELD bit1:  1;
      FIELD bit0:  1;
      #endif
   } bit;
} BITS_16;

typedef volatile union _VBITS_8
{
   BYTE byte;

   struct
   {
      #if BIT0_FIRST
      FIELD bit0: 1;
      FIELD bit1: 1;
      FIELD bit2: 1;
      FIELD bit3: 1;
      FIELD bit4: 1;
      FIELD bit5: 1;
      FIELD bit6: 1;
      FIELD bit7: 1;
      #else
      BIT_PADDING
      FIELD bit7: 1;
      FIELD bit6: 1;
      FIELD bit5: 1;
      FIELD bit4: 1;
      FIELD bit3: 1;
      FIELD bit2: 1;
      FIELD bit1: 1;
      FIELD bit0: 1;
      #endif
   } bit;
} VBITS_8;

typedef volatile union _VBITS_16
{
   WORD word;

   struct
   {
      #if BIT0_FIRST
      FIELD bit0:  1;
      FIELD bit1:  1;
      FIELD bit2:  1;
      FIELD bit3:  1;
      FIELD bit4:  1;
      FIELD bit5:  1;
      FIELD bit6:  1;
      FIELD bit7:  1;
      FIELD bit8:  1;
      FIELD bit9:  1;
      FIELD bit10: 1;
      FIELD bit11: 1;
      FIELD bit12: 1;
      FIELD bit13: 1;
      FIELD bit14: 1;
      FIELD bit15: 1;
      #else
      FIELD bit15: 1;
      FIELD bit14: 1;
      FIELD bit13: 1;
      FIELD bit12: 1;
      FIELD bit11: 1;
      FIELD bit10: 1;
      FIELD bit9:  1;
      FIELD bit8:  1;
      FIELD bit7:  1;
      FIELD bit6:  1;
      FIELD bit5:  1;
      FIELD bit4:  1;
      FIELD bit3:  1;
      FIELD bit2:  1;
      FIELD bit1:  1;
      FIELD bit0:  1;
      #endif
   } bit;
} VBITS_16;

/* FLAGS are used for general bits. */
typedef struct _FLAGS
{
   FIELD bit0:  1;
   FIELD bit1:  1;
   FIELD bit2:  1;
   FIELD bit3:  1;
   FIELD bit4:  1;
   FIELD bit5:  1;
   FIELD bit6:  1;
   FIELD bit7:  1;
   FIELD bit8:  1;
   FIELD bit9:  1;
   FIELD bit10: 1;
   FIELD bit11: 1;
   FIELD bit12: 1;
   FIELD bit13: 1;
   FIELD bit14: 1;
   FIELD bit15: 1;
} FLAGS;

/* INT_VAR is used for some variables that can be modified by interrupts. */
typedef volatile struct _INT_VAR
{
   FIELD Active_Aux_Port: 2;   /* 0 = no active auxiliary port,
                                  1 = port 1, etc.
                                  The port may not actually be active.
                                  This could signify from which port
                                  an ACK is expected. */
   FIELD Ticker_10:       4;   /* 10 mS timer. */
   FIELD Low_Power_Mode:  1;   /* Was in low power mode. */
   FIELD Scan_Lock:       1;   /* 1 = Scanner transmission locked. */

   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif I2C_SUPPORTED
   FIELD SMB_Service_Request: 1; /* When this is set, 1 mS timer
                                    generates a request for service. */
   #endif
	FIELD Ticker_100:       7;   /* 100 mS timer. */

} INT_VAR;

/* AUX_STATUS is used for auxiliary devices. */
typedef volatile struct _AUX_STATUS
{
   FIELD Bit_Count:       4;   /* Count of bits sent/received. */
   FIELD Parity_Flag:     1;   /* */
   FIELD Send_Flag:       1;   /* 1 = sending data to aux dev. */
   FIELD Data_Bit:        1;   /* Transmit data bit pre-fetch for aux dev. */
   FIELD Send_RST:        1;   /* Send Reset command to Aux in older to get AA, 00 */
} AUX_STATUS;

typedef struct _AUX_PORT
{
   FIELD retry:      2; /* Used for retry count.*/
   FIELD sequence:   4; /* Sequence number for port handling. */
   FIELD delay:      1; /* Delayed transmission to the device. */
   FIELD lock:       1; /* Device locked. */

   FIELD kbd:        1; /* 1 = device is keyboard, 0 = mouse. */
   FIELD valid:      1; /* 0 = error occured. */
   FIELD ack:        1; /* 1 = expect acknowledge from device. */
   FIELD disable:    1; /* 1 = port is disable*/
   FIELD setflag1:   1; /* 1 = sending 2 byte command to auxiliary device. */
   FIELD setflag2:   1; /* 1 = sending 2 byte command to auxiliary device. */
   FIELD intm:       1; /* 1 = The port is Wheel mouse. */
   FIELD intm5:      1; /* 1 = The port is Wheel mouse. */
   FIELD intseq:     1; /* 1 = The port had done the Intelligent mouse initialization. */

   FIELD state_byte0: 8;
   FIELD state_byte1: 8;
   FIELD state_byte2: 8;

   FIELD ticker:      8;
} AUX_PORT;

typedef volatile union _TIMER
{
   BYTE byte;

   struct
   {
      FIELD count: 4;
      FIELD bit4:  1;
      FIELD bit5:  1;
      FIELD bit6:  1;
      FIELD bit7:  1;
   } bit;
} TIMER;

/* >>> Vincent 2003/3/10 Add. */
typedef union _KEYPRO_STAT
{
   BYTE byte;

   struct
   {
      FIELD OBreak:  1; /* Occur Break status. */
      FIELD :  1;
      FIELD :  1;
      FIELD :  1;
      FIELD :  4;
   } field;
} KEYPRO_STAT;
/* <<< End Add. */

typedef union _EXT_CB2
{
   BYTE byte;

   struct
   {
      #if BIT0_FIRST
      FIELD :            1;   /* Bit 0.     */
      FIELD Break_Count: 3;   /* Bit 1 - 3. */
      FIELD :            1;   /* Bit 4.     */
      FIELD Make_Count:  3;   /* Bit 5 - 7. */
      #else
      BIT_PADDING
      FIELD Make_Count:  3;   /* Bit 7 - 5. */
      FIELD :            1;   /* Bit 4.     */
      FIELD Break_Count: 3;   /* Bit 3 - 1. */
      FIELD :            1;   /* Bit 0.     */
      #endif
   } field;
} EXT_CB2;

typedef union _INT_MOUSE
{
   WORD word;
   struct
   {
      FIELD INT_DRV:      1;  // Bit 0.
      FIELD Add4byte:     1;  // Bit 1.
      FIELD Seq:          3;  // Bit 2 - 4.
      FIELD Cmd_Count:    3;  // Bit 5 - 7.
      FIELD Byte_Count:   2;  // Bit 8 - 9.
      FIELD INT_DRV5:     1;  // Bit 10.
      FIELD :             1;  // Bit 11.
      FIELD :             1;  // Bit 12.
      FIELD :             1;  // Bit 13.
      FIELD :             1;  // Bit 14.
      FIELD :             1;  // Bit 15.
   } field;
} INT_MOUSE;

/* FUNCT_PTR_V_V is a pointer to a function that returns nothing
   (V for void) and takes nothing for a parameter (V for void). */
typedef void (*FUNCT_PTR_V_V)(void);

/* FUNCT_PTR_V_W is a pointer to a function that returns nothing
   (V for void) and takes a WORD for a parameter (W for WORD). */
typedef void (*FUNCT_PTR_V_W)(WORD);
/* FUNCT_PTR_V_BP is a pointer to a function that returns nothing
   (V for void) and takes a BYTE Pointer for a parameter
   (BP for BYTE pointer). */
typedef void (*FUNCT_PTR_V_BP)(BYTE *);

/* FUNCT_PTR_V_BP is a pointer to a function that returns nothing
   (V for void) and takes a BYTE Pointer for a parameter
   (BP for BYTE pointer). */
typedef void (*FUNCT_PTR_V_BPB)(BYTE *, BYTE);

/* FUNCT_PTR_W_V is a pointer to a function that returns a WORD
   (W for WORD) and takes nothing for a parameter (V for void). */
typedef WORD (*FUNCT_PTR_W_V)(void);

/* FUNCT_PTR_W_W is a pointer to a function that returns a WORD
   (W for WORD) and takes a WORD for a parameter (W for WORD). */
typedef WORD (*FUNCT_PTR_W_W)(WORD);

/* FUNCT_PTR_V_B8 is a pointer to a function that returns nothing
   (V for void) and takes a BITS_8 for a parameter (B8 for BITS_8). */
typedef void (*FUNCT_PTR_V_B8)(BITS_8);

/* FUNCT_PTR_W_B is a pointer to a function that returns a WORD
   (W for WORD) and takes a BYTE for a parameter (B for BYTE). */
typedef WORD (*FUNCT_PTR_W_B)(BYTE);

/* FUNCT_PTR_W_BB is a pointer to a function that returns a WORD
   (W for WORD) and takes 2 BYTEs for parameters (B for BYTE). */
typedef WORD (*FUNCT_PTR_W_BB)(BYTE, BYTE);

/* FUNCT_PTR_S_V is a pointer to a function that returns a SMALL
   (S for SMALL) and takes nothing for a parameter (V for void). */
typedef DWORD (*FUNCT_PTR_S_V)(void);
typedef DWORD (*FUNCT_PTR_S_B)(BYTE);

typedef VBYTE * PORT_BYTE_PNTR;
typedef VWORD * PORT_WORD_PNTR;
typedef VDWORD * PORT_DWORD_PNTR;

#define PORT_BYTE *(PORT_BYTE_PNTR)
#define PORT_WORD *(PORT_WORD_PNTR)
#define PORT_DWORD *(PORT_DWORD_PNTR)

struct PORT_BYTE_INIT
{
   PORT_BYTE_PNTR pntr;
   VBYTE data;
};


typedef volatile struct _AD_VALUE
{
   WORD  AD0;  /*  */
   WORD  AD1;
   WORD  AD2;
   WORD  AD3;
   WORD  AD4;
   WORD  AD5;
   WORD  AD6;
} AD_VALUE;


typedef union _MS_TIMER
{
   WORD word;

   struct
   {
      #if BIT0_FIRST
      FIELD ones:         4;
      FIELD tens:         4;
      FIELD hundreds:     3;
      FIELD fivehundreds: 1;
      FIELD thousands:    1;
      FIELD y:            3;
      #else
      BIT_PADDING
      FIELD y:            3;
      FIELD thousands:    1;
      FIELD fivehundreds: 1;
      FIELD hundreds:     3;
      FIELD tens:         4;
      FIELD ones:         4;
      #endif
   } field;
} MS_TIMER;

typedef struct _HIF_VAR
{
    BYTE Cmd_Byte;              /* Holds a command byte. */
    BYTE Tmp_Load;              /* Temporary byte. */
    BYTE Tmp_Byte[4];    /* Byte array for multibyte responses */
    BYTE *Tmp_Pntr;             /* Pointer temporary byte. */
} HIF_VAR;

typedef WORD (*FUNCT_PTR_W_HPBB)(HIF_VAR *, BYTE, BYTE);
typedef WORD (*FUNCT_PTR_W_HPBBB)(HIF_VAR *, BYTE, BYTE, BYTE);

#endif /* ifndef TYPES_H */
