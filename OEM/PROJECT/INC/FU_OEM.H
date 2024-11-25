/*------------------------------------------------------------------------------
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.
 *------------------------------------------------------------------------------
 * File: FU_OEM.H
 *----------------------------------------------------------------------------*/

#ifndef FU_OEM_H
#define FU_OEM_H

#include "swtchs.h"
#include "types.h"
#include "fu.h"
#include "i2c.h"

/*------------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/
#if SMB_FLUPDATE_SUPPORTED

#define FL_Event_Active     0   // GPIO event
#define FL_Event_Inactive   1   // GPIO event

#define SMB_Flash_Channel   CH_SOC_EC
#define SMB_Flash_Addr      (0x16 << 1)
#define FLASH_WCB_WRITE     0x10
#define FLASH_WCB_READ      0x30
#define FLASH_STS_READ      0x40

/*-----------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------*/
#define SMBFL_STATUS_OK             0xFA
#define SMBFL_STATUS_BUSY           0xFB
#define SMBFL_STATUS_ERROR          0xFE


#define SMB_FL_Buf_Size     34 // command + count + 32 byte data
#define CH_SOC_EC           SMB_1

extern BYTE    SMB_FL_Rbuf[SMB_FL_Buf_Size];
extern BYTE    SMB_FL_Wbuf[SMB_FL_Buf_Size];
extern BYTE    SMB_Flupdate_Sts;

void Enter_Flash_Mode(void);

#define CREATE_MASK(bit_position)       (0x1 << (bit_position))

#endif //SMB_FLUPDATE_SUPPORTED
void SMB_Flupdate_WcbWrite(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

#endif  // FU_OEM_H
