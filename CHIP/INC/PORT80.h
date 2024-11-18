/*----------------------------------------------------------------------------------------------------------*/
/* Copyright (c) 2006-2010 by Nuvoton Technology Corporation                                                */
/* All rights reserved.                                                                                     */
/*<<<-------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                           */
/*     PORT80.h - Debug port 80 (DP80) driver                                                               */
/*                                                                                                          */
/* Project:                                                                                                 */
/*     Driver Set for uRider Notebook Embedded Controller Peripherals                                     */
/*------------------------------------------------------------------------------------------------------->>>*/

#ifndef PORT80_H
#define PORT80_H

#define DP80_BUFFER_SIZE  0x10


void DP80_init(void);


// SDP stuff //

typedef enum
{
    SDP_PORT_0 = 0,
    SDP_PORT_1 = 1
} SDP_Port_t;

typedef enum
{
    SDP_VISIBILITY_MODE = 0,
    SDP_NORMAL_MODE  = 1
} SDP_Mode_Select_t;


#define SDP_REG(reg_name)   (PORT_BYTE(reg_name))

#define SDP_ENABLE()  (SET_BIT(SDP_REG(SDP_CTS), SDP_EN))

#define SDP_DISABLE()  (CLEAR_BIT(SDP_REG(SDP_CTS), SDP_EN))

#define SDP_WRITE(port, data)    (PORT_BYTE(SDP_DATA_BASE + (port << 1)) = data)

void SDP_config(SDP_Mode_Select_t mode);


#endif // PORT80_H

