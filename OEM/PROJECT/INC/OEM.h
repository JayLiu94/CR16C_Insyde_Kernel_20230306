/* ----------------------------------------------------------------------------
 * MODULE OEM.H
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
 * Contains OEM defined equates used by the core code.  In
 * general, anything that may need to be configured differently
 * for different OEM hardware platforms (in order for core code
 * to still function correctly).  Also, header for OEM.C.
 *
 * Revision History (Started August 9, 1996 by GLP)
 *
 * GLP
 * 13 Dec 96   Added HFCGMH_INIT, HFCGML_INIT, and HFCGN_INIT and
 *             set them to run at 4 MHz.
 * 19 Dec 96   Changed HFCGMH_INIT, HFCGML_INIT, and HFCGN_INIT
 *             to run at 10 MHz.
 * 17 Jan 97   Changed OEM_RST_A20_DELAY, OEM_WRITE_STROBE_DELAY,
 *             and PORT_PIN_PULSE_DELAY for 10 MHz clock.
 * 21 Jan 97   Increased OEM_WRITE_STROBE_DELAY.  Adjusted
 *             OEM_RST_A20_DELAY and PORT_PIN_PULSE_DELAY
 *             using data from an oscilloscope.
 * 05 May 97   Added OEM_ACPI_Gen_Int.
 * 29 Jun 98   Updated for use with CORE 1.01.20.
 * 05 Oct 98   Updated for new Keyboard Controller functionality.
 * 05 Aug 99   Moved OEM_detect_crisis to the OEMCRSIS.H file.
 *             Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef OEM_H
#define OEM_H

#include "swtchs.h"
#include "hfcg.h"
#include "regs.h"
#include "types.h"


void OEM_Reset(void);
FLAG OEM_Warm_Reset_Check(void);


void OEM_Write_Leds(BITS_8 data);
void OEM_Update_Ovl_Led(void);


void OEM_Auto_Detect_Ps2(void);
void Microsecond_Delay(WORD delay);

/* -------------------- Timer_A, Timer_B ---------------------
   Timer A and Timer B defines that may be
   influenced by CPU clock speed differences.

   The timer count and clock rate should be adjusted based on
   the CPU speed so that timeout values, etc., are the same
   regardless of CPU speed.

   Timer A should be set to generate an IRQ approximately
   every 1 millisecond when enabled.

   Timer B should be set to generate an IRQ approximately
   every 150 microseconds when enabled. */

/* The timer from the TWM (Timing and Watchdog Module)
   is used for Timer A.  It uses the Slow Speed Clock.
   With the clock connected to a 32.768 KHz signal,
   32.768 KHz divided by (32+1) causes T0OUT to be a
   992.97 Hz signal.  1 / 992.97 Hz = 1.007 mS. */
#if TICKER_1MS
/* Ticker base is 1mS. */
#define OEM_TIMER_A_CNT 32
#else // TICKER_1MS
// Ticker base is 5mS
#define OEM_TIMER_A_CNT 32*5
#endif // TICKER_1MS

#define OEM_TIMER_D_CNT 32*5

#if KERNEL_TMRB_MFT16
/* Timer/Counter 2 from the MFT16 (Multi-Function-16-Bit-
   Timer) Module is used for Timer B.  It uses the Slow
   Speed Clock.  With the clock connected to a 32.768 KHz signal,
   32.768 KHz / 5 = 6.5536 KHz.  1 / 6.5536 KHz = 152.6 uS. */
#define OEM_TIMER_B_CNT 5
#else
/* The timer from the ITIM162 (Internal 16bit Timer 2)
   is used for Timer B.  It uses the CORE clock, scaled to 1MHz. */
#define OEM_TIMER_B_CNT 150
#endif

/* Timer/Counter 2 from the MFT16 (Multi-Function-16-Bit-
   Timer) Module is used for Timer PS/2.  It uses the Slow
   Speed Clock. */

#define OEM_TIMER_PS2_CNT 32


/* ---------------------------------------------------------------
   Settings for system clock.
   --------------------------------------------------------------- */

/* Set SYS_CLK to 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, or 20. */
/* We suggest set SYS_CLK 4, 6, 8, 12, 16 for REV-A1. */
#define SYS_CLK 16

#if SYS_CLK==4
 #define HFCGMH_INIT 0x03
 #define HFCGML_INIT 0xCF
 #define HFCGN_INIT  0x08
 #define HFCGP_INIT	0x17
#elif SYS_CLK==5
 #define HFCGMH_INIT 0x03
 #define HFCGML_INIT 0x92
 #define HFCGN_INIT  0x06
#elif SYS_CLK==6
 #define HFCGMH_INIT 0x03
 #define HFCGML_INIT 0x92
 #define HFCGN_INIT  0x05
 #define HFCGP_INIT	0x0F
#elif SYS_CLK==7
 #define HFCGMH_INIT 0x07
 #define HFCGML_INIT 0x81
 #define HFCGN_INIT  0x09
#elif SYS_CLK==8
 #define HFCGMH_INIT 0x0A
 #define HFCGML_INIT 0x7C
 #define HFCGN_INIT  0x0B
 #define HFCGP_INIT	0x0B
#elif SYS_CLK==9
 #define HFCGMH_INIT 0x0A
 #define HFCGML_INIT 0xB9
 #define HFCGN_INIT  0x0A
#elif SYS_CLK==10
 #define HFCGMH_INIT 0x0A
 #define HFCGML_INIT 0xB9
 #define HFCGN_INIT  0x09
#elif SYS_CLK==12
 #define HFCGMH_INIT 0x11
 #define HFCGML_INIT 0x2A
 #define HFCGN_INIT  0x0C
 #define HFCGP_INIT	0x07
#elif SYS_CLK==14
 #define HFCGMH_INIT 0x19
 #define HFCGML_INIT 0x07
 #define HFCGN_INIT  0x0F
#elif SYS_CLK==16
 #define HFCGMH_INIT 0x11
 #define HFCGML_INIT 0x29
 #define HFCGN_INIT  0x09
 #define HFCGP_INIT	0x05
#elif SYS_CLK==18
 #define HFCGMH_INIT 0x11
 #define HFCGML_INIT 0x29
 #define HFCGN_INIT  0x08
#elif SYS_CLK==20
 #define HFCGMH_INIT 0x17
 #define HFCGML_INIT 0xD6
 #define HFCGN_INIT  0x0A
#else
 #error Unknown system clock (SYS_CLK in OEM.H).
#endif

/* ---------------------------------------------------------------
   Settings for Analog to Digital Converter (A to D, ADC).
   --------------------------------------------------------------- */

/* ADC internal or external reference.
   0 = external reference,
   1 = internal reference. */
#define ADCCNT1_INTREF_INIT 0

/* ADC sampling time. */
#define ADCCNT3_DELAY_INIT 1

#define VOLDLY_INIT 3
#define TMPDLY_INIT 4

#if SYS_CLK==4
 /* System clock divided by 8. */
#define ACLKCTL_INIT 8
#elif SYS_CLK==5
 /* System clock divided by 10. */
#define ACLKCTL_INIT 10
#elif SYS_CLK==6
 /* System clock divided by 12. */
#define ACLKCTL_INIT 12
#elif SYS_CLK==7
 /* System clock divided by 14. */
#define ACLKCTL_INIT 14
#elif SYS_CLK==8
 /* System clock divided by 16. */
#define ACLKCTL_INIT 16
#elif SYS_CLK==9
 /* System clock divided by 18. */
#define ACLKCTL_INIT 18
#elif SYS_CLK==10
 /* System clock divided by 20. */
#define ACLKCTL_INIT 20
#elif SYS_CLK==12
 /* System clock divided by 24. */
#define ACLKCTL_INIT 24
#elif SYS_CLK==14
 /* System clock divided by 28. */
#define ACLKCTL_INIT 28
#elif SYS_CLK==16
 /* System clock divided by 32. */
#define ACLKCTL_INIT 32
#elif SYS_CLK==18
 /* System clock divided by 36. */
#define ACLKCTL_INIT 36
#else// SYS_CLK==20
 /* System clock divided by 40. */
#define ACLKCTL_INIT 40
#endif
/* ---------------------------------------------------------------
   Name: OEM_Host_Reset_High, OEM_Host_Reset_Low

   Host reset handling.

   C prototypes:
      void OEM_Host_Reset_High(void);
      void OEM_Host_Reset_Low(void);
   --------------------------------------------------------------- */
 /* To generate a Host reset, use OEM_Host_Reset_Low to
    lower the reset pin.  This is done by raising the HRSTOB
    bit in SIBCST2. */
// #define OEM_Host_Reset_High() (MSWCTL1 &= ~MSWCTL1_HRSTOB)
// mask 0xE7 - bit 3 (VHCFGA) 1C and 4 (HCFGLK) 1S
#define OEM_Host_Reset_High() (MSWCTL1 = (MSWCTL1 & 0xE7) & ~MSWCTL1_HRSTOB)
// #define OEM_Host_Reset_Low()  (MSWCTL1 |=  MSWCTL1_HRSTOB)
// mask 0xE7 - bit 3 (VHCFGA) 1C and 4 (HCFGLK) 1S
#define OEM_Host_Reset_Low()  (MSWCTL1 = (MSWCTL1 & 0xE7) | MSWCTL1_HRSTOB)

/* ---------------------------------------------------------------
   Name: OEM_A20_High, OEM_A20_Low, OEM_Get_A20

   Gate A20 handling.

   C prototypes:
      void OEM_A20_High(void);
      void OEM_A20_Low(void);
      FLAG OEM_Get_A20(void);
   --------------------------------------------------------------- */
#define GATE_A20_PORT MSWCTL1
#define GATE_A20_BIT  (1 << 7)
// #define OEM_A20_High() (GATE_A20_PORT |=  GATE_A20_BIT)
// mask 0xE7 - bit 3 (VHCFGA) 1C and 4 (HCFGLK) 1S
#define OEM_A20_High() (GATE_A20_PORT  = (GATE_A20_PORT & 0xE7) |  GATE_A20_BIT)
// #define OEM_A20_Low()  (GATE_A20_PORT &= ~GATE_A20_BIT)
// mask 0xE7 - bit 3 (VHCFGA) 1C and 4 (HCFGLK) 1S
#define OEM_A20_Low()  (GATE_A20_PORT  = (GATE_A20_PORT & 0xE7) & ~GATE_A20_BIT)
#define OEM_Get_A20() ((GATE_A20_PORT &   GATE_A20_BIT) ? 1 : 0)



#endif /* ifndef OEM_H */

