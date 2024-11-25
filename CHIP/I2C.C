/*-----------------------------------------------------------------------------
 * MODULE I2C.C
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
 * Contains the I2C bus primitive handlers.  These are routines for generic I2C
 * operations.  Only master transmit and master receive modes are supported.
 * Slave mode may be handled through an irq in another module.
 *
 * Revision History (Started November 20, 1996 by GLP)
 *
 * GLP
 * 08 May 97   Added support for ACPI.  Moved Smb_Alarm_xxx to PURDAT.H and
 *             PURDAT.C.  Removed the clearing of the busy bit from
 *             I2c_Slave_Init (It is cleared when the stop condition is sent).
 *             Removed unused I2c_Mstr_Xmit_Last_Data routine.
 * 26 Feb 98   Changed references to Flag.ACPI_ENABLED to use the OEM_ACPI_Mode
 *             routine.
 * 09 Mar 98   Changed to run I2C on interrupt.  Use I2c_Execute_Protocol to
 *             start data transfer.  Other routines were changed to support
 *             I2c_Execute_Protocol.
 * 30 Mar 98   Changed irq_i2c_send_address to clear the read/write bit for the
 *             SMBus protocols that use a repeated start.
 * 06 Apr 98   The Read Byte protocol did not produce a negative acknowledge
 *             after the received byte.  Code was added in irq_i2c_check_xmit
 *             to set the ACK bit before the address is sent on the repeated
 *             start for the SMB_READ_BYTE protocol.
 * 14 Apr 98   Changed the alarm handling so that the i2c_alarm_buffer will
 *             always have valid data. Changed I2c_Get_Alarm to return buffer
 *             pointer without posting a request for service.
 * 20 Apr 98   Changed irq_idle_error_recovery() to disable the I2C interrupt
 *             (it was enabling it.) Changed the alarm handling to prevent an
 *             alarm message from being received from the SMBus until the
 *             previous alarm has been taken.  For an alarm that may only
 *             happen once (such as AC change of status), the device sending
 *             the alarm may retry until the message has been received.  Added
 *             I2c_Get_Alarm_State() and I2c_Reset_Alarm_State(). Moved alarm
 *             status from I2c_Update_State() to I2c_Get_Alarm_State().
 * 21 Apr 98   Added I2c_Set_Alarm().  Changed clearing of the bits in SMB1ST
 *             from "SMB1ST |= xxx" to "SMB1ST = xxx". A read-modify-write
 *             should not be done on the SMB1ST register, a 1 should be written
 *             to clear a bit.
 * 18 May 98   Changed irq_idle_error_recovery() to clear the NEGACK bit when
 *             generating a stop condition in master mode.  This allows a stop
 *             condition to be generated when an acknowledge signal is not
 *             received from a device.
 * 22 May 98   Added to irq_slave_aborter to clear the NMATCH and BER bits.
 *             A bus error (BER) that occurred (such as when a battery is
 *             inserted) was not being cleared causing an interrupt to be
 *             continually generated.  Also added a call to irq_slave_aborter
 *             in irq_i2c_idle if a bus error occurred.
 * 09 Jun 98   Removed Int_Var.SMB_Busy.
 * 26 Jun 98   Added I2C_SLAVE_SUPPORTED.  Corrected I2c_Set_Alarm to check
 *             for IDLE_STATE before checking if I2C bus is busy in slave mode
 *             to allow an alarm to be set.
 * 18 Aug 99   Updated copyright.  In i2c_mstr_xmit_start, if the number of
 *             bytes to write is set to 0, this is an error. Then, a NOP
 *             protocol was checked.  This has been changed to check for the
 *             the NOP protocol first.  This allows the number of bytes to be
 *             set to 0 for a NOP protocol.
 * 01 Sep 99   Corrected error for Read Byte protocol in irq_i2c_check_xmit.
 * EDH
 * 02 Jan 01   Changed the I2C process from direct register to pointer.
 *             Support the secondary SMBus.
 * 06 Apr 01   Fixed the SMBus communication error don't set the error flag.
 *             This will cause the Host or Firmware get wrong information.
 * 05 Mar 04   SMBus channel 1 and channel 2 are using seperate I2c_Irq_State.
 *             Because when EC is handling channel 2 and SMBus device on
 *             channel 1 send an alarm to EC (Host) at the same time. This
 *             will effect SMBus process.
 * 13 May 04   Implement SMBus 1.1 and 2.0 for PEC and Write Block process call.
 * ------------------------------------------------------------------------- */

#define I2C_C

#include "swtchs.h"
#include "hfcg.h"

/* The alarm module handles alarms.  The slave mode is not used. */

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#elif !I2C_SUPPORTED
/* If this file is not being used, there is no need to take up code
   or RAM space.  But, ANSI C does not allow an empty source file.
   The following is an attempt at keeping code size small
   while allowing the I2C bus code to be added easily. */
static void nothing(void);
static void nothing(void)
{
   ;
}
#else

#include "types.h"
#include "i2c.h"
#include "icu.h"
#include "defs.h"
#include "proc.h"
#include "com_defs.h"
#include "purdat.h"
#include "host_if.h"
#include "oem.h"

#if HID_OV_I2C_SUPPORTED
#include "hid.h"
#include "hid_i2c.h"
#endif // HID_OV_I2C_SUPPORTED


typedef void (*FUNCT_PTR_V_I2CPB)(I2C_VAR *, BYTE);
/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static FLAG i2c_mstr_xmit_start(I2C_VAR *I2c_Var_Pntr, BYTE Channel, FLAG Prtcl_Type);
static void i2c_wait_stop(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

#if I2C_SLAVE_SUPPORTED
static void irq_i2c_errini(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_idle(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_getid(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_getlo(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_gethi(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_getstop(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_notme(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_getcmd(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_chkrep(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_slv_xmit_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_slv_receive(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
#else
static void irq_i2c_unused(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
#endif

static void irq_i2c_send_address(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_check_address(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_check_xmit(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_check_repeated(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_receive_one(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_receive(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_check_pec(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_check_xmit_pec(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

static void irq_slave_aborter(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static WORD irq_get_i2c_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_mstr_xmit_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_mstr_xmit_stop(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_i2c_slave_init(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

static void irq_idle_error_recovery(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
static void irq_idle_error_recovery2(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
#if I2C_GPIO_RECOVERY
void I2C_Devalt_Cconfig(FLAG enable, BYTE Channel);
#endif // I2C_GPIO_RECOVERY
#if I2C_SLAVE_SUPPORTED
static void I2C_Slave_Init(BYTE Channel);
#endif // I2C_SLAVE_SUPPORTED

BYTE Hookc_GetReadPrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
BYTE Hookc_GetWritePrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Hookc_SlaveDone(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Hookc_MasterWrite(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Hookc_Cfg_Slave_BufPntr(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
BYTE Hookc_ReceivePrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

#define SMB_0_FREE          bit0    /* SMBus level 0 is free to be used. */
#define DENY_OVERRIDE       bit1    /* For SMBus level 0, allows
                                    access to protected commands. */
#define SELECTOR_LOADED     bit2    /* For SMBus selector status. */
#define SEPARATE_SELECTOR   bit3    /* Selector is not part of charger. */
#define FOUND_SELECTOR      bit4    /* Selector command was accepted. */
#define SMB_Service_Request bit5    /* When this is set, 1 mS timer
                                    generates a request for service. */

void OEM_SMB_callback(BITS_8 i2c_state, BYTE Channel);
void Set_I2C_Pntr(BYTE Chnl);
extern void Microsecond_Delay(WORD delay);
extern BYTE ok[];
extern BYTE i2c_done[];
extern BYTE ACPI_Timer;

#define I2C_TMO (I2c_Var_Pntr->i2c_state.bit.TIMEOUT ? 1 : 0)

void Enable_Timer_A(void);



/* ------------------------- i2c_state -------------------------
   I2C state.

   INPUT_READY
      1 = Input buffer (i2c_buffer) has
          data waiting to be sent to I2C bus.

      Set by I2c_Execute_Protocol.
      Clear when transfer complete or error detected.

   TRANSFER_DONE
      1 = I2C transfer is complete.

      Set when transfer is complete, cleared by I2c_Get_Data.

      This flag is set at the end of transfer even when an
      error is detected.  The TRANSFER_ERR flag is not to
      be acted on until the TRANSFER_DONE flag is set.

      This flag is set whether or not there is data from the I2C
      bus.  I2c_Get_Data must be used to clear the flag.  In this
      case, I2c_Get_Data will indicate that no data is available.

   TRANSFER_ERR
      1 = An error was detected in I2C transfer.

      Set on error detection, cleared by I2c_Get_Data.

      The TRANSFER_ERR flag is not to be acted on
      until the TRANSFER_DONE flag is set.

   ALARM_RDY
      1 = An alarm has been received.

   ALARM_WAITING
      1 = An alarm was received,
          I2c_Get_Alarm_State() was called, and the
          alarm is waiting to be processed by the Host. */

/* Care must be taken because i2c_state can be updated by interrupt. */

#define INPUT_READY     bit0
#define TRANSFER_DONE   bit1
#define TRANSFER_ERR    bit2
#define WAIT_STOP       bit3
#define ALARM_RDY       bit4
#define ALARM_WAITING   bit5
#define PEC_ERR         bit6
#define TIMEOUT         bit7


I2C_VAR I2c_Var[SMB_CHNLS];


/* ---------------------------------------------------------------
   Name: I2c_Table, I2c_Irq_State

   The I2C interrupt calls the appropriate function
   based on I2c_Irq_State using the I2c_Table.
   --------------------------------------------------------------- */

const FUNCT_PTR_V_I2CPB I2c_Table[] =
{
   /* The following are slave mode interrupts.
      Used for alarms. */

   #if I2C_SLAVE_SUPPORTED
   irq_i2c_errini,         /* NOT USED      =  0. */
   irq_i2c_idle,           /* IDLE_STATE    =  1. */
   irq_i2c_getid,          /* GETID_STATE   =  2. */
   irq_i2c_getlo,          /* GETLO_STATE   =  3. */
   irq_i2c_gethi,          /* GETHI_STATE   =  4. */
   irq_i2c_getstop,        /* GETSTOP_STATE =  5. */
   irq_i2c_getcmd,         /* GETCMD_STATE  =  6. */
   irq_i2c_chkrep,         /* CHKREP_STATE  =  7. */
   irq_i2c_slv_xmit_data,  /* SNDDATA_STATE =  8. */
   irq_i2c_slv_receive,    /* GETDATA_STATE =  9. */
   irq_i2c_notme,          /* NOTME_STATE   =  10. */
   #else
   irq_i2c_unused,         /* NOT USED      =  0. */
   irq_i2c_unused,         /* NOT USED      =  1. */
   irq_i2c_unused,         /* NOT USED      =  2. */
   irq_i2c_unused,         /* NOT USED      =  3. */
   irq_i2c_unused,         /* NOT USED      =  4. */
   irq_i2c_unused,         /* NOT USED      =  5. */
   irq_i2c_unused,         /* NOT USED      =  6. */
   irq_i2c_unused,         /* NOT USED      =  7. */
   irq_i2c_unused,         /* NOT USED      =  8. */
   irq_i2c_unused,         /* NOT USED      =  9. */
   irq_i2c_unused,         /* NOT USED      = 10. */
   #endif

   /* The following are master mode interrupts. */

   irq_i2c_send_address,   /* I2C_SND_ADDR  =  11. */
   irq_i2c_check_address,  /* I2C_CHK_ADDR  =  12. */
   irq_i2c_check_xmit,     /* I2C_CHK_XMIT  =  13. */
   irq_i2c_check_repeated, /* I2C_CHK_REP   = 14. */
   irq_i2c_receive_one,    /* I2C_RCV_ONE   = 15. */
   irq_i2c_receive,         /* I2C_RCV       = 16. */
   irq_i2c_check_pec,       /* I2C_CHK_PEC   = 17. */
   irq_i2c_check_xmit_pec   /* I2C_SND_PEC   = 18. */
};

/* States for I2c_Irq_State. */
typedef enum
{
    IDLE_STATE = 0x01,  /* IDLE_STATE    =  1. */
    GETID_STATE,        /* GETID_STATE   =  2. */
    GETLO_STATE,        /* GETLO_STATE   =  3. */
    GETHI_STATE,        /* GETHI_STATE   =  4. */
    GETSTOP_STATE,      /* GETSTOP_STATE =  5. */
    GETCMD_STATE,       /* GETCMD_STATE  =  6. */
    CHKREP_STATE,       /* CHKREP_STATE  =  7. */
    SNDDATA_STATE,      /* SNDDATA_STATE =  8. */
    GETDATA_STATE,      /* GETDATA_STATE =  9. */
    NOTME_STATE,        /* NOTME_STATE   =  10. */
    I2C_SND_ADDR,       /* I2C_SND_ADDR  =  11. */
    I2C_CHK_ADDR,       /* I2C_CHK_ADDR  =  12. */
    I2C_CHK_XMIT,       /* I2C_CHK_XMIT  =  13. */
    I2C_CHK_REP,        /* I2C_CHK_REP   =  14. */
    I2C_RCV_ONE,        /* I2C_RCV_ONE   =  15. */
    I2C_RCV,            /* I2C_RCV       =  16. */
    I2C_CHK_PEC,        /* I2C_CHK_PEC   =  17. */
    I2C_SND_PEC         /* I2C_SND_PEC   =  18. */
} I2C_HANDLE_T;

#define LAST_SLI2C_STATE    NOTME_STATE
#define LAST_I2C_IRQ_STATE  I2C_SND_PEC
/* ---------------- Buffer & Protocol Description ----------------

   The SMB_Buffer:

      SMB_Buffer[level][0] holds the SMB protocol type.
      SMB_Buffer[level][1] holds the write byte count.
      SMB_Buffer[level][2] holds the slave address and read/write bit.
      SMB_Buffer[level][3] holds the command byte.
      SMB_Buffer[level][4] holds the
               .           data
               .           to be
               .           sent to
      SMB_Buffer[level][x] the device.

   i2c_buffer:

      (i2c_state holds the state of the I2C transfer.)

      i2c_buffer[0] holds the SMB protocol type.
      i2c_buffer[1] holds the write byte count.
      i2c_buffer[2] holds the slave address and read/write bit.
      i2c_buffer[3] holds the command byte.
      i2c_buffer[4] holds the
            .       data
            .       to be
            .       sent to
      i2c_buffer[x] the device.

   After reading data from a device, i2c_buffer looks like:

      i2c_buffer[0] holds the SMB protocol type.
      i2c_buffer[1] holds the
            .       data
            .       read
            .       from
      i2c_buffer[x] the device.

   After calling I2c_Get_Data, the SMB_Buffer looks like:

      SMB_Buffer[level][0] holds the number of bytes read.
      SMB_Buffer[level][1] holds the
               .           data
               .           read
               .           from
      SMB_Buffer[level][x] the device.



   The SMB protocol type holds the type and number of bytes to read.
   Bit 7 and 6 is the protocol and bits 5 through 0 holds the read
   byte count.  The read byte count is the number of bytes to read
   or a 0x3F.  A read byte count of 0x3F indicates that the actual
   read byte count is the first byte that is received from the device.

   The write byte count is the number of bytes after the write
   byte count.  This is the number of data bytes to write to
   the slave device.  Since the slave address is always part
   of a transfer, the write byte count is at least 1.

   The slave address and read/write bit is handled based on
   the protocol.  The table below shows the handling of the
   read/write bit.

      SMBus
     Protocol                 Read/Write Bit Handling
   ------------- -------------------------------------------------------
   Quick Command Uses the bit that is passed with the slave address.

   Send Byte     Bit that is passed with slave address must be 0.

   Receive Byte  Bit that is passed with slave address must be 1.

   Write Byte    Bit that is passed with slave address must be 0.

   Write Word    Bit that is passed with slave address must be 0.

   Read Byte     Cleared for 1st start condition, set for repeated start.

   Read Word     Cleared for 1st start condition, set for repeated start.

   Process Call  Cleared for 1st start condition, set for repeated start.

   Block Write   Bit that is passed with slave address must be 0.

   Block Read    Cleared for 1st start condition, set for repeated start.



   The following shows the handling of the SMBus protocols.

      SMBus      Protocol
     Protocol      Type                  I2C Bus Action
   ------------- -------- -----------------------------------------------
   Quick Command 01000000 S ADR R/W SA P

   Send Byte     01000000 S ADR W SA CMD SA P

   Receive Byte  10000001 S ADR R SA SDAT MA\ P

   Write Byte    01000000 S ADR W SA CMD SA MDAT SA P

   Write Word    01000000 S ADR W SA CMD SA MDAT SA MDAT SA P

   Read Byte     11000001 S ADR W SA CMD SA
                          S ADR R SA SDAT MA\ P

   Read Word     11000010 S ADR W SA CMD SA
                          S ADR R SA SDAT MA SDAT MA\ P

   Process Call  11000010 S ADR W SA CMD SA MDAT SA MDAT SA
                          S ADR R SA SDAT MA SDAT MA\ P

   Block Write   01000000 S ADR W SA CMD SA MNUM SA MDAT SA ... MDAT SA P

   Block Read    11111111 S ADR W SA CMD SA
                          S ADR R SA SNUM MA SDAT MA ... SDAT MA\ P

      S    = Start Condition
      ADR  = 7-bit Slave Address
      R    = Read  (high bit)
      W    = Write (low  bit)
      CMD  = Command byte from master
      MNUM = Number of data bytes from master
      SNUM = Number of data bytes from slave
      MA   = Acknowledge from master     (low  bit)
      MA\  = Not Acknowledge from master (high bit)
      SA   = Acknowledge from slave      (low  bit)
      SA\  = Not Acknowledge from slave  (high bit)
      MDAT = Data byte from master
      SDAT = Data byte from slave
      P    = Stop Condition
   --------------------------------------------------------------- */

/* ---------------------------------------------------------------
   Name: I2C_Init

   Called on reset (interrupts disabled).
   --------------------------------------------------------------- */
void I2C_Init(BYTE Channel)
{
    unsigned long i2c_clock;

    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];

    /* Disable I2C module.  This also clears some registers. */
    I2C_CTL2 &= ~SMBCTL2_ENABLE;

    #if I2C_SLAVE_SUPPORTED
    /* Set slave address. */
    I2C_Slave_Init(Channel);
    #endif // I2C_SLAVE_SUPPORTED


   // set CTL4.HLDT - SDA hold time - to 15 (decimal) in case core clock is 12MHz and above
   // if core clock is less than 12MHz than CTL4.HLDT should stay at default value (7)
    #if (HFCG_CORE_CLK >= 12000000UL)
    I2C_CTL4 = 0xF;
    #endif
    switch(Channel)
    {
        case SMB_1:
            i2c_clock = SMB1_CLOCK;
            break;
        case SMB_2:
            i2c_clock = SMB2_CLOCK;
            break;
        case SMB_3:
            i2c_clock = SMB3_CLOCK;
            break;
        case SMB_4:
            i2c_clock = SMB4_CLOCK;
            break;
    }

    I2C_CTL2 &= ~SMBCTL2_SCLFRQ;
    I2C_CTL3 &= ~SMBCTL3_SCLFRQ;

    if (i2c_clock >= 400000)
    {
        //I2C_SCLLT = (BYTE) ((HFCG_CORE_CLK * 16 + 10000000) / 20000000);
        //I2C_SCLHT = (BYTE) ((HFCG_CORE_CLK * 9 + 10000000) / 20000000);
        I2C_SCLLT = (BYTE) ((clock_get_freq() * 16 + 10000000) / 20000000);
        I2C_SCLHT = (BYTE) ((clock_get_freq() * 9 + 10000000) / 20000000);
        SET_BIT(I2C_CTL3, SMBCTL3_400K_MODE);
    }
    else
    {
        //I2C_CTL2 |= (BYTE) ((HFCG_CORE_CLK/(i2c_clock*4) & 0x7F) + 1) << 1;
        //I2C_CTL3 |= (BYTE) ((HFCG_CORE_CLK/(i2c_clock*4) & 0x180) + 1) >> 7;
        I2C_CTL2 |= (BYTE) ((clock_get_freq()/(i2c_clock*4) & 0x7F) + 1) << 1;
        I2C_CTL3 |= (BYTE) ((clock_get_freq()/(i2c_clock*4) & 0x180) + 1) >> 7;
        CLEAR_BIT(I2C_CTL3, SMBCTL3_400K_MODE);
    }

    /* Enable I2C module. */
    SET_BIT(I2C_CTL2, SMBCTL2_ENABLE);

    /* Clear all i2c_state flags. */
    I2c_Var_Pntr->i2c_state.byte = 0;
    I2c_Var_Pntr->i2c_state2.bit.I2C_FREE = 1;


    irq_slave_aborter(I2c_Var_Pntr, Channel); // Check ???


    I2c_Var_Pntr->i2c_alarm_buffer[0] = 0;
    I2c_Var_Pntr->i2c_alarm_buffer[1] = 0;
    I2c_Var_Pntr->i2c_alarm_buffer[2] = 0;

    #if I2C_SLAVE_SUPPORTED
    /* Set new match interrupt enable.
       Interrupt enable (SMBCTL1_INTEN) was set in irq_slave_aborter. */
     SET_BIT(I2C_CTL1, SMBCTL1_NMINTE);
   #endif // I2C_SLAVE_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: I2c_Execute_Protocol

   Start the I2C transfer based on the protocol.

   Input: smb_pntr points to the SMBus buffer.

   Setup i2c_buffer and i2c_state.

   See "Buffer & Protocol Description" above for more information.
   --------------------------------------------------------------- */
void I2c_Execute_Protocol(BYTE *smb_pntr, BYTE Channel, FLAG Prtcl_Type)
{
    I2C_VAR * I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];

    // Reenable Timer_A for I2C timeout.
    Enable_Timer_A();

   /* Fill i2c_buffer with data from the SMB_Buffer. */
    ok[Channel] = FALSE;
    i2c_done[Channel] = FALSE;
    I2c_Var_Pntr->CRC_8 = 0;
    I2c_Var_Pntr->i2c_pntr = (BYTE *) &I2c_Var_Pntr->i2c_buffer; //Check ???
    I2c_Var_Pntr->count = *(smb_pntr+1) + 2;
    if (I2c_Var_Pntr->count >= 37)
        I2c_Var_Pntr->count = 37;
    while (I2c_Var_Pntr->count)
    {
        *I2c_Var_Pntr->i2c_pntr++ = *smb_pntr++;
        I2c_Var_Pntr->count--;
    }
    I2c_Var_Pntr->i2c_state.bit.TIMEOUT = 0;
    I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 0;
    I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 1;
    I2c_Var_Pntr->i2c_state.bit.PEC_ERR = 0;
    I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 0;

    I2c_Var_Pntr->i2c_state2.bit.TRANSFER_TYPE = Prtcl_Type;

    I2c_Var_Pntr->i2c_state2.bit.I2C_FREE = 0;


    if (Prtcl_Type)
    {
        Disable_Irq();

        I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 1;

   /* Setup so 1 mS timer will
      post a request for service. */

        i2c_mstr_xmit_start(I2c_Var_Pntr, Channel, Prtcl_Type);
        Enable_Irq();
    }
    else
    {
        Disable_Irq();
        if (i2c_mstr_xmit_start(I2c_Var_Pntr, Channel, Prtcl_Type))
        {   // No error failure.
            Enable_Irq();
            do
            {
                I2c_Table[I2c_Var_Pntr->I2c_Irq_State](I2c_Var_Pntr, Channel);
                while (((I2C_ST & (SMBST_SDAST + SMBST_BER + SMBST_NEGACK)) == 0)
                        && (I2c_Var_Pntr->I2c_Irq_State != IDLE_STATE) && !I2C_TMO ) ; // Wait for transaction completed.
            }
            while ((I2c_Var_Pntr->I2c_Irq_State != IDLE_STATE) && !I2C_TMO);
        }
        Enable_Irq();
    }
}

/* ---------------------------------------------------------------
   Name: I2c_Irq

   --------------------------------------------------------------- */
void I2c_Irq(BYTE Channel)
{
    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];
    if (I2c_Var_Pntr->I2c_Irq_State <= LAST_I2C_IRQ_STATE)
    {
        I2c_Table[I2c_Var_Pntr->I2c_Irq_State](I2c_Var_Pntr, Channel);
    }
    else
    {
        irq_idle_error_recovery(I2c_Var_Pntr, Channel);
    }
}




/* ---------------------------------------------------------------
   Name: I2c_Get_Data

   Get data from last I2C transfer,
   clear i2c_state TRANSFER_DONE and TRANSFER_ERR flags,
   and setup timer for posting a request to return to I2C checking.

   Returns: data in caller's buffer.

   See "Buffer & Protocol Description" above for more information.
   --------------------------------------------------------------- */
void I2c_Get_Data(BYTE *smb_buffer, BYTE Channel)
{
    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];
   if (I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR)
   {
      *smb_buffer = 0;  /* Set number of bytes read to 0. */
   }
   else if ( (I2c_Var_Pntr->i2c_buffer[0] & 0x3F) == 0x3F )
   {
      /* For Block Read SMBus protocol.

         The first byte received from the device is a value
         indicating how many more bytes to expect from the device.
         In other words, the number of bytes received from the
         device is one more than the value of the first byte received. */
      *smb_buffer = I2c_Var_Pntr->i2c_buffer[1] + 1;
   }
   else
   {
      /* This is for the following SMBus protocols:

         Quick Command
         Send Byte
         Receive Byte
         Write Byte
         Write Word
         Read Byte
         Read Word
         Process Call
         Block Write

         It is also used for a "No Operation" protocol.

         The number of bytes read is contained
         in bits 0 through 6 of the protocol type. */

      *smb_buffer = I2c_Var_Pntr->i2c_buffer[0] & 0x3F;
   }

   I2c_Var_Pntr->count = *smb_buffer++;
   I2c_Var_Pntr->i2c_pntr = (BYTE *) &I2c_Var_Pntr->i2c_buffer[1];
   if (I2c_Var_Pntr->count >= 32)
        I2c_Var_Pntr->count = 32;
   while (I2c_Var_Pntr->count)
   {
      *smb_buffer++ = *I2c_Var_Pntr->i2c_pntr++;
      I2c_Var_Pntr->count--;
   }
}

/* ---------------------------------------------------------------
   Name: I2c_Get_Alarm_State

   Check if alarm received and update state if appropriate.

   Returns:
           Value                       To indicate
      ------------------   -------------------------------------
      I2C_NO_ALARM         No alarm.
      I2C_NEW_ALARM        An alarm has been received.
      I2C_ALARM_WAITING    Alarm has been received and is
                           waiting to be processed by Host.
   --------------------------------------------------------------- */
SMALL I2c_Get_Alarm_State(BYTE Channel)
{
   SMALL rval;
    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];

   if (I2c_Var_Pntr->i2c_state.bit.ALARM_WAITING)
   {
      /* An alarm has been received an is waiting to be processed. */
      rval = I2C_ALARM_WAITING;
   }
   else if (I2c_Var_Pntr->i2c_state.bit.ALARM_RDY)
   {
      /* A new alarm has been received. */

      /* Signal that the alarm is waiting to be processed. */
      Disable_Irq();
      I2c_Var_Pntr->i2c_state.bit.ALARM_WAITING = 1;
      Enable_Irq();

      rval = I2C_NEW_ALARM;
   }
   else
   {
      /* No new alarm has been received. */
      rval = I2C_NO_ALARM;
   }

   return(rval);
}

/* ---------------------------------------------------------------
   Name: I2c_Reset_Alarm_State

   Called after the alarm has been processed by the Host.
   --------------------------------------------------------------- */
void I2c_Reset_Alarm_State(BYTE Channel)
{
    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];
   Disable_Irq();

   I2c_Var_Pntr->i2c_state.bit.ALARM_RDY     = 0;
   I2c_Var_Pntr->i2c_state.bit.ALARM_WAITING = 0;

   if (I2c_Var_Pntr->I2c_Irq_State == IDLE_STATE)
   {
      /* Enable the slave mode interrupt. */

      /* Turn off I2C interrupt. */
      CLEAR_BIT(I2C_CTL1, SMBCTL1_INTEN);

      /* Disable I2C module.  This also clears some registers. */
      CLEAR_BIT(I2C_CTL2, SMBCTL2_ENABLE);

      #if I2C_SLAVE_SUPPORTED
      /* Set slave address. */
      I2C_Slave_Init(Channel);
      #endif // I2C_SLAVE_SUPPORTED

      /* Enable I2C module. */
      SET_BIT(I2C_CTL2, SMBCTL2_ENABLE);

      #if I2C_SLAVE_SUPPORTED
      /* Now turn on I2C interrupts.  Set new match interrupt
         enable and ensure that interrupt enable bit is set. */
      SET_BIT(I2C_CTL1, SMBCTL1_NMINTE + SMBCTL1_INTEN);
      #endif // I2C_SLAVE_SUPPORTED
   }

   Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: I2c_Get_Alarm

   Get (a pointer to) the alarm.

   Returns: Pointer to alarm data.
   --------------------------------------------------------------- */
BYTE *I2c_Get_Alarm(BYTE Channel)
{
   return((BYTE *) &I2c_Var[Channel].i2c_alarm_buffer);
}

/* ---------------------------------------------------------------
   Name: I2c_Set_Alarm

   Set an alarm as if it came from the I2C bus.
   Can be used when emulating I2C bus (SMBus) devices.

   Input: alarm_buffer points to alarm data.
          alarm_buffer[0] = Device address (left justified).
          alarm_buffer[1] = Low data byte.
          alarm_buffer[1] = High data byte.

   Returns: 1 = Alarm was set.
            0 = Alarm could not be set.  Try again later.
   --------------------------------------------------------------- */
FLAG I2c_Set_Alarm(BYTE *alarm_buffer, BYTE Channel)  // Check ???
{
   FLAG rval;
    I2C_VAR *I2c_Var_Pntr;
    I2c_Var_Pntr = &I2c_Var[Channel];

   rval = 0;

   Disable_Irq();

   if (I2c_Var_Pntr->i2c_state.bit.ALARM_RDY || I2c_Var_Pntr->i2c_state.bit.ALARM_WAITING)
   {
      /* The previous alarm has not been processed.
         Don't allow this new alarm to be set. */
      ;
   }
   else if (I2c_Var_Pntr->I2c_Irq_State == IDLE_STATE)
   {
      /* I2C bus is not being used.  Allow the new alarm to be set. */

      /* Turn off I2C interrupt. */
      CLEAR_BIT(I2C_CTL1, SMBCTL1_NMINTE + SMBCTL1_INTEN);

      /* Because the I2C interrupt is a level sensitive interrupt,
         at this point there is no pending I2C interrupt.  If an
         alarm is being sent on the I2C bus, it will be ignored. */

      /* Ensure that the new match bit is clear by writing a 1 to it. */
      I2C_ST = SMBST_NMATCH;

      rval = 1;
   }
   else if (I2c_Var_Pntr->I2c_Irq_State <= LAST_SLI2C_STATE)
   {
      /* I2C bus is busy in slave mode.  An alarm is being
         received.  Don't allow this new alarm to be set. */
      ;
   }
   else
   {
      /* I2C bus is busy in master mode.
         Allow the new alarm to be set. */
      rval = 1;
   }

   if (rval == 1)
   {
      I2c_Var_Pntr->i2c_alarm_buffer[0] = alarm_buffer[0];
      I2c_Var_Pntr->i2c_alarm_buffer[1] = alarm_buffer[1];
      I2c_Var_Pntr->i2c_alarm_buffer[2] = alarm_buffer[2];

      I2c_Var_Pntr->i2c_state.bit.ALARM_RDY = 1;
   }

   Enable_Irq();

   return(rval);
}

static const BYTE crc8tab[256] =
{
   0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

static BYTE updcrc(BYTE crc, BYTE data)
{
   BYTE tmp;

    tmp=crc^data;
    crc=(crc>>8)^crc8tab[tmp];
    return crc;
}

/* ---------------------------------------------------------------
   Name: i2c_mstr_xmit_start

   Set for master transmit mode and transmit the address
   and the read/write bit on the I2C bus.

   Input: interrupts are disabled.
   --------------------------------------------------------------- */
static FLAG i2c_mstr_xmit_start(I2C_VAR *I2c_Var_Pntr, BYTE Channel, FLAG Prtcl_Type)
{
    BYTE BB_Count;
    FLAG rval = 1;
    I2c_Var_Pntr->count = I2c_Var_Pntr->i2c_buffer[1];  /* Write byte count. */

   if ( (I2c_Var_Pntr->i2c_buffer[0] & 0xC0) == (I2C_NOP << 6) )
   {
      /* A "No Operation" protocol was requested. */

        Disable_Irq();
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 0;
        I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 0;
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 1;
        I2c_Var_Pntr->i2c_state.bit.PEC_ERR = 0;
        Enable_Irq();

      /* Force I2c_Get_Data to return the number of bytes read as 0. */
        I2c_Var_Pntr->i2c_buffer[0] = 0;

   }
   else if ( (I2c_Var_Pntr->count == 0) || (I2c_Var_Pntr->count > (MAX_SMB_INPUT_BYTES-2)) )
   {
      /* There must be at least one byte sent to the I2C
         bus (the address) and there cannot be a request
         to send more bytes than the buffer can hold.  At
         this point, an attempt is being made to send an
         illegal number of bytes.  Flag an error condition. */

        Disable_Irq();
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 1;
        I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 0;
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 1;
        Enable_Irq();

      /* Because the TRANSFER_ERR flag is set,
         I2c_Get_Data will return the number of bytes read as 0. */

      /* Setup so 1 mS timer will
         post a request for service. */

   }
   else
   {
      /* Check bus busy before issue start condition. */
      if (IS_BIT_SET(I2C_CST, SMBCST_BB))
      {
         /* Bus Busy flag set try 10 times to clear it. */
         for (BB_Count = 0; BB_Count < 10; BB_Count++)
         {
            // Clear Bus Busy flag.
            SET_BIT(I2C_CST, SMBCST_BB);
            Microsecond_Delay(10);
            if (IS_BIT_CLEAR(I2C_CST, SMBCST_BB))
            { /* Bus Busy flag was cleared. */
               break;
            }
         }

         if (BB_Count >= 10)
         {  /* Bus Busy flag could not be cleared, set corresponding error flag. */
            I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 1;
            I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 0;
            I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 1;
            irq_idle_error_recovery(I2c_Var_Pntr, Channel);
            return (0);
         }
      }

        /* Clear new match interrupt enable. */
        CLEAR_BIT(I2C_CTL1, SMBCTL1_NMINTE);
        /* Reset bus fail timeout timer and flag. */
        I2c_Var_Pntr->I2c_Timeout.byte = START_FAIL_TIME;

        /* Write 1s to the following bits to clear them. */
        I2C_ST = (SMBST_NMATCH + SMBST_STASTR +
               SMBST_NEGACK + SMBST_BER + SMBST_SLVSTP);

        /* Clear the ACK bit. */
        CLEAR_BIT(I2C_CTL1, SMBCTL1_ACK);

        if (Channel == 0)
        {
            ICU_Clr_Pending_Irq(ICU_EINT_I2C);   /* Clear the interrupt. */
        }
        else if (Channel == 1)
        {
            ICU_Clr_Pending_Irq(ICU_EINT_I2C2);   /* Clear the interrupt. */
        }
        else if (Channel == 2)
        {
            ICU_Clr_Pending_Irq(ICU_EINT_I2C3);   /* Clear the interrupt. */
        }
        else if (Channel == 3)
        {
            ICU_Clr_Pending_Irq(ICU_EINT_I2C4);   /* Clear the interrupt. */
        }

        CLEAR_BIT(I2C_CTL1, SMBCTL1_INTEN);
        /* Setup state for I2C interrupt.
           The I2C_SND_ADDR state transfers control to irq_i2c_send_address. */
        I2c_Var_Pntr->I2c_Irq_State = I2C_SND_ADDR;
        if (Prtcl_Type)
        {   // SMBus interrupt mode.
            /* Ensure that the the I2C interrupt is enabled. */
            SET_BIT(I2C_CTL1, SMBCTL1_START + SMBCTL1_INTEN);
        }
        else
        {   // SMBus polling mode
            SET_BIT(I2C_CTL1, SMBCTL1_START);
            Enable_Irq();

            /* Setup state for I2C interrupt.
               The I2C_SND_ADDR state transfers control to irq_i2c_send_address. */
            while (IS_BIT_SET(I2C_CTL1, SMBCTL1_START) && !I2C_TMO);

            if (I2C_TMO)
            {
                rval = 0;
            }

        }
    }
    return (rval);
}

/* ---------------------------------------------------------------
   Name: i2c_wait_stop

   This is part of error recovery and is
   called by I2c_Update_State not by interrupt.
   --------------------------------------------------------------- */
static void i2c_wait_stop(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Wait until stop done (but don't wait forever). */

    if (IS_BIT_CLEAR(I2C_CTL1, SMBCTL1_STOP) ||
        I2c_Var_Pntr->I2c_Timeout.bit.I2C_FAIL )
    {
        Disable_Irq();
        irq_idle_error_recovery2(I2c_Var_Pntr, Channel);
        I2c_Var_Pntr->i2c_state.bit.WAIT_STOP = 0;
        Enable_Irq();
   }
}

/* ---------------------------------------------------------------
   Interrupt routines.
   --------------------------------------------------------------- */

/* --- Slave mode routines. --- */

#if I2C_SLAVE_SUPPORTED

static void irq_i2c_errini(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   ;
}

static void irq_i2c_idle(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE temp;

    if (IS_BIT_SET(I2C_ST, SMBST_BER))
    {
       /* Interrupt from a bus error.  Clean up interface. */
         irq_slave_aborter(I2c_Var_Pntr, Channel);
    }
    /* Interrupt on address match.

       It is assumed that data is being received
       from a battery.  This is an alarm message. */

    /* The ACK bit is clear and ready to acknowledge each byte. */

     I2c_Var_Pntr->i2c_state2.bit.I2C_FREE = 0;   // Check ???

     if (IS_BIT_SET(I2C_CST, SMBCST_MATCH))
     {
         if (I2C_CST2 & MATCHA1F)        I2c_Var_Pntr->Addr = I2C_ADDR1;
         else if (I2C_CST2 & MATCHA2F)   I2c_Var_Pntr->Addr = I2C_ADDR2;
         else if (I2C_CST2 & MATCHA3F)   I2c_Var_Pntr->Addr = I2C_ADDR3;
         else if (I2C_CST2 & MATCHA4F)   I2c_Var_Pntr->Addr = I2C_ADDR4;
         else if (I2C_CST2 & MATCHA5F)   I2c_Var_Pntr->Addr = I2C_ADDR5;
         else if (I2C_CST2 & MATCHA6F)   I2c_Var_Pntr->Addr = I2C_ADDR6;
         else if (I2C_CST2 & MATCHA7F)   I2c_Var_Pntr->Addr = I2C_ADDR7;
         else if (I2C_CST3 & MATCHA8F)   I2c_Var_Pntr->Addr = I2C_ADDR8;

         I2c_Var_Pntr->Addr = (I2c_Var_Pntr->Addr & ~0x80) << 1;

         #if HID_OV_I2C_SUPPORTED
         if ((Channel != HID_CHANNEL) || (I2c_Var_Pntr->Addr != I2C_HID_ADDR))
         {
             I2c_Var_Pntr->CRC_8 = 0;
             I2c_Var_Pntr->I2c_Irq_State = GETCMD_STATE;  /* Advance to next state. */
             I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8, I2c_Var_Pntr->Addr);
         }
         else
         {   /* SMBus channel is Host channle and slave address is HID over I2C. */
             I2c_Var_Pntr->i2c_slave_rcount = 0;
             I2c_Var_Pntr->i2c_slave_wcount = 0;

             Hookc_Cfg_Slave_BufPntr(I2c_Var_Pntr, Channel);

             if (IS_BIT_SET(I2C_ST, SMBST_XMIT))
             {
                 I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8, I2c_Var_Pntr->Addr | 1);
                 if(1 == Hookc_ReceivePrtl(I2c_Var_Pntr, Channel))
                 {
                     I2c_Var_Pntr->i2c_pntr = I2c_Var_Pntr->i2c_slave_wbuf;
                     I2c_Var_Pntr->I2c_Irq_State = SNDDATA_STATE;
                     irq_i2c_slv_xmit_data(I2c_Var_Pntr, Channel);
                 }
             }
             else
             {
                 I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8, I2c_Var_Pntr->Addr);
                 I2c_Var_Pntr->i2c_pntr = I2c_Var_Pntr->i2c_slave_rbuf;
                 if ((I2C_CTL1 & SMBCTL1_NMINTE) == 0)
                 {
                     I2C_ST = SMBST_NMATCH;
                     irq_i2c_slv_receive(I2c_Var_Pntr, Channel);
                 }
                 else
                 {
                     temp = I2C_SDA;
                 }
                 I2c_Var_Pntr->I2c_Irq_State = GETDATA_STATE;

             }
         }
         #else // HID_OV_I2C_SUPPORTED
         if (I2C_CTL1 & SMBCTL1_NMINTE)
         {   /* New match interrupt is enabled. */
             temp = I2C_SDA;
         }
         I2c_Var_Pntr->CRC_8 = 0;
         I2c_Var_Pntr->I2c_Irq_State = GETCMD_STATE;  /* Advance to next state. */
         I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8, I2c_Var_Pntr->Addr);
         #endif // HID_OV_I2C_SUPPORTED
     }
     else
     {
         irq_idle_error_recovery(I2c_Var_Pntr, Channel);
         Free_Smb_Level_0(Channel);
     }

     /* Clear the new match bit by writing a 1 to it. */
     I2C_ST = SMBST_NMATCH;


}

static void irq_i2c_getid(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Interrupt on data received. */

   /* Store presumed device address (or garbage if error occurred). */
   I2c_Var_Pntr->i2c_alarm_buffer[0] = (BYTE) (irq_get_i2c_data(I2c_Var_Pntr, Channel) & 0xFF);
}

static void irq_i2c_getlo(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Interrupt on data received. */

   /* Store low data byte (or garbage if error occurred). */
   I2c_Var_Pntr->i2c_alarm_buffer[1] = (BYTE) (irq_get_i2c_data(I2c_Var_Pntr, Channel) & 0xFF);
}

static void irq_i2c_gethi(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Interrupt on data received. */

   WORD temp;

   temp = irq_get_i2c_data(I2c_Var_Pntr, Channel);
   if (temp != ~0)
   {
      I2c_Var_Pntr->i2c_alarm_buffer[2] = (BYTE) (temp & 0xFF); /* Store high data byte. */

      /* Indicate that the irq_alarm_xxx bytes have been updated. */
      I2c_Var_Pntr->i2c_state.bit.ALARM_RDY = 1;
   }
}

static void irq_i2c_getstop(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* It is expected that this is an interrupt caused
      by a Stop Condition after a slave transfer. */

    if (IS_BIT_SET(I2C_ST, SMBST_SLVSTP))
   {
      /* Clear the slave stop bit by writing a 1 to it. */
        I2C_ST = SMBST_SLVSTP;
   }

   /*
   Force the I2C timer into a timed-out state (so the
   timer interrupt won't bother to increment it), then
   setup so 1 mS timer will post a request for service. */

    I2c_Var_Pntr->I2c_Timeout.bit.I2C_FAIL = 1;
    I2c_Var_Pntr->I2c_Irq_State = IDLE_STATE;   /* Set to idle state. */
    Hookc_SlaveDone(I2c_Var_Pntr, Channel);
}

static void irq_i2c_notme(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   ;
}

static void irq_i2c_getcmd(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    /* Check the command of protocol.
        Read/Write Byte
        Read/Write WORD
        Write WORD process call
        Read/Write Block
        Block Process call.*/
    I2c_Var_Pntr->i2c_slave_cmd = (BYTE) (irq_get_i2c_data(I2c_Var_Pntr, Channel) & 0xFF);
    I2c_Var_Pntr->I2c_Irq_State = CHKREP_STATE;
}


static void irq_i2c_chkrep(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    /* Check repeart start condition here. If restart then the protocol
       is read protocol. else the protocol should be a write protocol. This
       data byte should be byte count if this is block write/process call.*/

    I2c_Var_Pntr->i2c_pntr = (BYTE *) &I2c_Var_Pntr->i2c_slave_buffer[0];

    if (IS_BIT_SET(I2C_ST, SMBST_XMIT))
    {
        I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,I2C_SDA);
        /* Repeat start condition detected. It means SMBus read protocol. */
        I2C_ST = (SMBST_NMATCH + SMBST_NEGACK);
        /* Reset bus fail timeout timer and flag.  This enables the timer. */

        /* Hook to OEM. */

        I2c_Var_Pntr->i2c_slave_prtl = Hookc_GetReadPrtl(I2c_Var_Pntr, Channel);

        if (I2c_Var_Pntr->i2c_slave_prtl == SMB_READ_WORD)
        {
            I2c_Var_Pntr->count = 0x03;
        }
        else
        {
            I2c_Var_Pntr->count = I2c_Var_Pntr->i2c_slave_buffer[0];
        }

        irq_i2c_slv_xmit_data(I2c_Var_Pntr, Channel);
        I2c_Var_Pntr->I2c_Irq_State = SNDDATA_STATE;
    }
    else if (I2C_ST & SMBST_NMATCH)
    {
        I2c_Var_Pntr->i2c_slave_buffer[0] = I2C_SDA;        /* cache register data for use later */
        irq_i2c_idle(I2c_Var_Pntr, Channel);
    }
    else
    {   /* SMBus write protocol. */
        /* Hook to OEM. */
        I2c_Var_Pntr->i2c_slave_prtl = Hookc_GetWritePrtl(I2c_Var_Pntr, Channel);

        if (I2c_Var_Pntr->i2c_slave_prtl == SMB_WRITE_WORD)
        {
            *I2c_Var_Pntr->i2c_pntr++ = (BYTE) (irq_get_i2c_data(I2c_Var_Pntr, Channel) & 0xFF);
            I2c_Var_Pntr->count = 0x02;
        }
        else
        {
            *I2c_Var_Pntr->i2c_pntr++ = I2c_Var_Pntr->count = (BYTE) (irq_get_i2c_data(I2c_Var_Pntr, Channel) & 0xFF);
            I2c_Var_Pntr->count += 1;
        }
        I2c_Var_Pntr->I2c_Irq_State = GETDATA_STATE;
    }
}
static void irq_i2c_slv_xmit_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    /* SMBus now is working as slave transmit mode.
        Read BYTE
        Read WORD
        Read BLOCK. */
    if (IS_BIT_SET(I2C_ST, SMBST_BER))
    {
        irq_idle_error_recovery(I2c_Var_Pntr, Channel);
    }
    else
    {
        if (IS_BIT_SET(I2C_ST, SMBST_NEGACK))
        {
            I2c_Var_Pntr->I2c_Irq_State = GETSTOP_STATE;
            I2C_ST = SMBST_NEGACK;
        }
        else
        {
            #if HID_OV_I2C_SUPPORTED
            if (I2c_Var_Pntr->count)
            {
                I2c_Var_Pntr->i2c_slave_wcount++;
                I2C_SDA = *I2c_Var_Pntr->i2c_pntr++;      /* Send the data. */
                I2c_Var_Pntr->count--;
            }
            else
            {	// all data has transmit, return dummy data directly.
                I2C_SDA = 0xFF;      /* Send the data. */
            }
            #else // HID_OV_I2C_SUPPORTED
            if (I2c_Var_Pntr->count == 1)
            {
                I2C_SDA = I2c_Var_Pntr->CRC_8;
            }
            else if (I2c_Var_Pntr->count != 0)
            {
                I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,*I2c_Var_Pntr->i2c_pntr);
                I2C_SDA = *I2c_Var_Pntr->i2c_pntr++;      /* Send the data. */
            }
            else
            {   /* all data has transmit, return dummy data directly. */
                I2C_SDA = 0xFF;
            }
            I2c_Var_Pntr->count--;
            #endif // HID_OV_I2C_SUPPORTED
        }
    }
}
static void irq_i2c_slv_receive(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    /* SMBus now is working as slave receive mode.
        Write WORD
        Write Block. */
    BYTE temp;

    if (I2C_ST & SMBST_BER)
    {
        irq_slave_aborter(I2c_Var_Pntr, Channel);
    }
    else if (I2C_ST & SMBST_NMATCH)
    {
        temp = I2C_SDA;

        /* Repeat start condition detected. It means SMBus read protocol. */
        I2C_ST = SMBST_NMATCH;

        if (I2C_ST & SMBST_XMIT) // address with Rd bit, change to transmit data
        {
            /* Hook to OEM. */
            Hookc_GetReadPrtl(I2c_Var_Pntr, Channel);

            /* Reset bus fail timeout timer and flag.  This enables the timer. */
            //I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;
            I2c_Var_Pntr->i2c_pntr = I2c_Var_Pntr->i2c_slave_wbuf;

            //I2c_Var_Pntr->i2c_slave_rcount = 0;
            irq_i2c_slv_xmit_data(I2c_Var_Pntr, Channel);
            I2c_Var_Pntr->I2c_Irq_State = SNDDATA_STATE;
        }
        else
        {
            I2c_Var_Pntr->i2c_slave_rcount = 0;
            I2c_Var_Pntr->i2c_slave_wcount = 0;

            Hookc_Cfg_Slave_BufPntr(I2c_Var_Pntr, Channel);

            I2c_Var_Pntr->i2c_pntr = I2c_Var_Pntr->i2c_slave_rbuf;
            //I2c_Var_Pntr->I2c_Irq_State = GETDATA_STATE;
        }
    }
    else if (I2C_ST & SMBST_SDAST)
    {
        temp = I2C_SDA;
        I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8, temp);

        I2c_Var_Pntr->i2c_slave_rcount++;
        *I2c_Var_Pntr->i2c_pntr++ = temp;

        if(I2c_Var_Pntr->i2c_slave_rcount == 1) // data byte for send byte protocol, or command byte for other protocol
        {
            Hookc_GetWritePrtl(I2c_Var_Pntr, Channel);
        }
        else
        {
            if(I2c_Var_Pntr->count) /* [MCHT 2014/06/05] change to "i2c_slave_rcount" later ?? */
            {
                I2c_Var_Pntr->count--;
            }
        }
    }

    if (IS_BIT_SET(I2C_ST, SMBST_SLVSTP))
    {
        /* Clear the slave stop bit by writing a 1 to it. */
        I2C_ST = SMBST_SLVSTP;
        /* Hook to OEM */
        Hookc_MasterWrite(I2c_Var_Pntr, Channel);
        irq_i2c_getstop(I2c_Var_Pntr, Channel);
    }

}

#else // I2C_SLAVE_SUPPORTED

static void irq_i2c_unused(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    if (IS_BIT_SET(I2C_ST, SMBST_BER))
    {
        I2C_ST = SMBST_BER;
    }
    irq_idle_error_recovery(I2c_Var_Pntr, Channel);
}

#endif // I2C_SLAVE_SUPPORTED

/* --- Master mode routines. --- */

static void irq_i2c_send_address(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Check if successful in becoming the master.
      Send the slave address and read/write bit. */

   BYTE test;
   BYTE temp;
   test = I2C_ST;
   if (IS_BIT_SET(test, SMBST_BER))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if ( (test & (SMBST_MASTER + SMBST_SDAST)) == (SMBST_MASTER + SMBST_SDAST) )
   {
      /* The chip is the active master of the bus. */

      SET_BIT(I2C_CTL1, SMBCTL1_STASTRE);/* Stall the bus after sending byte. */

      if ( (I2c_Var_Pntr->i2c_buffer[0] & 0xC0) == (I2C_READ << 6) )
      {
         /* For Receive Byte SMBus protocol. */

         /* Sending the ACK bit when the protocol is for PEC. For the receive
            byte EC should get two bytes back. One is data another is PEC.*/

         if (!I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
         {
            CLEAR_BIT(I2C_CTL1, SMBCTL1_STASTRE);   /* Don't stall the bus after sending byte. */
            SET_BIT(I2C_CTL1, SMBCTL1_ACK);         /* Set the ACK bit.  This will cause
                                                       a negative ACK to be sent after the
                                                       next byte is received. */
         }
         /* Setup next state for I2C interrupt.
            The I2C_RCV_ONE state transfers control to irq_i2c_receive_one. */
         I2c_Var_Pntr->I2c_Irq_State = I2C_RCV_ONE;
      }
      else
      {
         /* For the other SMBus protocols. */

         //*I2C.CTL1 |= SMBCTL1_STASTRE;/* Stall the bus after sending byte. */

         /* Setup next state for I2C interrupt.
            The I2C_CHK_ADDR state transfers control to irq_i2c_check_address. */
         I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_ADDR;
      }

      if ( (I2c_Var_Pntr->i2c_buffer[0] & 0xC0) == (I2C_REPEAT << 6) )
      {
         /* This is for the following SMBus protocols:

            Read Byte
            Read Word
            Process Call
            Block Read

            These protocols have a repeated start.  The read/write
            bit is cleared (write) for the first start condition
            and set (read) for the repeated start condition.

            Send the address with the read/write bit cleared for write. */

            //*I2C.SDA = i2c_buffer[2] & ~1;
            I2C_SDA = temp = I2c_Var_Pntr->i2c_buffer[2] & ~1;
      }
      else
      {
         /* This is for the following SMBus protocols:

            Quick Command
            Send Byte
            Receive Byte
            Write Byte
            Write Word
            Block Write

            Send the address and read/write bit. */

         //*I2C.SDA = i2c_buffer[2];
         I2C_SDA = temp = I2c_Var_Pntr->i2c_buffer[2];
      }
      I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,temp);
      /* Reset bus fail timeout timer and flag. */
      I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;
   }
   else
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
}

static void irq_i2c_check_address(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Check if successful in sending address. */

   BYTE test;

   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_SET(test, SMBST_STASTR))
   {
      /* Bump write byte count by 1.  Address has been sent. */
      I2c_Var_Pntr->count--;

      if (I2c_Var_Pntr->count)
      {
         I2c_Var_Pntr->i2c_pntr = (BYTE *) &I2c_Var_Pntr->i2c_buffer[3]; /* Point to the command byte. */
         irq_i2c_mstr_xmit_data(I2c_Var_Pntr, Channel);
      }
      else
      {
         /* The write byte count is now 0.
            This is the Quick Command protocol. */
         irq_i2c_mstr_xmit_stop(I2c_Var_Pntr, Channel);
      }
   }
   else
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
}

static void irq_i2c_check_xmit(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Check if successful in sending data. */

   BYTE test;
   BYTE temp;
   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (SET_BIT(test, SMBST_SDAST))
   {
      I2c_Var_Pntr->count--;

      if (I2c_Var_Pntr->count)
      {
         /* Data in i2c_buffer to be transmitted. */
         irq_i2c_mstr_xmit_data(I2c_Var_Pntr, Channel);
      }
      else if ( (I2c_Var_Pntr->i2c_buffer[0] & 0xC0) == (I2C_REPEAT << 6) )
      {
         /* The following SMBus protocols use a repeated start:

            Read Byte
            Read Word
            Process Call
            Block Read

            The read/write bit is cleared (write) for the first start
            condition and set (read) for the repeated start condition.
         */

         /* Reset bus fail timeout timer and flag. */
         I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

         if (I2c_Var_Pntr->i2c_buffer[0] == SMB_READ_BYTE)
         {
            /* For the Read Byte protocol, set the ACK bit to send
               a negative ACK after the data byte is received. */
            if (!I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
               SET_BIT(I2C_CTL1, SMBCTL1_ACK);

            /* Setup next state for I2C interrupt.
               The I2C_RCV_ONE state transfers control to irq_i2c_receive_one. */
            I2c_Var_Pntr->I2c_Irq_State = I2C_RCV_ONE;
         }
         else
         {
            /* Clear the ACK bit. */
            CLEAR_BIT(I2C_CTL1, SMBCTL1_ACK);
            /* Setup next state for I2C interrupt.
               The I2C_CHK_REP state transfers control to irq_i2c_check_repeated. */
            I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_REP;
         }

         CLEAR_BIT(I2C_CTL1, SMBCTL1_STASTRE);  /* Don't stall the bus after sending byte. */
         I2C_ST = SMBST_STASTR;                 /* Clear STASTR bit by writing 1 to it. */
         SET_BIT(I2C_CTL1, SMBCTL1_START);      /* Send start condition with address. */

         /* Send the address with the read/write bit set for read. */

         I2C_SDA = temp = I2c_Var_Pntr->i2c_buffer[2] | 1;
         I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,temp);
      }
      else
      {
         if (!I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
            irq_i2c_mstr_xmit_stop(I2c_Var_Pntr, Channel);
         else
         {
            I2C_SDA = I2c_Var_Pntr->CRC_8;
            I2c_Var_Pntr->I2c_Irq_State = I2C_SND_PEC;
         }
      }
   }
   else
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
}

static void irq_i2c_check_repeated(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Check if successful in sending address.
      Get first byte transferred. */

   BYTE test;

   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_CLEAR(test, SMBST_SDAST))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else
   {
      /* Data is being received.  SMBST.SDAST is
         set because data is in the data register. */

      /* Setup next state for I2C interrupt.
         The I2C_RCV state transfers control to irq_i2c_receive. */
      I2c_Var_Pntr->I2c_Irq_State = I2C_RCV;

      I2c_Var_Pntr->i2c_pntr = (BYTE *) &I2c_Var_Pntr->i2c_buffer[1]; /* Point to buffer for the incoming data. */

        /* Setup read byte count using bits 0 through 6 of the SMB
           protocol type byte.

           If count is not 0x3F, set count equal to
           [(protocol & 0x3F) - 1)].  It is one less than the value
           in bits 0 through 6 of the protocol type byte because one
           byte has already been received at this point.

           If count is 0x3F, set count equal to the value of the byte
           just received from the I2C device.  It is expected that
           this many more bytes will be received from the device. */
        I2c_Var_Pntr->count = (I2c_Var_Pntr->i2c_buffer[0] & 0x3F) - 1;
        /* Check using 0x3E instead of 0x3F because 1 has
           been subtracted from the count at this point. */
        if (I2c_Var_Pntr->count == 0x3E)
        {
            *I2c_Var_Pntr->i2c_pntr = I2C_SDA;        /* Get and store the data. */
            I2c_Var_Pntr->count = *I2c_Var_Pntr->i2c_pntr;
            if ((I2c_Var_Pntr->count > 0x20) || (I2c_Var_Pntr->count == 0))
            {
                I2c_Var_Pntr->count = 0x20;
                //irq_idle_error_recovery(I2c_Var_Pntr, Channel);
            }
        }
        else
        {
            if(I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
            {
                if (I2c_Var_Pntr->count == 0)
                    I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_PEC;
                I2c_Var_Pntr->count++;
            }

            if (I2c_Var_Pntr->count == 1)   /* If the count == 1 then this is a read word protocol. */
            {
                SET_BIT(I2C_CTL1, SMBCTL1_ACK); /* Non ACK. */
            }
            else if (I2c_Var_Pntr->count == 0)
            {
                /* This is the last byte to be received. Ready to send Stop Condition. */

                SET_BIT(I2C_CTL1, SMBCTL1_STOP);
                /* This is the last byte to be received. */
                irq_i2c_slave_init(I2c_Var_Pntr, Channel);
            }
            *I2c_Var_Pntr->i2c_pntr = I2C_SDA;        /* Get and store the data. */
      }
      I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,*I2c_Var_Pntr->i2c_pntr);
      I2c_Var_Pntr->i2c_pntr++;
   }
}

static void irq_i2c_receive_one(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* For Receive Byte and Read Byte SMBus protocols.

      Check if successful in sending address.
      Get byte transferred. */

   BYTE test;

   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_CLEAR(test, SMBST_SDAST))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else
   {  /*
      Data is being received.  SMBST.SDAST is
      set because data is in the data register.

      This is the last byte to be received.
      Ready to send Stop Condition. */

      if (I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
      {
         SET_BIT(I2C_CTL1, SMBCTL1_ACK);  /* None ACK */
         I2c_Var_Pntr->i2c_buffer[1] = I2C_SDA;

         /* Update CRC-8. */
         I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,I2c_Var_Pntr->i2c_buffer[1]);

         /* Setup next state for I2C interrupt.
            The I2C_CHK_PEC state transfers control to irq_i2c_check_pec. */
         I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_PEC;
      }
      else
      {
         SET_BIT(I2C_CTL1, SMBCTL1_STOP);

         I2c_Var_Pntr->i2c_buffer[1] = I2C_SDA; /* Get and store the data. */

         /* Reset bus fail timeout timer and flag. */
         I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

         irq_i2c_slave_init(I2c_Var_Pntr, Channel);
      }
   }
}

static void irq_i2c_receive(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   /* Check if successful in receiving next byte. */

   BYTE test;

   test = I2C_ST;
   if (IS_BIT_SET(test, SMBST_BER))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_CLEAR(test, SMBST_SDAST))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else
   {
      /* Data is being received.  SMBST.SDAST is
         set because data is in the data register. */

      I2c_Var_Pntr->count--;

      if (I2c_Var_Pntr->count == 0)
      {  /*
         This is the last byte to be received.
         Ready to send Stop Condition. */

         SET_BIT(I2C_CTL1, SMBCTL1_STOP);
      }
      else if (I2c_Var_Pntr->count == 1)
      {  /*
         The next data byte (after this one) will
         be the last data byte.  Set the ACK bit.
         This will cause a negative ACK to be
         sent after the next byte is received. */

         SET_BIT(I2C_CTL1, SMBCTL1_ACK);
         if (I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
         {
            /* Setup next state for I2C interrupt.
               The I2C_CHK_PEC state transfers control to irq_i2c_check_pec. */
            I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_PEC;
         }
      }

      *I2c_Var_Pntr->i2c_pntr++ = I2C_SDA;   /* Get and store the data. */

      if (I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED)
         I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,*(I2c_Var_Pntr->i2c_pntr-1));

      /* Reset bus fail timeout timer and flag. */
      I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

      if (I2c_Var_Pntr->count == 0)
      {  /*
         This is the last byte to be received. */

         irq_i2c_slave_init(I2c_Var_Pntr, Channel);
      }
   }
}

static void irq_i2c_check_pec(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   BYTE test;

   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_CLEAR(test, SMBST_SDAST))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else
   {
      /* This is the last byte to be received.
         Ready to send Stop Condition. */
      SET_BIT(I2C_CTL1, SMBCTL1_STOP);
      test = I2C_SDA;   /* Get and store the data. */
      if (I2c_Var_Pntr->CRC_8 != test)
         I2c_Var_Pntr->i2c_state.bit.PEC_ERR = 1;
      /* Reset bus fail timeout timer and flag. */
      I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

      irq_i2c_slave_init(I2c_Var_Pntr, Channel);
   }
}

static void irq_i2c_check_xmit_pec(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   BYTE test;
   test = I2C_ST;

   if (IS_BIT_SET(test, SMBST_BER + SMBST_NEGACK))
   {
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
   }
   else if (IS_BIT_SET(test, SMBST_SDAST))
      irq_i2c_mstr_xmit_stop(I2c_Var_Pntr, Channel);
   else
      irq_idle_error_recovery(I2c_Var_Pntr, Channel);
}


/* ---------------------------------------------------------------
   Routines called by interrupt routines.
   --------------------------------------------------------------- */

/* ---------------------------------------------------------------
   Name: irq_slave_aborter

   Called on slave I2C timeout and by the slave interrupt
   routines to clean up the I2C interface when it stalls.

   Called with interrupts disabled.
   --------------------------------------------------------------- */
static void irq_slave_aborter(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE temp;

    I2C_CTL1 &= ~(SMBCTL1_INTEN);  /* Disable interrupt. */

    /* Dummy read to clear interface. */
    temp = I2C_SDA;
    /* Clear NMATCH and BER bits by writing 1s to them. */
    I2C_ST = (SMBST_NMATCH + SMBST_BER);

    /*
    Force the I2C timer into a timed-out state (so the
    timer interrupt won't bother to increment it), then
    setup so 1 mS timer will post a request for service. */

    I2c_Var_Pntr->I2c_Timeout.bit.I2C_FAIL = 1;
    I2c_Var_Pntr->I2c_Irq_State = IDLE_STATE;   /* Set to idle state. */
    #if I2C_SLAVE_SUPPORTED
    if (I2c_Var_Pntr->i2c_state.bit.ALARM_RDY == 0)
    {
        /* Turn on I2C interrupts if the
            last alarm has been processed. */
        SET_BIT(I2C_CTL1, SMBCTL1_INTEN);
    }
    Free_Smb_Level_0(Channel);
   #endif // I2C_SLAVE_SUPPORTED
}

/* ---------------------------------------------------------------
   Name: irq_get_i2c_data

   Used by slave mode routines based on I2C_Irq_State.

   Returns: a value with all bits set on error.
            otherwise, returns the data
            (byte in lower part of WORD) received.
   --------------------------------------------------------------- */
#if I2C_SLAVE_SUPPORTED
static WORD irq_get_i2c_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   BYTE test;

   test = I2C_ST & (SMBST_BER | SMBST_SDAST); /* Get BER and SDAST bits. */
   test ^= SMBST_SDAST;                      /* Complement SDAST bit. */

   if (test)
   {  /*
      The BER bit is set and/or the SDAST bit is clear.
      Aborted transfer.  Clean up interface, goto Idle. */
        irq_slave_aborter(I2c_Var_Pntr, Channel);

      return(WORD) ~0;           /* Indicate error. */
   }
   else
   {
      /* The data was received. */

      /* Reset bus fail timeout timer and flag. */
      I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

        I2c_Var_Pntr->I2c_Irq_State += 1;           /* Advance to next state. */
        test = I2C_SDA;
        I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,test);
        return(test & 0xFF);     /* Return data received. */
   }
}
#endif // I2C_SLAVE_SUPPORTED

/* ---------------------------------------------------------------
   Name: irq_i2c_mstr_xmit_data

   Send data and setup I2c_Irq_State for I2C_CHK_XMIT state.
   --------------------------------------------------------------- */
static void irq_i2c_mstr_xmit_data(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   if (IS_BIT_SET(I2C_CTL1, SMBCTL1_STASTRE))
      I2C_ST = SMBST_STASTR;   /* Clear STASTR bit by writing 1 to it. */

   /* Reset bus fail timeout timer and flag. */
   I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

   /* Setup next state for I2C interrupt.
      The I2C_CHK_XMIT state transfers control to irq_i2c_check_xmit. */
   I2c_Var_Pntr->I2c_Irq_State = I2C_CHK_XMIT;
   I2c_Var_Pntr->CRC_8 = updcrc(I2c_Var_Pntr->CRC_8,*I2c_Var_Pntr->i2c_pntr);
   I2C_SDA = *I2c_Var_Pntr->i2c_pntr++;      /* Send the data. */
}

/* ---------------------------------------------------------------
   Name: irq_i2c_mstr_xmit_stop

   Transmit Stop Condition on the I2C bus.  Already in master mode.
   --------------------------------------------------------------- */
static void irq_i2c_mstr_xmit_stop(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
   SET_BIT(I2C_CTL1, SMBCTL1_STOP);         /* Send Stop Condition. */
   I2C_ST = (SMBST_NEGACK + SMBST_STASTR);  /* Clear these bits
                                               by writing 1s. */

   irq_i2c_slave_init(I2c_Var_Pntr, Channel);
}

/* ---------------------------------------------------------------
   Name: irq_i2c_slave_init

   Put the I2C bus back in slave mode.
   --------------------------------------------------------------- */
static void irq_i2c_slave_init(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{   /*
    The module automatically goes into
    slave mode after a Stop Condition.

    Force the I2C timer into a timed-out state (so the timer
    update routine won't bother to increment it), then
    setup so 1 mS timer will post a request for service. */

    I2c_Var_Pntr->I2c_Timeout.bit.I2C_FAIL = 1;

    I2c_Var_Pntr->I2c_Irq_State = IDLE_STATE;   /* Go to idle state. */

    #if I2C_SLAVE_SUPPORTED

    I2C_Slave_Init(Channel);

    /* Now turn on I2C interrupts if the last alarm has
       been processed.  Set new match interrupt enable
       and ensure that interrupt enable bit is set. */
    if (I2c_Var_Pntr->i2c_state.bit.ALARM_RDY == 0)
    {
        SET_BIT(I2C_CTL1, SMBCTL1_NMINTE + SMBCTL1_INTEN);
    }

    #else // I2C_SLAVE_SUPPORTED
       CLEAR_BIT(I2C_CTL1, SMBCTL1_INTEN);
    #endif // I2C_SLAVE_SUPPORTED

    I2c_Var_Pntr->i2c_state.bit.INPUT_READY = 0;
    I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 1;

    if (!I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR)
    {
        I2c_Get_Data(&SMB_Buffer[Channel][0], Channel);
    }
    OEM_SMB_callback((BITS_8) I2c_Var_Pntr->i2c_state.byte, Channel);
    ACPI_Timer = 1;

    Disable_Irq();
    I2c_Var_Pntr->i2c_state.bit.TRANSFER_DONE = 0;
    I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR  = 0;
    I2c_Var_Pntr->i2c_state.bit.PEC_ERR  = 0;
    I2c_Var_Pntr->i2c_state2.bit.PEC_SUPPORTED = 0;
    Enable_Irq();

}

/* ---------------------------------------------------------------
   Name: irq_idle_error_recovery
   --------------------------------------------------------------- */
static void irq_idle_error_recovery(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE dummy;
    /* Turn off I2C interrupt. */
    CLEAR_BIT(I2C_CTL1, SMBCTL1_INTEN);

    /* If master, send Stop Condition. */
    if (IS_BIT_SET(I2C_ST, SMBST_MASTER))
    {
        SET_BIT(I2C_CTL1, SMBCTL1_STOP);      /* Ready to send Stop Condition. */

        /* Dummy read to clear interface. */
        dummy = I2C_SDA;

        CLEAR_BIT(I2C_CTL1, SMBCTL1_STASTRE);  /* Clear "Stall After Start Enable" bit. */

        /* Clear STASTR and NEGACK bits by writing 1s to them. */
        I2C_ST = (SMBST_STASTR + SMBST_NEGACK);

        /* Reset bus fail timeout timer and flag. */
        I2c_Var_Pntr->I2c_Timeout.byte = I2C_FAIL_TIME;

        /* Set flag to transfer control to i2c_wait_stop. */
        I2c_Var_Pntr->i2c_state.bit.WAIT_STOP = 1;

        Disable_Irq();
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 1;
        Enable_Irq();

        /* Because the TRANSFER_ERR flag is set,
           I2c_Get_Data will return the number of bytes read as 0. */
        /* Setup so 1 mS timer will
           post a request for service. */
    }
    else
    {
        irq_idle_error_recovery2(I2c_Var_Pntr, Channel);
   }
}


#if I2C_GPIO_RECOVERY
typedef struct _I2C_GPIO {
    volatile unsigned char *scldir;
    volatile unsigned char *scldout;
    unsigned char          sclpin;
    volatile unsigned char *sdadir;
    volatile unsigned char *sdadin;
    volatile unsigned char *sdadout;
    unsigned char          sdapin;
} I2C_GPIO;

const I2C_GPIO I2CTable[]=
{
    {&P1DIR, &P1DOUT, 7, &P2DIR, &P2DIN, &P2DOUT, 2},
    {&P7DIR, &P7DOUT, 3, &P7DIR, &P7DIN, &P7DOUT, 4},
    {&P2DIR, &P2DOUT, 3, &P3DIR, &P3DIN, &P3DOUT, 1},
    {&P4DIR, &P4DOUT, 7, &P5DIR, &P5DIN, &P5DOUT, 3},
};

/* ---------------------------------------------------------------
   Name: irq_idle_error_recovery2
   --------------------------------------------------------------- */
static void irq_idle_error_recovery2(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE test;
    BYTE error_recover_exit;
    BYTE retry_count;

    I2C_GPIO *i2cport;

    CLEAR_BIT(I2C_CTL2, SMBCTL2_ENABLE);   /* Disable I2C module. */
    i2cport = (I2C_GPIO *) &I2CTable[Channel];

    /* Set SCL and SDA pin output low. */
    *i2cport->scldout &= ~(1 << i2cport->sclpin);
    *i2cport->sdadout &= ~(1 << i2cport->sdapin);
    /* Set SCL and SDA pin as input. */
    *i2cport->scldir &= ~(1 << i2cport->sclpin);
    *i2cport->sdadir &= ~(1 << i2cport->sdapin);

    /* Disable SMBus Alternative function. */
    I2C_Devalt_Cconfig(FALSE, Channel);

    if((*i2cport->sdadin & (1 << i2cport->sdapin)) == 0)      // check the SDA line is active or not
    { /* SDA in low. */
        retry_count = 8;
        error_recover_exit = 0;     // continue to check the SDA line
        do
        {
            if((*i2cport->sdadin & (1 << i2cport->sdapin)) == 0)      // check the SDA line is active or not
            { /* SDA in low. */
                /* Pull low SCL pin. */
                *i2cport->scldir |= (1 << i2cport->sclpin);
                Microsecond_Delay(5);
                /* Set SCL as OD output and driven high by pull up registor. */
                *i2cport->scldir &= ~(1 << i2cport->sclpin);
                Microsecond_Delay(5);
            }
            else
            {   /* SDA is recovery. */
                error_recover_exit = 1;
            }
            retry_count--;
        }while(!error_recover_exit && (retry_count != 0));

        /* Enable SMBus Alternative function. */
        I2C_Devalt_Cconfig(TRUE, Channel);


        SET_BIT(I2C_CTL2, SMBCTL2_ENABLE);    /* Enable I2C module. */
        Microsecond_Delay(10);

        SET_BIT(I2C_CTL1, SMBCTL1_START);  // issue START condition
        Microsecond_Delay(20);
        irq_i2c_mstr_xmit_stop(I2c_Var_Pntr, Channel);
    }
    else
    {
        /* Enable SMBus Alternative function. */
        I2C_Devalt_Cconfig(TRUE, Channel);
        /* Enable SMBus Module */
        SET_BIT(I2C_CTL2, SMBCTL2_ENABLE);    /* Enable I2C module. */
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 1;
        irq_i2c_slave_init(I2c_Var_Pntr, Channel);
    }
}

void I2C_Devalt_Cconfig(FLAG enable, BYTE Channel)
{
    /* Set SCL and SDA pins as GPIO. */
    switch(Channel)
    {
        case SMB_1:
            //CLEAR_BIT(DEVALT2, SMB1_SL);   /* Disable I2C Alternative function. */
            DEVALT2 = (DEVALT2 & ~SMB1_SL) | enable << 6;   /* Disable I2C Alternative function. */
            break;
        case SMB_2:
            //CLEAR_BIT(DEVALT2, SMB2_SL);   /* Disable I2C Alternative function. */
            DEVALT2 = (DEVALT2 & ~SMB2_SL) | enable << 7;   /* Disable I2C Alternative function. */
            break;
        case SMB_3:
            //CLEAR_BIT(DEVALTA, SMB3_SL);   /* Disable I2C Alternative function. */
            DEVALTA = (DEVALT2 & ~SMB3_SL) | enable << 6;
            break;
        case SMB_4:
            //CLEAR_BIT(DEVALTA, SMB3_SL);   /* Disable I2C Alternative function. */
            DEVALTA = (DEVALT2 & ~SMB3_SL) | enable << 7;
            break;
    }
}
#else // I2C_GPIO_RECOVERY
/* ---------------------------------------------------------------
   Name: irq_idle_error_recovery2
   --------------------------------------------------------------- */
static void irq_idle_error_recovery2(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE test;
    BYTE error_recover_exit;
    BYTE retry_count;
    CLEAR_BIT(I2C_CTL2, SMBCTL2_ENABLE);   /* Disable I2C module. */
    Microsecond_Delay(20);
    SET_BIT(I2C_CTL2, SMBCTL2_ENABLE);    /* Enable I2C module. */

    Microsecond_Delay(10);

    if(IS_BIT_CLEAR(I2C_CST, SMBCST_TSDA))
    {
        SET_BIT(I2C_CTL1, SMBCTL1_START);  // issue START condition

        retry_count = 128;
        error_recover_exit = 0;     // continue to check the SDA line
        do
        {
            if(IS_BIT_CLEAR(I2C_CST, SMBCST_TSDA))      // check the SDA line is active or not
            {
                I2C_CST = SMBCST_TGSCL;     // issue one SCL cycle

                Microsecond_Delay(100);

                if(IS_BIT_SET(I2C_ST, SMBST_MASTER))  // check is in MASTER mode or not
                {
                    error_recover_exit = 1; // exit SDA check loop
                }
            }
            else
            {
                error_recover_exit = 1;     // exit SDA check loop
            }
            retry_count--;
        }while(!error_recover_exit && (retry_count != 0));

        SET_BIT(I2C_CST, SMBCST_BB);   // clear the BB flag

        irq_i2c_mstr_xmit_stop(I2c_Var_Pntr, Channel);
    }
    else
    {
        I2c_Var_Pntr->i2c_state.bit.TRANSFER_ERR = 1;
        irq_i2c_slave_init(I2c_Var_Pntr, Channel);
    }
}
#endif // I2C_GPIO_RECOVERY

/* ---------------------------------------------------------------
   Name: Smb_Level_0_Free

   Tells if SMBus level 0 can be used.

   Returns: 1 if SMBus level 0 can be used, 0 if it is busy.
   --------------------------------------------------------------- */
FLAG Smb_Level_0_Free(BYTE Channel)
{
   return(I2c_Var[Channel].i2c_state2.bit.I2C_FREE);
}

/* ---------------------------------------------------------------
   Name: Free_Smb_Level_0

   Allows another process to use SMBus level 0.

   Returns: The selector setting for level 0.
   --------------------------------------------------------------- */
void Free_Smb_Level_0(BYTE Channel)
{
   I2c_Var[Channel].i2c_state2.bit.I2C_FREE = 1;
}

/* ---------------------------------------------------------------
   Name: Handle_SMBus

   Service SMBus request.  This is called at a 1 mS interval.
   --------------------------------------------------------------- */
void Handle_SMBus(void)
{
    SMALL index;

    for (index = 0; index < SMB_CHNLS; index++)
    {

        if (I2c_Var[index].I2c_Timeout.bit.I2C_FAIL == 0)
        {
            /* Timer did not time out. */

            /* Increment timer.
               This increments timer until I2C_FAIL bit is set. */
            I2c_Var[index].I2c_Timeout.byte++;

            if (I2c_Var[index].I2c_Timeout.bit.I2C_FAIL)
            {
                /* I2C bus timer timed out.  Flag for SMBus service. */
                I2c_Var[index].i2c_state.bit.TIMEOUT = 1;
                if(I2c_Var[index].i2c_state.bit.WAIT_STOP)
                {
                    i2c_wait_stop(&I2c_Var[index], index);
                }
                else
                {
                    irq_idle_error_recovery(&I2c_Var[index], index);
                }
                // End of Johnny 2015/3/4 Modified.
            }
        }
        if (I2c_Var[index].i2c_state.bit.WAIT_STOP)
        {
            i2c_wait_stop(&I2c_Var[index], index);
        }
    }
}

#if I2C_SLAVE_SUPPORTED
/* ---------------------------------------------------------------
   Name: I2C_Slave_Init

   Initalized SMBus slave address and enable.
   --------------------------------------------------------------- */
static void I2C_Slave_Init(BYTE Channel)
{
    /* Set slave address. */
    switch(Channel)
    {
        case 00:
            I2C_ADDR1 = S_MYADR1_1;
            I2C_ADDR2 = S_MYADR1_2;
            I2C_ADDR3 = S_MYADR1_3;
            I2C_ADDR4 = S_MYADR1_4;
            I2C_ADDR5 = S_MYADR1_5;
            I2C_ADDR6 = S_MYADR1_6;
            I2C_ADDR7 = S_MYADR1_7;
            I2C_ADDR8 = S_MYADR1_8;
            break;
        #if (SMB_CHNLS >= 2)
        case 01:
            I2C_ADDR1 = S_MYADR2_1;
            I2C_ADDR2 = S_MYADR2_2;
            I2C_ADDR3 = S_MYADR2_3;
            I2C_ADDR4 = S_MYADR2_4;
            I2C_ADDR5 = S_MYADR2_5;
            I2C_ADDR6 = S_MYADR2_6;
            I2C_ADDR7 = S_MYADR2_7;
            I2C_ADDR8 = S_MYADR2_8;
            break;
        #if (SMB_CHNLS >= 3)
        case 02:
            I2C_ADDR1 = S_MYADR3_1;
            I2C_ADDR2 = S_MYADR3_2;
            I2C_ADDR3 = S_MYADR3_3;
            I2C_ADDR4 = S_MYADR3_4;
            I2C_ADDR5 = S_MYADR3_5;
            I2C_ADDR6 = S_MYADR3_6;
            I2C_ADDR7 = S_MYADR3_7;
            I2C_ADDR8 = S_MYADR3_8;
            break;
        #if (SMB_CHNLS >= 4)
        case 03:
            I2C_ADDR1 = S_MYADR4_1;
            I2C_ADDR2 = S_MYADR4_2;
            I2C_ADDR3 = S_MYADR4_3;
            I2C_ADDR4 = S_MYADR4_4;
            I2C_ADDR5 = S_MYADR4_5;
            I2C_ADDR6 = S_MYADR4_6;
            I2C_ADDR7 = S_MYADR4_7;
            I2C_ADDR8 = S_MYADR4_8;
            break;
        #endif // (SMB_CHNLS >= 2)
        #endif // (SMB_CHNLS >= 3)
        #endif // (SMB_CHNLS >= 4)
    }
}
#endif // I2C_SLAVE_SUPPORTED




#endif /* if I2C_SUPPORTED */
