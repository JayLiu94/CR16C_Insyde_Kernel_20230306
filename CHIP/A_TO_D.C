/*-----------------------------------------------------------------------------
 * MODULE A_TO_D.C
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 * This file contains code to handle the A/D converter.
 *
 * Revision History (Started April 30, 1998 by GLP)
 *
 * 02 Jul 98   Added code to Read_AtoD to enable the A/D input as
 *             an analog input if that particular input is to be
 *             read through the A/D converter.
 * 01 Oct 98   Changed the initialization of the A/D module and
 *             added a delay between enabling the A/D converter
 *             module and starting the conversion.
 * 19 Aug 99   Updated copyright.
 *             Calculate_Microsecond_Delay expects a Microsecond
 *             Timer Setting value.  Changed call from 100 to
 *             TIMER_US(100).
 * EDH
 * 24 OCT 00   Changed the A/D converter from PC87570 to PC87591.
 * 25 Feb 01   Changed the A/D converter from wait result to interrupt.
 *					Voltage Channel 1 is used for Insyde extended command.
 *					Voltage Channel 2-3 is used for OEM service routine,
 * 				this can be defined by OEM.
 * ------------------------------------------------------------------------- */

#define A_TO_D_C

#include "a_to_d.h"
#include "com_defs.h"
#include "oem.h"
#include "proc.h"
#include "timers.h"
#include "purdat.h"

/* ---------------------------------------------------------------
   Name: Read_AtoD

   Read data from A/D converter.

   Input: converter channel.

   Returns: A/D value.
   --------------------------------------------------------------- */
WORD Read_AtoD(BYTE data)
{
   WORD AD_Value;
   return ((WORD) AD_Value);
}

void Irq_ADC(void)
{
    BYTE index;
    WORD *pntr;
    WORD *ch_pntr;

    ICU_Disable_Irq(ICU_EINT_ADC);        /* Disable ADC interrupt. */
    pntr = (WORD *) &AD_Value;
    ch_pntr = (WORD *) &CHN0DAT;

    while (ch_pntr <= &CHN5DAT)
    {
        *pntr = *ch_pntr;
        pntr++;
        ch_pntr++;
    }


   ICU_Clr_Pending_Irq(ICU_EINT_ADC);   /* Clear any pending interrupt. */
   ICU_Enable_Irq(ICU_EINT_ADC);        /* Enable ADC interrupt. */
}
/* ---------------------------------------------------------------
   Name: inital_ADC
   Function: SET Software conversion
   Input: CORE clock Division Factor.
   --------------------------------------------------------------- */
void inital_ADC(BYTE divitor)
{
//	DEVALT4 |= 0x01; /* ADC0-3 Select */
	SET_FIELD(ATCTL, SCLKDIV, divitor & 0x3F);   /* SET SCLKDIV */
	SET_BIT(ADCCNF, ADCEN);  /* SET ADCMD = 0  channel conversioin mode
	                   SET ADCRPTC = 0 one-shot
	                   SET ADCEN = 1 Enable ADC module */
}
