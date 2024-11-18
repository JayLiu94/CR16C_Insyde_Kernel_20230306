/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     fu_init.c - Flash Update Init
 *
 * Project:
 *     Firmware for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "types.h"
#include "com_defs.h"
#include "fu.h"
#include "shm.h"
#include "fu_utils.h"
#include "swtchs.h"
#include "regs.h"

#if RAM_BASED_FLASH_UPDATE

#undef USE_SHM_INTERRUPT

/* CTL_HERES field values */
#define HERES_IGNORE_WRITES   0x01

/*------------------------------------------------------------------------------------*
 * Buffer (implemented in Shared Memory) through which the host communicates with the *
 * Flash Update firmware                                                              *
 *------------------------------------------------------------------------------------*/

extern const BYTE _FWU_IMAGE_START;
extern BYTE _FWU_START, _FWU_END;

/*------------------------------------------------------------------------------
 * Forward function declarations
 *----------------------------------------------------------------------------*/

// Forward declaration
void FLASH_UPDATE_callback(BYTE smc_sts);

#if !RUN_FROM_MRAM
void FLASH_UPDATE_copy_code_to_ram(){
	WORD i;
    // Copy the FW Update code to RAM
    for (i = 0; i < &_FWU_END - &_FWU_START; i++)
        *(&_FWU_START + i) = *(&_FWU_IMAGE_START + i);
}
#endif // !RUN_FROM_MRAM

/*------------------------------------------------------------------------------
 * Function:
 *    FLASH_UPDATE_init
 *
 * Description:
 *    Initializes the Flash Update mechanism.
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_UPDATE_init(void)
{
	BYTE smc_ctl = 0;

 	/* Initialize global variables */
	FLASH_device_size = GET_FIELD(SPI_FL_CFG, DEV_SIZE) * DEV_SIZE_UNIT;
	FLASH_UPDATE_active = FALSE;

    /*------------------------------------------------------------------------
     * Configure Host Access
     *------------------------------------------------------------------------*/

    /* Ignore write transactions on forbidden access */
    SET_FIELD(smc_ctl, CTL_HERES, HERES_IGNORE_WRITES);

    /*------------------------------------------------------------------------
     * Configure access
     *------------------------------------------------------------------------*/
    /* Enable prefetch */
    SET_BIT(smc_ctl, PREF_EN);

#ifdef USE_SHM_INTERRUPT
	/*------------------------------------------------------------------------
     * Enable Semaphore 2 interrupt
     *------------------------------------------------------------------------*/
    SET_BIT(smc_ctl, HSEM2_IE);

    SHM_init(FLASH_UPDATE_callback, smc_ctl);
#else
    SMC_CTRL = smc_ctl;
    SMC_STS = SMC_STS;  /* Clear status */
#endif


    /*------------------------------------------------------------------------
     * Configure Shared RAM Window
     * Window 2 is used (since Window 1 is the only flash-capable window)
     *------------------------------------------------------------------------*/
    /* Set Shared RAM window start to overlay write command buffer
     * Window base is the offset of the window from RAM start */

    /* Set Shared RAM window size to 16 (2**4) -
     * WCB protocol requires 16 byte Write Command buffer */

    /* Remove protections from Window 2 to enable use as Write Command Buffer */
    SHM_window_config(SHM_WINDOW_TWO, (DWORD)write_command_buffer, 4, 0);

	// Copy the FW Update code to RAM
    #if !RUN_FROM_MRAM
    FLASH_UPDATE_copy_code_to_ram();
    #endif // !RUN_FROM_MRAM
    FLASH_jbb = 0;
}

#endif // RAM_BASED_FLASH_UPDATE


