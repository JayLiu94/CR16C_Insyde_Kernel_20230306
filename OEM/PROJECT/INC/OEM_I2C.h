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

#ifndef OEM_I2C_H
#define OEM_I2C_H

#include "swtchs.h"

#define SMB_1  0x00
#define SMB_2  0x01
#define SMB_3  0x02
#define SMB_4  0x03

/* EC Slave addresses definitons */
/* Slave address index - SMBxCST2 & SMBxCST3 */
#define SADDR_IDX1  1
#define SADDR_IDX2  2
#define SADDR_IDX3  3
#define SADDR_IDX4  4
#define SADDR_IDX5  5
#define SADDR_IDX6  6
#define SADDR_IDX7  7
#define SADDR_IDX8  8

#define SMB1_CLOCK  400000
#define SMB2_CLOCK  400000
#define SMB3_CLOCK  400000
#define SMB4_CLOCK  400000


/* The following addresses (V_MYADR and V_BATADR) are shifted
   left by one bit.  These are 7-bit addresses defined as an
   8-bit value with the least significant bit set to 0. */
#define V_MYADR            0x10  /* SMBus host slave-address. */
#define V_BATADR           0x16  /* SMBus battery slave-address. */

#define I2C_CMD_ADDR    0x36    /* 0x1B */
#define I2C_HID_ADDR    0xD0    /* 0x68 */

#define SA_Enable    (1 << 7)    // SMBus Slave Address Enable.
#define SA_Disable   (0 << 7)    // SMBus Slave Address Disable.

#if HID_OV_I2C_SUPPORTED
#define HID_CHANNEL     SMB_4
#define HID_ADDR_IDX    SADDR_IDX1
#endif // HID_OV_I2C_SUPPORTED

#define S_MYADR1_1      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_2      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_3      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_4      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_5      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_6      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_7      ((0x00 >> 1) | SA_Disable)
#define S_MYADR1_8      ((0x00 >> 1) | SA_Disable)

#define S_MYADR2_1      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_2      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_3      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_4      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_5      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_6      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_7      ((0x00 >> 1) | SA_Disable)
#define S_MYADR2_8      ((0x00 >> 1) | SA_Disable)

#define S_MYADR3_1      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_2      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_3      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_4      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_5      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_6      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_7      ((0x00 >> 1) | SA_Disable)
#define S_MYADR3_8      ((0x00 >> 1) | SA_Disable)

#define S_MYADR4_1      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_2      ((I2C_HID_ADDR >> 1) | SA_Enable)
#define S_MYADR4_3      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_4      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_5      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_6      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_7      ((0x00 >> 1) | SA_Disable)
#define S_MYADR4_8      ((0x00 >> 1) | SA_Disable)




#endif /* ifndef OEM_I2C_H */

