/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     fu_hndl.c - Flash Update Handle
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
#include "swtchs.h"
#include "regs.h"

#if RAM_BASED_FLASH_UPDATE

#undef USE_SHM_INTERRUPT

/*------------------------------------------------------------------------------
 * Forward function declarations
 *----------------------------------------------------------------------------*/

void FLASH_UPDATE_callback(BYTE smc_sts);
#if !RUN_FROM_MRAM
void FLASH_UPDATE_copy_code_to_ram(void);
#endif // !RUN_FROM_MRAM

/*------------------------------------------------------------------------------
 * Function:
 *    FLASH_UPDATE_callback/handle
 *
 * Description:
 *    Handle Flash Update
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
#ifdef USE_SHM_INTERRUPT
extern void _FLASH_UPDATE_callback(BYTE smc_sts);
void FLASH_UPDATE_callback(BYTE smc_sts)
{
     if (FLASH_jbb == 0xAA55){
         // the code will get here after progaming new EC code.
         #if !RUN_FROM_MRAM
         FLASH_UPDATE_copy_code_to_ram();
         #endif // !RUN_FROM_MRAM
         FLASH_jbb = 0;
     }
    _FLASH_UPDATE_callback(smc_sts);
}
#else
void _FLASH_UPDATE_handle(void);
void FLASH_UPDATE_handle(void)
{
     if (FLASH_jbb == 0xAA55){
         // the code will get here after progaming new EC code.
         #if !RUN_FROM_MRAM
         FLASH_UPDATE_copy_code_to_ram();
         #endif // !RUN_FROM_MRAM
         FLASH_jbb = 0;
     }
    _FLASH_UPDATE_handle();
}
#endif // USE_SHM_INTERRUPT

#endif // RAM_BASED_FLASH_UPDATE


