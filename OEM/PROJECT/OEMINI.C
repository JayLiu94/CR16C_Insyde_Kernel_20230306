/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMINI.C - OEM variables and features initialized.
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "swtchs.h"
#include "types.h"
#if ECDT_SHM_SUPPORTED
#include "ecdthif.h"
#else
#include "com_defs.h"
#endif
#include "regs.h"
#include "smb_tran.h"
#include "oemdat.h"
#include "events.h"
#include "pwrseq.h"
#include "pindef.h"
#include "oemmain.h"


#if PORT80_SUPPORTED
#include "port80.h"
#endif // PORT80_SUPPORTED

#if PECI_SUPPORTED
void OEM_PECI_Init(void);
#endif // PECI_SUPPORTED

#if CR_UART_SUPPORTED
#include "cr_uart.h"
#endif // CR_UART_SUPPORTED

/* ---------------------------------------------------------------
   Name: Hookc_Warm_Reset_Begin

   Called before Warm_Reset.
   --------------------------------------------------------------- */
void Hookc_Warm_Reset_Begin(void)
{

}

/* ---------------------------------------------------------------
   Name: Hookc_Warm_Reset_End

   Called after Warm_Reset.
   --------------------------------------------------------------- */
void Hookc_Warm_Reset_End(void)
{

}

/* ---------------------------------------------------------------
   Name: Hookc_Cold_Reset_Begin

   Called before Cold_Reset.
   --------------------------------------------------------------- */
void Hookc_Cold_Reset_Begin(void)
{

}

/* ---------------------------------------------------------------
   Name: Hookc_Cold_Reset_End

   Called after Cold_Reset.
   --------------------------------------------------------------- */

void Hookc_Cold_Reset_End(void)
{
    SysState = InS4;


    EventsInit();
    //Init_SMB_Transfer();
    PwrSeq_Init(SeqNullHandle, 0);

    // release Host wait -
    // Should be done here if system is not shared BIOS.
    // In shared BIOS system this should be done only
    // at the end of the power sequence (on S0 entry)
    // SMC_CTRL = SMC_CTRL  | 0x80;

    SET_BIT(RSTCTL, LRESET_MODE);


#if PORT80_SUPPORTED
    // Disable SDP and SDP visibility mode
    SDP_DISABLE();
    SDP_config(SDP_NORMAL_MODE);

    // init PORT80
    DP80_init();
#endif

#if PECI_SUPPORTED
   OEM_PECI_Init();
#endif

#if CR_UART_SUPPORTED
	CR_UART_init();
	DEBUG_PRINTF("Nuvoton EC START\n");
#endif // CR_UART_SUPPORTED

}

/* ---------------------------------------------------------------
   Name: Modules_Control

   Called after Cold_Reset.
   --------------------------------------------------------------- */
void Modules_Control(void)
{
    SET_BIT(DISWT_CTL, ACC_DW | RAM_DW | ROM_DW);
    SET_BIT(DISWT_CTL1, CR_LPW | CB_DW);

    SET_BIT(PWDWN_CTL1, 0x80 | SDP_PD | FIU_PD | UART_PD);
    PWDWN_CTL2 = 0xFC;  /* Only keep PWM A & B work. */
    PWDWN_CTL3 = 0xDC;  /* Only SMB1 & SMB2 work. */
    PWDWN_CTL4 = 0xC0;  /* Power Down SPIP & OWI */
#if CR_UART_SUPPORTED
    PWDWN_CTL5 = 0xC0;  /* Power Down MSWC, DP80 */
#else // CR_UART_SUPPORTED
    PWDWN_CTL5 = 0xC8;  /* Power Down MSWC, DP80, C2HACC */
#endif // CR_UART_SUPPORTED
}

