/*----------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                              */
/* Copyright (c) 2009 by Nuvoton Technology Corporation                                                     */
/* All rights reserved                                                                                      */
/*----------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                           */
/*       ecdthif.h - Header file for Embeded Controller Debug Tool - Host Interface implementation          */
/*                                                                                                          */
/* Project: Firmware for Nuvoton Notebook Embedded Controller Peripherals                                   */
/*----------------------------------------------------------------------------------------------------------*/


#ifndef LPC_I2C_H
#define LPC_I2C_H

#include "kernel\core\inc\swtchs.h"

#if LPC_TO_I2C_SUPPORTED

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "kernel\core\inc\defs.h"

/*------------------------------------------------------------------------------
 * WRITE COMMAND BUFFER (WCB) Interface Definitions
 *----------------------------------------------------------------------------*/
#define LPC2I2C_WIN_SIZE    4
#define LPC2I2C_SHM_SIZE    (0x01 << LPC2I2C_WIN_SIZE)

#define HOST_REQUEST_BUFFER_MAX_SIZE       0x2000
#define CORE_SEND_BUFFER_MAX_SIZE          0x2000

/*------------------------------------------------------------------------------
 * Global declarations
 *----------------------------------------------------------------------------*/
extern volatile BYTE lpci2c_command_buffer[LPC2I2C_SHM_SIZE];
extern volatile BYTE lpci2c_request_buffer[HOST_REQUEST_BUFFER_MAX_SIZE];
extern volatile BYTE lpci2c_reply_buffer[CORE_SEND_BUFFER_MAX_SIZE];

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/
void LPC_I2C_SHM_init(void);
void LPC_I2C_SHM_Int_handle(void);
//void LPC_I2C_SHM_Service_loop(void);
BYTE LPC_I2C_SHM_Service_loop(void);
void LPC_I2C_SMBus_Callback(BITS_8 i2c_state, BYTE Channel);

#endif //LPC_TO_I2C_SUPPORTED

#endif // LPC_I2C_H
