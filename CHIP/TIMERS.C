/*-----------------------------------------------------------------------------
 * MODULE TIMERS.C
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
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started April 3, 2000 by GLP)
 *
 * ------------------------------------------------------------------------- */

#define TIMERS_C

#include <asm.h>

#include "swtchs.h"
#include "timers.h"
#include "com_defs.h"
#include "icu.h"
#include "proc.h"
#include "oem.h"
#include "clock.h"

WORD time_minus_overhead;
WORD us_overhead;

/* ---------------------------------------------------------------
   Name: Load_Timer_A

   Load Timer A to time-out in 1 mS.
   --------------------------------------------------------------- */
void Load_Timer_A(void)
{
    #if KERNEL_TICKER_T0
    /* The timer from the TWD (Timer and Watchdog
       Module) is used for Timer A.  The pre-scale and
       TWDT0 registers have already been initialized. */

    SET_BIT(T0CSR, T0CSR_RST);                    /* Cause the timer to reload
                                              on the next input clock. */

    ICU_Clr_Pending_Irq(ICU_EINT_T0OUT);   /* Clear any pending interrupt. */
    ICU_Enable_Irq(ICU_EINT_T0OUT);        /* Enable timer interrupt. */
    #else // KERNEL_TICKER_T0
    /* The timer from the ITIM81 (Internal 8bit Timer 1)
       is used for Timer A.  The pre-scale and
       COUNTER registers have already been initialized. */

    // disable timer, interrupt and select 32k clock source
    ITCTS1 = TO_STS;
    // wait ITEN bit clear
    while(IS_BIT_SET(ITCTS1, ITEN)) ;
    ITPRE1 = 0;
    ITCNT1 = OEM_TIMER_A_CNT;

    // enable timer and interrupt
    SET_BIT(ITCTS1, CKSEL | ITEN | TO_IE);
    ICU_Clr_Pending_Irq(ICU_EINT_ITIM81);   /* Clear any pending interrupt. */
    ICU_Enable_Irq(ICU_EINT_ITIM81);        /* Enable timer interrupt. */
    #endif // KERNEL_TICKER_T0
}

/* ---------------------------------------------------------------
   Name: Enable_Timer_A

   Load the 1 mS timer if not already enabled.
   --------------------------------------------------------------- */
void Enable_Timer_A(void)
{
    #if KERNEL_TICKER_T0
    /* The timer from the TWM (Timing and Watchdog Module) is
       used for Timer A.  If the interrupt for this timer is
       enabled, Timer A is running.  Otherwise, load the timer. */

    if (!ICU_Irq_Is_Enabled(ICU_EINT_T0OUT)) Load_Timer_A();
    #else
    /* The timer from the ITIM81 (Internal 8 BITS Timer 1) is
       used for Timer A.  If the interrupt for this timer is
       enabled, Timer A is running.  Otherwise, load the timer. */

    if (IS_BIT_CLEAR(ITCTS1, ITEN)) Load_Timer_A();
    #endif
}

/* ---------------------------------------------------------------
   Name: Stop_Timer_A

   Stop the 1 mS timer.
   --------------------------------------------------------------- */
void Stop_Timer_A(void)
{
    #if KERNEL_TICKER_T0
    /* The timer from the TWM (Timing and Watchdog Module)
       is used for Timer A.  Just disable the interrupt. */

    WORD psr_value;

    _spr_("psr", psr_value);               /* Get state of global
                                              interrupt enable flag. */
    _di_();                                /* Disable interrupts before
                                              disabling interrupt. */
    ICU_Clr_Pending_Irq(ICU_EINT_T0OUT);   /* Clear any pending interrupt. */
    ICU_Disable_Irq(ICU_EINT_T0OUT);       /* Disable timer interrupt. */
    if (psr_value & (1 << (PSR_E_BIT)))
    _ei_();                             /* Put interrupt state back. */
    #else // KERNEL_TICKER_T0
    // disable timer, interrupt and clear the timeout status
    ITCTS1 = TO_STS;
    #endif // KERNEL_TICKER_T0
}

/* ---------------------------------------------------------------
   Name: Load_Timer_B

   Load Timer B to time-out in 150 microseconds.
   --------------------------------------------------------------- */
void Load_Timer_B(void)
{
    DWORD OpFreq;
   /* Timer/Counter 2 from the MFT16 (Multi-Function-
      16-Bit-Timer) Module is used for Timer B.  The
      timer/counter has already been initialized as a timer. */
   #if KERNEL_TMRB_MFT16
   /* Stop the timer by setting the clock source to "none". */
   //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_NOCLK2;
   SET_FIELD(T2CKC, TCKC_C2CSEL, TCKC_NOCLK2);

   /* Clear the interrupt pending flag. */
   T2ICLR = TICLR_TDCLR;

   /* Set the timer. */
   T2CNT2 = OEM_TIMER_B_CNT;

   /* Start the timer. */
   //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_SLOWCLK2;
   SET_FIELD(T2CKC, TCKC_C2CSEL, TCKC_SLOWCLK2);

   /* Enable the interrupt. */
   SET_BIT(T2IEN, TIEN_TDIEN);
   #else
   OpFreq = clock_get_freq();
    /* The timer from the ITIM162 (Internal 16bit Timer 2)
       is used for Timer B.  The pre-scale and
       COUNTER registers have already been initialized. */

    // disable timer, interrupt and select core clock as clock source
    ITCTS2 = TO_STS;
    // wait ITEN bit clear
    while(IS_BIT_SET(ITCTS2, ITEN)) ;
    ITPRE2 = (BYTE) ((OpFreq/1000000L) - 1);
    IT16CNT2 = OEM_TIMER_B_CNT;

    // enable timer and interrupt
    SET_BIT(ITCTS2, ITEN | TO_IE);
    ICU_Clr_Pending_Irq(ICU_EINT_ITIM82);   /* Clear any pending interrupt. */
    ICU_Enable_Irq(ICU_EINT_ITIM82);        /* Enable timer interrupt. */
   #endif
}

/* ---------------------------------------------------------------
   Name: Stop_Timer_B

   Stop the timer.
   --------------------------------------------------------------- */
void Stop_Timer_B(void)
{
    /* Timer/Counter 2 from the MFT16 (Multi-Function-
       16-Bit-Timer) Module is used for Timer B. */
    #if KERNEL_TMRB_MFT16
    /* Stop the timer by setting the clock source to "none". */
    //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_NOCLK2;
    SET_FIELD(T2CKC, TCKC_C2CSEL, TCKC_NOCLK2);
    #else
    ITCTS2 = TO_STS;                       /* Clear timer out status. */
    #endif
}

/* ---------------------------------------------------------------
   Name: Load_Timer_B

   Load Timer PS2 to time-out for PS/2 transacion.
   --------------------------------------------------------------- */
void Load_Timer_PS2(BYTE time)
{
   /* Timer/Counter 2 from the MFT16 (Multi-Function-
      16-Bit-Timer) Module is used for Timer PS2.  The
      timer/counter has already been initialized as a timer. */
   /* Stop the timer by setting the clock source to "none". */
   //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_NOCLK2;
   SET_FIELD(T2CKC, TCKC_C1CSEL, TCKC_NOCLK1);

   /* Clear the interrupt pending flag. */
   T2ICLR = TICLR_TACLR;

   /* Set the timer. */
   T2CNT1 = OEM_TIMER_PS2_CNT * time;

   /* Start the timer. */
   //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_SLOWCLK2;
   SET_FIELD(T2CKC, TCKC_C1CSEL, TCKC_SLOWCLK1);

   /* Enable the interrupt. */
   SET_BIT(T2IEN, TIEN_TAIEN);
}

/* ---------------------------------------------------------------
   Name: Stop_Timer_B

   Stop the timer.
   --------------------------------------------------------------- */
void Stop_Timer_PS2(void)
{
    /* Timer/Counter 1 from the MFT16 (Multi-Function-
       16-Bit-Timer) Module is used for Timer PS2. */

    /* Stop the timer by setting the clock source to "none". */
    //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_NOCLK2;
    SET_FIELD(T2CKC, TCKC_C1CSEL, TCKC_NOCLK1);
}


/* ---------------------------------------------------------------
   Name: Load_Timer_D

   Load Timer D to time-out in 10 mS.
   --------------------------------------------------------------- */
void Load_Timer_D(void)
{
    /* The timer from the ITIM81 (Internal 8bit Timer 1)
       is used for Timer A.  The pre-scale and
       COUNTER registers have already been initialized. */

    // disable timer, interrupt and select 32k clock source
    ITCTS3 = TO_STS;
    // wait ITEN bit clear
    while(IS_BIT_SET(ITCTS3, ITEN)) ;
    ITPRE3 = 19;
    ITCNT3 = OEM_TIMER_D_CNT;

    // enable timer and interrupt
    SET_BIT(ITCTS3, CKSEL | ITEN | TO_IE);
    SET_BIT(ITCTS3, ITEN);
}

/* ---------------------------------------------------------------
   Name: Enable_Timer_D

   Load the 10 mS timer if not already enabled.
   --------------------------------------------------------------- */
void Enable_Timer_D(void)
{
    /* The timer from the ITIM81 (Internal 8 BITS Timer 2) is
       used for Timer D.  If the interrupt for this timer is
       enabled, Timer D is running.  Otherwise, load the timer. */

    if (IS_BIT_CLEAR(ITCTS2, ITEN)) Load_Timer_D();
}

/* ---------------------------------------------------------------
   Name: Stop_Timer_A

   Stop the 1 mS timer.
   --------------------------------------------------------------- */
void Stop_Timer_D(void)
{
    // disable timer, interrupt and clear the timeout status
    ITCTS1 = TO_STS;
}



/* ---------------------------------------------------------------
   Name: Timers_Init

   Setup Timer A as a 1 millisecond timer.
   Setup Timer B as a 150 microsecond timer.
   --------------------------------------------------------------- */
void Timers_Init(void)
{
   #if KERNEL_TICKER_T0
   /* Use the timer from the TWM (Timing
      and Watchdog Module) for Timer A. */

   TWCP = TWCP_MDIV & 0;  /* 32.768 KHz divided by 2**0,
                             causes the prescaler to pass
                             the signal to the timer. */

   TWDT0 = OEM_TIMER_A_CNT;
   #else // KERNEL_TICKER_T0
   Enable_Timer_A();
   Enable_Timer_D();
   #endif // KENEL_TICKER_T0

   /* Use Timer/Counter 2 from the MFT16-2 (Multi-Function
      16-Bit Timer) Module for Timer PS/2.  Set it to be a timer. */



   /* Use Timer/Counter 2 from the MFT16-2 (Multi-Function
      16-Bit Timer) Module for Timer B. Set it to be a timer.

      Use Timer/Counter 1 from the MFT16-2 (Multi-Function
      16-Bit Timer) Module for Timer PS/2.  Set it to be a timer.      */

   T2MCTRL = TCTRL_MODE3;



    /* Timer/Counter 2 from the MFT16 (Multi-Function 16-Bit
       Timer) Module has already been initialized as a timer. */

    /* Stop the timer by setting the clock source to "none". */
    //T1CKC = (T1CKC & ~TCKC_C2CSEL) | TCKC_NOCLK2;
    SET_FIELD(T2CKC, TCKC_C2CSEL, TCKC_NOCLK2);

    /* Clear the interrupt pending flag. */
    T2ICLR = TICLR_TACLR | TICLR_TDCLR;

    /* Ensure that the prescaler is cleared to allow
       the system clock to decrement the timer. */
    T2PRSC = 0;

    /* Set the timer. */
    T2CNT2 = 0xFFFF;
    }

