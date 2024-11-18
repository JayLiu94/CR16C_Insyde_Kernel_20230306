/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     smb.h - Shared Memory Interface
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef SHM_H
#define SHM_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "com_defs.h"


#define CREATE_MASK(bit_position)      (0x1 << (bit_position))

/*------------------------------------------------------------------------------
 * Memory Map values
 *----------------------------------------------------------------------------*/
#define RAM_BASE_ADDRESS       0x10000

/*------------------------------------------------------------------------------
 * Typedefs
 *----------------------------------------------------------------------------*/
 /* SHM window number. */
typedef enum {
    SHM_WINDOW_ONE = 0,  /* do not change - code relies on enum value */
    SHM_WINDOW_TWO = 2,
    SHM_WINDOW_LAST
} SHM_window_t;

typedef void (*SHM_callback_t)(BYTE smc_sts);

/*------------------------------------------------------------------------------
 * Global variables
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Macros
 *----------------------------------------------------------------------------*/

#define SHM_GET_STATUS SMC_STS
#define SHM_CLEAR_STATUS(bitmask)  (SMC_STS = (BYTE)bitmask)

#define SHM_SET_SEM1(bitmask)       (SHAW1_SEM = (BYTE)bitmask)
#define SHM_CLEAR_SEM1(bitmask)     (SHAW1_SEM &= ~((BYTE)bitmask))
#define SHM_GET_SEM1                SHAW1_SEM
#define SHM_SET_SEM2(bitmask)       (SHAW2_SEM = (BYTE)bitmask)
#define SHM_CLEAR_SEM2(bitmask)     (SHAW2_SEM &= ~((BYTE)bitmask))
#define SHM_GET_SEM2                SHAW2_SEM


/*------------------------------------------------------------------------------
 * Macro:
 *    SHM_STALL
 *
 * Description:
 *    Stalls host read transactions and does not enable interrupts notifying the
 *    core of a pending transaction. It is intented to be called whenever a flash
 *    write or erase operation is initiated - the SHM transaction are stalled until
 *    read and write operation is complete.
 *
------------------------------------------------------------------------------*/
#define SHM_STALL    SET_BIT(FLASH_SIZE, STALL_HOST)

/*------------------------------------------------------------------------------
 * Macro:
 *    SHM_ENABLE
 *
 * Description:
 *    Re-enables host read transactions - all transactions will be served automatically
 *    by the hardware, including transactions that are pending from the period when the
 *    transactions were stalled. It is intended to be called when flash write or
 *    erase operation is complete.
 *
------------------------------------------------------------------------------*/
#define SHM_ENABLE    CLEAR_BIT(FLASH_SIZE, STALL_HOST)

/*------------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/

void SHM_init(SHM_callback_t shm_callback, BYTE smc_ctl);
void SHM_window_config(SHM_window_t window_num,
                       DWORD       window_base,
                       BYTE        window_size,
                       BYTE        window_prot);

#endif // SMB_H


