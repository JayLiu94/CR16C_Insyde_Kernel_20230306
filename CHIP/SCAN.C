/*-----------------------------------------------------------------------------
 * MODULE SCAN.C
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
 * Revision History (Started August 13, 1996 by GLP)
 *
 * GLP
 * 12 Dec 96   Added code for scanner interrupt.
 * 21 Jan 97   Moved Read_Scan_Lines() from SCAN.H.
 * 27 Feb 98   Removed references to EMUL.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 12 Oct 98   Added Get_Kbd_Type.
 * 19 Aug 99   Updated copyright.
 * 25 Aug 99   Added Setup_Ext_Hotkey_Pntr.  Changed code to allow
 *             four scanner tables in ROM if the RAM_SCAN_TABLE
 *             option is not used.
 * 07 Sep 99   Changed Get_Kbd_Type to save code space.  Added
 *             Get_Custom_Table_Size.  Changed SCAN_TABLE_SIZE to
 *             MAX_SCAN_TABLE_SIZE.
 * EDH
 * 05 Feb 01   Added and Modified for supporting the matrix 8*18
 *					Function. If the scan line > 0x0F then output to
 *					the other port as scan output line.
 * 08 Mar 01   Fixed the matrix can't work issue.
 * 26 Aug 01   Fixed the Matrix 8*18 issue.
 * NSC PC8763 supported.
 * 18 Feb 05   Replaced 17'th and 18'th scan output line from GPIO
 *             PC.0 and PC.1 to KBSOUT1.
 * ------------------------------------------------------------------------- */

#define SCAN_C

#include "swtchs.h"
#include "purdat.h"
#include "scan.h"
#include "oem.h"
#include "staticfg.h"
#include "purscn.h"
#include "purhook.h"
#include "purxlt.h"
#include "chiphook.h"
#include "com_defs.h"


#if HW_KB_SCN_SUPPORTED
void OEM_Chage_Default_HW_KBS_CFG();
#endif // HW_KB_SCN_SUPPORTED


/* Pointer to end of current external keyboard hotkey table. */
#if EXT_HOTKEY
const BYTE *Ext_Hotkey_Pntr;
#endif  // EXT_HOTKEY

/* ---------------------------------------------------------------
   Name: Enable_Any_Key_Irq

   Setup for any key from scanner to generate an interrupt.

   Lower the "strobe" lines so that when any key is pressed at
   least one input line will go low.  Any bits (out of the
   possible 16) that are not used for the internal keyboard
   (scanner) are left alone.  This is done using
   Msk_Strobe_H and Msk_Strobe_L.

   Then, if the internal keyboard (scanner) is enabled, allow
   the interrupt to occur when a key is pressed on the scanner.

   In the NSC hardware, there are no direction pins.  The two
   keyboard ports are dedicated to the scanner function.  Provision
   is carried over for using some output pins for non-keyscan use.
   --------------------------------------------------------------- */
void Enable_Any_Key_Irq(void)
{
   /* Clear pending input sources used by scanner. */
   WKPCL3 = Msk_Scan_In;

   /* Lower all the lines that are used for the scanner. */
   KBSOUT0 &= ~(((WORD)Msk_Strobe_H << 8) | Msk_Strobe_L);


   #if MATRIX_18
   KBSOUT1 = 0;
   #endif

   if(Ext_Cb0.bit.ENABLE_SCAN)
   {
      /* If internal keyboard (scanner) is enabled ... */

      /* enable wakeup inputs */
      WKEN3 |= Msk_Scan_In;

      /* and enable the interrupt. */
      ICU_Enable_Irq(ICU_EINT_KBD_SCAN);
   }
}

/* ---------------------------------------------------------------
   Name: Enable_Any_Key_Wakeup

   Setup for any key from scanner to wakeup from low power mode.

   Lower the "strobe" lines so that when any key is pressed at
   least one input line will go low.  Any bits (out of the
   possible 16) that are not used for the internal keyboard
   (scanner) are left alone.  This is done using
   Msk_Strobe_H and Msk_Strobe_L.

   Then allow the interrupt to occur when a key is pressed on
   the scanner.

   In the NSC hardware, there are no direction pins.  The two
   keyboard ports are dedicated to the scanner function.  Provision
   is carried over for using some output pins for non-keyscan use.
   --------------------------------------------------------------- */
void Enable_Any_Key_Wakeup(void)
{
   /* Clear pending input sources used by scanner. */
   WKPCL3 = Msk_Scan_In;

   /* Lower all the lines that are used for the scanner. */
   KBSOUT0 &= ~(((WORD)Msk_Strobe_H << 8) | Msk_Strobe_L);
   #if MATRIX_18
   KBSOUT1 = 0;
   #endif
   /* Enable any key wakeup feature. */
   WKEN3 = Msk_Scan_In;         /* Enable input bits
                                          used for scanner. */
   ICU_Enable_Irq(ICU_EINT_KBD_SCAN);  /* Enable interrupt. */
}

/* ---------------------------------------------------------------
   Name: Disable_Any_Key_Wakeup

   Setup to disable the wakeup from scanner.

   Raise the "strobe" lines so that when any key is pressed
   no input lines will go low.  Any bits (out of the possible
   16) that are not used for the internal keyboard (scanner)
   are left alone.  This is done using Msk_Strobe_H and
   Msk_Strobe_L.
   --------------------------------------------------------------- */
void Disable_Any_Key_Wakeup(void)
{
   /* Raise all the lines that are used for the scanner. */
   KBSOUT0 |= ((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L;
   #if MATRIX_18
   KBSOUT1 = 0x03;
   #endif
   /* Disable any key wakeup feature. */
   Disable_Any_Key_Irq();
   WKEN3 &= ~(Msk_Scan_In);
}

/* ---------------------------------------------------------------
   Name: Write_Strobe

   Writes the internal keyboard (scanner) output lines 0 - 15
   based on Scan Line bit number.

   Output lines 0 - 15 will be all floats or open except for the
   bit corresponding to the Scan Line number.  This bit will be
   low to be detected by the sense line input port later.

   Msk_Strobe_H and Msk_Strobe_L specify which of
   the 16 bits on the port are used for the scanner.  (1 = Scanner,
   0 = Other OEM function).  This allows port pins which are not
   used for the scanner output lines to be used for something else.

   Input: Scan line number.
   --------------------------------------------------------------- */
void Write_Strobe(SMALL scan_line_num)
{
    #if HW_KB_SCN_SUPPORTED
    KBS_BUF_INDX = scan_line_num;
    #else // HW_KB_SCN_SUPPORTED
    WORD temp;

    #if MATRIX_18
    BYTE temp18;
    #endif // MATRIX_18

    BYTE    retry;

    /* Raise all lines that are used for the scanner. */
    temp = KBSOUT0 | (((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L);

    KBSOUT0 = temp;
    #if MATRIX_18
    KBSOUT1 = 0x03;
    #endif // MATRIX_18

    // delay till all out lines are really up
    for (retry = 0; retry < 255; retry++) {
        Microsecond_Delay(Scanner_Strobe_Delay);
        if((KBSIN & Msk_Scan_In) == Msk_Scan_In) break;
    }

   #if MATRIX_18
   if (scan_line_num > 0x0F)
   {
      scan_line_num &= 0x0F;
      KBSOUT1 = ~(1 << scan_line_num);
      KBSOUT0 = temp;
   }
   else
   {
      KBSOUT1 = 0x03;
      KBSOUT0 = temp & ~(1 << scan_line_num);
   }
    #else // // MATRIX_18
   /* Lower the line defined by scan_line_num. */
   KBSOUT0 = temp & ~(1 << scan_line_num);
    #endif // MATRIX_18
    #endif // HW_KB_SCN_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: Read_Scan_Lines

   Reads the internal keyboard (scanner) input sense lines 0 - 7.

   Returns: Data from sense lines that are enabled.
            The lines that are disabled return 1s.

   C prototype:
      BYTE Read_Scan_Lines();
   --------------------------------------------------------------- */
BYTE Read_Scan_Lines(void)
{
    #if HW_KB_SCN_SUPPORTED
    return (KBS_BUF_DATA|(BYTE) ~Msk_Scan_In);
    #else // HW_KB_SCN_SUPPORTED
    BYTE scan_data;

    /* Delay to allow the lines to settle after
       the Write_Strobe() routine was called. */
    Microsecond_Delay(Scanner_Strobe_Delay);
    scan_data = KBSIN | (BYTE) ~Msk_Scan_In;

    // raise all out lines for next write strobe
    KBSOUT0 |= (((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L);

    #if MATRIX_18
    KBSOUT1 = 0x03;
    #endif

    return(scan_data);
    #endif // HW_KB_SCN_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: Scan_Init

   Initialize internal keyboard scanner.
   --------------------------------------------------------------- */
void Scan_Init(void)
{
    /* Raise all the lines that are used for the scanner. */
    KBSOUT0 |= ((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L;

    /* Disable wakeup inputs. */
    WKEN3 &= ~(Msk_Scan_In);

    /* Select high to low transition (falling edge). */
    WKEDG3 |= Msk_Scan_In;

    /* Clear pending input sources used by scanner. */
    WKPCL3 = Msk_Scan_In;

    #if HW_KB_SCN_SUPPORTED

    OEM_Chage_Default_HW_KBS_CFG();

    // set automatic HW scan mode
    SET_BIT(KBSCTL, KBSMODE);
    #endif // HW_KB_SCN_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: Setup_Scanner_Pntr

   A code is used to define the possible scanner tables.

      0 Rc_RAM_Table
      1 ROM Table 1
      .     .
      .     .
      .     .
   --------------------------------------------------------------- */
void Setup_Scanner_Pntr(void)
{
#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#endif

    SMALL table_number;
    table_number = (Wakeup3.byte >> shiftSCAN_SELECT) & maskSCAN_SELECT;

    Scanner_Table_Pntr = Gen_Hookc_Get_Rc_Pntr();
    if (Scanner_Table_Pntr == (BYTE *)0)
    {
        /* If the hook set the pointer to NULL,
           point to row/column table here.

           If the RAM_SCAN_TABLE option is not used,
           four tables can be in ROM */

        #if RAM_SCAN_TABLE
        if (table_number == 0)
           /* Point to RAM table. */
           Scanner_Table_Pntr = Rc_RAM_Table;
        else
        #endif
        {
            /* Point to start of ROM tables. */
            Scanner_Table_Pntr = (BYTE *) &Rc_ROM_Tables;

            #if RAM_SCAN_TABLE
            while (table_number > 1)
            #else
            while (table_number > 0)
            #endif
            {
                /* Move pointer past first ROM table. */
                Scanner_Table_Pntr +=         /* Move pointer */
                   1 +                        /* past size of Custom Table, */
                   CUSTOM_KEY_TABLE_INDEX +   /* up to start of Custom Table, */
                   Scanner_Table_Pntr[0] +    /* past Custom Table, */
                   1;                         /* past internal keyboard type and number
                                                      of external keyboard hotkeys byte. */

                /* Bump table number. */
                table_number--;
            }

            /* Move pointer to scanner table. */
            Scanner_Table_Pntr +=   /* Move pointer */
                  1;                /* past size of Custom Table. */
        }
    }
}

/* ---------------------------------------------------------------
   Name: Get_Kbd_Type

   Returns the type of the internal keyboard.

   Returns: 0 = U.S. keyboard, 1 = Japanese keyboard.
   --------------------------------------------------------------- */
FLAG Get_Kbd_Type(void)
{
#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#endif

   BYTE temp;
   const BYTE *pntr;

   temp = Gen_Hookc_Get_Kbd_Type();

   if (temp < 2)
   {
      /* If the hook returned 0, it has determined that the keyboard
         is a U.S. keyboard, return 0.

         If the hook returned 1, it has determined that the keyboard
         is a Japanese keyboard, return 1. */
      return((FLAG) temp);
   }
   else
   {
      /* Find the keyboard type.

         If the RAM_SCAN_TABLE option is not used,
         four tables can be in ROM */

      /* Get table number. */
      temp = (Wakeup3.byte >> shiftSCAN_SELECT) & maskSCAN_SELECT;

      #if RAM_SCAN_TABLE
      if (temp == 0)
      {
         /* Point to byte containing keyboard type. */
         pntr = Rc_RAM_Table + (MAX_SCAN_TABLE_SIZE - 1);
      }
      else
      #endif
      {
         /* Point to start of ROM tables. */
         pntr = (BYTE *) &Rc_ROM_Tables;
         #if RAM_SCAN_TABLE
         while (temp > 1)
         #else
         while (temp > 0)
         #endif
         {
            /* Move pointer past first ROM table. */
            pntr +=                       /* Move pointer */
               1 +                        /* past size of Custom Table, */
               CUSTOM_KEY_TABLE_INDEX +   /* up to start of Custom Table, */
               pntr[0] +                  /* past Custom Table, */
               1;                         /* past internal keyboard type and number
                                             of external keyboard hotkeys byte. */

            /* Bump table number. */
            temp--;
         }

         /* Move pointer to byte containing keyboard type. */
         pntr +=                       /* Move pointer */
            1 +                        /* past size of Custom Table, */
            CUSTOM_KEY_TABLE_INDEX +   /* up to start of Custom Table, */
            pntr[0];                   /* past Custom Table. */
      }

      if (((*pntr) & MASK(bit4)) == 0)
      {
         return(0);  /* U.S. keyboard. */
      }
      else
      {
         return(1);  /* Japanese keyboard. */
      }
   }
}

/* ---------------------------------------------------------------
   Name: Get_Custom_Table_Size

   This routine assumes that the Scanner_Table_Pntr has been setup.

   Returns the size of the Custom Key Table.
   --------------------------------------------------------------- */
SMALL Get_Custom_Table_Size(void)
{
#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#endif

   SMALL rval;

   rval = Gen_Hookc_Get_Custom_Table_Size();

   if (rval == 0xFF)
   {
      /* If the hook set the size to FFh, find the size here.

         If the RAM_SCAN_TABLE option is not used,
         four tables can be in ROM */

      #if RAM_SCAN_TABLE
      if (((Wakeup3.byte >> shiftSCAN_SELECT) & maskSCAN_SELECT) == 0)
      {
         /* The scanner tables are in RAM so the size is fixed. */
         rval = MAX_CUSTOM_TABLE_SIZE;
      }
      else
      #endif
      {
         /* The size is in the byte before the byte pointed to by
            the Scanner_Table_Pntr. */

         rval = *(Scanner_Table_Pntr-1);
      }
   }

   return(rval);
}

#if EXT_HOTKEY
/* ---------------------------------------------------------------
   Name: Setup_Ext_Hotkey_Pntr

   A code is used to define the possible scanner tables.

      0 Rc_RAM_Table
      1 ROM Table 1
      .     .
      .     .
      .     .

   The external keyboard hotkeys are located in the end (the higher
   addresses) of the Custom Key Table.  After the custom key table
   is a byte that holds the number of external keyboard hotkeys.
   Bits 0 - 3 of this byte defines the number of hotkeys.

   The first hotkey definition is at the end (the higher addresses)
   of the Custom Key Table.  The other definitions follow in the
   lower value addresses.  The order of the hotkey scan code and
   hotkey function are setup so the hotkey function is at an even
   address.

   The table looks like:

      .
      .
      .

      WORD holding the hotkey function for hotkey #4.
      BYTE holding the hotkey scan code for hotkey #4.

      BYTE holding the hotkey scan code for hotkey #3.
      WORD holding the hotkey function for hotkey #3.

      WORD holding the hotkey function for hotkey #2.
      BYTE holding the hotkey scan code for hotkey #2.

      BYTE holding the hotkey scan code for hotkey #1.
      WORD holding the hotkey function for hotkey #1.

      BYTE with bits 0 - 3 holding the number of hotkeys.

   The WORDs hold the System Control Function with the high byte of
   the function in the lower address and the low byte of the function
   in the higher address.

   The Ext_Hotkey_Pntr will be set to point to the byte past the end
   of the Custom Key Table to allow the number of hotkeys to be
   accessed.
   --------------------------------------------------------------- */
void Setup_Ext_Hotkey_Pntr(void)
{
#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#endif

   SMALL table_number;

   table_number = (Wakeup3.byte >> shiftSCAN_SELECT) & maskSCAN_SELECT;

   Ext_Hotkey_Pntr = Gen_Hookc_Get_Ext_Hotkey_Pntr();
   if (Ext_Hotkey_Pntr == (BYTE *)0)
   {
      /* If the hook set the pointer to NULL, point to the end of the
         external keyboard hotkey table table here.

         If the RAM_SCAN_TABLE option is not used,
         four tables can be in ROM */

      #if RAM_SCAN_TABLE
      if (table_number == 0)
         Ext_Hotkey_Pntr = Rc_RAM_Table + (MAX_SCAN_TABLE_SIZE - 1);
      else
      #endif
      {
         /* Point to start of ROM tables. */
         Ext_Hotkey_Pntr = (BYTE *) &Rc_ROM_Tables;
         #if RAM_SCAN_TABLE
         while (table_number > 1)
         #else
         while (table_number > 0)
         #endif
         {
            /* Move pointer past first ROM table. */
            Ext_Hotkey_Pntr +=            /* Move pointer */
               1 +                        /* past size of Custom Table, */
               CUSTOM_KEY_TABLE_INDEX +   /* up to start of Custom Table, */
               Ext_Hotkey_Pntr[0] +       /* past Custom Table, */
               1;                         /* past internal keyboard type and number
                                             of external keyboard hotkeys byte. */

            /* Bump table number. */
            table_number--;
         }

         /* Move pointer to end of table. */
         Ext_Hotkey_Pntr +=            /* Move pointer */
            1 +                        /* past size of Custom Table, */
            CUSTOM_KEY_TABLE_INDEX +   /* up to start of Custom Table, */
            Ext_Hotkey_Pntr[0];        /* past Custom Table. */
      }
   }
}
#endif  // EXT_HOTKEY
