/*-----------------------------------------------------------------------------
 * Copyright (c) 2006-2010 by Nuvoton Technology Corporation
 * All rights reserved.
 *<<<--------------------------------------------------------------------------
 * File Contents:
 *     HID.h - HID over I2C module
 *
 * Project:
 *     Driver Set for WPCE78nx Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------>>>*/

#ifndef HID_H
#define HID_H

#include "i2c.h"

typedef volatile struct _HID_BUF
{
    WORD    Length;
    BYTE    HID_Buffer[6];
} HID_BUF;

typedef volatile struct _HID_KBD
{
    WORD    Length;
    BYTE    HID_Buffer[8];
} HID_KBD;

typedef volatile struct _HID_AUX
{
    WORD    Length;
    BYTE    HID_Buffer[4];
} HID_AUX;

void Handle_Read_Devices(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Handle_Write_Devices(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_SlaveDone(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_Init(void);
void KBD_to_HID(BYTE data, SMALL event);
void AUX_to_HID(BYTE data, SMALL port);

#define Output_mBuffer_Full() ((P7DOUT & 0x40) ? 0 : 1)
#define Output_kBuffer_Full() ((P7DOUT & 0x40) ? 0 : 1)

/* Aux_Control */
extern BITS_8  Aux_Control;
#define aux_reset   bit0
#define aux_enable  bit1

#endif // HID_H
