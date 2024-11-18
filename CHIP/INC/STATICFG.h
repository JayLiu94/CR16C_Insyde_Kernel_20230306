/*-----------------------------------------------------------------------------
 * MODULE STATICFG.H
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
 * Definitions for the variables in the Static Configuration Table.
 *
 * Revision History (Started August 6, 1998 by GLP)
 *
 * 15 Oct 98   Moved configuration table offset 0x138 - 0x18F to
 *             0x140 - 0x197.  This was done to make room in
 *             hardware configuration space for Benchmarq values
 *             at 0x138 through 0x13C and reserved values
 *             at 0x13D through 0x13F.
 *             Removed bytes from end of table.
 * 19 Oct 98   Added definitions for ACPI EC space overlapping Config.
 *             Corrected TIMER_US to allow settings of 33 to 126
 *             microseconds in steps of 3.
 * 19 Aug 99   Updated copyright.
 * 07 Sep 99   Included purxlt.h and added CUSTOM_TABLE_BASE.
 * ------------------------------------------------------------------------- */

#ifndef STATICFG_H
#define STATICFG_H

#include "swtchs.h"
#include "types.h"
#include "purxlt.h"

#define PORT_PIN(port, pin) ((((port) & 0x0F) << 4) + ((pin) & 0x07))
/* The number of monitored input port pins. */
#define NUM_INPUTS 8


/* ---------------------------------------------------------------
   Definition of timer setting.

   Byte = yyyx,xxxx

    Y         formula                 delay
   ---   -----------------   --------------------------
    0    (X+1) *    1 +  0      1 to    32 microseconds
    1    (X+1) *    3 + 30     33 to   126 microseconds
    2    (X+1) *    1 +  0      1 to    32 milliseconds
    3    (X+1) *   10 +  0     10 to   320 milliseconds
    4    (X+1) *  100 +  0    100 to  3200 milliseconds
    5    (X+1) *  500 +  0    500 to 16000 milliseconds
    6    (X+1) * 1000 +  0   1000 to 32000 milliseconds
    7    (X+1) * 2000 +  0   2000 to 64000 milliseconds
   --------------------------------------------------------------- */

#define CAUSE_ERROR 0xFFFFFFFF

/* TIMER_US can be used to setup a timer setting byte for microseconds.
   Valid values are 1 to 126. */
#define SETUP_US(us) ((us <= 32) ? us : (0x20+((us-30)/3)))
#define TIMER_US(us) ( ((us > 0) && (us <= 126)) ? (SETUP_US(us)-1) : CAUSE_ERROR )

/* TIMER_MS can be used to setup a timer setting byte for milliseconds.
   ms can be 1 through 32 and mult can be 1, 10, 100, 500, 1000, or 2000. */
#define SETUP(mult) ((mult==1) ? 0x40 : ((mult==10) ? 0x60 : ((mult==100) ? 0x80 : ((mult==500) ? 0xA0 : ((mult==1000) ? 0xC0 : ((mult==2000) ? 0xE0 : CAUSE_ERROR))))))
#define TIMER_MS(ms, mult) ( ((ms > 0) && (ms <= 32)) ? (SETUP(mult)+(ms-1)) : CAUSE_ERROR )

/* ---------------------- Cfg0C ----------------------- */

#define x          bit0
#define FAST_A20   bit1
#define xxx        bit2
#define xxxx       bit3
#define xxxxx      bit4
#define xxxxxx     bit5
#define CPU_SPEED0 bit6
#define CPU_SPEED1 bit7

/* ---------------------- Cfg0E -----------------------
   External shift register type and
   SCI/SMI Port Pin Operation.

   Bits 7 and 6 are used for the external shift register type.
        -------
        0     0   No shift register used
        0     1   No shift register used
        1     0   Unlatched
        1     1   Latched */

#define LATCHED_SHIFT_REG bit6
#define SHIFT_REG_ACTIVE  bit7

   /*
   Bits 5, 4, and 3 are used to define the SCI pin operation.
   Bits 2, 1, and 0 are used to define the SMI pin operation.

   Code     Operation
   -----   ------------
   0 0 0   No Operation
   0 0 1   Set High
   0 1 0   Set Low
   0 1 1   Pulse High
   1 0 0   Pulse Low */

/* shiftSCI_OP_SELECT is used to shift the bits for the SCI
   operation designation to the lower part of the byte.
   maskSCI_OP_SELECT is then used to isolate those bits. */
#define shiftSCI_OP_SELECT 3
#define maskSCI_OP_SELECT  0x07

/* shiftSMI_OP_SELECT is used to shift the bits for the SMI
   operation designation to the lower part of the byte.
   maskSMI_OP_SELECT is then used to isolate those bits. */
#define shiftSMI_OP_SELECT 0
#define maskSMI_OP_SELECT  0x07

#define SXI_NOP      0
#define SXI_SET_HI   1
#define SXI_SET_LO   2
#define SXI_PULSE_HI 3
#define SXI_PULSE_LO 4

/* Start of scanner tables. */
#define SCAN_TABLE_BASE     0x0010

/* Start of custom key table. */
#define CUSTOM_TABLE_BASE   (SCAN_TABLE_BASE+ \
                             RC_TABLE_SIZE+   \
                             COMB_TABLE_SIZE)

/* Maximum size of scanner tables. */
#define MAX_SCAN_TABLE_SIZE (RC_TABLE_SIZE+         /* Row/Column table. */  \
                             COMB_TABLE_SIZE+       /* Combination Table. */ \
                             MAX_CUSTOM_TABLE_SIZE+ /* Custom Key Table. */  \
                             1)                     /* Byte for number of external keyboard hotkeys. */



#define Cfg0E ((SXI_PULSE_LO << shiftSCI_OP_SELECT) | (SXI_SET_LO << shiftSMI_OP_SELECT))


                                    /* See notes below on scanner masks. */
#define Msk_Scan_In       0xFF  /* Scanner input mask value. */
#define Msk_Strobe_H      0xFF  /* Scanner output mask value, high byte. */
#define Msk_Strobe_L      0xFF  /* Scanner output mask value, low byte. */
#define Scan_Read_Delay   TIMER_US(16)    /* Delay between setting up scan line on
                                       internal keyboard and reading input lines.
                                       Timer setting from 1 to 126 microseconds. */



#define Rst_A20_Pulse    TIMER_US(6)     /* CPU Reset and Gate A20 pulse for Fxh commands.
                                       Timer setting from 1 to 126 microseconds. */

                                    /* Bit 3 of the LED port pin definition
                                       is used to define the active state. */
#define Num_Lock_Pin      PORT_PIN(P3_CODE, 6)  /* Num Lock LED port and pin number. */
#define Caps_Lock_Pin     PORT_PIN(P5_CODE, 1)  /* Caps Lock LED port and pin number. */
#define Scroll_Lock_Pin   UNUSED_PIN  /* Scroll Lock LED port and pin number. */
#define Overlay_Pin       UNUSED_PIN

                                    /* System Control Interrupt. */
#define SCI_Pin PORT_PIN(HW_CODE, 2)           /* SCI port and pin number. */
#define SCI_Pulse TIMER_US(16)         /* Timer setting from 1 to 126 microseconds. */

                                    /* System Management Interrupt. */
#define SMI_Pin PORT_PIN(HW_CODE, 2)           /* SMI port and pin number. */
#define SMI_Pulse TIMER_US(16)        /* Timer setting from 1 to 126 microseconds. */

#define Wakeup_Task_H   0
#define Wakeup_Task_L   0
#endif /* ifndef STATICFG_H */
