/* ----------------------------------------------------------------------------
 * MODULE OEM.C
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
 * Contains OEM specific code. Some "HOOKS" from HOOK.C are copied
 * into this file in order to replace (or augment) core functions.
 *
 * Revision History (Started July 23, 1996 by GLP)
 *
 * GLP
 * 12/06/96 Added OEM_detect_crisis routine.
 * 05/05/97 Added OEM_ACPI_Gen_Int.
 * 02/13/98 Updated OEM_detect_crisis for new Boot Code definition.
 * 06/29/98 Updated for use with CORE 1.01.20.
 * 10/05/98 Updated for new Keyboard Controller functionality.
 * 08/05/99 Moved OEM_detect_crisis to the OEMCRSIS.C file.
 *          Updated copyright.
 * ------------------------------------------------------------------------- */

#define OEM_C

#include "swtchs.h"
#include "oem.h"
#include "types.h"
#include "purdat.h"
#include "regs.h"
#include "staticfg.h"
#include "proc.h"
#include "regs.h"
#include "ini.h"
#include "oem.h"
#include "id.h"
#include "purcver.h"
#include "com_defs.h"
#include "hfcg.h"



#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void send_leds(BITS_8 data);

#define led bit
#define all byte

#define MicroSecond_DELAY {__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");}
/* ---------------------------------------------------------------
   Name: Microsecond_Delay

   Delay a number of microseconds.

   Input: Code for delay.
   --------------------------------------------------------------- */
void Microsecond_Delay(WORD microsec_delay)
{
    // disable ITIM8-3 and select input CORE clock
    CLEAR_BIT(ITCTS3, (CKSEL | ITEN));

    // wait till ITCTS3.ITEN is indeed 0
    while (IS_BIT_SET(ITCTS3, ITEN));

    // set prescalar
    ITPRE3 = (HFCG_CORE_CLK/1000000UL) - 1;

    // 0x100/256's delays
    while (microsec_delay > 0x100)
    {
        microsec_delay -= 0x100;

        // wait till ITCTS3.ITEN is indeed 0
        while (IS_BIT_SET(ITCTS3, ITEN));

        // set counter
        ITCNT3 = 0xFF;

        // enable ITIM8-3
        SET_BIT(ITCTS3, ITEN);

        // wait for counting complete
        while (IS_BIT_CLEAR(ITCTS3, TO_STS));

        // disable ITIM8-3
        CLEAR_BIT(ITCTS3, ITEN);
    }

    // remainder delay
    if (microsec_delay != 0)
    {
        // wait till ITCTS3.ITEN is indeed 0
        while (IS_BIT_SET(ITCTS3, ITEN));

        // set counter
        ITCNT3 = microsec_delay - 1;

        // enable ITIM8-3
        SET_BIT(ITCTS3, ITEN);

        // wait for counting complete
        while (! IS_BIT_SET(ITCTS3, TO_STS));

        // disable ITIM8-3
        CLEAR_BIT(ITCTS3, ITEN);
    }

}




/* ---------------------------------------------------------------
   Name: OEM_Write_Leds

   Write to the SCROLL LOCK, NUM LOCK, CAPS LOCK,
   and any custom LEDs (including an "Fn" key LED).

   Input: LED_ON bit.
          data = Data to write to the LEDs
             bit 0 - SCROLL, Scroll Lock
             bit 1 - NUM,    Num Lock
             bit 2 - CAPS,   Caps Lock
             bit 3 - OVL,    Fn LED
             bit 4 - LED4,   Undefined
             bit 5 - LED5,   Undefined
             bit 6 - LED6,   Undefined
             bit 7 - LED7,   Undefined

   Updates Led_Data variable and calls send_leds if appropriate.
   --------------------------------------------------------------- */
void OEM_Write_Leds(BITS_8 data)
{
   Led_Data.all = data.all;

   if (Flag.LED_ON)
   {
      /* Local LED's should be on. */

      send_leds(data);
   }
}

/* ---------------------------------------------------------------
   Name: OEM_Update_Ovl_Led

   Update the OVL bit of Led_Data.

   Input: OVL bit of Led_Ctrl.
          FN and OVERLAY bits of Scanner_State.
   --------------------------------------------------------------- */
void OEM_Update_Ovl_Led(void)
{
   if (Led_Ctrl.led.OVL)
   {
      Led_Data.led.OVL =
         (Scanner_State.bit.FN ^ Ext_Cb3.bit.OVL_STATE);
   }
}

/* ---------------------------------------------------------------
   Name: send_leds

   Sends the LED data to the LEDs.

   Input: data = Data to write to the LEDs
          bit 0 - SCROLL, Scroll Lock
          bit 1 - NUM,    Num Lock
          bit 2 - CAPS,   Caps Lock
          bit 3 - OVL,    Fn LED
          bit 4 - LED4,   Undefined
          bit 5 - LED5,   Undefined
          bit 6 - LED6,   Undefined
          bit 7 - LED7,   Undefined
   --------------------------------------------------------------- */
static void send_leds(BITS_8 data)
{
   BYTE signal;

   /* Bit 3 of the data sent to Extended_Write_Port is used to modify the LED.

                  Complement
      Desired     of Desired   Active     LED      Exclusive
      LED state   LED state     Level     Output      OR
      ---------   ----------   --------   ------   ---------
      Off (0)         1        high (1)     0          0
      Off (0)         1        low  (0)     1          1
      On  (1)         0        high (1)     1          1
      On  (1)         0        low  (0)     0          0 */

   /* The Fn (overlay) LED is in bit 3 of data variable. */
   if (Overlay_Pin != UNUSED_PIN)
   {
      /* Output Overlay. */
      signal = data.byte;
      signal = (~signal) & MASK(bit3);
      signal = signal ^ Overlay_Pin;
      Extended_Write_Port(signal, 0);
   }

   /* Put Caps Lock bit into bit 3 of data variable. */
   data.byte = data.byte << 1;
   if (Caps_Lock_Pin != UNUSED_PIN)
   {
      /* Output Caps Lock. */
      signal = data.byte;
      signal = (~signal) & MASK(bit3);
      signal = signal ^ Caps_Lock_Pin;
      Extended_Write_Port(signal, 0);
   }

   /* Put Num Lock bit into bit 3 of data variable. */
   data.byte = data.byte << 1;
   if (Num_Lock_Pin != UNUSED_PIN)
   {
      /* Output Num Lock. */
      signal = data.byte;
      signal = (~signal) & MASK(bit3);
      signal = signal ^ Num_Lock_Pin;
      Extended_Write_Port(signal, 0);
   }

   /* Put Scroll Lock bit into bit 3 of data variable. */
   data.byte = data.byte << 1;
   if (Scroll_Lock_Pin != UNUSED_PIN)
   {
      /* Output Scroll Lock. */
      signal = data.byte;
      signal = (~signal) & MASK(bit3);
      signal = signal ^ Scroll_Lock_Pin;
      Extended_Write_Port(signal, 0);
   }

}



/* ---------------------------------------------------------------
   Name: OEM_Reset

   Control has been transferred from the RESET.S file.
   The stack pointers and interrupt base register have been set.
   --------------------------------------------------------------- */
void OEM_Reset(void)
{
   /* GLP DEBUG. { */
   extern BYTE _STACK_START[];
   extern const int _STACK_SIZE;
   BYTE count;
   BYTE *pntr;
   int  i;

   pntr = (BYTE *) 0x10000;
   // Do not init top RAM which contains flash update code/variables
#if RAM_BASED_FLASH_UPDATE
   for (i = 0; i < 0xB00; i++)
#else
   for (i = 0; i < 0xFB0; i++)
#endif
   {
      *pntr = 0;
      pntr ++;
   }


   /* _STACK_SIZE is a number.  The compiler recognizes it as a variable
      so it will put the contents of the variable in count.  To force
      the compiler to use it as a number, take the address of the
      "variable" with &. */
   count = (BYTE) &_STACK_SIZE;
   pntr = _STACK_START;
   do
   {
      pntr--;
      *pntr = 0xFF;
      count--;
   }
   while(count);

   /* Next, control is transferred to the
      C entry point of the program: main().

      Since there is no return from main(), the stack
      pointer is reset to save stack space which would
      never be used and a jump is made to main. */

   __asm__("movd $__STACK_START,(sp)");
   __asm__("br _main");
}

/* ---------------------------------------------------------------
   Name: Version, core_vcb, oem_vcb

   Version data.

   Change OVER.H file to change the OEM version number.
   --------------------------------------------------------------- */

/* Version data for Command ACh. */
const BYTE Version[] =
{
   TRANS(COREMAJ), 0x34,
   TRANS(COREMID_H), TRANS(COREMID_L), 0x34,
   TRANS(COREMIN_H), TRANS(COREMIN_L),
   0x1A, // [
   //TRANS(PROJECT_1), TRANS(PROJECT_2),
   //TRANS(PROJECT_3), TRANS(PROJECT_4),
   TRANS('2'),  // Ver                      // New Version Marker
   0x34,
   TRANS('0'),  // Rev Major                // New Version Marker
   0x34,
   TRANS('0'),  // Rev Minor                // New Version Marker
   //TRANS(OEM_VER1),  TRANS(OEM_VER2),
   0x1B, // ]
   //TMP_LET
   //0x0C,             /* - */
   //ID_MARK,
   0xAA              /* Terminator. */
};

/* Version Control Block (VCB) to allow VCB.EXE utility to
   display Keyboard Controller's version from a binary file. */
static const BYTE core_vcb[] =
{
   ID1, ID2,
   PRODUCT, 0x00,
   COREMAJ, '.',
   COREMID_H, COREMID_L, '.',
   COREMIN_H, COREMIN_L, 0x00,
   COREDATE, 0x00,
   '0', '0', '0', '0', 0x00
};

/* Version Control Block (VCB) to allow VCB.EXE utility to
   display Keyboard Controller's version from a binary file. */
static const BYTE oem_vcb[] =
{
   ID1, ID2,
   OEMPRODUCT, 0x00,
   '0', '.', OEM_VER1, OEM_VER2, '.', '0', ENGNUM, 0x00,
   OEMDATE, 0x00,
   PROJECT_1, PROJECT_2,
   PROJECT_3, PROJECT_4, 0x00
};

/* ---------------------------------------------------------------
   Name: OEM_Warm_Reset_Check

   Checks to see if a warm reset initialization should be done.

   Returns: 0 if cold reset initialization should be done,
            1 if warm reset initialization should be done.
   --------------------------------------------------------------- */
FLAG OEM_Warm_Reset_Check(void)
{
   return 0;
}


