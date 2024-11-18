/* ---------------------------------------------------------------
 * MODULE SMB_TRAN.C
 *
 *       Copyright 2004, all rights reserved
 *       Insyde Software Corp.
 *
 * This file contains OEM code for SMBus transfer requests.
 *
 * Revision History (Started March 23, 1999 by GLP)
 *
 * --------------------------------------------------------------- */

#define SMB_TRAN_C

#include "smb_tran.h"
#include "types.h"
#include "defs.h"
#include "purdat.h"
#include "regs.h"
#include "i2c.h"

extern BITS_8 i2c_state[];
#define INPUT_READY   bit0
#define TRANSFER_DONE bit1
#define TRANSFER_ERR  bit2
#define WAIT_STOP     bit3
#define ALARM_RDY     bit4
#define ALARM_WAITING bit5
#define PEC_ERR  bit6
#define TIMEOUT  bit7

BYTE protocol[SMB_CHNLS]; /* Saved protocol byte. */
BYTE ok[SMB_CHNLS];       /* Result of SMBus transfer.  FALSE = error. */
BYTE i2c_done[SMB_CHNLS];       /* Result of SMBus transfer.  FALSE = error. */
extern void Free_Smb_Level_0(BYTE Channel);


FLAG SMB_wrWORD(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, WORD wData,
                FLAG Prtcl_Type /*BYTE RETRY_CNT*/)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts.


      FLAG == TRUE  -> SMBus write word is ok.
           == FALSE -> SMBus write word failed. */
   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_WRITE_WORD;          /* Save protocol byte. */
   *smb_pntr++ = SMB_WRITE_WORD;    /* Protocol byte. */
   *smb_pntr++ = 4;                 /* Number of bytes following. */
   *smb_pntr++ = SMB_SLAVE & ~0x01; /* Address and read/write = 0 for write. */
   *smb_pntr++ = SMB_COMMAND;       /* SMBus command. */
   *smb_pntr++ = (BYTE) (wData & 0xFF);
   *smb_pntr = (BYTE) ((wData >> 8) & 0xFF);

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (I2c_Var[Channel].i2c_state.bit.TRANSFER_ERR)
        return (0);
    else
        return (1);
}

FLAG SMB_wrBYTE(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, BYTE wData,
                FLAG Prtcl_Type /* BYTE RETRY_CNT */)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts.


      FLAG == TRUE  -> SMBus write byte is ok.
           == FALSE -> SMBus write byte failed. */
   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_WRITE_BYTE;          /* Save protocol byte. */
   *smb_pntr++ = SMB_WRITE_BYTE;    /* Protocol byte. */
   *smb_pntr++ = 3;                 /* Number of bytes following. */
   *smb_pntr++ = SMB_SLAVE & ~0x01; /* Address and read/write = 0 for write. */
   *smb_pntr++ = SMB_COMMAND;       /* SMBus command. */
   *smb_pntr = (BYTE) wData;

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (I2c_Var[Channel].i2c_state.bit.TRANSFER_ERR)
        return (0);
    else
        return (1);
}


BYTE * SMB_rdWORD(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type /* BYTE RETRY_CNT */)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts. */

    smb_pntr = &SMB_Buffer[Channel][0];
    protocol[Channel] = SMB_READ_WORD;      /* Save protocol byte. */
    *smb_pntr++ = SMB_READ_WORD;            /* Protocol byte. */
    *smb_pntr++ = 2;                        /* Number of bytes following. */
    *smb_pntr++ = SMB_SLAVE;                /* Address and read/write = x.  The
                                               read/write bit is handled by core code. */
    *smb_pntr = SMB_COMMAND;                /* SMBus command. */

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (Prtcl_Type)
    {   // Interrupt mode.
        return 0;
    }
    // Polling mode
    return Get_Result(Channel);
}


BYTE * SMB_rdBYTE(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type/* BYTE RETRY_CNT */)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts. */
   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_READ_BYTE;        /* Save protocol byte. */
   *smb_pntr++ = SMB_READ_BYTE;  /* Protocol byte. */
   *smb_pntr++ = 2;              /* Number of bytes following. */
   *smb_pntr++ = SMB_SLAVE;      /* Address and read/write = x.  The
                                    read/write bit is handled by core code. */
   *smb_pntr = SMB_COMMAND;    /* SMBus command. */
    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (Prtcl_Type)
    {   // Interrupt mode.
        return 0;
    }
    // Polling mode
    return Get_Result(Channel);
}




BYTE * SMB_RECEIVE(BYTE Channel, BYTE SMB_SLAVE, FLAG Prtcl_Type/* BYTE RETRY_CNT */)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts. */
   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_RECEIVE_BYTE;        /* Save protocol byte. */
   *smb_pntr++ = SMB_RECEIVE_BYTE;              /* Protocol byte. */
   *smb_pntr++ = 1;                             /* Number of bytes following. */
   *smb_pntr = SMB_SLAVE | 1;                   /* Address and read/write = x.  The
                                                    read/write bit is handled by core code. */
    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (Prtcl_Type)
    {
        return 0;
    }
    return Get_Result(Channel);
}

FLAG SMB_SEND(BYTE Channel, BYTE SMB_SLAVE,  BYTE wData, FLAG Prtcl_Type/* BYTE RETRY_CNT */)
{
   BYTE *smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts.

      FLAG == TRUE  -> SMBus write byte is ok.
           == FALSE -> SMBus write byte failed. */
    smb_pntr = &SMB_Buffer[Channel][0];
    protocol[Channel] = SMB_SEND_BYTE;           /* Save protocol byte. */
    *smb_pntr++ = SMB_SEND_BYTE;     /* Protocol byte. */
    *smb_pntr++ = 2;                 /* Number of bytes following. */
    *smb_pntr++ = SMB_SLAVE & ~0x01; /* Address and read/write = 0 for write. */
    *smb_pntr = (BYTE) wData;

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (I2c_Var[Channel].i2c_state.bit.TRANSFER_ERR)
        return (0);
    else
        return (1);
}


BYTE * SMB_rdBLOCK(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, FLAG Prtcl_Type/* BYTE RETRY_CNT */)
{
   BYTE * smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts.

      FLAG == TRUE  -> SMBus read block is ok.
           == FALSE -> SMBus read block failed. */

   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_READ_BLOCK;       /* Save protocol byte. */
   *smb_pntr++ = SMB_READ_BLOCK; /* Protocol byte. */
   *smb_pntr++ = 2;              /* Number of bytes following. */
   *smb_pntr++ = SMB_SLAVE;      /* Address and read/write = x.  The
                                    read/write bit is handled by core code. */
   *smb_pntr = SMB_COMMAND;    /* SMBus command. */

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (Prtcl_Type)
    {   // Interrupt mode.
        return 0;
    }
    // Polling mode
    return Get_Result(Channel);
}




FLAG SMB_wrBLOCK(BYTE Channel, BYTE SMB_SLAVE, BYTE SMB_COMMAND, BYTE Send_CNT, BYTE * trans_pntr, FLAG Prtcl_Type/* BYTE RETRY_CNT */)
{
   BYTE * smb_pntr;
   /* SMB_SLAVE: Slave address.

      RETRY_CNT: Number of times to attempt an SMBus transfer.
                 Must be >= 1 and <= 3.  The RETRY_CNT is not
                 the number of retries but is the number of
                 SMBus transfer attempts.


      FLAG == TRUE  -> SMBus write word is ok.
           == FALSE -> SMBus write word failed. */

   BYTE i;
   smb_pntr = &SMB_Buffer[Channel][0];
   protocol[Channel] = SMB_WRITE_BLOCK;         /* Save protocol byte. */
   /* SMB_Buffer[Channel][0] */ *smb_pntr++ = SMB_WRITE_BLOCK;   /* Protocol byte. */
   if (Send_CNT >= 0x20)               /* Block to tarnsfer a maximum of 32 data byte. */
      *smb_pntr++ = 0x20;           /* Number of bytes following. */
   else
   *smb_pntr++ = Send_CNT+3;     /* Number of bytes following. */
   *smb_pntr++ = SMB_SLAVE & ~0x01; /* Address and read/write = 0 for write. */
   *smb_pntr++ = SMB_COMMAND;       /* SMBus command. */
   *smb_pntr++ = Send_CNT;          /* SMBus data count. */

   for (i = 0; i < Send_CNT ; i++)
      *smb_pntr++ = *(trans_pntr+i) & 0xFF;

    I2c_Execute_Protocol(SMB_Buffer[Channel], Channel, Prtcl_Type);
    if (I2c_Var[Channel].i2c_state.bit.TRANSFER_ERR)
        return (0);
    else
        return (1);
}

BYTE * Get_Result(BYTE Channel)
{
    /* Get result of SMBus transfer.

        Pointer points to tran_buffer if SMBus transfer is ok.
            SMB_Buffer[0] has count of bytes received.
            SMB_Buffer[1] is first byte.
            SMB_Buffer[2] is second byte.
            etc.

        Pointer == NULL if SMBus transfer failed. */

    BYTE * pntr;

    pntr = (BYTE *)SMB_Buffer[Channel];
    Free_Smb_Level_0(Channel);
    if (ok[Channel])
    {
        if (protocol[Channel] != SMB_READ_BLOCK)
        {
            pntr++;
        }
    }
    else    pntr = (BYTE *) 0;

    return(pntr);
}
extern BYTE ACPI_Timer;
void OEM_SMB_callback(BITS_8 i2c_state, BYTE Channel)
{
    /* Callback from I2C low level driver when the SMBus transaction done.
       i2c_done[Channel] flag was set to indicate SMBus transaction was done.
       ok[Channel] flag was set when trasaction done without error detected. */
    i2c_done[Channel] = TRUE;
    if (i2c_state.bit.TRANSFER_DONE && !i2c_state.bit.TRANSFER_ERR
        && !i2c_state.bit.PEC_ERR && !i2c_state.bit.TIMEOUT)
    {
        ok[Channel] = TRUE;
    }

    SMB_Status[Channel].bit.SMB_IBF = 0;
    SMB_Status[Channel].bit.SMB_OBF = 1;
    SMB_Status[Channel].bit.SMB_ERROR = i2c_state.bit.TRANSFER_ERR;
    SMB_Status[Channel].bit.SMB_PEC_ERROR = i2c_state.bit.PEC_ERR;
    SMB_Status[Channel].bit.SMB_TMO_ERROR = i2c_state.bit.TIMEOUT;
}

FLAG Transfer_Finished(BYTE Channel)
{
   /* Check if SMBus transfer has completed.
      Use Get_Result() when this function returns TRUE. */

    if (i2c_done[Channel])
    {
        return(TRUE);    /* Transfer completed. */
    }
    else
    {
        return(FALSE);   /* Not finished yet. */
    }
}

