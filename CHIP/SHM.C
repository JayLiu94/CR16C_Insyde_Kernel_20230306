/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents: 
 *     shm.c - Shared Memory Interface - Initialization and Interrupt Handler
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#include "types.h"
#include "shm.h"
#include "swtchs.h"
#include "regs.h"

#if RAM_BASED_FLASH_UPDATE

SHM_callback_t SHM_callback;
void SHM_int_handler(void);
/*------------------------------------------------------------------------------
 * Function: 
 *    SHM_init
 *
 * Description:
 *    Initializes Shared memory hardware module elements that are relevant for
 *    both Shared Memory RAM windows.
 *    Note that flash-related initializations which are expected to be done by
 *    the boot ROM are not done here (e.g. flash size, read burst size, memory
 *    protection, HOSTWAIT clearing).
 *
 * Parameters:
 *    shm_callback  Function to be called upon shared memory interrupt
 *    smc_ctl       Value to be set in bits 0..6 of Shared Memory Core Control
 *                  register. Bit 7 is ignored. See Datasheet for detailed
 *                  description of SMC_CTL register.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
#ifdef USE_SHM_INTERRUPT
void SHM_init(SHM_callback_t shm_callback, BYTE smc_ctl)
 {
    /*------------------------------------------------------------------------
     * Configure Host Access
     *------------------------------------------------------------------------*/

    /* Don't stall SHM transactions (default chip state) */
    SHM_ENABLE;

    /*------------------------------------------------------------------------
     * Configure interrupt
     *------------------------------------------------------------------------*/

    ICU_mask_int(INT_SHM);
    ICU_install_handler(INT_SHM, SHM_int_handler);
    ICU_clear_int(INT_SHM);
    ICU_unmask_int(INT_SHM);
    
    // Save callback
    SHM_callback = shm_callback;
    
    /* Set required control value */
    SMC_CTL = smc_ctl;
    
    /* Clear status */
    SMC_STS = SMC_STS;
}
#endif //USE_SHM_INTERRUPT

/*------------------------------------------------------------------------------
 * Function: 
 *    SHM_window_config
 *
 * Description:
 *    Initializes Shared memory hardware module elements that are relevant for
 *    the WCB protocol implementation.
 *    Note that flash-related initializations which are expected to be done by
 *    the boot ROM are not done here (e.g. flash size, read burst size, memory
 *    protection, HOSTWAIT clearing).
 *
 * Parameters:
 *    window_num    Shared Memory Window to be configured
 *    window_base   Base address of window in range 0..0x1000
 *                  Size of 0xFFFF designates that window is disabled
 *    window_size   Exponent of 2 - e.g. 4 designate RAM window of 16 (2**4)
 *                  Valid values are 0..12
 *    window_prot   Mask of Shared Access RAM Window Protection from Host:
 *                  Bit 0 = Low Half of window is Read Protected
 *                  Bit 1 = High Half of window is Write Protected
 *                  Bit 2 = Low Half of window is Read Protected
 *                  Bit 3 = High Half of window is Write Protected
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void SHM_window_config(SHM_window_t window_num,
                       DWORD       window_base,
                       BYTE        window_size,
                       BYTE        window_prot)
 {
    /*------------------------------------------------------------------------
     * Configure Shared RAM Window
     *------------------------------------------------------------------------*/
    
    /* Set Shared RAM window size - */
    if (window_num == SHM_WINDOW_ONE)
        SET_FIELD(WIN_SIZE, WIN_SIZE_RWIN1_SIZE, window_size);
    else
        SET_FIELD(WIN_SIZE, WIN_SIZE_RWIN2_SIZE, window_size);

    /* Set required Host Read/Write Protection for Window */
    if (window_num == SHM_WINDOW_ONE)
        SET_FIELD(WIN_PROT, WIN_PROT_RWIN1, window_prot);
    else
        SET_FIELD(WIN_PROT, WIN_PROT_RWIN2, window_prot);

    /* Set Shared RAM window start -
     * Window base is the offset of the window from RAM start */
    if (window_num == SHM_WINDOW_ONE)
        WIN_BASE1 = (WORD)(window_base - RAM_BASE_ADDRESS);
    else
        WIN_BASE2 = (WORD)(window_base - RAM_BASE_ADDRESS);
}

/*------------------------------------------------------------------------------
 * Function: 
 *    SHM_int_handler
 *
 * Description:
 *    Shared Memory interrupt handler.
 *
 * Side effects:
 *    Interrupt status is not cleared - 
 *    a. To enable callbacks to see the interrupt trigger reason
 *    b. to enable callback to re-use interrupt (e.g. in Flash Update invocation)
 *    It is the responsibility of each callback to clear the status of the
 *    trigger it handles.
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
#ifdef USE_SHM_INTERRUPT
#pragma interrupt (SHM_int_handler)
void SHM_int_handler(void)
{
	/* Call Shared Memory callback */
    if (SHM_callback != NULL)
        SHM_callback(SMC_STS);
        
}
#endif //USE_SHM_INTERRUPT

#endif // RAM_BASED_FLASH_UPDATE

