/* ----------------------------------------------------------------------------
 * MODULE ICU.C
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
 * Interrupt Control Unit (ICU).  This file contains the
 * interrupt dispatch (or interrupt vector) table.  Based
 * on PC87570 code version 2.00.07[2667-21]-n.
 *
 * The PS2 devices are handled by the hardware shift
 * mechanism.  To handle the devices in the legacy
 * mode, the initialization (Icu_Init) and the PS2.C
 * file will have to be changed.
 *
 * Revision History (Started March 15, 2000 by GLP)
 *
 * EDH
 * 11 Dec 00 	Support the fourth PS/2 port as port 4. It will be
 * 				recognized as port 0 with Active PS/2 Multiplexing
 *					driver.
 * 05 Mar 04   SMBus channel 1 and channel 2 are using seperate I2c_Irq_State.
 *             Because when EC is handling channel 2 and SMBus device on
 *             channel 1 send an alarm to EC (Host) at the same time. This
 *             will effect SMBus process.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * ------------------------------------------------------------------------- */

#define ICU_C

#include <asm.h>

#include "swtchs.h"
#include "icu.h"
#include "irq.h"
#include "ps2.h"
//#include "regs.h"
#include "timers.h"
#include "i2c.h"
#include "oem.h"
#include "purdat.h"
#include "proc.h"
#include "a_to_d.h"

#if CR_UART_SUPPORTED
#include "cr_uart.h"
#endif /* CR_UART_SUPPORTED */
#if ESPI_SUPPORTED
#include "espi.h"
#endif /* ESPI_SUPPORTED */

// delay between wakeup edge setting and clear pending
#define WKEDG_WKPCL_DELAY   {__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");}


#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif

#ifndef EXT_GPIO44_WAKEUP
#error EXT_GPIO44_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO1_WAKEUP
#error EXT_GPIO1_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO46_WAKEUP
#error EXT_GPIO46_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO3_WAKEUP
#error EXT_GPIO3_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO4_WAKEUP
#error EXT_GPIO4_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO5_WAKEUP
#error EXT_GPIO5_WAKEUP switch is not defined.
#endif

// #define Nmi nmi_handler
// #pragma trap(nmi_handler)
// void nmi_handler(void);

 #pragma interrupt(null_handler)
 void null_handler(void );

#pragma interrupt(Svc_handler)
void Svc_handler(void);

#pragma interrupt(Dvz_handler)
void Dvz_handler(void);

#pragma interrupt(Flag_handler)
void Flag_handler(void);

#pragma interrupt(Bpt_handler)
void Bpt_handler(void);

#pragma interrupt(Trc_handler)
void Trc_handler(void);

#pragma interrupt(Und_handler)
void Und_handler(void);

#pragma interrupt(Iad_handler)
void Iad_handler(void);

/* Primary interface (keyboard and mouse) output buffer empty. */
#define km_obe_handler Irq_Obe1

/* Secondary interface (power management) output buffer empty. */
#define pm_obe_handler null_handler

/* INT6 - PORT80 or PECI Interrupt.*/
#if PORT80_SUPPORTED
#pragma interrupt(DP80_int_handler)
void DP80_int_handler(void);
#define int6_handler DP80_int_handler
#elif PECI_SUPPORTED
#pragma interrupt (PECI_IntHandler)
void PECI_IntHandler(void);
#define int6_handler PECI_IntHandler
#else
#define int6_handler null_handler
#endif

#pragma interrupt(miwu2_handler)
void miwu2_handler(void);

#pragma interrupt(miwu3_handler)
void miwu3_handler(void);


#pragma interrupt(ext_gpio3_handler)
void ext_gpio3_handler(void);

#pragma interrupt(mft162_handler)
void mft162_handler(void);

#pragma interrupt(ITIM81_handler)
#if !KERNEL_TICKER_T0
void ITIM81_handler(void);
#else
#define ITIM81_handler null_handler
#endif

#if !KERNEL_TMRB_MFT16
#pragma interrupt(ITIM82_handler)
void ITIM82_handler(void);
#else
#define ITIM82_handler null_handler
#endif
#define ITIM83_handler null_handler

/* Analog to digital convertor. */
#pragma interrupt(adc_handler)
void adc_handler(void);

#pragma interrupt(access_bus1_handler)
void access_bus1_handler(void);

#pragma interrupt(access_bus2_handler)
void access_bus2_handler(void);

#pragma interrupt(access_bus3_handler)
void access_bus3_handler(void);

#pragma interrupt(access_bus4_handler)
void access_bus4_handler(void);

/* Internal keyboard (scanner). */
#define scan_handler Irq_Anykey

#define mswc_handler null_handler

#if KERNEL_TICKER_T0
/* The TWM (Timing and Watchdog Module).
   This timer is used for Timer A (1 millisecond timer). */
#pragma interrupt (twm_handler)
#define twm_handler Irq_Timer_A
#else // KERNEL_TICKER_T0
#define twm_handler null_handler
#endif // KERNEL_TICKER_T0

#define usart_handler null_handler

#pragma interrupt(ext_gpio4_handler)
void ext_gpio4_handler(void);

#pragma interrupt(CR_UART_handler)
void CR_UART_handler(void);

#pragma interrupt(ext_gpio5_handler)
void ext_gpio5_handler(void);

#pragma interrupt(ps2_ch3_handler)
void ps2_ch3_handler(void);

#pragma interrupt(ps2_ch2_handler)
void ps2_ch2_handler(void);

#pragma interrupt(ps2_sm_handler)
void ps2_sm_handler(void);

//mark remove #pragma interrupt(ext_gpio2_handler)
// mark remove void ext_gpio2_handler(void);

#define mft16_handler null_handler

//#define flash_shared_handler null_handler
#define flash_shared_handler Irq_Shm

/* Primary interface (keyboard and mouse) input buffer full. */
#define km_ibf_handler Irq_Ibf1

/* Secondary interface (power management) input buffer full. */
#define pm_ibf_handler Irq_Ibf2

#define dma1_handler null_handler

#define dma2_handler null_handler

#pragma interrupt(ext_gpio1_handler)
void ext_gpio1_handler(void);

#pragma interrupt(ext_gpio44_handler)
void ext_gpio44_handler(void);

#pragma interrupt(ext_gpio46_handler)
void ext_gpio46_handler(void);

/* ---------------------------------------------------------------
   Name: _dispatch_table[]

   This is the interrupt dispatch table.  This table contains
   pointers to handlers for all the traps and interrupts, including
   debug specific interrupts.  These handlers may be changed or
   even replaced with null handlers, but if you want debugging
   support be sure to retain the call to _tmon() in the Nuvoton
   handlers.  See Nuvoton's handlers' specific files for more information.
   If you want to retain the ability to debug your production code to some
   extent, be sure to retain Nuvoton's ISE, TRC and BPT handlers in your
   production code.  The code size cost is 70 bytes but you gain an ability
   to partialy debug production code.
   --------------------------------------------------------------- */

void (*const _dispatch_table[])(void) =
{
   null_handler, //0,                   /* Reserved. */
   null_handler, //Nmi,                 /* Non-Maskable interrupt handler. */
   null_handler, //0,                   /* Reserved. */
   null_handler, //0,                   /* Reserved. */
   null_handler, //0,                   /* Reserved. */
   Svc_handler,  //Svc,                 /* Supervisor call trap handler. */
   Dvz_handler,  //Dvz,                 /* Divide by zero trap handler. */
   Flag_handler, //Flg,                 /* Flag trap handler. */
   Bpt_handler,  //Bpt,                 /* Breakpoint trap handler. */
   Trc_handler,  //Trc,                 /* Trace trap handler. */
   Und_handler,  //Und,                 /* Undefined instruction trap. */
   null_handler, //0,                   /* Reserved. */
   Iad_handler,  //Iad,                 /* Invalid Address/Data trap. */
   null_handler, //0,                   /* Reserved. */
   null_handler, //0,                   /* Reserved. */
   null_handler, //Ise,                 /* In-System emulator interrupt. */

   /* The following are maskable interrupts. */

                        /* Interrupt
                             Vector     INT
                             Number    Number             Details
                           ---------   ------   ---------------------------   */
   null_handler,        /*    10h      INT0     Error condition occurred.     */
   ext_gpio44_handler,  /*    11h      INT1     Reserved.                     */
   km_obe_handler,      /*    12h      INT2     Host I/F Keyboard/Mouse OBE.  */
   pm_obe_handler,      /*    13h      INT3     Host I/F PM channel 1/2 OBE.  */
   access_bus3_handler, /*    14h      INT4    ACCESS.bus 3 interrupt.        */
   access_bus4_handler, /*    15h      INT5    ACCESS.bus 4 interrupt.        */
   int6_handler,        /*    16h      INT6     AHB Watcher Interrupt.        */
   miwu2_handler,       /*    17h      INT7     Reserved.                     */
   ext_gpio3_handler,   /*    18h      INT8     External Interrupt EXWGPIO3.  */
   mft16_handler,       /*    19h      INT9     MFT16 1 (INT1 OR INT2).       */
   adc_handler,         /*    1Ah      INT10    ADC interrupt.                */
   miwu3_handler,       /*    1Bh      INT11    MIWU WKINTE.                  */
   null_handler,         /*    1Ch      INT12    SPI.                          */
   access_bus1_handler, /*    1Dh      INT13    ACCESS.bus 1 interrupt.       */
   access_bus2_handler, /*    1Eh      INT14    ACCESS.bus 2 interrupt.       */
   scan_handler,        /*    1Fh      INT15    KBD Scan (KBSINT) or ACM.     */
   mswc_handler,        /*    20h      INT16    MSWC.                         */
   ITIM83_handler,      /*    21h      INT17    IMIT8-3.                      */
   CR_UART_handler,     /*    22h      INT18    CR_UART (TX Interrupt or RX Interrupt.     */
   ext_gpio5_handler,   /*    23h      INT19    External Interrupt EXWGPIO5.  */
   ext_gpio4_handler,   /*    24h      INT20    External Interrupt EXWGPIO4.  */
   ps2_sm_handler,      /*    25h      INT21    PS2 shift mechanism (PSINT1). */
   ext_gpio46_handler,  /*    26h      INT22    Reserved.                     */
   mft162_handler,      /*    27h      INT23    MFT16 2 (INT1 OR INT2).       */
   flash_shared_handler,/*    28h      INT24    Flash I/F write violation.    */
   km_ibf_handler,      /*    29h      INT25    Host I/F Keyboard/Mouse IBF.  */
   pm_ibf_handler,      /*    2Ah      INT26    Host I/F PM channel 1/2 IBF.  */
   ITIM82_handler,      /*    2Bh      INT27    IMIT8-2.                      */
   ITIM81_handler,      /*    2Ch      INT28    IMIT8-1.                      */
   null_handler,        /*    2Dh      INT29    Reserved.                     */
   ext_gpio1_handler,   /*    2Eh      INT30    External Interrupt EXWGPIO1.  */
   twm_handler          /*    2Fh      INT31    TWD System Tick.              */
};

/* ---------------------------------------------------------------
   Name: nmi_handler

   Non-maskable interrupt handler.

                        ! WARNING !
   If the NMI handler uses any RAM locations, DON'T USE RAM
   LOCATIONS THAT ARE IN THE RAM AREA THAT IS TESTED WHEN THE
   "SELF TEST" COMMAND IS ISSUED!  If an NMI occurs during the
   self-test, and the NMI handler modifies RAM, it is possible
   that a RAM error could be generated even though RAM is good.
   --------------------------------------------------------------- */
void nmi_handler(void)
{
   BYTE read_nmistat;

   /* Get the cause of the nmi.  This clears the NMISTAT register. */
   read_nmistat = NMISTAT;
}


void null_handler(void)
{
   ;
}

void Svc_handler(void)
{
    ;
}

void Dvz_handler(void)
{
  __asm__("push   $4,r0");\
  __asm__("sprd   isp,(r3,r2)");\
  __asm__("loadd  0(r3,r2),(r1,r0)");\
  __asm__("addd   $1,(r1,r0)");\
  __asm__("stord  (r1,r0),0(r3,r2)");\
  __asm__("pop     $4, r0 ");\
}

void Flag_handler(void)
{
    ;
}
void Bpt_handler(void)
{
    ;
}

void Trc_handler(void)
{
    ;
}
void Und_handler(void)
{
    ;
}

void Iad_handler(void)
{
    ;
}


void miwu2_handler(void)
{
   /* Multi-input wakeup unit

      miwu2 (PS/2, GPIO06, GPIO07, GPIO10, GPIO11, SensorPath and
                ACCESS.bus wakeup) */
   #if ESPI_SUPPORTED
   eSPI_IntHandler();
   #endif /* ESPI_SUPPORTED */

   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   {
      /* Not recovering from low power mode. */
      /* Set flag so that external interrupt handler will run. */
        //Service.bit.EXT_IRQ_SVC = 1;

      if ((WKPND1 & 0x09) & (WKEN1 & 0x09))
      {
          WKEN1  &= ~0x09;
          WKPCL1  =  0x09;
          Int_Lock_Aux_Port1();
          Load_Timer_B();
          Timer_B.bit.UNLOCK_TMO = 1;
          Aux_Port1.sequence = ACTIVE;
          Aux_Port1.ticker = 0;
          Enable_Timer_A();

      }

      /* Clear Interrupt Pending Flag. */
      ICU_Clr_Pending_Irq(ICU_EINT_MIWU2);

   }
}

void miwu3_handler(void)
{
   /* Multi-input wakeup unit

      miwu3 (Host Access Wake Up, LRESET# Active, GPIO12~17.) */

   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   {
      /* Not recovering from low power mode. */
      /* Set flag so that external interrupt handler will run. */
        //Service.bit.EXT_IRQ_SVC = 1;
        if ((WKPND5 & 0xC0) && (WKEN5 & 0xC0))
        {
            //HostModuleInit();

            WKPCL5 = 0x40;
            WKEN5 &= ~0xC0;
            DIDLE_Delay = 10;    // Delay for 10mS
            Enable_Timer_A();   // Load timer A if not enabled.
        }

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_MIWU3);
   }
}



void ext_gpio3_handler(void)
{
   /* External Interrupt EXWINT20 through the MIWU. */
   #if EXT_GPIO3_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO3);
   }
}


void mft162_handler(void)
{
   /* Multi-Function-16-Bit-Timer 2 interrupt from either timer. */

   #if KERNEL_TMRB_MFT16
   if (T2ICTRL & TICTRL_TDPND)
   {  /*
      An interrupt from interrupt source D has occurred.

      Timer/Counter 2 from the MFT162 (Multi-Function-16-Bit-Timer)
      Module is used for Timer B (150 microsecond timer).  This is
      interrupt source D. */

      Irq_Timer_B();       /* Handle the interrupt. */
      T2ICLR = TICLR_TDCLR; /* Clear the pending interrupt. */
   }
   #endif // KERNEL_TMRB_MFT16
   #if TICKER_1MS
   if (T2ICTRL & (TICTRL_TAPND | TICTRL_TBPND | TICTRL_TCPND))
   {  /*
      An interrupt from interrupt source A, B, or C has occurred. */

      /* Code for handling the interrupts may go here.
         Disable the interrupts if not being used. */
      T2IEN &= ~(TIEN_TAIEN | TIEN_TBIEN | TIEN_TCIEN);
   }
   #else // TICKER_1MS
   if (T2ICTRL & TICTRL_TAPND)
   {  /*
      An interrupt from interrupt source A has occurred.

      Timer/Counter 1 from the MFT16 (Multi-Function-16-Bit-Timer)
      Module is used for Timer PS2.  This is
      interrupt source A. */

      Irq_Timer_PS2();       /* Handle the interrupt. */
      T2ICLR = TICLR_TACLR; /* Clear the pending interrupt. */
   }

   if (T2ICTRL & (TICTRL_TBPND | TICTRL_TCPND))
   {  /*
      An interrupt from interrupt source A, B, or C has occurred. */

      /* Code for handling the interrupts may go here.
         Disable the interrupts if not being used. */
      T2IEN &= ~(TIEN_TBIEN | TIEN_TCIEN);
   }
   #endif // TICKER_1MS
}
#if !KERNEL_TICKER_T0
void ITIM81_handler(void)
{
    ICU_Clr_Pending_Irq(ICU_EINT_ITIM81);
    SET_BIT(ITCTS1, TO_STS); // clear the timeout status
    Irq_Timer_A();       /* Handle the interrupt. */
}
#endif // !KERNEL_TICKER_T0

#if !KERNEL_TMRB_MFT16
void ITIM82_handler(void)
{
    ICU_Clr_Pending_Irq(ICU_EINT_ITIM82);
	if (ITCTS2 & TO_STS)
	{
        SET_BIT(ITCTS2, TO_STS); // clear the timeout status
        Irq_Timer_B();       /* Handle the interrupt. */
	}
}
#endif // !KERNEL_TMRB_MFT16



void access_bus1_handler(void)
{
   /* ACCESS.bus. */

   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif !I2C_SUPPORTED
   ;
   #else
   I2c_Irq(SMB_1);
   #endif
}


void access_bus2_handler(void)
{
   /* ACCESS.bus. */

   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif !I2C_SUPPORTED
   ;
   #else
   I2c_Irq(SMB_2);
   #endif
}

void access_bus3_handler(void)
{
   /* ACCESS.bus. */

   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif !I2C_SUPPORTED
   ;
   #else
   I2c_Irq(SMB_3);
   #endif
}

void access_bus4_handler(void)
{
   /* ACCESS.bus. */

   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif !I2C_SUPPORTED
   ;
   #else
   I2c_Irq(SMB_4);
   #endif
}

void Irq_ADC(void);
void adc_handler(void)
{
	Irq_ADC();
}

void CR_UART_handler(void)
{
#if CR_UART_SUPPORTED
	if(uart_get_int_type() == UART_RX_INT)
	{
		ConsoleProcessInputFromISR();
	}

	if (uart_get_int_type() == UART_TX_INT)
	{
		ConsoleProcessOutputFromISR();
	}
#endif // CR_UART_SUPPORTED
}

void ext_gpio4_handler(void)
{
   /* External Interrupt EXWINT21 through the MIWU. */
   #if EXT_GPIO4_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO4);
   }
}

void ext_gpio5_handler(void)
{
   /* External Interrupt EXWINT21 through the MIWU. */
   #if EXT_GPIO5_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO5);
   }
}


void ps2_sm_handler(void)
{
   ICU_Clr_Pending_Irq(ICU_EINT_PS2_1);

   /* Code for handling auxiliary device
      using hardware shift mechanism. */

   if ((PSTAT & PSTAT_ACH) == AUX_PORT1_ACTIVE)
      Irq_Aux_Port1();
   #if AUX_PORT2_SUPPORTED
   else if ((PSTAT & PSTAT_ACH) == AUX_PORT2_ACTIVE)
      Irq_Aux_Port2();
   #endif
   #if AUX_PORT3_SUPPORTED
    else if ((PSTAT & PSTAT_ACH) == AUX_PORT3_ACTIVE)
      Irq_Aux_Port3();
   #endif

}

void ext_gpio1_handler(void)
{
   /* GLP NOTE: THIS NEEDS WORK.  WAS IRQ21. */

   /* External Interrupt EXWINT21 through the MIWU. */

   #if EXT_GPIO1_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      //Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO1);
   }
}


void ext_gpio44_handler(void)
{
   /* External Interrupt EXWINT1 through the MIWU. */

   #if EXT_GPIO44_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      //Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO44);
   }
}


void ext_gpio46_handler(void)
{
   /* External Interrupt EXWINT22 through the MIWU. */

   #if EXT_GPIO46_WAKEUP
   if (Int_Var.Low_Power_Mode)
   {
      /* Recovering from low power mode.

         This must be a valid wakeup interrupt because
         only valid wakeup interrupts are enabled.  */

      Int_Var.Low_Power_Mode = 0;/* Clear Low_Power_Mode flag to
                                    notify IDLE loop of recovery. */
   }
   else
   #endif
   {
      /* Not recovering from low power mode. */

      /* Set flag so that external interrupt handler will run. */
      Service.bit.EXT_IRQ_SVC = 1;

      /* Disable interrupt as signal to external interrupt handler. */
      ICU_Disable_Irq(ICU_EINT_GPIO46);
   }
}


void Icu_Init(void)
{
   /* This routine assumes that the interrupts are disabled
      and that the interrupt base register has been set. */

   unsigned int psr_value;

   IENAM0 = 0;       /* Mask all interrupts. */
   IENAM1 = 0;       /* Mask all interrupts. */
   IECLR0 = 0xFFFF;  /* Clear all. */
   IECLR1 = 0xFFFF;  /* Clear all. */

   /* The timer from the TWM (Timing and Watchdog Module) is used for
      Timer A.  Configure timer interrupt as a rising edge interrupt.
      Interrupt is enabled as necessary.  It is not enabled here. */
   ICU_Clr_Pending_Irq(ICU_EINT_T0OUT);

   /* Timer/Counter 2 from the MFT16 (Multi-Function-16-Bit-
      Timer) Module is used for Timer B.  Configure interrupt
      from MFT16 timers as a high level interrupt.  The interrupt
      is enabled here.  The timer can be stopped as needed. */

   ICU_Enable_Irq(ICU_EINT_MFT162);

   /* Code for handling auxiliary device
      using hardware shift mechanism. */

   /* Configure PS/2 interrupt #1 (shift mechanism)
      as a high level interrupt. */
   ICU_Enable_Irq(ICU_EINT_PS2_1);

   /* Configure scan interrupt as a high level interrupt. */
   /* Mark add Enable KBD SCAN martix interrupt 2006/7/20 */
   ICU_Clr_Pending_Irq(ICU_EINT_KBD_SCAN);
   ICU_Enable_Irq(ICU_EINT_KBD_SCAN);
   /* Mark adden Enable KBD SCAN Martix interrupt 2006/7/20 */
   /* Primary interface (keyboard and mouse) input buffer full
      (IBF) interrupt.  Configure it as a high level interrupt
      and enable it.  It will be enabled/disabled using the Host
      Interface Control register. */
   ICU_Enable_Irq(ICU_EINT_IBF);

   /* Primary interface (keyboard and mouse) output buffer empty
      (OBE) interrupt.  Configure it as a high level interrupt
      and enable it.  It will be enabled/disabled using the Host
      Interface Control register. */
   ICU_Enable_Irq(ICU_EINT_OBE);

   /* Secondary interface (power management) input buffer full
      (IBF) interrupt.  Configure it as a high level interrupt
      and enable it.  It will be enabled/disabled using the Host
      Interface Control register. */
   ICU_Enable_Irq(ICU_EINT_IBF2);

   /* If the I2C bus (ACCESS.bus) is to be used. */
   #ifndef I2C_SUPPORTED
   #error I2C_SUPPORTED switch is not defined.
   #elif I2C_SUPPORTED
    /* Configure interrupt as a high level interrupt and enable it.
       It will be enabled/disabled using the SMB1 Control 1 register. */
    ICU_Enable_Irq(ICU_EINT_I2C);
    ICU_Enable_Irq(ICU_EINT_I2C2);
    ICU_Enable_Irq(ICU_EINT_I2C3);
    ICU_Enable_Irq(ICU_EINT_I2C4);
   #endif

    SET_BIT(WKEDG2, MASK(bit5));	/* Select the edge as falling edge. */
    // delay between edge setting and clear pending
    WKEDG_WKPCL_DELAY;
    SET_BIT(WKPCL2, MASK(bit5));	/* Clear the SWIN pending flag. */
	CLEAR_BIT(WKEN2, MASK(bit5));		/* Re-enable the input. */
    ICU_Enable_Irq(ICU_EINT14);

    /* Enable SHM interrupt. */
    SET_BIT(SMC_STS, HSEM2W);   /* Clear Interrupt pending flag. */
    //MASK_SET_BIT(SMC_CTRL, 0x7F, 0x10);    /* Enable Interrupt. */
    ICU_Clr_Pending_Irq(24); // ICU_EINT_SHM
    ICU_Enable_Irq(24); // ICU_EINT_SHM


#if PORT80_SUPPORTED
    // setup PORT80 interrupt
    ICU_Disable_Irq(ICU_EINT6);
    ICU_Clr_Pending_Irq(ICU_EINT6);
    ICU_Enable_Irq(ICU_EINT6);
#endif

#if PECI_SUPPORTED
       // setup PECI interrupt
       ICU_Disable_Irq(ICU_EINT6);
       ICU_Clr_Pending_Irq(ICU_EINT6);
       ICU_Enable_Irq(ICU_EINT6);
#endif

    ICU_Clr_Pending_Irq(ICU_EINT_ITIM81);
    ICU_Enable_Irq(ICU_EINT_ITIM81);

    ICU_Clr_Pending_Irq(ICU_EINT_ITIM82);
    ICU_Enable_Irq(ICU_EINT_ITIM82);

   /* Set the I bit in the Processor Status Register.
      Enable interrupts later using the EI instruction.*/
   _spr_("psr", psr_value);         /* psr_value = Processor Status Register. */
   psr_value |= (1 << (PSR_I_BIT)); /* Set I bit in psr_value. */
   _lpr_("psr", psr_value);         /* Processor Status Register = psr_value. */
}
