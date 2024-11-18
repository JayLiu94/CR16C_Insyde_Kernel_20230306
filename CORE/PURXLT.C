/* ----------------------------------------------------------------------------
 * MODULE PURXLT.C
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
 * The procedure Send_Scan2 of this module is called to translate
 * keyboard scanner contact events into set 2 scan codes.
 *
 * Revision History (Started April 26, 1995 by GLP)
 *
 * GLP
 * 20 Jan 97   Added kcs_vector_table entries #5 (PrintScreen only),
 *             #6 (break only), and #7 (PrntScreen.SysReq).  Enry #7
 *             is same as entry #0, Entry #0 does not work with Fn down.
 *             As a result, SS key #B4 (F11, <Fn> PrntSCR.SysReq)) was
 *             not giving the Fn active code.  Entries #5 and #6 are
 *             used for the new SystemSoft keys 73h and 74h respectively.
 *             Changed access to kcs_vector_table.  The offset
 *             into the table is not:
 *                offset = (6 * index) + (2 * event)
 *             it is:
 *                offset = (3 * index) + event
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 09 Oct 98   Changed to use Ext_Cb3.bit.OVL_STATE instead of
 *             Scanner_State.bit.OVERLAY.
 * 19 Aug 99   Updated copyright and comments.
 * EDH
 * 13 Jan 01   Supported for enhanced keyboard features under WinME.
 * 05 Feb 01 	Added and Modified for supporting the matrix 8*18
 *					Function. If the scan line > 0x0F then output to
 *					the other port as scan output line.
 * ------------------------------------------------------------------------- */

#define PURXLT_C

#include "defs.h"
#include "types.h"
#include "purxlt.h"
#include "purscn.h"
#include "purdat.h"
#include "proc.h"
#include "purhook.h"
#include "oem.h"
#include "purfunct.h"
#if HID_OV_I2C_SUPPORTED
#include "hid.h"
#endif // HID_OV_I2C_SUPPORTED

BITS_8 temp_scanner_state;

#define SSKEY2_SPE_CODE 0x9C
#define SSKEY2_OVL_CODE 0xAA

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static BYTE fn_ov_comb_proc(BYTE index);
static void sskey3_proc(BYTE sskey2, SMALL event);
static BYTE custom_key_proc(BYTE index, SMALL event);
static BYTE calc_index_comb(BYTE comb);
static void simple_code(BYTE scan_code, SMALL event);
static void e0_prefix_code(BYTE scan_code, SMALL event);
static void cursor_shift(BYTE scan_code, SMALL event);
static void cursor_numlock(BYTE scan_code, SMALL event);
static void special_code(BYTE code, SMALL event);
static void control_effect(BYTE state, SMALL event);
static void control_effect_e0(BYTE state, SMALL event);
static void custom_code_proc(const BYTE *pntr, SMALL event);
static BYTE any_combination(WORD data, const BYTE *pntr);
typedef struct _A2_TABLE
{
   BYTE comb;  /* Bit   Setting Combination Key
                  ---   -----------------------
                   7    Overlay
                   6    Latched Fn
                   5    Fn
                   4    Numlock
                   3    Ctrl
                   2    Alt
                   1    Shift
                   0    reserved */
   const BYTE *pntr;    /* Pointer to array for key. */
} A2_TABLE;

const A2_TABLE sskey2_A2_table[];
const BYTE sskey2_overlay_table[];
const BYTE sskey3_80_table[];
const BYTE SS_Qkey_table[];

/* ---------------------------------------------------------------
   Name: Send_Scan2

   Input: key   = Row/Column number
                  key.field.output = 0 - 15
                  key.field.input  = 0 - 7

          event = Contact event
                  MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT

          Scanner_State byte

          Ext_Cb3.bit.OVL_STATE

   On return, Scanner_State byte and Ext_Cb3.bit.OVL_STATE are updated.

   Combination flag for each row/column
      0 = Insyde Software key number 2 in Row/Column Table
      1 = index for control_ptr_table in Row/Column Table

   Insyde Software Key Number 2
      01-7F = Same as Scan Code, Set 2
      80-FF = E0-prefix, cursor, printscreen, pause/break, special key
   --------------------------------------------------------------- */
void Send_Scan2(union KEY key, SMALL event)
{
   BYTE table_entry;
   BYTE temp, fn_temp;
   const BYTE *pntr;
   Gen_Hookc_Key_Proc();

   temp_scanner_state.byte = Scanner_State.byte;
   temp_scanner_state.bit.OVERLAY = Ext_Cb3.bit.OVL_STATE;

   /* Get a sskey2 or index from Row/Column Table. */
   temp = (key.field.input << 4) | key.field.output;
   pntr = Scanner_Table_Pntr + RC_TABLE_INDEX + temp;
   table_entry = *pntr;

   /* Get a combination flag for the key from row/column
         0 = no custom setting with combination key.
         1 = custom setting.
      input(bit 3 - 1) + high bit of output(bit 0) */
   fn_temp = temp = (key.field.input << 1) | (key.field.output >> 3);
   pntr = Scanner_Table_Pntr + COMB_TABLE_INDEX + temp;
   if (bfn_matrix[temp] & Byte_Mask((SMALL) (key.field.output & 7)))
   {
      temp_scanner_state.bit.FN_SILENT = 1;
      Scanner_State.bit.FN_SILENT = 1;
   }
   temp = *pntr;
   if (temp & Byte_Mask((SMALL) (key.field.output & 7)))
   {
      /* Flag is set.  table_entry = index for combination. */
      table_entry = fn_ov_comb_proc(table_entry);
   }

   Send_Key(table_entry, event);

   Scanner_State.bit.FN_SILENT = 0;
   if (event == BREAK_EVENT)
   {
      bfn_matrix[fn_temp]&= ~(1 << (key.field.output & 7));
   }
}

void Send_Key(BYTE table_entry, SMALL event)
{
   BYTE temp;
   const BYTE *pntr;

   /* Mark Buffer in case of overrun. */
   Buffer_Mark();

   temp_scanner_state.byte = Scanner_State.byte;
   temp_scanner_state.bit.OVERLAY = Ext_Cb3.bit.OVL_STATE;

   while (table_entry >= 0xC0)
   {
      /* table_entry >= C0h, custom key.
         Get the sskey2 table entry. */

      table_entry = custom_key_proc(table_entry, event);

      /* If custom key is system control, table_entry == 0. */
   }

   /* At this point, table_entry = 00h to BFh. */

   if (table_entry >= SSKEY2_OVL_CODE)
   {
      /* Overlay key. */

      /* Get index into overlay table. */
      temp = (table_entry - SSKEY2_OVL_CODE);

      /* Multiply by 2 because each entry takes two bytes. */
      temp = temp << 1;

      if (temp_scanner_state.bit.OVERLAY ^ temp_scanner_state.bit.FN_SILENT)
      {
         /* Overlay state xor with Fn state.  Overlay condition.
            Increment index to get the odd byte of the table entry. */
         temp++;
      }

      /* Get a sskey2 value. */
      table_entry = sskey2_overlay_table[temp];
   }
   #if !HID_KBD_SUPPORTED
   if (table_entry >= SSKEY2_SPE_CODE)
   {
      /* Special code like cursor keys, pause/break etc.. */

      /* Get index into table. */
      temp = (table_entry - SSKEY2_SPE_CODE);

      /* table_entry contains the combination table entry. */
      table_entry = sskey2_A2_table[temp].comb;

      /* pntr points to the first key byte. */
      pntr = sskey2_A2_table[temp].pntr;

      /* Check combination.  Point to the correct byte. */
      pntr += calc_index_comb(table_entry);

      /* Get a sskey2. */
      table_entry = *pntr;
   }
   #endif // !HID_KBD_SUPPORTED

   /* Generate scan code, set 2. */
   sskey3_proc(table_entry, event);

   /* Update scanner state. */
   Scanner_State.byte = temp_scanner_state.byte;
   Ext_Cb3.bit.OVL_STATE = temp_scanner_state.bit.OVERLAY;
}

/* ---------------------------------------------------------------
   Name: Clear_Fn_Keys
   --------------------------------------------------------------- */
void Clear_Fn_Keys(void)
{
   Fn_Make_Key1 = 0;
}

/* ---------------------------------------------------------------
   Name: fn_ov_comb_proc

   Input: index for Custom Key Table

   Returns: Insyde Software Key number 2.
   --------------------------------------------------------------- */
static BYTE fn_ov_comb_proc(BYTE index)
{
   const BYTE *pntr;

   if (index < 0x40)
   {
      /* index = 0 through 3Fh, Fn combination.
         High bits of index are already 0.  */

      /* Multiply by 2 because each entry takes two bytes. */
      index = index << 1;

      if (temp_scanner_state.bit.FN_SILENT)
      {
         /* Fn key is pressed.  Bump index to get an odd byte for Fn. */
         index++;
      }

      pntr = Scanner_Table_Pntr + CUSTOM_KEY_TABLE_INDEX + index;
      index = *pntr; /* index = sskey2. */
   }
   else if (index < 0x80)
   {
      /* index = 40h through 7Fh, reserved. */
      index = 0;  /* Set to return null code (00). */
   }
   else if (index < 0xC0)
   {
      /* index = 80h through BFh, Overlay combination. */

      /* Clear out high bits.  Now index = 0 through 3Fh. */
      index &= 0x3F;

      /* Multiply by 2 because each entry takes two bytes. */
      index = index << 1;

      if (temp_scanner_state.bit.OVERLAY)
      {
         /* Overlay key is pressed.  Bump index
            to get an odd byte for overlay. */
         index++;
      }

      pntr = Scanner_Table_Pntr + CUSTOM_KEY_TABLE_INDEX + index;
      index = *pntr; /* index = sskey2. */
   }
   else
   {
      /* index = C0h through FFh, reserved. */
      index = 0;  /* Set to return null code (00). */
   }

   return(index);
}

/* ---------------------------------------------------------------
   Name: sskey3_proc

   Input: sskey2 = Insyde Software Key 2
          event =  Contact event
                   (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
   --------------------------------------------------------------- */


   /* FUNCT_PTR_V_BS is a pointer to a function that returns nothing
      (V for void) and takes a BYTE and a SMALL for parameters
      (B for BYTE, S for SMALL). */
   typedef void (*FUNCT_PTR_V_BS)(BYTE,SMALL);

   /* Definitions for indices into Key Code Process Table. */
   #define DO_SIMPLE_CODE       0
   #define DO_E0_PREFIX_CODE    1
   #define DO_CURSOR_SHIFT      2
   #define DO_CURSOR_NUMLOCK    3
   #define DO_SPECIAL_CODE      4
   #define DO_CONTROL_EFFECT    5
   #define DO_CONTROL_EFFECT_E0 6

   /* Key Code Process Table. */
   const FUNCT_PTR_V_BS kcp_vector_table[] =
   {
      simple_code,      /* index 00h IBM key number 83,84 */
      e0_prefix_code,   /* index 01h */
      cursor_shift,     /* index 02h */
      cursor_numlock,   /* index 03h */
      special_code,     /* index 04h IBM key number 126 */
      control_effect,   /* index 05h Shift, Ctrl, Alt, Numlock */
      control_effect_e0 /* index 06h Ctrl, Alt */
   };

static void sskey3_proc(BYTE sskey2, SMALL event)
{
   BYTE code;
   SMALL index;
	FLAG quick_key = 0;
   if (sskey2 == 0)
   {
      /* Null code. */
      ;
   }
   #if !HID_KBD_SUPPORTED
   else if ((sskey2 & 0x80) == 0)
   {
   	/* <<< V5.1 2001/2/1 modify for MicroSoft Enhance keyboard feature. */
      quick_key = 0;
      if ((sskey2 >= 0x0F) && (sskey2 <= 0x12))
      {
      	sskey2 -= 0x0F;
      	quick_key = 1;
      }
      else if ((sskey2 >= 0x17) && (sskey2 <= 0x19))
      {
      	sskey2 -= 0x13;
      	quick_key = 1;
      }
      else if ((sskey2 >= 0x37) && (sskey2 <= 0x39))
      {
      	sskey2 -= 0x30;
      	quick_key = 1;
      }
      else if ((sskey2 > 0x55) && (sskey2 < 0x58))
      {
      	sskey2 -= 0x4C;
      	quick_key = 1;
      }

      else if ((sskey2 >= 0x5E) && (sskey2 <= 0x60))
      {
      	sskey2 -= 0x52;
      	quick_key = 1;
      }
      else if ((sskey2 >= 0x6D) && (sskey2 <= 0x6F))
      {
      	sskey2 -= 0x5E;
      	quick_key = 1;
      }

      if (quick_key)
      {
	      sskey2 = sskey2 << 1;

	      /* The first byte of the entry in
	         SS_Qkey_table contains the code. */
	      code = SS_Qkey_table[sskey2+0];

	      /* The second byte of the entry in sskey3_80_table
	         contains the index into the kcp_vector_table. */
	      index = SS_Qkey_table[sskey2+1];

	      /* Do procedure. */
	      (kcp_vector_table[index])(code, event);
      }
		else

      /* 01h through 7Fh = scan code. */
      simple_code(sskey2, event);
   }
   #else // !HID_KBD_SUPPORTED
   else if (sskey2 != 0x8E)
   {
      KBD_to_HID(sskey2, event);
   }
   #endif // !HID_KBD_SUPPORTED
   else
   {
      /* 80h through FFh. */

      /* Clear high bit.  Now sskey2 = 00h through 7Fh. */
      sskey2 &= 0x7F;

      /* Use sskey2 as an index into sskey3_80_table.  Multiply
         by 2 because each entry in the table takes two bytes. */
      sskey2 = sskey2 << 1;

      /* The first byte of the entry in
         sskey3_80_table contains the code. */
      code = sskey3_80_table[sskey2+0];

      /* The second byte of the entry in sskey3_80_table
         contains the index into the kcp_vector_table. */
      index = sskey3_80_table[sskey2+1];

      /* Do procedure. */
      (kcp_vector_table[index])(code, event);
   }

   if (Gen_Hookc_enable_FnSticky())
   {
       if(BREAK_EVENT == event)
       {
            if (MASK(FN) == code)
            {
                /* Fn key break event - mark it sticky */
                /* Recover Fn state. */
                temp_scanner_state.bit.FN = 1;
            }
            else if(temp_scanner_state.bit.FN == 1)
            {
                /* Next key after Fn is released - release Fn */
                temp_scanner_state.bit.FN = 0;
            }
       }
   }
}

/* ---------------------------------------------------------------
   Name: custom_key_proc

   Input: lower bits of index = index into Custom Key Table
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)

   Returns: Insyde Software Key number 2.
   --------------------------------------------------------------- */
static BYTE custom_key_proc(BYTE index, SMALL event)
{
   WORD data;
   const BYTE *pntr;

   /* Clear higher bits of index. */
   index &= 0x3F;

   /* Multiply by 2 because each entry takes two bytes. */
   index = index << 1;

   pntr = Scanner_Table_Pntr + CUSTOM_KEY_TABLE_INDEX + index;
   data =  (WORD) *(pntr+0) << 8;
   data |= *(pntr+1);

   if ((data & 0x8000) == 0)
   {
      /* Since table entry bit 15 is not set,
         this is a system control function. */

      if (Gen_Info.bit.PASS_ENABLE == 1)
      {
         ; /* Leave if password is activated. */
      }
      else
      {
         System_Control_Function(data, event);
      }

      /* Setup to return a 0 to cause an
         exit from the Send_Scan2 routine. */
      index = 0;
   }
   else if ((data & 0x4000) == 0)
   {
      /* Since table entry bit 15 is set and bit 14 is
         not set, generate a customized scan code. */

      custom_code_proc(pntr, event);

      /* Setup to return a 0 to cause an
         exit from the Send_Scan2 routine. */
      index = 0;
   }
   else
   {
      /* Since table entry bit 15 and 14 are
         set, this is a combination function. */

      index = any_combination(data, pntr);
   }

   return(index);
}

/* ---------------------------------------------------------------
   Name: calc_index_comb

   Input: combination setting
            Bit   Meaning
            ---   --------
            7     reserved
            6     Overlay
            5     Fn
            4     Num Lock
            3     Ctrl
            2     Alt
            1     Shift
            0     reserved

          temp_scanner_state

   Returns: offset.
   --------------------------------------------------------------- */
static BYTE calc_index_comb(BYTE comb)
{
   BYTE offset;
   SMALL bit_num;

   offset = 0;
   bit_num = 0;

   if (comb & MASK(bit1))
   {
      /* Combination has Shift. */

      if (temp_scanner_state.bit.LEFT | temp_scanner_state.bit.RIGHT)
      {
         /* Either left or right shift is pressed. */
         offset |= Byte_Mask(bit_num);
      }

      bit_num++;  /* Increment bit position. */
   }

   if (comb & MASK(bit2))
   {
      /* Combination has Alt. */

      if (temp_scanner_state.bit.ALT)
      {
         /* Alt is pressed. */
         offset |= Byte_Mask(bit_num);
      }

      bit_num++;  /* Increment bit position. */
   }

   if (comb & MASK(bit3))
   {
      /* Combination has Ctrl. */

      if (temp_scanner_state.bit.CONTROL)
      {
         /* Ctrl is pressed. */
         offset |= Byte_Mask(bit_num);
      }

      bit_num++;  /* Increment bit position. */
   }

   if (comb & MASK(bit4))
   {
      /* Combination has Num Lock. */

      if (temp_scanner_state.bit.NUM_LOCK)
      {
         /* Num Lock has been pressed. */
         offset |= Byte_Mask(bit_num);
      }

      bit_num++;  /* Increment bit position. */
   }

   if (comb & MASK(bit5))
   {
      /* Combination has Fn. */

      if (temp_scanner_state.bit.FN_SILENT)
      {
         /* Num Lock is pressed. */
         offset |= Byte_Mask(bit_num);
      }

      bit_num++;  /* Increment bit position. */
   }

   if (comb & MASK(bit6))
   {
      /* Combination has Overlay. */

      if (temp_scanner_state.bit.OVERLAY)
      {
         /* Num Lock is pressed. */
         offset |= Byte_Mask(bit_num);
      }
   }

   return(offset);
}

/* ---------------------------------------------------------------
   Name: simple_code

   Input: scan_code, Set 2
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
   --------------------------------------------------------------- */
static void simple_code(BYTE scan_code, SMALL event)
{
   if (event == BREAK_EVENT)
   {
      /* Buffer break prefix for break contact.
         F0h is break prefix. */
      Buffer_Key(0xF0);
   }

   /* Buffer base code. */
   Buffer_Key(scan_code);
}

/* ---------------------------------------------------------------
   Name: e0_prefix_code

   Input: scan_code, Set 2
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
   --------------------------------------------------------------- */
static void e0_prefix_code(BYTE scan_code, SMALL event)
{
   /* Buffer E0h prefix. */
   Buffer_Key(0xE0);

   if (event == BREAK_EVENT)
   {
      /* Buffer break prefix for break contact.
         F0h is break prefix. */
      Buffer_Key(0xF0);
   }

   /* Buffer base code. */
   Buffer_Key(scan_code);
}

/* ---------------------------------------------------------------
   BREAK_EVENT key string for cursor_shift and cursor_numlock.
   --------------------------------------------------------------- */
const BYTE csr_brk1[] = {0xE0,0xF0,0x00};

/* ---------------------------------------------------------------
   Name: cursor_shift

   Input: scan_code, Set 2
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
          temp_scanner_state
   --------------------------------------------------------------- */
static void cursor_shift(BYTE scan_code, SMALL event)
{
   static const BYTE csr_sftl_mk[]   = {0xE0,0xF0,0x12,0x00};
   static const BYTE csr_sftr_mk[]   = {0xE0,0xF0,0x59,0x00};
   static const BYTE csr_sftl_brk2[] = {0xE0,0x12,0x00};
   static const BYTE csr_sftr_brk2[] = {0xE0,0x59,0x00};

   if (event == BREAK_EVENT)
   {
      /* Key has just been released.
         This is a "break event". */

      /* Buffer pre-string. */
      Buffer_String(csr_brk1);

      /* Buffer base code. */
      Buffer_Key(scan_code);

      if (temp_scanner_state.bit.LEFT)
      {
         /* Left shift has been pressed. */

         /* Buffer tail-string. */
         Buffer_String(csr_sftl_brk2);
      }

      if (temp_scanner_state.bit.RIGHT)
      {
         /* Right shift has been pressed. */

         /* Buffer tail-string. */
         Buffer_String(csr_sftr_brk2);
      }
   }
   else if (event == MAKE_EVENT)
   {
      /* Key is pressed for the first time.
         This is a "make event". */

      /* The left and/or right shift is pressed. */

      if (temp_scanner_state.bit.LEFT)
      {
         /* Left shift is pressed. */

         /* Buffer pre-string. */
         Buffer_String(csr_sftl_mk);
      }

      if (temp_scanner_state.bit.RIGHT)
      {
         /* Right shift is pressed. */

         /* Buffer pre-string. */
         Buffer_String(csr_sftr_mk);
      }

      /* Buffer end of pre-string. */
      Buffer_Key(0xE0);

      /* Buffer base code. */
      Buffer_Key(scan_code);
   }
   else
   {
      /* Key is being held pressed for a length
         of time.  This is a "repeat event". */

      /* Buffer E0h prefix. */
      Buffer_Key(0xE0);

      /* Buffer base code. */
      Buffer_Key(scan_code);
   }
}

/* ---------------------------------------------------------------
   Name: cursor_numlock

   Input: scan_code, Set 2
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
   --------------------------------------------------------------- */
static void cursor_numlock(BYTE scan_code, SMALL event)
{
   static const BYTE csr_numlock_mk[]   = {0xE0,0x12,0xE0,0x00};
   static const BYTE csr_numlock_brk2[] = {0xE0,0xF0,0x12,0x00};

   if (event == BREAK_EVENT)
   {
      /* Key has just been released.
         This is a "break event". */

      /* Buffer pre-string. */
      Buffer_String(csr_brk1);

      /* Buffer base code. */
      Buffer_Key(scan_code);

      /* Buffer tail-string. */
      Buffer_String(csr_numlock_brk2);
   }
   else if (event == MAKE_EVENT)
   {
      /* Key is pressed for the first time.
         This is a "make event". */

      /* The Num Lock key is pressed. */

      /* Buffer pre-string. */
      Buffer_String(csr_numlock_mk);

      /* Buffer base code. */
      Buffer_Key(scan_code);
   }
   else
   {
      /* Key is being held pressed for a length
         of time.  This is a "repeat event". */

      /* Buffer E0h prefix. */
      Buffer_Key(0xE0);

      /* Buffer base code. */
      Buffer_Key(scan_code);
   }
}

/* ---------------------------------------------------------------
   Name: special_code

   For IBM Key Number 126.
   This key generates only make code.

   Input: code
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
   --------------------------------------------------------------- */
static void special_code(BYTE code, SMALL event)
{
   static const BYTE key126_normal_mk[] = {0xE1,0x14,0x77,0xE1,0xF0,0x14,0xF0,0x77,0x00};
   static const BYTE key126_ctrl_mk[]   = {0xE0,0x7E,0xE0,0xF0,0x7E,0x00};

   if (event == MAKE_EVENT)
   {
      if (code == 0)
      {
         /* Buffer normal code string. */
         Buffer_String(key126_normal_mk);
      }
      else
      {
         /* Buffer Ctrl case string. */
         Buffer_String(key126_ctrl_mk);
      }
   }
}

/* ---------------------------------------------------------------
   Name: control_effect

   Generate scan code set 2 and update scanner status.

   Input: state flags
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
          temp_scanner_state

      The following bits are defined for
      state flags and temp_scanner_state:

         NUM_LOCK
         LEFT
         RIGHT
         ALT
         CONTROL
         OVERLAY
         FN
         FN_SILENT

   Returns: Updated temp_scanner_state
   --------------------------------------------------------------- */
static void control_effect(BYTE state, SMALL event)
{
   /* First generate scan code set 2. */

   if      (state & MASK(LEFT))    simple_code(0x12, event);
   else if (state & MASK(ALT))     simple_code(0x11, event);
   else if (state & MASK(CONTROL)) simple_code(0x14, event);
   else if (state & MASK(RIGHT))   simple_code(0x59, event);

   /* Then update scanner state. */

   if (event == MAKE_EVENT)
   {
      /* Key is pressed for the first time.
         This is a "make event". */

      if (Ext_Cb3.bit.FN_CANCEL == 1)
      {
         /* Any shift cancels Fn condition. */

         if (state & (MASK(LEFT)+MASK(RIGHT)+MASK(ALT)+MASK(CONTROL)))
         {
            /* Left Shift, Right Shift, Alt, or Ctrl is pressed. */
            if (state & MASK(FN))
            {
               /* Fn key is pressed. */

               /* Fn_Make_Key flag = 1. */
               Fn_Make_Key1 |= MASK(FN);

               /* Turn Fn state off. */
               temp_scanner_state.bit.FN = 0;
            }
         }
      }

      if (state & MASK(OVERLAY))
      {
         /* Set scanner state and LED for Overlay. */

         /* Toggle overlay state. */
         temp_scanner_state.bit.OVERLAY = ~temp_scanner_state.bit.OVERLAY;
         Ext_Cb3.bit.OVL_STATE = temp_scanner_state.bit.OVERLAY;

         /* Update LED data. */
         Led_Data.led.OVL = temp_scanner_state.bit.OVERLAY;

         /* Update LEDs. */
         OEM_Write_Leds(Led_Data);
      }
      else
      {
         /* Set scanner state for Shift, Alt, Ctrl, or Fn. */
         temp_scanner_state.byte |= state;
      }
   }
   else if (event == BREAK_EVENT)
   {
      /* Key has just been released.
         This is a "break event". */

      if (state & MASK(OVERLAY))
      {
         /* Overlay is released.  Do nothing. */
         ;
      }
      else
      {
         if (state & (MASK(LEFT)+MASK(RIGHT)+MASK(ALT)+MASK(CONTROL)))
         {
            /* Left Shift, Right Shift, Alt, or Ctrl is released. */

            if (Fn_Make_Key1 & MASK(FN))
            {
               /* Clear Fn_Make_Key flag. */
               Fn_Make_Key1 &= ~(MASK(FN));

               /* Recover Fn state. */
               temp_scanner_state.bit.FN = 1;
            }
         }
         else
         {
            if (state & MASK(FN))
            {
               /* Clear Fn_Make_Key flag. */
               Fn_Make_Key1 &= ~(MASK(FN));
            }
         }

         /* Invert key pressed condition to clear scanner state flags. */
         temp_scanner_state.byte &= ~state;
      }
   }
   else
   {
      /* Key is being held pressed for a length
         of time.  This is a "repeat event".
         Do nothing for repeat event. */
      ;
   }
}

/* ---------------------------------------------------------------
   Name: control_effect_e0

   Generate scan code set 2 and update scanner status.
   Only for Alt-Right and Ctrl-Right.

   Input: state flags
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)
          temp_scanner_state

      The following bits are defined for
      state_flags and temp_scanner_state:

         NUM_LOCK
         LEFT
         RIGHT
         ALT
         CONTROL
         OVERLAY
         FN
         FN_SILENT

   Returns: Updated temp_scanner_state
   --------------------------------------------------------------- */
static void control_effect_e0(BYTE state, SMALL event)
{
   /* This is only called for Alt-Right and Ctrl-Right. */

   BYTE scan_code = 0;

   /* First setup to generate scan code set 2. */

        if (state & MASK(ALT))     scan_code = 0x11;
   else if (state & MASK(CONTROL)) scan_code = 0x14;

   /* Then if scan code is ready, generate
      scan code set 2 and update scanner state. */

   if (scan_code)
   {
      /* Code generation with E0h prefix. */
      e0_prefix_code(scan_code, event);

      if (event == MAKE_EVENT)
      {
         /* Key is pressed for the first time.
            This is a "make event". */

         /* Set one flag in scanner state flags. */
         temp_scanner_state.byte |= state;
      }
      else if (event == BREAK_EVENT)
      {
         /* Key has just been released.
            This is a "break event". */

         /* Clear one flag in scanner state flags. */
         temp_scanner_state.byte &= ~state;
      }
      else
      {
         /* Key is being held pressed for a length
            of time.  This is a "repeat event".
            Do nothing for repeat event. */
         ;
      }
   }
}

/* ---------------------------------------------------------------
   Name: custom_code_proc

   Generate a custom code string.

   Input: pntr to entry in Custom Key Table[index]
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)

      The pntr points to the scan code header.  Its format is as follows:

      *(pntr+0) = 1000,0wxy

         w: 0 = No Repeat Code (make codes only)
            1 = Repeat Code

         x: 0 = Repeat Code is same as make code
            1 = Repeat Code is different from make code

         y: 0 = No Break Code
            1 = Break Code

      *(pntr+1) = zzzz,zzzz

         zzzz,zzzz = Terminator byte

      The code string(s) come after the two byte header.

      A scan code description string is layed
      out in the Custom Key Table, as follows:

          Description            Included in String
      ---------------------   -------------------------
      header word high byte	        always
      header word low byte            always

      make/repeat code 1              always
      make/repeat code 2              always
             .                          .
             .                          .
             .                          .
      make/repeat code n              always
      terminator                      always

      repeat code 1           only when w = 1 and x = 1
      repeat code 2           only when w = 1 and x = 1
             .                          .
             .                          .
             .                          .
      repeat code n           only when w = 1 and x = 1
      terminator              only when w = 1 and x = 1

      break code 1            only when y = 1
      break code 2            only when y = 1
             .                          .
             .                          .
             .                          .
      break code n            only when y = 1
      terminator              only when y = 1
   --------------------------------------------------------------- */
static void custom_code_proc(const BYTE *pntr, SMALL event)
{
   FLAG buffer_codes;
   BYTE flags;
   BYTE end_byte;

   buffer_codes = FALSE;
   flags = *pntr++;
   end_byte = *pntr;

   if (event == BREAK_EVENT)
   {
      /* Key has just been released.
         This is a "break event". */

      if (flags & MASK(bit0))
      {
         /* A break code string exists.
            Buffer the custom break code. */

         /* Skip to the end of the make/repeat string. */
         do
         {
            pntr++;
         }
         while(*pntr != end_byte);

         if (flags & MASK(bit1))
         {
            /* The repeat string is not the same as the make
               string.  Skip to the end of the repeat string. */
            do
            {
               pntr++;
            }
            while(*pntr != end_byte);
         }

         buffer_codes = TRUE;
      }
   }
   else if (event == REPEAT_EVENT)
   {
      /* Key is being held pressed for a length
         of time.  This is a "repeat event". */

      if (flags & MASK(bit2))
      {
         /* A repeat code string exists.
            Buffer the custom repeat code. */

         if (flags & MASK(bit1))
         {
            /* The repeat string is not the same as the make
               string.  Skip to the end of the make/repeat string. */
            do
            {
               pntr++;
            }
            while(*pntr != end_byte);
         }

         buffer_codes = TRUE;
      }
   }
   else
   {
      /* Key is pressed for the first time.
         This is a "make event". */
      buffer_codes = TRUE;
   }

   if (buffer_codes)
   {
      pntr++;
      while (*pntr != end_byte)
      {
         Buffer_Key(*pntr);
         pntr++;
      }
   }
}

/* ---------------------------------------------------------------
   Name: any_combination

   Input: combination setting
            Bit   Meaning
            ---   --------
            8     0=multiple combination, 1=just match

            7     reserved
            6     Overlay
            5     Fn
            4     Num Lock
            3     Ctrl
            2     Alt
            1     Shift
            0     reserved

          pntr points to current entry in Custom Key Table.

          temp_scanner_state

   Returns: Insyde Software Key number 2.
   --------------------------------------------------------------- */
static BYTE any_combination(WORD data, const BYTE *pntr)
{
   BYTE sskey2;
   BITS_8 temp;

   pntr += 2;

   /* pntr points to the first key byte. */

   if (data & 0x0100)
   {
      /* Just matching. */

      temp.byte = 0;

      if (temp_scanner_state.bit.LEFT | temp_scanner_state.bit.RIGHT)
      {
         /* Either left or right shift is pressed. */
         temp.bit.bit1 = 1;
      }

      temp.bit.bit2 = temp_scanner_state.bit.ALT;
      temp.bit.bit3 = temp_scanner_state.bit.CONTROL;
      temp.bit.bit4 = temp_scanner_state.bit.NUM_LOCK;
      temp.bit.bit5 = temp_scanner_state.bit.FN_SILENT;
      temp.bit.bit6 = temp_scanner_state.bit.OVERLAY;

      temp.byte &= data & 0xFF;

      /* Point to the odd byte for overlay. */
      if ((data & 0xFF) == (temp.byte)) pntr++;
   }
   else
   {
      /* Check combination.  Point to the correct byte. */
      pntr += calc_index_comb((BYTE) (data & 0xFF));
   }

   /* Get a sskey2. */
   sskey2 = *pntr;

   return(sskey2);
}

/* ---------------------------------------------------------------
   Name: sskey2_A2_table

   The A2_TABLE structure contains the following.

   BYTE comb;  Bit   Setting Combination Key
               ---   -----------------------
                7    Overlay
                6    Latched Fn
                5    Fn
                4    Numlock
                3    Ctrl
                2    Alt
                1    Shift
                0    reserved
   BYTE *pntr; Pointer to array for key.
   --------------------------------------------------------------- */
const BYTE ss2_9C[] =   /* Printscreen (124) */
{
   0x95, /* Normal */
   0x94, /* Shift */
   0x94, /* Ctrl */
   0x94  /* Shift+Ctrl */
};

const BYTE ss2_9D[] =   /* Printscreen/SysRq (124) */
{
   0x95, /* Normal */
   0x94, /* Shift */
   0x93, /* Alt */
   0x93, /* Shift+Alt */
   0x94, /* Ctrl */
   0x94, /* Ctrl+Shift */
   0x93, /* Ctrl+Alt */
   0x93  /* Ctrl+Alt+Shift */
};

const BYTE ss2_9E[] =   /* Pause-Break (126) */
{
   0x91, /* Normal */
   0x92  /* Ctrl */
};

const BYTE ss2_9F[] =   /* Numpad (95) */
{
   0x96, /* Normal */
   0x97  /* Shift */
};

const BYTE ss2_A0[] =   /* Insert (75) */
{
   0x98, /* Normal */
   0x99, /* Shift */
   0x9A, /* Numlock */
   0x98  /* Shift+Numlock */
};

const BYTE ss2_A1[] =   /* Delete (76) */
{
   0x9B, /* Normal */
   0x9C, /* Shift */
   0x9D, /* Numlock */
   0x9B  /* Shift+Numlock */
};

const BYTE ss2_A2[] =   /* Home */
{
   0x9E, /* Normal */
   0x9F, /* Shift */
   0xA0, /* Numlock */
   0x9E  /* Shift+Numlock */
};

const BYTE ss2_A3[] =   /* (End) */
{
   0xA1, /* Normal */
   0xA2, /* Shift */
   0xA3, /* Numlock */
   0xA1  /* Shift + Numlock */
};

const BYTE ss2_A4[] =   /* (PageUp) */
{
   0xA4, /* Normal */
   0xA5, /* Shift */
   0xA6, /* NumLock */
   0xA4  /* Shift+Numlock */
};

const BYTE ss2_A5[] =   /* PageDown (86) */
{
   0xA7, /* Normal */
   0xA8, /* Shift */
   0xA9, /* Numlock */
   0xA7  /* Shift+Numlock */
};

const BYTE ss2_A6[] =   /* Left_Csr (79) */
{
   0xAA, /* Normal */
   0xAB, /* Shift */
   0xAC, /* Numlock */
   0xAA  /* Shift+Numlock */
};

const BYTE ss2_A7[] =   /* Up_csr (83) */
{
   0xAD, /* Normal */
   0xAE, /* Shift */
   0xAF, /* Numlock */
   0xAD  /* Shift+Numlock */
};

const BYTE ss2_A8[] =   /* Down_csr (84) */
{
   0xB0, /* Normal */
   0xB1, /* Shift */
   0xB2, /* Numlock */
   0xB0  /* Shift+Numlock */
};

const BYTE ss2_A9[] =   /* Right_csr (89) */
{
   0xB3, /* Normal */
   0xB4, /* Shift */
   0xB5, /* Numlock */
   0xB3  /* Shift+Numlock */
};

const A2_TABLE sskey2_A2_table[] =
{
   { 0x0A, ss2_9C }, /* index = 00h. */
   { 0x0E, ss2_9D }, /* index = 01h. */
   { 0x08, ss2_9E }, /* index = 02h. */
   { 0x02, ss2_9F }, /* index = 03h. */
   { 0x12, ss2_A0 }, /* index = 04h. */
   { 0x12, ss2_A1 }, /* index = 05h. */
   { 0x12, ss2_A2 }, /* index = 06h. */
   { 0x12, ss2_A3 }, /* index = 07h. */
   { 0x12, ss2_A4 }, /* index = 08h. */
   { 0x12, ss2_A5 }, /* index = 09h. */
   { 0x12, ss2_A6 }, /* index = 0Ah. */
   { 0x12, ss2_A7 }, /* index = 0Bh. */
   { 0x12, ss2_A8 }, /* index = 0Ch. */
   { 0x12, ss2_A9 }  /* index = 0Dh. */
};

/* ---------------------------------------------------------------
   Name: sskey2_overlay_table
   --------------------------------------------------------------- */
const BYTE sskey2_overlay_table[] =
{
               /* Index    Normal       Overlay
                  -----   --------   ------------- */
   0x3D, 0x6C, /*  00h      7 &      7 Home        */
   0x3E, 0x75, /*  01h      8 *      8 Up Arrow    */
   0x46, 0x7D, /*  02h      9 (      9 Page Up     */
   0x3C, 0x6B, /*  03h      u U      4 Left Arrow  */
   0x43, 0x73, /*  04h      i I      5 (numpad)    */
   0x44, 0x74, /*  05h      o O      6 Right Arrow */
   0x3B, 0x69, /*  06h      j J      1 End         */
   0x42, 0x72, /*  07h      k K      2 Down Arrow  */
   0x4B, 0x7A, /*  08h      l L      3 Page Down   */
   0x3A, 0x70, /*  09h      m M      0 Insert      */
   0x49, 0x71, /*  0Ah      . >      . Delete      */
   0x4A, 0x9F, /*  0Bh      / ?      Numpad /      */
   0x4C, 0x79, /*  0Ch      ; :      +             */
   0x45, 0x7C, /*  0Dh      0 )      *             */
   0x4D, 0x7B, /*  0Eh      p P      -             */
   0x5A, 0x81, /*  0Fh     Enter     Numpad Enter  */
   0x4E, 0x7B, /*  10h      -        -             */
   0x54, 0x81, /*  11h      [ {      Enter         */
// 0x52, 0x7C, /*  12h      , <      *             */
   0x4A, 0x79, /*  12h      / ?      +             */
   0x4C, 0x7B, /*  13h      ; :      -             */
   0x45, 0x9F, /*  14h      0 )      Numpad /      */
   0x4D, 0x7C  /*  15h      p P      *             */
};

/* ---------------------------------------------------------------
   Insyde Software Key Number 2

   00h         Null
   01h - 7Fh   Same as IBM Scan Code, Set2
   80h - BFh   Pre-Index for generation Scan Code, Set2
   C0h - DFh   Pre-Index for PPK function
   C0h - DFh   Pre-Index for SMI function
   --------------------------------------------------------------- */

/* ---------------------------------------------------------------
   Name: sskey3_80_table

   Insyde Software Key Number 2

   80h - BFh   Pre-Index for generation Scan Code, Set2
   --------------------------------------------------------------- */
const BYTE sskey3_80_table[] =
{
   #define LSFT (MASK(LEFT))
   #define RSFT (MASK(RIGHT))
   #define LALT (MASK(ALT))
   #define RALT (MASK(ALT))
   #define LCTL (MASK(CONTROL))
   #define RCTL (MASK(CONTROL))
   #define FNKY (MASK(FN))
   #define OVKY (MASK(OVERLAY))

   /*                               [   ] = Insyde Software Key2, ()=IBM Key */
   0x83, DO_SIMPLE_CODE,         /* [80h] F7 (118) */
   0x5A, DO_E0_PREFIX_CODE,      /* [81h] Numpad Enter (108) */
   0x1F, DO_E0_PREFIX_CODE,      /* [82h] Windows Left */
   0x27, DO_E0_PREFIX_CODE,      /* [83h] Windows Right */
   0x2F, DO_E0_PREFIX_CODE,      /* [84h] Application */
   0x37, DO_E0_PREFIX_CODE,      /* [85h] Power event */
   0x3F, DO_E0_PREFIX_CODE,      /* [86h] Sleep event */
   0x5E, DO_E0_PREFIX_CODE,      /* [87h] Wake event */
   LSFT, DO_CONTROL_EFFECT,      /* [88h] (44) 12 Shift(L) */
   RSFT, DO_CONTROL_EFFECT,      /* [89h] (57) 59 Shift(R) */
   LALT, DO_CONTROL_EFFECT,      /* [8Ah] (60) 11 Alt(L) */
   RALT, DO_CONTROL_EFFECT_E0,   /* [8Bh] (62)    Alt(R) */
   LCTL, DO_CONTROL_EFFECT,      /* [8Ch] (58) 14 Crtl(L) */
   RCTL, DO_CONTROL_EFFECT_E0,   /* [8Dh] (64)    Ctrl(R) */
   FNKY, DO_CONTROL_EFFECT,      /* [8Eh] Fn */
   OVKY, DO_CONTROL_EFFECT,      /* [8Fh] Overlay */
   0x00, DO_SIMPLE_CODE,         /* [90h] Reserved */
   0x00, DO_SPECIAL_CODE,        /* [91h] Pause (126) */
   0x01, DO_SPECIAL_CODE,        /* [92h] Break */
   0x84, DO_SIMPLE_CODE,         /* [93h] (124) Alt-Case (SysRq) */
   0x7C, DO_E0_PREFIX_CODE,      /* [94h] (124) Ctrl or Shift */
   0x7C, DO_CURSOR_NUMLOCK,      /* [95h] (124) base */
   0x4A, DO_E0_PREFIX_CODE,      /* [96h] (95) base */
   0x4A, DO_CURSOR_SHIFT,        /* [97h] (95) Shift */
   0x70, DO_E0_PREFIX_CODE,      /* [98h] (75) base or NumLock+Shift */
   0x70, DO_CURSOR_SHIFT,        /* [99h] (75) Shift */
   0x70, DO_CURSOR_NUMLOCK,      /* [9Ah] (75) NumLock */
   0x71, DO_E0_PREFIX_CODE,      /* [9Bh] (76) base or NumLock+Shift */
   0x71, DO_CURSOR_SHIFT,        /* [9Ch] (76) Shift */
   0x71, DO_CURSOR_NUMLOCK,      /* [9Dh] (76) NumLock */
   0x6C, DO_E0_PREFIX_CODE,      /* [9Eh] (80) base or NumLock+Shift */
   0x6C, DO_CURSOR_SHIFT,        /* [9Fh] (80) Shift */
   0x6C, DO_CURSOR_NUMLOCK,      /* [A0h] (80) NumLock */
   0x69, DO_E0_PREFIX_CODE,      /* [A1h] (81) base or NumLock+Shift */
   0x69, DO_CURSOR_SHIFT,        /* [A2h] (81) Shift */
   0x69, DO_CURSOR_NUMLOCK,      /* [A3h] (81) NumLock */
   0x7D, DO_E0_PREFIX_CODE,      /* [A4h] (85) base or NumLock+Shift */
   0x7D, DO_CURSOR_SHIFT,        /* [A5h] (85) Shift */
   0x7D, DO_CURSOR_NUMLOCK,      /* [A6h] (85) NumLock */
   0x7A, DO_E0_PREFIX_CODE,      /* [A7h] (86) base or NumLock+Shift */
   0x7A, DO_CURSOR_SHIFT,        /* [A8h] (86) Shift */
   0x7A, DO_CURSOR_NUMLOCK,      /* [A9h] (86) NumLock */
   0x6B, DO_E0_PREFIX_CODE,      /* [AAh] (79) base or NumLock+Shift */
   0x6B, DO_CURSOR_SHIFT,        /* [ABh] (79) Shift */
   0x6B, DO_CURSOR_NUMLOCK,      /* [ACh] (79) NumLock */
   0x75, DO_E0_PREFIX_CODE,      /* [ADh] (83) base or NumLock+Shift */
   0x75, DO_CURSOR_SHIFT,        /* [AEh] (83) Shift */
   0x75, DO_CURSOR_NUMLOCK,      /* [AFh] (83) NumLock */
   0x72, DO_E0_PREFIX_CODE,      /* [B0h] (84) base or NumLock+Shift */
   0x72, DO_CURSOR_SHIFT,        /* [B1h] (84) Shift */
   0x72, DO_CURSOR_NUMLOCK,      /* [B2h] (84) NumLock+Shift */
   0x74, DO_E0_PREFIX_CODE,      /* [B3h] (89) base or NumLock+Shift */
   0x74, DO_CURSOR_SHIFT,        /* [B4h] (89) Shift */
   0x74, DO_CURSOR_NUMLOCK       /* [B5h] (89) NumLock+Shift */
};

const BYTE SS_Qkey_table[] =
{
	/* Internet Quick key. */
   0x4D, DO_E0_PREFIX_CODE,      /* [0Fh] Next Track event */
   0x15, DO_E0_PREFIX_CODE,      /* [10h] Prev Track event */
   0x3B, DO_E0_PREFIX_CODE,      /* [11h] Stop event */
   0x34, DO_E0_PREFIX_CODE,      /* [12h] Play/Pause event */
   0x23, DO_E0_PREFIX_CODE,      /* [17h] Mute event */
   0x32, DO_E0_PREFIX_CODE,      /* [18h] Volume Up event */
   0x21, DO_E0_PREFIX_CODE,      /* [19h] Volume Down event */
   0x48, DO_E0_PREFIX_CODE,      /* [37h] Mail event */
   0x10, DO_E0_PREFIX_CODE,      /* [38h] Search event */
   0x3A, DO_E0_PREFIX_CODE,      /* [39h] Web/Home event */
   0x38, DO_E0_PREFIX_CODE,      /* [56h] Back event */
   0x30, DO_E0_PREFIX_CODE,      /* [57h] Forward event */
   0x28, DO_E0_PREFIX_CODE,      /* [5Eh] Stop event */
   0x20, DO_E0_PREFIX_CODE,      /* [5Fh] Refresh event */
   0x18, DO_E0_PREFIX_CODE,      /* [60h] Favorites event */
   0x2B, DO_E0_PREFIX_CODE,      /* [61h] Caluator event */
   0x40, DO_E0_PREFIX_CODE,      /* [62h] My Computer event */
   0x50, DO_E0_PREFIX_CODE,      /* [63h] Media event */
};
