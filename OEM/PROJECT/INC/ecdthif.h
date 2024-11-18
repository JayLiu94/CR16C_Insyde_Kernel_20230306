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


#ifndef ECDT_HOST_IF_H
#define ECDT_HOST_IF_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "com_defs.h"

#define OFFSET_SEMAPHORE         0
#define OFFSET_OPERATION         3
#define OFFSET_ADDRESS           4
#define OFFSET_DATA              4    // Commands have either address or data
#define OFFSET_DATA_GENERIC      8    // Generic commands have both address and data
#define OFFSET_GENERIC_CODE      12
#define OFFSET_GENERIC_HAS_ADDR  13
#define OFFSET_GENERIC_NUM_BYTES 14
#define OFFSET_GENERIC_WRITE     15
/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/
bool ECDT_SHM_HIF_handle(void);

#endif // ECDT_HOST_IF_H
