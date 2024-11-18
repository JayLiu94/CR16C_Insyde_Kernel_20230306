/* ----------------------------------------------------------------------------
 * MODULE PURSCN.C
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
 * Contains code to handle local keyboard scanning.
 *
 * Revision History (Started April 24, 1995 by GLP)
 *
 * GLP
 * 21 Jan 97   Removed call to Write_Strobe_Delay().
 *             Read_Scan_Lines includes any needed delay.
 * 24 Jan 97   Added OEM_Update_Ovl_Led() before updating
 *             the LEDs instead of directly changing the LED data
 *             like: "Led_Data.led.OVL = Scanner_State.bit.OVERLAY".
 * 24 Feb 98   Added process_hotkey.  Changed change_valid to allow
 *             Hotkeys to have typematic action.
 * 17 Mar 98   Added include files.
 * 19 Mar 98   Removed use of Generate_SCI.  Other code will
 *             generate an SCI if pending SCI events exist.
 * 23 Mar 98   Disabled interrupts before writing to Int_Var.xxx
 *             variable.
 * 12 May 98   Removed Port_Pin_Status and Port_Pin_Key_Event,
 * 14 May 98   Updated code because ENABLE_PPKT flag was moved.
 * 10 Jul 98   Added code to set Timer_B.bit.INH_CLEAR flag in
 *             Start_Scan_Transmission.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 19 Aug 99   Updated copyright.
 * 24 Aug 99   Added call to Setup_Ext_Hotkey_Pntr in Init_Kbd.
 * EDH
 * 11 Dec 00   Support the fourth PS/2 port as port 4. It will be
 *             recognized as port 0 with Active PS/2 Multiplexing
 *             driver.
 * 05 Jan 01   Removed send the Diagnostic R/C during repeat.
 *             Fixed the NSC compiler issue.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 05 Feb 01 	Added and Modified for supporting the matrix 8*18
 *					Function. If the scan line > 0x0F then output to
 *					the other port as scan output line.
 * 08 Mar 01 	Fixed the matrix can't work issue.
 * 29 May 01   Fixed the 8*18 matrix break code issue.
 *
 * EDH
 * 12 May 15    Moved Keybaord command/data from PURCMD.C.
 * ------------------------------------------------------------------------- */

#define PURSCN_C

#include "swtchs.h"
#include "defs.h"
#include "types.h"
#include "purscn.h"
#include "purcmd.h"
#include "purdat.h"
#include "proc.h"
#include "scan.h"
#include "purxlt.h"
#include "oem.h"
#include "purhook.h"
#include "timers.h"
#include "purmain.h"
#include "staticfg.h"
#include "com_defs.h"
#include "port80.h"
#include "raminit.h"

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static SMALL scan_keys(void);
static void check_tm(union KEY key);
//static void debounce_key(void);
static void debounce_key(BYTE index);
//static void change_valid(void);
static void change_valid(BYTE index);
static void check_scan(BYTE changes, SMALL scan_address);
static void cscfnd(SMALL bit_num, SMALL scan_address);
static void setup_debounce(SMALL bit_num, SMALL scan_address, SMALL event);
static FLAG find_paths(BYTE change_make_key, SMALL scan_address);


/* ---------------------------------------------------------------
   The TM_xxx definitions are part of typematic
   handling for the internal keyboard (scanner).
   --------------------------------------------------------------- */
#if TICKER_1MS
#define TM_SCALE   8
#define TM_REPEAT 10
#define TM_DELAY  55
#else // TICKER_1MS
#define TM_SCALE   2
#define TM_REPEAT  8
#define TM_DELAY  50
#endif // TICKER_1MS
#define KBF_SIZE 16
BYTE bKEY_BUFF[KBF_SIZE];  /* Scanned keyboard buffer storage. */

struct
{
   FIELD kbf_head:   4; /* Keyboard buffer head (0-15). */
   FIELD kbf_tail:   4; /* Keyboard buffer tail (0-15). */
   FIELD kbf_mark:   4; /* Keyboard buffer overflow marker (0-15). */
   FIELD TMscale:    4; /* Typematic scale. */
	/* <<< V5.3 2001/3/8 Modified for matrix 8*18 issue. */
   #if MATRIX_18
   FIELD scan_line:  5;
   #else
   FIELD scan_line:  4;
   #endif
	/* >>> V5.3 2001/3/8 Modified. */
   FIELD saf_make:   1; /* Scan activity flag - make. */
   FIELD saf_break:  1; /* Scan activity flag - break. */
   FIELD saf_keys:   1; /* Scan activity flag - keys active. */
   FIELD ghost:      1; /* Ghost key indicator. */
} scan;

BYTE bTMrepeat;
BYTE bTMdelay;
BYTE bTMcount;

BYTE diode_key;

/* Pointer to current scanner tables. */
const BYTE *Scanner_Table_Pntr;
union KEY new_keyh[KBF_SIZE];     /* Holds new scan key address and debounce status. */
BYTE last_key;
//union KEY new_keyh;     /* Holds new scan key address and debounce status. */
union KEY typematic;    /* Holds new_keyh for typematic action. */

union KEYL
{
   BYTE byte;

   struct
   {
      FIELD state:   1; /* Bit 0. 1 means debounce counted out. */
      FIELD trans:   1; /* Bit 1. 1 for BREAK. 0 for MAKE. */
      //FIELD :        1; /* Bit 2. */
      //FIELD ghost:   1; /* Bit 3. Ghost key indicator. */
      FIELD same:    1; /* Bit 4. 1 = key activity last time
                                         was SAME as this time. */
      FIELD count:   4; /* Bit 5 - 7. Debounce counter. */
   } field;
} ;

union KEYL new_keyl[KBF_SIZE];     /* Holds new scan key address and debounce status. */

/* <<< V5.1 2001/2/5 Modified for matrix 8*18 */
#if MATRIX_18
#define MAX_SCAN_LINES 18
#else
#define MAX_SCAN_LINES 16
#endif
BYTE bscan_matrix[MAX_SCAN_LINES];  /* KI for each KO bit is
                                       stored in bscan_matrix table. */
/* >>> V5.1 2001/2/5 Modified. */

BYTE bfn_matrix[MAX_SCAN_LINES];
/* ---------------------------------------------------------------
   Name: scan_keys

   Scan key matrix once.

   Returns: value != 0, scan activity detected, scan again later.
            value  = 0, no scan activity detected.
   --------------------------------------------------------------- */
   const BYTE grey_table[] = /* Grey code to grey code. */
   {
      #if MATRIX_18
      0x01, 0x03, 0x06, 0x02, /* 0x01 -> 0x03 -> 0x02 -> 0x06 -> */
      0x0C, 0x04, 0x07, 0x05, /* 0x07 -> 0x05 -> 0x04 -> 0x0C -> */
      0x10, 0x08, 0x0B, 0x09, /* 0x0D -> 0x0F -> 0x0E -> 0x0A -> */
      0x0D, 0x0F, 0x0A, 0x0E, /* 0x0B -> 0x09 -> 0x08 -> 0x10 -> */
      0x11, 0x00              /* 0x11 -> 0x00 */
      #else
      0x01, 0x03, 0x06, 0x02, /* 0x01 -> 0x03 -> 0x02 -> 0x06 -> */
      0x0C, 0x04, 0x07, 0x05, /* 0x07 -> 0x05 -> 0x04 -> 0x0C -> */
      0x00, 0x08, 0x0B, 0x09, /* 0x0D -> 0x0F -> 0x0E -> 0x0A -> */
      0x0D, 0x0F, 0x0A, 0x0E  /* 0x0B -> 0x09 -> 0x08 -> 0x00    */
      #endif
   };

/*---------------------------------------------------------------------------------------------------------*/
/* IMPORTANT: The following documentation was NOT written by the writer of the function.                   */
/*                                                                                                         */
/* Function:                                                                                               */
/*   scan_keys                                                                                             */
/*                                                                                                         */
/* Global vars in use:                                                                                     */
/*    scan      -   A struct that holds status info about the scan.                                        */
/*       scan.saf_make  -   Equals to 1 if a MAKE event is being checked                                   */
/*       scan.saf_break -   Equals to 1 if a BREAK event is being checked                                  */
/*       scan.saf_key   -   Equals to 1 if a key is pressed (after the make, before the break).            */
/*    new_keyh  -   Holds information about the newest change in the keyboard matrix (its row\column)      */
/*                  coordinates.                                                                           */
/*    new_keyl  -   Holds more information about the newest change (this info mainly used in debounce_key()*/
/*                  that makes sure that the change we've encountered on the keyboard was made by human,   */
/*                  and that it is not some kind of disturbance).                                          */
/*    typematic -   Holds the information of a key which is currently pressed. After the debounce function */
/*                  approved a new key, typematic is assigned with the new_key data.                       */
/*    diode_key -   .                                                                                      */
/*                                                                                                         */
/* Important note:                                                                                         */
/*    The function manages a kind of state machine which reacts to interrupts from key actions, and from a */
/*    1ms timer. The timer made the function run in a kind of iterations, which in each iteration,         */
/*    decisions about moving from state to state are being made by the function.                           */
/*                                                                                                         */
/* General pseudo-code of the function:                                                                    */
/*    1. Check if there are conditions that prevent us from scanning the keyboard. If so, return.          */
/*    2. Check if a typematic check is necessary (a key is pressed after debounce approved it).            */
/*    3. Check if a debounce check is necessary (a new key press was just encountered, but haven't approved*/
/*       yet). If so, return.                                                                              */
/*    4. Start a full keyboard scan. If a new key press was found, save its data in new_keyh for the next  */
/*       time function would be called.                                                                    */
/*                                                                                                         */
/* Description:                                                                                            */
/*    The goal of the function is to scan the keyboard matrix, to find CHANGES, and to send BREAK, MAKE and*/
/*    REPEAT events to the host according to those changes.                                                */
/*    The function is called by Handle_Scan() which is called from service_scan() (the interrupt handler). */
/*    It will be called in 2 cases:                                                                        */
/*      1. The keyboard was untouched, and then a key was pressed.                                         */
/*      2. The case above connected us with Timer_A's interrupt which occured each 1ms and activates       */
/*         service_scan().                                                                                 */
/*    Each time the function is called (and we should remind ourselves that during a keyboard activity, the*/
/*    function is being called each 1ms), different part/s of the function are being processed.            */
/*    We can divide the function to 4 different parts (and not by the order they appear in the code):      */
/*      1. Full keyboard scan looking for changes in the keyboard matrix.                                  */
/*      2. Debounce check.                                                                                 */
/*      3. Typematic check.                                                                                */
/*      4. Ghost check.                                                                                    */
/*    An explanation about each part will be brought later.                                                */
/*                                                                                                         */
/*    A Running example of a human key press (and release):                                                */
/*    -----------------------------------------------------                                                */
/*    A simple key press scenario starts with a full keyboard scan that finds out which key was pressed.   */
/*    Once it is found, a global var named new_keyh is being updated with the properties of the found key. */
/*    At the next time we will run the function, and due to the fact that new_keyh is assigned, and not    */
/*    equals to 0, we will start the debounce check. As long as we are on debounce "mode", a full scan     */
/*    keyboard is not runned. We will be in debounce "mode" until the debounce function will decide that   */
/*    we have found a legal key press. At this moment, typematic global var will be assigned with the data */
/*    in new_keyh global var, and a MAKE event would be sent to the host. On the next time the function    */
/*    would be called (after 1 ms), a typematic check will be done on the approved key. Debounce check will*/
/*    not take place in this iteration because new_keyh was assigned with 0 at the end of debounce         */
/*    function. Because new_keyh == 0, we are not processing debounce function, and we are not turning done*/
/*    flag on, so the full keyboard would be processed. When the key would be released, we will find       */
/*    another change, a BREAK event. Debounce will check it to make sure it is a real break, and will send */
/*    it to the host. On each time the function ends, it return the keyboard activity status to HandleScan.*/
/*    On the last iteration (the one after BREAK was detected, confirmed by the debounce check and sent to */
/*    host), the activity would be 0, which means no activity was detected. When there is no activity,     */
/*    Handle_Scan() is turning off the timer (infact, the timer itself is not turned off, but the TMR_SCN  */
/*    bit in the timer struct is turned off, and then we don't get interrupts from it that makes           */
/*    service_scan run). From now and on (until another key press), the function would not be called       */
/*    periodically each 1ms.                                                                               */
/*                                                                                                         */
/*
/* Return value:                                                                                           */
/*    Activity status of the keyboard, i.e. if there was a make or break event, or if one of the keys is   */
/*    currently pressed.                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static SMALL scan_keys(void)
{
   BYTE test, lines;
   SMALL index, num_lines;
   FLAG done = FALSE;
   WORD msk_strobe;
   BYTE i;

#if HW_KB_SCN_SUPPORTED
    if (! IS_BIT_SET(KBSEVT, KBSDONE))
    {
        // scan in progress
        return 1;
    }
#endif

   Gen_Hookc_Scan_Keys();

   scan.saf_make = 0;
   scan.saf_break = 0;
   scan.saf_keys = 0;

   if (Ext_Cb0.bit.ENABLE_SCAN == 0)
   {
      /* Scanner is not enabled, exit. */
      done = TRUE;
   }
   else if (Gen_Info.bit.LED_CHANGE || Gen_Info.bit.TYPM_CHANGE ||
            Gen_Info.bit.CODE_CHANGE || Gen_Info.bit.SCAN_LOCKED)
   {
      /* Scanner is disabled by pending command. */
      scan.saf_keys = 1;   /* Return again. */
      done = TRUE;
   }
   else
   {
      /* If typematic in progress, repeat. */
      /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
      #if MATRIX_18
      if (typematic.word) check_tm(typematic);  /* Check typematic. */
      //if (new_keyh[0].word)   /* Exist Debounce key? */
      #else
      if (typematic.byte) check_tm(typematic);  /* Check typematic. */
      //if (new_keyh[0].byte)   /* Exist Debounce key? */
      #endif
      /* >>> V5.1 2001/2/5 Modified. */
      for (i = 0; i < KBF_SIZE; i++)
      {

        #if MATRIX_18
        if (new_keyh[i].word)   /* Exist Debounce key? */
        #else
        if (new_keyh[i].byte)   /* Exist Debounce key? */
        #endif

      {
         /* Yes, bounding. */
//            scan.saf_make = 1;   /* Set debounce make bit. */
            debounce_key(i);
//            done = TRUE;
        }
      }
      /* Otherwise, scan all. */
   }

   if (!done)
   {  /*
      All scan line outputs (KO 0 - 15) are brought low one at a
      time in grey code sequence.  (Grey Code sequence may not be
      necessary with all processors but is used for compatibility
      with the 8051SL.  Grey Code sequence is necessary on the
      8051SL to prevent glitches from the PORT 0 decoder).

      For each KO, the sense line inputs (KI 0 - 7) are read.  For
      each KO, if there is a difference between KI received this
      time and KI received last time through this loop,
      check_scan() is called.  Returns from scan_keys() when all
      KO lines have been processed once. */

      Write_Strobe(scan.scan_line);

      /* Set Scan Matrix Index position. */
      index = scan.scan_line;

      /* KO 0 - 15 are now all 1's except for bit corresponding
         to scan.scan_line.  The bit that is 0 pulls one scan line
         low to be detected by KI 0 - 7 sense line here. */

      msk_strobe = ((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L;

      /* msk_strobe contains a flag for each of the valid
         scanner output lines.  Flag set = output line is used. */

      /* Scan up to MAX_SCAN_LINES output lines. */
      num_lines = MAX_SCAN_LINES;
      do
      {
         lines = Read_Scan_Lines(); /* Read in KI 0 - 7 sense line data. */

         /* --------- Setup Next Strobe --------- */
         scan.scan_line = index;
			/* <<< V5.3 2001/3/8 Modified for matrix 8*18 issue. */
         #if MATRIX_18
         scan.scan_line = grey_table[scan.scan_line];
         if (num_lines) num_lines--;
         #else
         do
         {
            /* Grey code to grey code conversion. */
            scan.scan_line = grey_table[scan.scan_line];

            /* Decrement number of lines (but not under 0). */
            if (num_lines) num_lines--;
         }/* Loop until valid scan line found. */
         while ((msk_strobe & Word_Mask((SMALL) (scan.scan_line))) == 0);
         #endif
			/* >>> V5.3 2001/3/8 Modified. */
         Write_Strobe(scan.scan_line);

         /* --------- Check Key change --------- */

         lines = (~lines) ^ bscan_matrix[index];
         /* Any bit set in "lines" indicates change in
            state from last KI to this KI. */
         /* If a key changed, Check Ghost & Set Debounce. */

         if (lines != 0) check_scan(lines, index);

         if (bscan_matrix[index])
         {
            /* Here, if current still active. */
            scan.saf_keys = 1;   /* Set keys active bits.
                                    Check all key release. */
         }
         /* Otherwise, all break. */

         index = scan.scan_line; /* Set index to next scan line. */

         /* Continue next scan. */
      }
      while(num_lines != 0);

      /* If ghost exist, make key clear. */

      if (scan.ghost == 1)
      {
         /* Ghost flag is set. Clear it. */
         scan.ghost = 0;

         /* new_keyl[0] bits 2, 1, and 0 are zero if no Break.
            COULDN'T JUST THE trans BIT BE CHECKED FOR NO BREAK? */
         test = new_keyl[last_key].byte & 7;
         /* No ghost if Break (if test not zero). */

         if (test == 0)
         {
            /* No Break, diode key? */
            test = Byte_Mask((SMALL) (new_keyh[last_key].field.input));
            test = test & diode_key; /* Check the bit in diode_key byte. */
            /* No ghost if bit in diode_key byte is set (if test not zero). */
         }

         if (test == 0)
         {
            /* Clear new key. */
            /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
            #if MATRIX_18
            new_keyh[last_key].word = 0;
            #else
            new_keyh[last_key].byte = 0;
            #endif
            /* >>> V5.1 2001/2/5 Modified. */
            new_keyl[last_key].byte = 0;
         }
      }
   }

#if HW_KB_SCN_SUPPORTED
    // clear (W1C) KBSDONE bit
    SET_BIT(KBSEVT, KBSDONE);

    // trigger the next scan
    SET_BIT(KBSCTL, KBSSTART);
#endif

   /* Exit scan_keys.
      Return scan activity. */
   return (SMALL) (scan.saf_make + scan.saf_break + scan.saf_keys);
}

/* ---------------------------------------------------------------
   Name: check_tm

   Handle typematic function.

   Input: Typematic key

   Description: Has a count down mechanism, that when it is counted
                out, a REPEAT event is being sent to the host, and
                the counter is being reloaded.
   --------------------------------------------------------------- */
static void check_tm(union KEY key)
{
   BYTE sskey_num;
   SMALL test;
   FLAG send;
   FLAG done = FALSE;
   #if MATRIX_18
   union KEY temp_key;
   #endif

   /* Check Typematic. */
   test = Byte_Mask((SMALL) (key.field.input)); /* Get a mask for the input. */
   test = test & bscan_matrix[key.field.output];/* Check the bit. */
   if (test == 0) /* Release Typematic key? */
   {
      /* Clear Typematic. */
      /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
      #if MATRIX_18
      typematic.word = 0;
      #else
      typematic.byte = 0;
      #endif
      /* >>> V5.1 2001/2/5 Modified. */

      done = TRUE;
   }

   if (!done)
   {
      /* Count down Prescale. */
      scan.TMscale--;
      if (scan.TMscale != 0) done = TRUE;
   }

   if (!done)
   {
      scan.TMscale = TM_SCALE;/* Reload prescale counter. */
      bTMcount--;             /* Count down TMcount. */
      if (bTMcount != 0) done = TRUE;
   }

   if (!done)
   {
      bTMcount = bTMrepeat;   /* Reload TMcount. */

      /* Typematic. */
      if (Ext_Cb3.bit.DIAG_MODE == 0)  /* Normal mode? */
      {
         /* Translation mode.  Convert & Send keycode. */

         #if MATRIX_18
         if (key.field.output > 0x0F)
         {
            temp_key.word = key.word;
            key.field.input = ((temp_key.field.output >> 4) << 3);
            key.field.output = ((temp_key.field.input << 1) |
                                (temp_key.field.output & 0x01));
         }
         #endif
         /* Send keycode of Scan code 2. */
         Send_Scan2(key, REPEAT_EVENT);
      }
   }
}

/* ---------------------------------------------------------------
   Name: debounce_key
   --------------------------------------------------------------- */
//static void debounce_key(void)
static void debounce_key(BYTE index)
{
   BYTE lines, test;

   /* Check Debounce Scan Line. */
   //scan.scan_line = new_keyh[index].field.output;   /* Setup debounce scan line. */
   //Write_Strobe(scan.scan_line);             /* Set KO0-15 corresponding to KO. */

   lines = new_keyh[index].field.output;   /* Setup debounce scan line. */
   Write_Strobe(lines);             /* Set KO0-15 corresponding to KO. */

   lines = Read_Scan_Lines(); /* Get most recent KI0-7 data. */
   lines = ~lines;            /* Inverted so detected switches = 1. */

   /* Get a mask for the key input. */
   test = Byte_Mask((SMALL) (new_keyh[index].field.input));
   /* Check if the switch is pressed. */
   test = test & lines;
   if (test != 0)
   {
      /* Key activity was detected on this key last
         time and the same key is PRESSED this time. */

      if (new_keyl[index].field.trans == 0) new_keyl[index].field.same = 1;
      else                           new_keyl[index].field.same = 0;

      /* If last key detected as a MAKE,  same = 1. */
      /* If last key detected as a BREAK, same = 0. */
   }
   else
   {
      /* Key activity was detected on this key last
         time and the same key is RELEASED this time. */

      if (new_keyl[index].field.trans == 0) new_keyl[index].field.same = 0;
      else                           new_keyl[index].field.same = 1;

      /* If last key detected as a MAKE,  same = 0. */
      /* If last key detected as a BREAK, same = 1. */
   }

   /* same = 0 if key activity last time was DIFFERENT than this time.
      same = 1 if key activity last time was SAME as this time.
               ie: last was MAKE and this is MAKE or last was BREAK and
               this is BREAK.  */

   /* State = 1 means debounce counted out on last debounce check. */
   #if TICKER_1MS
   if (new_keyl[index].field.state == 0)
   {
      /* At this point, debounce did not count out on last debounce check. */
      if (new_keyl[index].field.same == 0)
         {
            /* Key activity last time was DIFFERENT than this time. */
            /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
            #if MATRIX_18
         new_keyh[index].word = 0;
            #else
            /* V5.2 2001/2/27 modified for the bug. */
         new_keyh[index].byte = 0;
            #endif
         new_keyl[index].byte = 0;
            /* V5.2 2001/2/27 Modified. */
            /* >>> V5.1 2001/2/5 Modified. */
         }
      else if (new_keyl[index].field.count != 0)
      {
         /* Decrement debounce counter. */
         new_keyl[index].field.count--;
      }
         else
         {
            /* Key activity last time was SAME as this time. */

            /* Reload count. */
         if (new_keyl[index].field.trans == 1)
            new_keyl[index].field.count = Ext_Cb2.field.Break_Count;
            else
            new_keyl[index].field.count = Ext_Cb2.field.Make_Count;

            /* Set state to signify that debounce counted out. */
         new_keyl[index].field.state = 1;
      }
   }
   else
   #endif // TICKER_1MS
   {
      /* At this point, state = 1.  Debounce counted out last time.

         same = 0 if key activity last time was DIFFERENT than this time.
         same = 1 if key activity last time was SAME as this time.
                ie: last was MAKE and this is MAKE or last was BREAK and
                this is BREAK.  */

      if (new_keyl[index].field.same == 0)
      {
         /* Debounce failed. */
         /* <<< V5.1 2001/2/5 Modified for 8*18 matrix */
         #if MATRIX_18
         new_keyh[index].word = 0;
         #else
         /* V5.2 2001/2/27 modified for the bug. */
         new_keyh[index].byte = 0;
         #endif
         new_keyl[index].byte = 0;
         /* V5.2 2001/2/27 Modified. */
         /* >>> V5.1 2001/2/5 Modified. */
      }
      else
      {
         /* Check Debounce one last time. */

         if (new_keyl[index].field.count != 0)
         {
            /* Decrement debounce counter. */
            new_keyl[index].field.count--;
         }
         else
         {
            /* Debounce OK. */
            change_valid(index);
         }
      }
   }
}

/* ---------------------------------------------------------------
   Name: change_valid
   --------------------------------------------------------------- */
static void change_valid(BYTE index)
{
   BYTE sskey_num, temp, mask;
   FLAG send;
   union KEY temp_key;
   /* Contact event.  MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT. */
   SMALL event;

   mask = Byte_Mask((SMALL) (new_keyh[index].field.input));  /* Get the bit mask for input. */

   /* Change Scan Matrix. */
   temp = bscan_matrix[new_keyh[index].field.output]; /* Get last change from scan_matrix table. */
   temp = temp ^ mask;                         /* Complement bit. */
   bscan_matrix[new_keyh[index].field.output] = temp; /* Put back in scan_matrix table. */

   event = BREAK_EVENT; /* Ready for BREAK indication. */

   if ((mask & temp) != 0)
   {
      /* New MAKE, setup typematic. */

      event = MAKE_EVENT;  /* Ready for MAKE indication. */

      /* Set New Typematic Key. */
      #if MATRIX_18
      typematic.word = new_keyh[index].word;
      #else
      typematic.byte = new_keyh[index].byte;
      #endif
      scan.TMscale = TM_SCALE;
      bTMcount = bTMdelay;
   }

   #if MATRIX_18
   if (new_keyh[index].field.output > 0x0F)
   {
      temp_key.word = new_keyh[index].word;
      new_keyh[index].field.input = ((temp_key.field.output >> 4) << 3);
      new_keyh[index].field.output = ((temp_key.field.input << 1) |
                               (temp_key.field.output & 0x01));
   }
   #endif
   if (event == MAKE_EVENT)
   {
      bfn_matrix[new_keyh[index].field.input << 1 | new_keyh[index].field.output >> 3] |=
                    ((Scanner_State.bit.FN) << (new_keyh[index].field.output & 7));
   }

   if (Ext_Cb3.bit.DIAG_MODE == 1)
   {
      /* For diagnostic mode, send RC position for MAKE event. */
      if (event == MAKE_EVENT)
      {
         /* For MAKE event, send unconverted RC. */
         Buffer_Mark();
         Buffer_Key((new_keyh[index].field.input << 4) | new_keyh[index].field.output);
      }
   }
   #if ECDT_SHM_SUPPORTED
   else if (SHM_RC_STATUS.bit.DIA_MODE_ACTIVE == 1)
   {    /* Windows Matrix is enabled. */
        if (event == MAKE_EVENT)
        {
            SHM_RC_STATUS.bit.NEW_KEY_PRESSED = 1;
            SHM_RC_VALUE = (new_keyh[index].field.input << 4) | new_keyh[index].field.output;
        }
        else if (event == BREAK_EVENT)
        {
            SHM_RC_STATUS.bit.NEW_KEY_PRESSED = 0;
        }
   }
   #endif // ECDT_SHM_SUPPORTED
   else
   {
      /* Translation mode. */

      /* Convert & Send Scan Set 2 Keycode. */
      Send_Scan2(new_keyh[index], event);
   }
   /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
   #if MATRIX_18
   new_keyh[index].word = 0;
   #else
   new_keyh[index].byte = 0;
   #endif
   new_keyl[index].byte = 0;
   /* >>> V5.1 2001/2/5 Modified. */
}

/* ---------------------------------------------------------------
   Name: check_scan

   Input:
      changes:    Any bit set indicates change in
                  state from last KI to this KI.
      scan_address:  Address of strobe line that was
                     low when changes was computed.
   --------------------------------------------------------------- */
static void check_scan(BYTE changes, SMALL scan_address)
{
   /* For the shift and bit test of "changes" to work, "changes"
      MUST be unsigned.  (Right shifting an unsigned quantity fills
      vacated bits with zeros.  Right shifting a signed quantity
      will fill vacated bits with sign bits on some machines and
      zero bits on others.) */

   FLAG flag;
   BYTE change_make_key;
   SMALL bit_num;

   if (Ext_Cb3.bit.DISABLE_NKEY) /* Skip if Nkey. */
   {
      /* Simple Matrix. */

      flag = FALSE;

      if (scan.ghost == 1)
      {
         flag = TRUE;
      }
      else
      {
         /* Find 0 -> 1 's. */
         change_make_key = changes & ~bscan_matrix[scan_address];
         if (change_make_key == 0)
            flag = FALSE;
         else if (find_paths(change_make_key, scan_address))
            flag = TRUE;   /* TRUE if ghost keys. */
      }

      if (flag)
      {
         /* Ghost exists. */
         scan.ghost = 1;  /* Set ghost flag. */

         /* Only look at 1 -> 0 transition & diode key always no ghost. */
         changes &= bscan_matrix[scan_address] | diode_key;
      }
   }

   /* No Ghost. */

   /* Any bit set to 1 in "changes" is processed by cscfnd().
      Because "changes" is unsigned, each shift moves the bit
      under test out and a 0 bit in.  When "changes" == 0, this
      indicates that all change bits have been processed. */
   bit_num = 0;
   while (changes != 0)
   {
      /* Look at changes 1 bit at a time. */
      if (changes & 0x01) cscfnd(bit_num, scan_address);
      bit_num++;
      changes = changes >> 1; /* Shift bit out (and a zero
                                 bit in) to check next bit. */
   }
}

/* ---------------------------------------------------------------
   Name: cscfnd

   Find change bit.  This subroutine is called for
   each bit in this KI that is different from last KI.
   --------------------------------------------------------------- */
static void cscfnd(SMALL bit_num, SMALL scan_address)
{
   /* KI for each KO bit is stored in scan_matrix table.
      bit_num indicates number of bit under test.
   */

   if (bscan_matrix[scan_address] & Byte_Mask(bit_num))
   {
      /* Change ocurred because of a key release or BREAK. */
      if (scan.saf_break == 0)
      {
         scan.saf_break = 1;
         setup_debounce(bit_num, scan_address, BREAK_EVENT);
      }
   }
   else
   {
      /* Change ocurred because of a key press or MAKE.
         Ghost keys can crop up here in membrane keyboards. */

      if (scan.saf_make == 0)
      {
         scan.saf_make = 1;
         setup_debounce(bit_num, scan_address, MAKE_EVENT);
      }
   }
}

/* ---------------------------------------------------------------
   Name: setup_debounce

   scan_address = number of bit of KO.
   bit_num = number of bit that changed from last KI to this KI for KO.
   event = contact event (MAKE_EVENT or BREAK_EVENT).
   --------------------------------------------------------------- */
static void setup_debounce(SMALL bit_num, SMALL scan_address, SMALL event)
{
   // Edward 2012/4/26 Added.
   BYTE index;
   FLAG find;
   find = FALSE;
   for (index = 0; index < KBF_SIZE; index++)
   {
      if (new_keyh[index].field.debounce_status && (new_keyh[index].field.output == scan_address))
      {
        return;
      }
   }

   if (find == FALSE)
   {
      index = 0;
      while ((index < KBF_SIZE) && !find)
      {
         if (new_keyh[index].field.debounce_status)
         {
            index += 1;
            if (index >= KBF_SIZE)
            {
               index = 0;
               find = TRUE;
            }
         }
         else
         {
            find = TRUE;
         }
      }
   }
   last_key = index;
   // End of Edward 2012/4/26 Added.
   new_keyh[index].field.output = scan_address;
   new_keyh[index].field.input = bit_num;
   new_keyh[index].field.debounce_status = 1;

   new_keyl[index].field.state = 0;  /* Debounce has not counted out. */
   new_keyl[index].field.same = 0;   /* Key activity indication. */

   if (event == MAKE_EVENT)
   {
      /* For MAKE key (key pressed). */

      new_keyl[index].field.trans = 0;
      new_keyl[index].field.count = Ext_Cb2.field.Break_Count; /* Shouldn't this be Make_Count? */
   }
   else
   {
      /* For BREAK key (key released). */

      new_keyl[index].field.trans = 1;
      new_keyl[index].field.count = Ext_Cb2.field.Make_Count; /* Shouldn't this be Break_Count? */
   }
}

/*************** MEMBRANE SCAN LOGIC *****************/

/* ---------------------------------------------------------------
   Name: find_paths

   Input: DOWN_STATE  scan_address
          change_make_key

   Returns: FALSE if no paths found, Otherwise returns TRUE.
   --------------------------------------------------------------- */
static FLAG find_paths(BYTE change_make_key, SMALL scan_address)
{
   FLAG paths_found, done;
   BYTE paths, temp, bits;
   SMALL first_address;

   /* For the shift and bit test to work, "bits" MUST be unsigned.
      (Right shifting an unsigned quantity fills vacated bits with
      zeros.  Right shifting a signed quantity will fill vacated
      bits with sign bits on some machines and zero bits on others.) */

   done = FALSE;
   paths_found = FALSE;
   first_address = scan_address;

   change_make_key &= ~(diode_key); /* Ignore diode key. */
   /* change_make_key = bKO_BITS. */

   if (change_make_key == 0) done = TRUE; /* paths_found == FALSE */

   if (!done)
   {
      paths = bscan_matrix[scan_address] | change_make_key;
      paths &= ~(diode_key);  /* Ignore diode key. */
      /* paths = bKO_PATHS. */

      if (paths == 0) done = TRUE;  /* paths_found == FALSE */
   }

   while (!done)
   {
      scan_address++;
      if (scan_address >= MAX_SCAN_LINES)
      {
         scan_address = 0; /* For wrap around. */
      }
      if (scan_address == first_address)
      {
         done = TRUE;      /* No scan lines left.  paths_found == FALSE */
      }
      if (!done)
      {
         /* Check Path. */
         temp = bscan_matrix[scan_address];  /* Any paths? */
         temp &= ~(diode_key);   /* Ignore diode key. */
         if (temp != 0)
         {
            /* Yes, paths. */
            temp &= paths; /* Do paths line up? */
         }

         if (temp != 0)
         {
            /* Yes, paths line up. */
            if (change_make_key != temp)  /* Only 1 bit set? */
            {
               /* No, ghost exists. */
               paths_found = TRUE;
               done = TRUE;
            }

            if (!done)
            {
               /* The following checks if more than one bit is set to 1.
                  Because "bits" is unsigned, each shift moves the bit
                  under test out and a 0 bit in.  When the first 1 bit
                  is found, if "bits" == 0, this indicates that there
                  is only one bit set. */
               bits = paths;  /* Only 1 bit set? */
               temp = FALSE;
               do
               {
                  if (bits & 0x01) temp = TRUE;

                  bits = bits >> 1; /* Shift bit out (and a zero
                                       bit in) to check next bit. */
               }
               while (temp == FALSE);
               if (bits != 0)
               {
                  /* No, more than 1 bit set. */
                  paths_found = TRUE;
                  done = TRUE;
               }
            }
         }
      }
   }

   return (paths_found);
}

/* ---------------------------------------------------------------
   Name: Lock_Scan

   Lock out the scanner via "Scan_Lock"
   and clear pending scanner "SEND" events.
   --------------------------------------------------------------- */
void Lock_Scan(void)
{
   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();
   Int_Var.Scan_Lock = 1;
   Timer_B.bit.SEND_ENABLE = 0;
   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Start_Scan_Transmission

   Lock out other devices so scanner can transmit data to Host,
   and start Timer B with event "SEND_ENABLE".  When the
   "RESPONSE_TIME" has expired, the Timer B interrupt handler
   will post a "SEND" service request to be dispatched by the
   main service loop.  At that time the data will be sent to the
   Host via the "SEND" service handler.
   --------------------------------------------------------------- */
void Start_Scan_Transmission(void)
{
   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();
   Int_Lock_Aux_Port1();
/* 2006/10/14 Mark add  AUX_PORT2_SUPPORTED Switch */
   #ifndef AUX_PORT2_SUPPORTED
   #error AUX_PORT2_SUPPORTED switch is not defined.
   #elif AUX_PORT2_SUPPORTED
   Int_Lock_Aux_Port2();
   #endif
/* 2006/10/14 Mark add enden */
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #elif AUX_PORT3_SUPPORTED
   Int_Lock_Aux_Port3();
   #endif

   /* SEND_DELAY is, actually, more than one bit. */
   Timer_B.bit.SEND_DELAY = RESPONSE_TIME;
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   Timer_B.bit.SEND_ENABLE = 1;
   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Check_Scan_Transmission

   See if a transmission of local keyboard
   data (to the Host) should be allowed.

   Returns:
      Transmission status indication.  TRUE if ok to transmit.
   --------------------------------------------------------------- */
FLAG Check_Scan_Transmission(void)
{
   FLAG allow_transmission = FALSE;
   if ( (Int_Var.Scan_Lock == 0) && /* 1 = Scanner transmission locked. */
        (Flag.SCAN_INH == 0) )      /* 1 = Scanner transmission inhibited. */
   {
      if (scan.kbf_head != scan.kbf_tail) /* Any data in local keyboard buffer? */
      {

         /* Yes. */
         allow_transmission = TRUE; /* Allow transmission. */

      }
   }
   return (allow_transmission);
}

/* ---------------------------------------------------------------
   Name: Init_Kbd

   Initialize internal keyboard (scanner).
   --------------------------------------------------------------- */

void Init_Kbd(void)
{
    BYTE i;
    Setup_Scanner_Pntr();
    #if EXT_HOTKEY
    Setup_Ext_Hotkey_Pntr();
    #endif   // EXT_HOTKEY
    Scanner_Strobe_Delay = Scan_Read_Delay;

    /* Clear debounce logic. */
    /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
    for (i = 0; i < KBF_SIZE; i ++)
    {
        #if MATRIX_18
        new_keyh[i].word = 0;
        #else
        new_keyh[i].byte = 0;
        #endif
       new_keyl[i].byte = 0;
    }
    /* >>> V5.1 2001/2/5 Modified. */
    Clear_Fn_Keys();
    Clear_Key();   /* Clear key matrix / buffer. */
    Clear_Typematic();
}
/* ---------------------------------------------------------------
   Name: Clear_Key

   Clear local keyboard buffer and related variables.
   --------------------------------------------------------------- */
void Clear_Key(void)
{
   SMALL i;
   WORD msk_strobe;

   /* Clear scan matrix. */
   for(i = 0; i < MAX_SCAN_LINES; i++)
   {
      bscan_matrix[i] = 0;
      bfn_matrix[i] = 0;
   }


   /* Clear key buffer. */
   for(i = 0; i < KBF_SIZE; i++) bKEY_BUFF[i] = 0;

   scan.kbf_head = 0;
   scan.kbf_tail = 0;
   /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
   #if MATRIX_18
   typematic.word = 0;
   #else
   typematic.byte = 0;
   #endif
   /* >>> V5.1 2001/2/5 Modified. */

   scan.saf_make = 0;
   scan.saf_break = 0;
   scan.saf_keys = 0;


   /* Initialize scan.scan_line variable. */
   msk_strobe = ((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L;
   /* msk_strobe contains a flag for each of the valid
      scanner output lines.  Flag set = output line is used. */
   scan.scan_line = 0;
   while ((msk_strobe & 0x0001) == 0)
   {
      /* Loop until valid scan line found. */
      scan.scan_line++;
      msk_strobe = msk_strobe >> 1;
   }
}

/* ---------------------------------------------------------------
   Name: Clear_Typematic

   Set default typematic delay and clear type-matic action.
   --------------------------------------------------------------- */
void Clear_Typematic(void)
{
   /* <<< V5.1 2001/2/5 Modified for 8*18 matrix. */
   #if MATRIX_18
   typematic.word = 0;
   #else
   typematic.byte = 0;
   #endif
   /* >>> V5.1 2001/2/5 Modified. */
   bTMdelay = TM_DELAY;
   bTMrepeat = TM_REPEAT;
}

/* ---------------------------------------------------------------
   Name: Set_Typematic

   Set the typematic rate.

   Input: typematic rate
            Bit 4 - 0 Typematic repeat interval.
            Bit 6 - 5 Delay time.
   --------------------------------------------------------------- */
   #if TICKER_1MS
   const BYTE repeat_tbl[] =
   {
       3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  9, 10, 11, 12, 13, 14,
      15, 16, 18, 20, 22, 24, 25, 27, 30, 33, 37, 41, 44, 48, 52, 55
   };

   const BYTE delay_tbl[] =
   {
      27, 55, 83, 111
   };
   #else    // TICKER_1MS
   const BYTE repeat_tbl[] =
   {
       3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  8,  9, 10, 11, 12, 12,
      13, 15, 17, 18, 20, 22, 23, 25, 27, 30, 33, 37, 40, 43, 47, 50
   };

   const BYTE delay_tbl[] =
   {
      25, 50, 75, 100
   };
   #endif   // TICKER_1MS

void Set_Typematic(WORD type_rate)
{
   Save_Typematic = type_rate;   /* Save for suspend/resume. */

   /* Bit 4 - 0 typematic repeat interval index. */
   bTMrepeat = repeat_tbl[type_rate & 0x1F];

   /* Bit 6 - 5 delay time index. */
   bTMdelay = delay_tbl[(type_rate >> 5) & 0x03];
}

/* ---------------------------------------------------------------
   Name: Handle_Scan

   Service the local keyboard in response to a key press.
   --------------------------------------------------------------- */
void Handle_Scan(void)
{
   SMALL scan_activity;

   if (Timer_A.bit.TMR_SCAN == 0)
   {
      /* Scan timer is not running.  Start it. */
      Disable_Irq();
      Timer_A.bit.TMR_SCAN = 1;  /* Activate scan timer. */
      Enable_Timer_A();          /* Load Timer A if not already enabled. */
      Enable_Irq();
   }

   scan_activity = scan_keys();  /* Scan keyboard matrix one time. */

   Disable_Irq();

   if (scan_activity)
   {
      /* There is scan activity.  The scan timer
         will cause a return to Handle_Scan. */
      ;
   }
   else
   {
      /* There is no scan activity. */

      Timer_A.bit.TMR_SCAN = 0;  /* Disable the scan timer. */
      Service.bit.KEY = 0;       /* Clear any pending service request. */
      Enable_Any_Key_Irq();      /* Enable Any Key interrupt. */

   }

   /* Check for new transmission. */

   if (Timer_B.bit.SEND_ENABLE == 0)   /* 1 = Device send request.
                                              (Request already set.) */
   {
      if (Check_Scan_Transmission())   /* Is data available? */
         Start_Scan_Transmission();    /* Yes, start new transmission. */
   }

   Enable_Irq();
}

/****************** SCANNER KEY BUFFER ROUTINES ******************/

/* ---------------------------------------------------------------
   Name: Get_Buffer

   Gets data byte from keyboard buffer (if buffer not empty) and
   updates "scan.kbf_head" with new index into keyboard buffer.

   Returns: Data from buffer; WORD of all ones if buffer is empty.
   --------------------------------------------------------------- */
WORD Get_Buffer(void)
{
   WORD data;

   if (scan.kbf_head != scan.kbf_tail)
   {
      data = (WORD) bKEY_BUFF[scan.kbf_head++];
      if (scan.kbf_head >= KBF_SIZE) scan.kbf_head = 0;
   }
   else
   {
      /* Buffer is empty. */
      data = (WORD) ~0;
   }

   return (data);
}

/* ---------------------------------------------------------------
   Name: Buffer_Mark

   Mark local keyboard buffer tail.
   --------------------------------------------------------------- */
void Buffer_Mark(void)
{
   scan.kbf_mark = scan.kbf_tail;   /* Copy scan.kbf_tail to scan.kbf_mark. */
}

/* ---------------------------------------------------------------
   Name: Buffer_Key

   Input: Row/Column (0iii,oooo) to put in buffer.

   Returns: TRUE operation successful, FALSE unsuccessful.
   --------------------------------------------------------------- */
FLAG Buffer_Key(BYTE row_column)
{
   FLAG flag = TRUE; /* Ready for successful operation. */

   /* Store Data to Buffer Tail. */
   bKEY_BUFF[scan.kbf_tail] = row_column;

   /* Increment Buffer Tail. */
   scan.kbf_tail++;        /* Increment Tail Pointer. */
   if (scan.kbf_tail >= KBF_SIZE)
   {
      scan.kbf_tail = 0;   /* Wrap pointer if too large. */
   }

   /* Check Overflow. */
   if (scan.kbf_tail == scan.kbf_head)
   {
      /* Overflow. */
      scan.kbf_tail = scan.kbf_mark;/* Restore buffer marker. */
      bKEY_BUFF[scan.kbf_tail] = 0; /* Set OverFlow Mark. */
      flag = FALSE;                 /* Ready to Indicate Overflow !!! */
   }

   return (flag);
}

/* ---------------------------------------------------------------
   Name: Buffer_String

   Places a string in the local keyboard buffer (without a
   terminating NULL).  Calls Buffer_Key to place data in key buffer.

   Input: Pointer to null terminated string.

   Returns: 80 Hex if buffer overflow error, else 0.
   --------------------------------------------------------------- */
SMALL Buffer_String(const BYTE *pntr)
{
   register SMALL error = 0;

   while ( (*pntr != 0) && (error == 0) )
   {
      if (Buffer_Key(*pntr++) == 0)
         error = 0x80;  /* Indicate Overflow error. */
   }

   return (error);
}

/* ---------------------------------------------------------------
   Name: sndto_kbd

   Passes data sent from the Host (port 60 write) to the keyboard.

   Input: data = data byte to send
   --------------------------------------------------------------- */
void sndto_kbd(BYTE data)
{
   BITS_8 external;
   AUX_PORT *aux_pntr;
   SMALL port;
   FLAG do_kbd_command = FALSE;
   FLAG do_stk_ack = FALSE;

   if (Gen_Info.bit.CODE_CHANGE)
   {
      /* Handle data that was part of a previously sent keyboard cmd. */

      /* Scan Code Set change. */

      Gen_Info.bit.CODE_CHANGE = 0;

      if (data == 0)
      {
         /* The Host is requesting the Scan Code Set. */

         if (Kbd_Response.byte)
            Kbd_Response.byte = 0x08;  /* Return scan code 2. */
         else
            Kbd_Response.byte = 0x0D;  /* Return scan code 2. */
         /* This clears
            Kbd_Response.bit.CMD_RESPONSE and
            Kbd_Response.bit.SECOND_ACK. */
      }
      else
      {
         /* The Host is setting the Scan Code Set. */

         if ((data & ~maskCODESET) == 0)
         {  /*
            Lock the internal keyboard for Scan Code Set 1 and 3.
            Unlock the internal keyboard for Scan Code Set 2. */
            Gen_Info.bit.SCAN_LOCKED = 1;
            if (data == 2) Gen_Info.bit.SCAN_LOCKED = 0;

            /* Save Scan Code Set number. */
            Save_Kbd_State &= ~(maskCODESET << shiftCODESET);
            Save_Kbd_State |= (data << shiftCODESET);
         }

         do_stk_ack = TRUE;
      }
   }
   else if (Gen_Info.bit.TYPM_CHANGE)
   {
      /* Handle data that was part of a previously sent keyboard cmd. */

      /* Handle data sent with "set typematic rate/delay" command (F3). */

      Gen_Info.bit.TYPM_CHANGE = 0;

      if (data & 0x80) do_kbd_command = TRUE;
      else
      {
         Set_Typematic(data);
         do_stk_ack = TRUE;
      }
   }
   else if (Gen_Info.bit.LED_CHANGE)
   {
      /* Handle data that was part of a previously sent keyboard cmd. */

      /* Handle data sent with update LEDs command (ED). */
      Gen_Info.bit.LED_CHANGE = 0;

      if ((data & ~maskLEDS) != 0)
      {
         do_kbd_command = TRUE;  /* Invalid LED byte. */
      }
      else
      {
         /* data = LED data from command "ED". */

         Save_Kbd_State &= ~maskLEDS;
         Save_Kbd_State |= data; /* Save updated LED state. */

         external.all = data;

         /* Led_Data will hold LED data to write to local keyboard LEDs.
            external holds original LED data sent with "ED" command. */

         if (Led_Ctrl.bit.SCROLL)
         {
            /* Change scroll lock LED. */
            Led_Data.led.SCROLL = external.led.SCROLL;
         }

         if (Led_Ctrl.bit.CAPS)
         {
            /* Change caps lock LED. */
            Led_Data.led.CAPS = external.led.CAPS;
         }

         if (Ext_Cb3.bit.NumLockC_OVR)
         {
            /* Overlay control by numlock enabled. */

            /* Write state to LED data and scanner state. */
            Led_Data = Set_Overlay_Mask(Led_Data, external.led.NUM);
         }
         else
         {
            if (Led_Ctrl.bit.NUM)
            {
               /* Change num lock LED. */
               Led_Data.led.NUM = external.led.NUM;
            }

            /* Update scanner numlock state. */
            Scanner_State.bit.NUM_LOCK = external.led.NUM;
         }

         /* Led_Data = byte to write to local keyboard LEDs.
            external = data = byte to send to external keyboard.
            These bytes will be the same if the Led_Ctrl bits are set. */

         /* OEM_Write_Leds updates Led_Data. */
         OEM_Write_Leds(Led_Data);
         do_stk_ack = TRUE;   /* Pass LED data to external keyboard. */
      }
   }
   else
   {
      do_kbd_command = TRUE;
   }

   if (do_kbd_command)
   {
      switch(data)
      {
         case 0xED:
            /* Update LEDs command. */
            Gen_Info.bit.LED_CHANGE = 1;
            do_stk_ack = TRUE;
            break;

         case 0xEE:
            /* ECHO command. */
            Kbd_Response.byte = 0x0B;
            /* This clears
               Kbd_Response.bit.CMD_RESPONSE and
               Kbd_Response.bit.SECOND_ACK. */
            break;

         case 0xF0:
            /* Set alternate scan codes. */
            Gen_Info.bit.CODE_CHANGE = 1;
            do_stk_ack = TRUE;
            break;

         case 0xF2:
            /* Read manufacturers ID. */
            Kbd_Response.byte = 1;
            /* This clears
               Kbd_Response.bit.CMD_RESPONSE and
               Kbd_Response.bit.SECOND_ACK. */
            break;

         case 0xF3:
            /* Set typematic rate/delay. */
            Gen_Info.bit.TYPM_CHANGE = 1;
            do_stk_ack = TRUE;
            break;

         case 0xF4:
            /* Enable scanning. */

            Save_Kbd_State |= maskAUXENAB;

            Gen_Info.bit.LED_CHANGE = 0;
            Gen_Info.bit.TYPM_CHANGE = 0;
            Gen_Info.bit.CODE_CHANGE = 0;
            Gen_Info.bit.SCAN_LOCKED = 0;
            Clear_Key();
            do_stk_ack = TRUE;
            break;

         case 0xF5:
            /* Default disable. */

            Save_Kbd_State &= ~maskAUXENAB;

            Gen_Info.bit.SCAN_LOCKED = 1;
            Clear_Key();
            Clear_Typematic();
            do_stk_ack = TRUE;
            break;

         case 0xF6:
            /* Set defaults. */
            Gen_Info.bit.LED_CHANGE = 0;
            Gen_Info.bit.TYPM_CHANGE = 0;
            Gen_Info.bit.CODE_CHANGE = 0;
            Gen_Info.bit.SCAN_LOCKED = 0;
            Clear_Key();
            Clear_Typematic();
            do_stk_ack = TRUE;
            break;

         case 0xFF:
            /* Reset keyboard. */

            Gen_Info.bit.LED_CHANGE = 0;
            Gen_Info.bit.TYPM_CHANGE = 0;
            Gen_Info.bit.CODE_CHANGE = 0;
            Gen_Info.bit.SCAN_LOCKED = 0;

            Clear_Key();
            Clear_Typematic();

            Scanner_State.byte = 0;
            Ext_Cb3.bit.OVL_STATE = 0;

            Clear_Fn_Keys();

            /* OEM_Write_Leds updates Led_Data. */
            Led_Data.led.SCROLL = 0;
            Led_Data.led.NUM = 0;
            Led_Data.led.CAPS = 0;
            OEM_Write_Leds(Led_Data);

            Save_Kbd_State = SAVE_KBD_STATE_INIT;
            Save_Typematic = Save_Typematic_Init;

            Pcxae(); /* Enable Keyboard Interface. */

            Kbd_Response.byte = 0x0A;
            /* This clears
               Kbd_Response.bit.CMD_RESPONSE and
               Kbd_Response.bit.SECOND_ACK. */
            break;

         default:
            /* F7-FD "set keys" commands and other non-defined
               commands will be passed to external keyboard. */

            Gen_Hookc_All_Others(data);

            Kbd_Response.byte = respRESEND;  /* Return resend if no response. */
            /* This clears
               Kbd_Response.bit.CMD_RESPONSE and
               Kbd_Response.bit.SECOND_ACK. */
            break;
      }
   }

   if (do_stk_ack)
   {
      if (Kbd_Response.byte)
         Kbd_Response.bit.SECOND_ACK = 1; /* Indicate that a second
                                             ACK is to be transmitted. */
      else
         Kbd_Response.byte = respACK;     /* Respond with ACK. */
         /* This clears
            Kbd_Response.bit.CMD_RESPONSE and
            Kbd_Response.bit.SECOND_ACK. */
   }

   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   /* There can be a keyboard on Auxiliary Port 1 and/or a keyboard on
      Auxiliary Port 2.  The primary keyboard is the keyboard that is
      to accept commands.  Any other keyboard on the other Auxiliary
      Port is a secondary keyboard.  It will be updated to match. */

   /* Reset inactivity timer (whether or not
      data will be sent to an auxilary keyboard). */
   Aux_Port_Ticker = 0;

   /* Get the port number of the primary keyboard. */
   port = (Ps2_Ctrl.byte >> shiftPRI_KBD) & maskPRI_KBD;

   aux_pntr = Make_Aux_Pntr(port);

   if ( (aux_pntr->kbd) && (Check_Transmit(port)) )
   {
      /* OK to send data to keyboard that is on the Auxiliary Port. */

      if (aux_pntr->ack)
      {
         /* Already waiting for an ACK,
            delay sending data to the keyboard. */

         aux_pntr->delay = 1;

         /* Save data in Tmp_Load.  It is safe to save it here
            as long as there is no multibyte command in progress. */
         Hif_Var[HIF_KBC].Tmp_Load = data;
      }
      else
      {
         /* Send data to external keyboard. */

         /* Command being sent to primary keyboard. */
         Aux_Cmd_Setup(aux_pntr, data);
         if (!Send_To_Aux(aux_pntr, port, data))
         {
            /* At this point, no error occurred. */
            aux_pntr->ack = 1;   /* Wait for acknowledge. */
         }
         else
         {  /*
            An error occurred.  If feature is enabled,
            signal that an attempt should be made to
            find a new primary keyboard. */

            if (Ps2_Ctrl.bit.K_AUTO_SWITCH) Flag.NEW_PRI_K = 1;
         }
      }
   }
   else
   {
      /* A mouse compatible device is on the Auxiliary
         Port or the port cannot accept data.

         Keyboard commands will be handled by scanner keyboard.
         "handle_unlock" will indirectly generate a local
         keyboard send request if it finds Kbd_Response.byte is
         non-zero, then the response will be sent to the Host. */

      /* If feature is enabled, signal that an attempt
         should be made to find a new primary keyboard. */
      if (Ps2_Ctrl.bit.K_AUTO_SWITCH) Flag.NEW_PRI_K = 1;
   }
   #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: Set_Overlay_Mask

   Set the overlay state in the scanner_state variable and
   set the led numlock and/or overlay bits for the LED data.

   Input: data = current LED data
          bit_state = 1 to set overlay/numlock bits
                    = 0 to clear overlay/numlock bits

   Returns: updated LED data (num lock and overlay state updated)
   --------------------------------------------------------------- */
BITS_8 Set_Overlay_Mask(BITS_8 data, FLAG bit_state)
{
   if (Led_Ctrl.bit.NUM) data.led.NUM = bit_state;
   if (Led_Ctrl.bit.OVL) data.led.OVL = bit_state;

   Scanner_State.bit.NUM_LOCK = bit_state;
   Ext_Cb3.bit.OVL_STATE = bit_state;

   return(data);
}


