/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     PORT80.C
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "types.h"
#include "com_defs.h"
#include "swtchs.h"
#include "icu.h"
#include "port80.h"

void OEM_Get_Port80_Val(BYTE port80_val);

#if PORT80_SUPPORTED


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DP80_init                                                                                     */
/*                                                                                                         */
/* Description:                                                                                            */
/*    This function init PORT module                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None                                                                                                 */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void DP80_init()
{
    SET_BIT(DP80CTL, RFIFO | CIEN | SYNCEN | DP80EN);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DP80_int_handler                                                                              */
/*                                                                                                         */
/* Description:                                                                                            */
/*    The interrupt handler is being called when a data is written to the hardware FIFO buffer.            */
/*    We are reading the data from the buffer                                                              */
/*    NOTE: Use the buffer only when there is a need to save the received data.                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None                                                                                                 */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#pragma interrupt(DP80_int_handler)
void DP80_int_handler(void)
{
    BYTE port80_val;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read all the available data from the HW buffer and write it in our buffer.                          */
    /*-----------------------------------------------------------------------------------------------------*/
    while (READ_BIT(DP80STS, FNE))
    {
        port80_val = DP80BUF;

        OEM_Get_Port80_Val(port80_val);
    }

    /* Clear Status */
    SET_BIT(DP80STS, FWR);

    /* Clear interrupt */
    ICU_Clr_Pending_Irq(ICU_EINT6);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: SDP_config                                                                                    */
/*                                                                                                         */
/* Description:                                                                                            */
/*    This function configure the SDP module.                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    SDP_Mode_Select_t     mode    Visibility mode, or normal mode.                                       */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void SDP_config(SDP_Mode_Select_t mode)
{
    if (mode == SDP_VISIBILITY_MODE)
    {
        CLEAR_BIT(SDP_REG(SDP_CTS), SDP_MOD);
    }
    else /* mode == SDP_NORMAL_MODE */
    {
        SET_BIT(SDP_REG(SDP_CTS), SDP_MOD);
    }
}


#endif // PORT80_SUPPORTED




