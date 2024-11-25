/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     sib.h - SuperI/O Internal Bus (SIB) driver
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

// ******************************************************************************
// * Global Data Structure                                                      *
// *                                                                            *
// * Name       Description                                                     *
// * ------------------ ------------------------------------------------------- *
// * SIB_device_t   SIB device id type.                                         *
// *                                                                            *
// * Global Variables                                                           *
// *                                                                            *
// * Name       Description                                                     *
// * ------------------ ------------------------------------------------------- *
// * RAM_BASE_ADDRESS   base address of ec ram.                                 *
// *                                                                            *
// * Global Function                                                            *
// *                                                                            *
// * Name       Description                                                     *
// * ------------------ ------------------------------------------------------- *
// * SIB_set_lock   lock legacy device .                                        *
// * SIB_clear_lock un-lock legacy device.                                      *
// * SIB_get_violation  check value.                                            *
// * SIB_read_reg   Read legacy device value.                                   *
// * SIB_write_reg  Write to legacy device.                                     *
// * SIO_CONFIG_READ    Read I/O device register.                               *
// * SIO_CONFIG_WRITE   Write to I/O device register.                           *
// *                                                                            *
// * Development History                                                        *
// *                                                                            *
// * Data   Version     Description                                             *
// * ---------- --------------- ----------------------------------------------- *
// * 2007/01/30 0.88        First release                                       *
// *                                                                            *
// ******************************************************************************
#ifndef SIB_H
#define SIB_H

#include "com_defs.h"
#include "swtchs.h"


#define LPC_INDEX           0
#define LPC_DATA            1

/* Global data structure */
typedef enum{
    SIB_CONFIG_DEV      = 0x0001,
    SIB_SP_DEV          = 0x0004,
    SIB_FIR_DEV         = 0x0020,
    SIB_HGPIO           = 0x0080,
    SIB_MSWC_DEV        = 0x0100,
    SIB_SHAR_MEM_DE     = 0x0400,
} SIB_device_t;

/* Global functions */
void SIB_set_lock(/*IN*/ SIB_device_t device);
void SIB_clear_lock(/*IN*/ SIB_device_t device);
WORD SIB_get_violation();
void SIB_read_reg(/*IN*/ SIB_device_t device,
                  /*IN*/ BYTE        offset,
                  /*OUT*/ BYTE*       value);

void SIB_write_reg(/*IN*/ SIB_device_t device,
                   /*IN*/ BYTE        offset,
                   /*IN*/ BYTE       value);

#define SIO_CONFIG_READ(reg,val) \
            SIB_write_reg(SIB_CONFIG_DEV,LPC_INDEX,reg);\
            SIB_read_reg(SIB_CONFIG_DEV,LPC_DATA,&val);

#define SIO_CONFIG_WRITE(reg,val) \
            SIB_write_reg(SIB_CONFIG_DEV,LPC_INDEX,reg);\
            SIB_write_reg(SIB_CONFIG_DEV,LPC_DATA,val);

#endif // SIB_H





