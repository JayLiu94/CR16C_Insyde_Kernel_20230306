/*----------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                              */
/* Copyright (c) 2010 by Nuvoton Technology Corporation                                                     */
/* All rights reserved                                                                                      */
/*----------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                           */
/*       ecdthif.c - Embeded Controller Debug Tool - Host Interface implementation                          */
/*                   The commands are defined using Write Command Buffer (WCB) protocol                     */
/*                                                                                                          */
/* Project: Firmware for Nuvoton Notebook Embedded Controller Peripherals                                   */
/*----------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "kernel\core\inc\swtchs.h"

#if LPC_TO_I2C_SUPPORTED

#include "types.h"
#include "defs.h"
#include "purdat.h"
#include "regs.h"
#include "shm.h"
#include "i2c.h"
#include "icu.h"
#include "ns_proc.h"
#include "host_if.h"
#include "lpc_i2c.h"
#include "pursmb.h"
#if LPC_TO_I2C_GSENSOR_SUPPORTED
#include "sensorhub.h"
#endif //LPC_TO_I2C_GSENSOR_SUPPORTED

///*------------------------------------------------------------------------------
// * WRITE COMMAND BUFFER (WCB) Interface Definitions
// *----------------------------------------------------------------------------*/
//#define LPC2I2C_WIN_SIZE    4
//#define LPC2I2C_SHM_SIZE    (0x01 << LPC2I2C_WIN_SIZE)
//
//#define HOST_REQUEST_BUFFER_MAX_SIZE       0x2000
////#define CORE_SEND_BUFFER_MAX_SIZE          0x2000

/*----------------------------------------------------------------------------------------------------------*/
/* Definitions.                                                                                             */
/*----------------------------------------------------------------------------------------------------------*/

#define HID_I2C_Channel     SMB_3
#define LPC_I2C_Prtl_Type   1   // 0: SMBus polling mode, 1: SMBus Interupt mode

// Byte offsets in the SHM space
#define    SHM_SEM                      0 /*UINT8*/
#define    SHM_SLAVE_ADDRESS            1 /*UINT8*/
#define    SHM_NUM_OF_WRITES            2 /*UINT16*/
#define    SHM_NUM_OF_READS             4 /*UINT16*/
#define    SHM_WRITE_BUFFER_START       6 /* actual buffer from FFC6 to FFCF */
#define    SHM_WRITE_BUFFER_START_NEXT  1 /* on subsequent packets, no header */
#define    SHM_READ_BUFFER_START        2 /* First reply chunk includes status byte */
#define    SHM_READ_BUFFER_START_NEXT   1 /* on subsequent reply chunk semaphore and data only */
#define    SHM_STATUS                   1 /*UINT8*/

/*------------------------------------------------------------------------------
* Shared Memory support
*----------------------------------------------------------------------------*/
// Byte offsets in the SHM space
#define SHM_SEM           0
#define SHM_CMD           1
// Host side - lower nibble
#define SEM_REQUEST       (0x01 << 0)
#define SEM_ACK           (0x01 << 1)
// Core side - nibble
#define SEM_SEND          (0x01 << 4)
#define SEM_CONT          (0x01 << 5)
#define SEM_INT           (0x01 << 6)


// SHM i/f State Machine
typedef enum LPC_TO_I2C_STATE_
{
    LPC_TO_I2C_STATE_CLEAR_TO_SEND = 0,
    LPC_TO_I2C_STATE_WAIT_FOR_ACK,
    LPC_TO_I2C_STATE_WAIT_FOR_ACK_CLEAR,
    LPC_TO_I2C_STATE_BUSY,
    LPC_TO_I2C_STATE_IDLE
}  LPC_TO_I2C_STATE;

// LPC to I2C status (returned onth first chunk)
typedef enum LPC_TO_I2C_STATUS_
{
    STATUS_TARGET_NACK =        0x00,
    STATUS_REPLY_OK =           0x01,
    STATUS_BAD_COMMAND_FORMAT = 0x02
}  LPC_TO_I2C_STATUS;


/*----------------------------------------------------------------------------------------------------------*/
/* Typedefs.                                                                                                */
/*----------------------------------------------------------------------------------------------------------*/
typedef struct _LPC_to_I2C_message
{
    BYTE    slaveAddress;
    WORD    num_of_writes_total;
    WORD    num_of_reads_total;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Current state of the FW                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    WORD    num_of_writes;
    WORD    num_of_reads;
    BYTE    section_counter;  // number of sections that where send to the host so far. This is just for debug
    LPC_TO_I2C_STATUS    status;
    BOOLEAN                bFirstCommandReceived;
    BOOLEAN                bIntGenerated;
} LPC_to_I2C_message;

/*---------------------------------------------------------------------------------------------------------*/
/* Globals for LPC_to_I2C_Demo                                                                             */
/*---------------------------------------------------------------------------------------------------------*/

volatile int    LPC2I2C_handel_i2c_command = 0;
volatile int    LPC2I2C_Loop = 0;
volatile int    NUM_OF_BYTES;

/*---------------------------------------------------------------------------------------------------------*/
/* SHM i/f State Machine                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static volatile LPC_TO_I2C_STATE               SHM_state;

/*---------------------------------------------------------------------------------------------------------*/
/* The following structure contains all the data to be sent to the host                                    */
/* sync_val; report_size; report_id; buff; check_sum;                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static volatile  LPC_to_I2C_message                     sLPC2I2C_message;


/*------------------------------------------------------------------------------
 * Function:
 *    SHM_init
 *
 * Description:
 *    Initializes Shared memory hardware module elements that are relevant for
 *    both Shared Memory RAM windows.
 *    Note that flash-related initializations which are expected to be done by
 *    the boot ROM are not done here (e.g. flash size, read burst size, memory
 *    protection, HOSTWAIT clearing).
 *
 * Parameters:
 *    shm_callback  Function to be called upon shared memory interrupt
 *    smc_ctl       Value to be set in bits 0..6 of Shared Memory Core Control
 *                  register. Bit 7 is ignored. See Datasheet for detailed
 *                  description of SMC_CTL register.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/

void LPC_I2C_SHM_init(void)
 {
	BYTE smc_ctrl = 0;

    Disable_Irq();
    DP80CTL &= ~DP80EN;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize State Machine                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    SHM_state = LPC_TO_I2C_STATE_IDLE;

    /*------------------------------------------------------------------------
     * Configure Host Access
     *------------------------------------------------------------------------*/

    /* Ignore write transactions on forbidden access */
    SET_FIELD(SMC_CTL, CTL_HERES, 0x01);
    smc_ctrl = SMC_CTL;
	/*------------------------------------------------------------------------
     * Enable Semaphore 2 interrupt
     *------------------------------------------------------------------------*/
    SET_RBIT(smc_ctrl, HSEM1_IE);

    /*------------------------------------------------------------------------
     * Configure Shared RAM Window
     *------------------------------------------------------------------------*/
    SET_FIELD(WIN_SIZE, WIN_SIZE_RWIN1_SIZE, LPC2I2C_WIN_SIZE);

    /* Set required Host Read/Write Protection for Window */
    SET_FIELD(WIN_PROT, WIN_PROT_RWIN1, 0);

    /* Set Shared RAM window start -
     * Window base is the offset of the window from RAM start */
    WIN_BASE1 = (WORD)(lpci2c_command_buffer - RAM_BASE_ADDRESS);

    /*------------------------------------------------------------------------
     * Configure Host Access
     *------------------------------------------------------------------------*/

    /* Don't stall SHM transactions (default chip state) */
    RSHM_ENABLE;

    /*------------------------------------------------------------------------
     * Configure interrupt
     *------------------------------------------------------------------------*/

    //ICU_Disable_Irq(ICU_EINT6);
    //ICU_Clr_Pending_Irq(ICU_EINT6);
    //ICU_Enable_Irq(ICU_EINT6);

    /* Set required control value */
    SMC_CTL = smc_ctrl;

    /* Clear status */
    SMC_STS = ShM_ACC + HRERR;
    SMC_STS = HSEM1W;
    // release Host wait
    SET_RBIT(SMC_CTL, HOSTWAIT);

    PROT_CFG |= 0x80;
    HIPM4IE |= 0x01;

    LPC2I2C_Loop = 0;

    Enable_Irq();
}


/*------------------------------------------------------------------------------
 * Function:
 *    LPC2I2C_send_single_chunk
 *
 * Description:  Divide the buffer in more than one transaction according to the size of the window defined
 *
 * Parameters:
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/

void LPC2I2C_send_single_chunk(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Find the buffer to copy to SHM                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    unsigned int iCnt = 0;
    unsigned int iStopBufInd = 0;
    unsigned int iShmOffset = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* If it's the firt chunk, need to add status byte                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if (sLPC2I2C_message.num_of_reads == 0 )
    {
        iShmOffset = SHM_READ_BUFFER_START;

        /*------------------------------------------------------------------------------------------------*/
        /* write the status byte on the first section                                                     */
        /*------------------------------------------------------------------------------------------------*/

        lpci2c_command_buffer[SHM_STATUS] = sLPC2I2C_message.status;
    }
    else
    {
        iShmOffset = SHM_READ_BUFFER_START_NEXT;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* calc locations to copy to                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if((sLPC2I2C_message.num_of_reads_total - sLPC2I2C_message.num_of_reads) > (LPC2I2C_SHM_SIZE - iShmOffset))
    {
        iStopBufInd = LPC2I2C_SHM_SIZE;
    }
    else
    {
        iStopBufInd = sLPC2I2C_message.num_of_reads_total - sLPC2I2C_message.num_of_reads + iShmOffset;
    }

    for (iCnt = iShmOffset; iCnt < iStopBufInd; iCnt++)
    {
         lpci2c_command_buffer[iCnt] = lpci2c_reply_buffer[sLPC2I2C_message.num_of_reads];
         sLPC2I2C_message.num_of_reads++;
    }
}

/*------------------------------------------------------------------------------
 * Function:
 *    LPC2I2C_get_single_chunk
 *
 * Description:  Divide the buffer in more than one transaction according to the size of the window defined
 *
 * Parameters:
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void LPC2I2C_get_single_chunk(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* calc location to copy from                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    unsigned int iStartSHM = 0;
    unsigned int iStopBufInd = 0;
    unsigned int iCnt = 0;

    /*---------------------------------------------------------------------------------------------------*/
    /* First chunk comes with a header                                                                   */
    /*---------------------------------------------------------------------------------------------------*/
    if ( sLPC2I2C_message.num_of_writes == 0 )
    {
        iStartSHM = SHM_WRITE_BUFFER_START;
    }
    else
    {
        iStartSHM = SHM_WRITE_BUFFER_START_NEXT;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* calc locations to copy to                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if((sLPC2I2C_message.num_of_writes_total - sLPC2I2C_message.num_of_writes) > (LPC2I2C_SHM_SIZE - iStartSHM))
    {
        iStopBufInd = LPC2I2C_SHM_SIZE;
    }
    else
    {
        iStopBufInd = sLPC2I2C_message.num_of_writes_total - sLPC2I2C_message.num_of_writes + iStartSHM;
    }

    for (iCnt = iStartSHM; iCnt < iStopBufInd; iCnt++)
    {
         lpci2c_request_buffer[sLPC2I2C_message.num_of_writes] = lpci2c_command_buffer[iCnt] ;
         sLPC2I2C_message.num_of_writes++;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ECDT_SHM_HIF_handle                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE - the ECDT HIF command was completed successfully                                 */
/*                  FALSE- otherwise                                                                       */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Should be called periodically by the application.                                      */
/*                  Checks whether the op code in the WCB buffer fits to one of the                        */
/*                  ECDT HIF op codes and if yes, executes the command according to the WCB protocol       */
/*---------------------------------------------------------------------------------------------------------*/
void LPC_I2C_SHM_Int_handle(void)
{
    //void   *data;
    //bool    success = FALSE;
    //BYTE  operation, DataSize;
    //int    index, i;
    //int PageSize; // default value for 256 page size

    //if (!(SMC_STS & HSEM1W)) {
    //    return;
    //}

    BYTE iSemaphore;
    BYTE bRequest;
    BYTE bAck;
    BYTE bSend;
    FLAG bSkip = 0;

    // Edward 2015/4/22 Modified.
    //SHM_STALL;
    // End of Edward 2015/4/22 Modified.
    SMC_STS = HSEM1W;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parse bits in semaphore                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    iSemaphore = SHAW1_SEM;
    bRequest   = READ_RBIT(iSemaphore, SEM_REQUEST);
    bAck       = READ_RBIT(iSemaphore, SEM_ACK);
    bSend      = READ_RBIT(iSemaphore, SEM_SEND);

    /*-----------------------------------------------------------------------------------------------------*/
    /* New request from host: no matter what the status is - restart everything                            */
    /* request bit on the semaphore is high only for the first packet                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (bRequest)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Even if FW is busy - restart everything                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SHM_state =  LPC_TO_I2C_STATE_BUSY;

        /*---------------------------------------------------------------------------------------------*/
        /* Note that we read the request                                                               */
        /*---------------------------------------------------------------------------------------------*/
        CLEAR_RBIT(iSemaphore, SEM_SEND);
        SET_RBIT(iSemaphore, SEM_CONT);

        /*-------------------------------------------------------------------------------------------------*/
        /* parse the new command from the host                                                             */
        /*-------------------------------------------------------------------------------------------------*/

        sLPC2I2C_message.slaveAddress  = lpci2c_command_buffer[SHM_SLAVE_ADDRESS];
        sLPC2I2C_message.num_of_writes_total = * ((WORD *) &lpci2c_command_buffer[SHM_NUM_OF_WRITES]);
        sLPC2I2C_message.num_of_reads_total  = * ((WORD *) &lpci2c_command_buffer[SHM_NUM_OF_READS]);
        sLPC2I2C_message.num_of_writes = 0;
        sLPC2I2C_message.num_of_reads  = 0;
        sLPC2I2C_message.section_counter = 0;
        sLPC2I2C_message.status = STATUS_BAD_COMMAND_FORMAT ; // expect for the worst

        /*-------------------------------------------------------------------------------------------------*/
        /* Buffers too small                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if ((sLPC2I2C_message.num_of_reads_total > CORE_SEND_BUFFER_MAX_SIZE) ||
            (sLPC2I2C_message.num_of_writes_total > HOST_REQUEST_BUFFER_MAX_SIZE))
        {
            SHM_state =  LPC_TO_I2C_STATE_CLEAR_TO_SEND; // send the error

            sLPC2I2C_message.num_of_reads_total = 0;
            lpci2c_command_buffer[SHM_STATUS] = sLPC2I2C_message.status;

            SET_RBIT(iSemaphore, SEM_SEND);
            SET_RBIT(iSemaphore, SEM_CONT);
        }
        else
        {
            /*-------------------------------------------------------------------------------------------------*/
            /* Adjust Semaphore ( notice we acctually write it at the end of the interrupt )                    */
            /*-------------------------------------------------------------------------------------------------*/
            CLEAR_RBIT(iSemaphore, SEM_SEND);
            SET_RBIT(iSemaphore, SEM_CONT);

            /*-------------------------------------------------------------------------------------------------*/
            /* copy the write buffer and send it to the device                                                 */
            /*-------------------------------------------------------------------------------------------------*/
            if (sLPC2I2C_message.num_of_writes_total > 0 )
            {
                LPC2I2C_get_single_chunk();
                sLPC2I2C_message.section_counter++;
            }

            SHM_state =  LPC_TO_I2C_STATE_WAIT_FOR_ACK;
        }
    }

    /*-------------------------------------------------------------------------------------------------*/
    /* got an ACK on a request EC got from host                                                        */
    /*-------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_WAIT_FOR_ACK)  &&
                bAck &&
                !bSend )
    {
        CLEAR_RBIT(iSemaphore, SEM_CONT);
        CLEAR_RBIT(iSemaphore, SEM_SEND);

        SHM_state =  LPC_TO_I2C_STATE_WAIT_FOR_ACK_CLEAR;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Ack clear - during request handling                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_WAIT_FOR_ACK_CLEAR)  &&
                !bAck &&
                !bSend )
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Adjust Semaphore ( notice we acctually write it at the end of the interrupt )                    */
        /*-------------------------------------------------------------------------------------------------*/
        CLEAR_RBIT(iSemaphore, SEM_SEND);
        SET_RBIT(iSemaphore, SEM_CONT);

        /*-------------------------------------------------------------------------------------------------*/
        /* copy the write buffer and send it to the device                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        if (sLPC2I2C_message.num_of_writes_total > 0 )
        {
            LPC2I2C_get_single_chunk();
            sLPC2I2C_message.section_counter++;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Got the last packet and we are clear to send                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if( sLPC2I2C_message.num_of_writes == sLPC2I2C_message.num_of_writes_total )
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Notify the main while loop to send the message to the i2c slave (click_pad)                 */
            /*---------------------------------------------------------------------------------------------*/
            LPC2I2C_handel_i2c_command = 1; // trigger LPC_TO_I2C_DEMO_main_loop
            //Service.bit.LPC_I2C = 1;
            /*---------------------------------------------------------------------------------------------*/
            /* section_counter is used both for sending and receiving. done receiving and start sending:   */
            /*---------------------------------------------------------------------------------------------*/
            sLPC2I2C_message.section_counter = 0;


            /*---------------------------------------------------------------------------------------------*/
            /* busy state - main loop os working                                                           */
            /*---------------------------------------------------------------------------------------------*/
            SHM_state =  LPC_TO_I2C_STATE_BUSY;
            //Load_Timer_A();
            // Edward 2015/4/22 Modified.
            //LPC2I2C_Loop = 2;
            iSemaphore = LPC_I2C_SHM_Service_loop();
            // End of Edward 2015/4/22 Modified.
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* waiting for more packets                                                                    */
            /*---------------------------------------------------------------------------------------------*/
             SHM_state =  LPC_TO_I2C_STATE_WAIT_FOR_ACK;
        }
    }

    /*-------------------------------------------------------------------------------------------------*/
    /* send next chunk of data. First chunk is sent from the main loop.                                */
    /*-------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_CLEAR_TO_SEND)  &&
                !bAck &&
                !bRequest     )
    {
        // Adjust Semaphore
        SET_RBIT(iSemaphore, SEM_SEND);
        SET_RBIT(iSemaphore, SEM_CONT);

        LPC2I2C_send_single_chunk();
        sLPC2I2C_message.section_counter++;

        // Set SHM State Machine: we are waiting for
        // the host acknowledge
        SHM_state = LPC_TO_I2C_STATE_WAIT_FOR_ACK;
    }

    /*-------------------------------------------------------------------------------------------------*/
    /* got an ACK on a packet we send                                                                  */
    /*-------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_WAIT_FOR_ACK)  &&
                bAck &&
                bSend)
    {
            SET_RBIT(iSemaphore, SEM_SEND);
            CLEAR_RBIT(iSemaphore, SEM_CONT);
        /*-------------------------------------------------------------------------------------------------*/
        /* is there nothing left to send?                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        if( sLPC2I2C_message.num_of_reads == sLPC2I2C_message.num_of_reads_total )
        {
            SHM_state =  LPC_TO_I2C_STATE_WAIT_FOR_ACK_CLEAR;
        }
        else
        {
            SHM_state =  LPC_TO_I2C_STATE_CLEAR_TO_SEND;
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Ack clear - after sending of reply                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_WAIT_FOR_ACK_CLEAR)  &&
                !bAck &&
                bSend )
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Adjust Semaphore ( notice we acctually write it at the end of the inerrupt )                    */
        /*-------------------------------------------------------------------------------------------------*/
        CLEAR_RBIT(iSemaphore, SEM_SEND);
        CLEAR_RBIT(iSemaphore, SEM_CONT);

        SHM_state =  LPC_TO_I2C_STATE_IDLE;
        // Enable PTP & SF interrupt.
        //WKEN4 |= 0x40;
        //WKEN9 |= 0x01;

    }

    /*-------------------------------------------------------------------------------------------------*/
    /* Nothing left to do                                                                              */
    /*-------------------------------------------------------------------------------------------------*/
    else if   ( ( SHM_state ==  LPC_TO_I2C_STATE_IDLE)  &&
                !bAck)
    {
        CLEAR_RBIT(iSemaphore, SEM_SEND);
        CLEAR_RBIT(iSemaphore, SEM_CONT);
        bSkip = 1;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write the semaphore - trig the host                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((SHM_state != LPC_TO_I2C_STATE_BUSY) && (bSkip == 0))
    {
        //SMC_STS = SMC_STS;
        SHAW1_SEM = iSemaphore;
    }

    //SMC_STS = SMC_STS;
    //RSHM_ENABLE;
    SET_RBIT(SMC_CTL, HSEM1_IE); // HSEM1_IE bit
}

void Lpc2I2c_Busy(void)
{

}

void Lpc2I2c_Wait_Ack(void)
{

}

void Lpc2I2c_Wait_AckClear(void)
{

}

void Lpc2I2c_Idle(void)
{

}

void Lpc2I2c_Clear_Send(void)
{

}




/*------------------------------------------------------------------------------
 * Function:
 *    LPC_TO_I2C_DEMO_main_loop
 *
 * Description:
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
//void LPC_I2C_SHM_Service_loop(void)
BYTE LPC_I2C_SHM_Service_loop(void)
{
    BYTE iSemaphore = 0;
    int i;
    BYTE *ptr;

    if ( LPC2I2C_handel_i2c_command == 1 )
    {
        #if LPC_TO_I2C_GSENSOR_SUPPORTED
        if (sLPC2I2C_message.slaveAddress == LPC_I2C_ADDR)
        {
	        if (sLPC2I2C_message.num_of_writes_total != 0)
	        {
	            //for (i = 0; i < sLPC2I2C_message.num_of_writes_total; i++)
	            //{
                //   Hidi2c_Slave_Buf[i] = lpci2c_request_buffer[i];
	            //}
	    	    //Hidi2c_Slave_Rx((BYTE *) &lpci2c_request_buffer, sLPC2I2C_message.num_of_writes_total);

                NUM_OF_BYTES = sLPC2I2C_message.num_of_writes_total;
	    	    Hidi2c_Slave_Rx((BYTE *) &lpci2c_request_buffer, NUM_OF_BYTES);
	        }

            if (sLPC2I2C_message.num_of_reads_total)
            {
                NUM_OF_BYTES = sLPC2I2C_message.num_of_reads_total;
                ptr = (BYTE*) lpci2c_reply_buffer;
	            //Hidi2c_Slave_Tx((const BYTE **)&lpci2c_reply_buffer, (WORD *) &sLPC2I2C_message.num_of_reads_total);
	            //Hidi2c_Slave_Tx((const BYTE **)&ptr, (WORD *) &sLPC2I2C_message.num_of_reads_total);
	            Hidi2c_Slave_Tx((const BYTE **)&ptr, (WORD *) &NUM_OF_BYTES);
	            //memcpy(ptr, lpci2c_reply_buffer, sLPC2I2C_message.num_of_reads_total);
	            for (i = 0; i < sLPC2I2C_message.num_of_reads_total; i++)
	            {
                   lpci2c_reply_buffer[i] = *ptr++;
	            }

	        }
            sLPC2I2C_message.status = STATUS_REPLY_OK;

            /*---------------------------------------------------------------------------------------------*/
            /* Operation done - send reply                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            //SHM_state =  LPC_TO_I2C_STATE_CLEAR_TO_SEND;

            LPC2I2C_handel_i2c_command = 0;

            SET_RBIT(iSemaphore, SEM_SEND);
            SET_RBIT(iSemaphore, SEM_CONT);

            LPC2I2C_send_single_chunk();
            sLPC2I2C_message.section_counter++;

            /*---------------------------------------------------------------------------------------------*/
            /* Set SHM State Machine: we are waiting for the host acknowledge                              */
            /*---------------------------------------------------------------------------------------------*/
            SHM_state = LPC_TO_I2C_STATE_WAIT_FOR_ACK;

            /*-----------------------------------------------------------------------------------------------------*/
            /* Clear status                                                                                        */
            /*-----------------------------------------------------------------------------------------------------*/

            //SMC_STS = SMC_STS;
            // Edward 2015/4/22 masked.
            //SHAW1_SEM = iSemaphore;
            // End of Edward 2015/4/22 masked.
        }

        #else //LPC_TO_I2C_GSENSOR_SUPPORTED

        I2c_Var[HID_I2C_Channel].i2c_generic_ptrl = 1;
        I2c_Var[HID_I2C_Channel].i2c_slave_addr = sLPC2I2C_message.slaveAddress << 1;
        I2c_Var[HID_I2C_Channel].i2c_write_count = sLPC2I2C_message.num_of_writes_total;
        I2c_Var[HID_I2C_Channel].i2c_read_count = sLPC2I2C_message.num_of_reads_total;
        I2c_Var[HID_I2C_Channel].i2c_write_pntr = (BYTE *) &lpci2c_request_buffer;
        I2c_Var[HID_I2C_Channel].i2c_read_pntr = (BYTE *) &lpci2c_reply_buffer;

        // Edward 2013/12/2 Added.
        SMB_Buffer[2][0] = I2C_GENERIC_WRITE; // Protocol byte.
        if (sLPC2I2C_message.num_of_reads_total)
        {
            SMB_Buffer[2][0] = I2C_GENERIC_READ; // Protocol byte.
        }
        //SMB_Buffer[2][1] = sLPC2I2C_message.num_of_writes_total + 1;   // Number of bytes following.
        SMB_Buffer[2][1] = 1;   // Number of bytes following.
        SMB_Buffer[2][2] = sLPC2I2C_message.slaveAddress << 1;
        // End of Edward 2013/12/2 Added.
        sLPC2I2C_message.bFirstCommandReceived = 1;
        //I2c_Execute_Protocol((BYTE *) &lpci2c_request_buffer, HID_I2C_Channel, LPC_I2C_Prtl_Type);
//        I2c_Execute_Protocol(SMB_Buffer[HID_I2C_Channel], HID_I2C_Channel, LPC_I2C_Prtl_Type);
        // Edward 2014/3/11 Added for performance mesaurement.
//        P6DOUT &= ~0x01;
        // End of Edward 2014/3/11 Added.


        if(LPC_I2C_Prtl_Type == 0) //polling mode
        {
            I2c_Var[HID_I2C_Channel].i2c_generic_ptrl = 0;

            if (I2c_Var[HID_I2C_Channel].i2c_state.byte & 0x04)
            {
                sLPC2I2C_message.status = STATUS_TARGET_NACK;
            }
            else
            {
                sLPC2I2C_message.status = STATUS_REPLY_OK;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Operation done - send reply                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            SHM_state =  LPC_TO_I2C_STATE_CLEAR_TO_SEND;

            LPC2I2C_handel_i2c_command = 0;

            SET_RBIT(iSemaphore, SEM_SEND);
            SET_RBIT(iSemaphore, SEM_CONT);

            LPC2I2C_send_single_chunk();
            sLPC2I2C_message.section_counter++;

            /*---------------------------------------------------------------------------------------------*/
            /* Set SHM State Machine: we are waiting for the host acknowledge                              */
            /*---------------------------------------------------------------------------------------------*/
            SHM_state = LPC_TO_I2C_STATE_WAIT_FOR_ACK;

            /*-----------------------------------------------------------------------------------------------------*/
            /* Clear status                                                                                        */
            /*-----------------------------------------------------------------------------------------------------*/

            //SMC_STS = SMC_STS;
            SHAW1_SEM = iSemaphore;
        }
    #endif //LPC_TO_I2C_GSENSOR_SUPPORTED
    }
    // Edward 2015/4/22 Added.
    return (iSemaphore);
    // End of Edward 2015/4/22 Added.
}

void LPC_I2C_SMBus_Callback(BITS_8 i2c_state, BYTE Channel)
{
    BYTE iSemaphore = 0;

#define INPUT_READY   bit0
#define TRANSFER_DONE bit1
#define TRANSFER_ERR  bit2
#define TIMEOUT         bit7

    I2c_Var[HID_I2C_Channel].i2c_generic_ptrl = 0;

    if (i2c_state.bit.TRANSFER_DONE && !i2c_state.bit.TRANSFER_ERR && !i2c_state.bit.TIMEOUT)
    {
        sLPC2I2C_message.status = STATUS_REPLY_OK;
    }
    else
    {
        sLPC2I2C_message.status = STATUS_TARGET_NACK;
    }
    sLPC2I2C_message.bIntGenerated = 0;
    /*---------------------------------------------------------------------------------------------*/
    /* Operation done - send reply                                                                 */
    /*---------------------------------------------------------------------------------------------*/
    SHM_state =  LPC_TO_I2C_STATE_CLEAR_TO_SEND;

    LPC2I2C_handel_i2c_command = 0;

    SET_RBIT(iSemaphore, SEM_SEND);
    SET_RBIT(iSemaphore, SEM_CONT);

    LPC2I2C_send_single_chunk();
    sLPC2I2C_message.section_counter++;

    /*---------------------------------------------------------------------------------------------*/
    /* Set SHM State Machine: we are waiting for the host acknowledge                              */
    /*---------------------------------------------------------------------------------------------*/
    SHM_state = LPC_TO_I2C_STATE_WAIT_FOR_ACK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear status                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/

    SHAW1_SEM = iSemaphore;
}

void Lpc2I2c_GenIRQ(BYTE IntNum)
{
    //if (( SHM_state == LPC_TO_I2C_STATE_IDLE) && (sLPC2I2C_message.bFirstCommandReceived == TRUE)
    //      || (IntNum != 0))
    //{
            // wait till the bus is free.
            sLPC2I2C_message.bIntGenerated = 1;
            HIPM4IC |= IRQB;
            //int Timeout = 1000;
            int Timeout = 100;

            while (Timeout-- > 1);
            HIPM4IC &= ~IRQB;
    //}

}

#endif //LPC_TO_I2C_SUPPORTED

