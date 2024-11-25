/*-----------------------------------------------------------------------------
 * MODULE I2C.H
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started November 20, 1996 by GLP)
 *
 * GLP
 * 08 May 97   Moved Smb_Alarm_xxx to PURDAT.H.  Removed unused
 *             I2c_Mstr_Xmit_Last_Data prototype.
 * 10 Mar 98   Changed to run I2C on interrupt.
 * 24 Mar 98   Added I2C_OEM_xxx definitions.
 * 21 Apr 98   Added I2c_Set_Alarm prototype.
 * 12 Jun 98   Added I2C_NO_OPERATION, I2C_DEV_DENIED,
 *             I2C_CMD_DENIED, and I2C_CMD_ERROR.
 * 19 Aug 99   Updated copyright.
 *
 * EDH
 * 05 Mar 04   SMBus channel 1 and channel 2 are using separate I2c_Irq_State.
 *             Because when EC is handling channel 2 and SMBus device on
 *             channel 1 send an alarm to EC (Host) at the same time. This
 *             will effect SMBus process.
 * 13 May 04   Implement SMBus 1.1 and 2.0 for PEC and Write Block process call.
 * ------------------------------------------------------------------------- */

#ifndef I2C_H
#define I2C_H

#include "swtchs.h"

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#elif I2C_SUPPORTED

#include "types.h"
#include "oem_i2c.h"
#include "clock.h"

//extern BYTE I2c2_Irq_State;

typedef volatile struct _I2C_VAR
{
    VBITS_8 I2c_Timeout;
    BITS_8 i2c_state;
    BITS_8 i2c_state2;
    BYTE *i2c_pntr;
    WORD count;
    BYTE i2c_buffer[37];
    BYTE I2c_Irq_State;
    BYTE i2c_alarm_buffer[3];
    BYTE CRC_8;
    #if I2C_SLAVE_SUPPORTED
    BYTE Addr;           /* MTACHAF == 0 => ADDR1 "HID Over I2C" */
                            /* MTACHAF == 1 => ADDR2 "NTC (SMBus)" */
    unsigned int rcv_size;
    unsigned int xmit_size;
    BYTE i2c_slave_cmd;
    BYTE i2c_slave_cmd2;
    BYTE i2c_slave_prtl;
    BYTE i2c_slave_buffer[37];
    #if HID_OV_I2C_SUPPORTED
    WORD    i2c_slave_rcount;       // I2C slave mode receive count
    WORD    i2c_slave_wcount;       // I2C slave mode transmit count
    BYTE    *i2c_slave_rbuf;
    BYTE    *i2c_slave_wbuf;
    #endif //HID_OV_I2C_SUPPORTED
    #endif //I2C_SLAVE_SUPPORTED
} I2C_VAR;

#define I2C_FREE        bit0
#define TRANSFER_TYPE   bit1
#define PEC_SUPPORTED   bit2


extern I2C_VAR I2c_Var[];

void I2C_Init(BYTE Channel);

/* ---------------------------------------------------------------
   Name: I2c_Irq

   I2C interrupt handling.

   Force calling routine to call the correct function
   based on I2c_Irq_State instead of calling I2c_Irq.

   C prototype:
      void I2c_Irq(void);
   --------------------------------------------------------------- */
//#define I2c_Irq() (I2c_Table[I2c_Irq_State])()

void I2c_Execute_Protocol(BYTE *smb_pntr, BYTE Channel, FLAG Prtcl_Type);
SMALL I2c_Update_State(BYTE Channel);
void I2c_Get_Data(BYTE *smb_buffer, BYTE Channel);
SMALL I2c_Get_Alarm_State(BYTE Channel);
void I2c_Reset_Alarm_State(BYTE Channel);
BYTE *I2c_Get_Alarm(BYTE Channel);
FLAG I2c_Set_Alarm(BYTE *alarm_buffer, BYTE Channel);
FLAG Smb_Level_0_Free(BYTE Channel);
void Free_Smb_Level_0(BYTE Channel);

void I2c_Irq(BYTE Channel);


/* -------------------- SMB Protocol Type --------------------
   SMB table PROTOCOL TYPE (Function_Data) equates
   and SMB buffer (byte 0) equates.

   Usage is the same in both cases.

   Bit 7,6 - 00 = No Operation.
             01 = Write number of bytes in SMB buffer [1].
             10 = Write number of bytes in SMB buffer [1]
                  then read  number of bytes in bits 5 - 0.
             11 = Write number of bytes in SMB buffer [1]
                  then do repeated start condition, then
                  read number of bytes in bits 5 - 0.

   Bit 5 - 0    = Number of read bytes:
                  0 - 3E : read fixed number of bytes from 0 - 3Eh
                  3F     : read number of bytes indicated by first
                           byte of return string from SMB device. */
#define I2C_NOP    0
#define I2C_WRITE  1
#define I2C_READ   2
#define I2C_REPEAT 3

#define SMB_QUICK_COMMAND ((I2C_WRITE  << 6) + 0x00)
#define SMB_SEND_BYTE     ((I2C_WRITE  << 6) + 0x00)
#define SMB_RECEIVE_BYTE  ((I2C_READ   << 6) + 0x01)
#define SMB_WRITE_BYTE    ((I2C_WRITE  << 6) + 0x01)
#define SMB_WRITE_WORD    ((I2C_WRITE  << 6) + 0x02)
#define SMB_READ_BYTE     ((I2C_REPEAT << 6) + 0x01)
#define SMB_READ_WORD     ((I2C_REPEAT << 6) + 0x02)
#define SMB_PROCESS_CALL  ((I2C_REPEAT << 6) + 0x02)
#define SMB_WRITE_BLOCK   ((I2C_WRITE  << 6) + 0x00)
#define SMB_READ_BLOCK    ((I2C_REPEAT << 6) + 0x3F)
#define SMB_PROCESS_BLOCK  SMB_READ_BLOCK



#define READ_PTRL       0x10
#define WRITE_PTRL      0x20
#define PROCESS_PTRL    0x30

#define I2C_READ_BYTE   (READ_PTRL + 0x00)
#define I2C_READ_WORD   (READ_PTRL + 0x01)
#define I2C_READ_BLOCK  (READ_PTRL + 0x02)

#define I2C_WRITE_BYTE  (WRITE_PTRL + 0x00)
#define I2C_WRITE_WORD  (WRITE_PTRL + 0x01)
#define I2C_WRITE_BLOCK (WRITE_PTRL + 0x02)
//

/* -------------- Returns from I2c_Update_State --------------
   Return values from I2c_Update_State routine.

   The I2C_TRANSFER_OK and I2C_TRANSFER_ERR values will be
   returned until I2c_Get_Data is called.  I2c_Get_Data is
   called even if no data is expected. */

#define I2C_IDLE           0
#define I2C_CMD_PENDING    1
#define I2C_CMD_PROCESSING 2
#define I2C_TRANSFER_OK    3
#define I2C_TRANSFER_ERR   4
#define I2C_BUSY           5
#define I2C_NO_OPERATION   6

#define I2C_OEM_PROCESSING 7
#define I2C_OEM_CMD_OK     8
#define I2C_OEM_CMD_ERR    9

#define I2C_DEV_DENIED    10
#define I2C_CMD_DENIED    11
#define I2C_CMD_ERROR     12
#define I2C_PEC_ERROR     13
/* ------------ Returns from I2c_Get_Alarm_State -------------
   Return values from I2c_Get_Alarm_State. */

#define I2C_NO_ALARM       0
#define I2C_NEW_ALARM      1
#define I2C_ALARM_WAITING  2

/* ----------------------- I2c_Timeout -----------------------
   I2C fail timeout timer. */
extern VBITS_8 I2c_Timeout[];

/* Bit 6 through 0 used as timer. */
#define I2C_FAIL bit7   /* Set if timer times out. */

/* The timer counts up at a 1 mS rate if the I2C_FAIL bit is
   clear.  It counts until I2C_FAIL bit is set.  This results
   in a timer value of 0x80 which automatically sets the
   I2C_FAIL bit and stops the timer.

   The timer cannot time more than 128 mS. */

#if TICKER_1MS
/* Ticker base is 1mS */
/* 100 mS for I2C bus to transmit or receive. */
#define I2C_FAIL_TIME (0x80-35)
/* 3 mS for I2C start condition generated. */
#define START_FAIL_TIME (0x80-3)

/* 5 mS idle time before checking if an I2C transfer is needed. */
#define I2C_RETRY_TIME (0x80-5)
#else // TICKER_1MS
/* Ticker base is 5mS */
/* 35 mS for I2C bus to transmit or receive. */
#define I2C_FAIL_TIME (0x80-7)
/* 10 mS for I2C start condition generated. */
#define START_FAIL_TIME (0x80-2)

/* 10 mS idle time before checking if an I2C transfer is needed. */
#define I2C_RETRY_TIME (0x80-2)
#endif //TICKER_1MS

/* ---------------------------------------------------------------
   Name: Disable_I2c_Irq, Enable_I2c_Irq

   Used to disable or enable the interrupt for the I2C bus.

   C prototypes:
      void Disable_I2c_Irq(void);
      void Enable_I2c_Irq(void);
   --------------------------------------------------------------- */

#define I2C_SDA     PORT_BYTE   (SMB1SDA_ADDR + Channel*0x40)
#define I2C_ST      PORT_BYTE   (SMB1ST_ADDR + Channel*0x40)
#define I2C_CST     PORT_BYTE   (SMB1CST_ADDR + Channel*0x40)
#define I2C_CTL1    PORT_BYTE   (SMB1CTL1_ADDR + Channel*0x40)
#define I2C_ADDR1   PORT_BYTE   (SMB1ADDR1_ADDR + Channel*0x40)
#define I2C_CTL2    PORT_BYTE   (SMB1CTL2_ADDR + Channel*0x40)
#define I2C_ADDR2   PORT_BYTE   (SMB1ADDR2_ADDR + Channel*0x40)
#define I2C_ADDR3   PORT_BYTE   (SMB1ADDR3_ADDR + Channel*0x40)
#define I2C_ADDR4   PORT_BYTE   (SMB1ADDR4_ADDR + Channel*0x40)
#define I2C_ADDR5   PORT_BYTE   (SMB1ADDR5_ADDR + Channel*0x40)
#define I2C_ADDR6   PORT_BYTE   (SMB1ADDR6_ADDR + Channel*0x40)
#define I2C_ADDR7   PORT_BYTE   (SMB1ADDR7_ADDR + Channel*0x40)
#define I2C_ADDR8   PORT_BYTE   (SMB1ADDR8_ADDR + Channel*0x40)
#define I2C_CTL3    PORT_BYTE   (SMB1CTL3_ADDR + Channel*0x40)
#define I2C_CTL4    PORT_BYTE   (SMB1CTL4_ADDR + Channel*0x40)
#define I2C_CST2    PORT_BYTE   (SMB1CST2_ADDR + Channel*0x40)
#define I2C_CST3    PORT_BYTE   (SMB1CST3_ADDR + Channel*0x40)
#define I2C_SCLLT   PORT_BYTE   (SMB1SCLLT_ADDR + Channel*0x40)
#define I2C_SCLHT   PORT_BYTE   (SMB1SCLHT_ADDR + Channel*0x40)


#endif /* if I2C_SUPPORTED */
#endif /* ifndef I2C_H */



