/*------------------------------------------------------------------------------
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.
 *------------------------------------------------------------------------------
 * File: FU_OEM.C - OEM Flash Update runtime implementation
 *----------------------------------------------------------------------------*/

#include "swtchs.h"
#include "com_defs.h"
#include "fu_oem.h"
#include "fu.h"
#include "timers.h"
#include "i2c.h"
#include "regs.h"

#if SMB_FLUPDATE_SUPPORTED

#define SMB_DISABLE         0
#define SMB_IDLE            1
#define SMB_NEW_MATCH       2
#define SMB_REPEATED_START  3
#define SMB_NEGACK          4
#define SMB_DATA_RECEIVE    5
#define SMB_DATA_TRANSMIT   6

#define SMB_Data_In         0
#define SMB_Data_Out        1

#define SMB_SDA     PORT_BYTE   (SMB1SDA_ADDR   + SMB_Flash_Channel*0x40)
#define SMB_ST      PORT_BYTE   (SMB1ST_ADDR    + SMB_Flash_Channel*0x40)
#define SMB_CST     PORT_BYTE   (SMB1CST_ADDR   + SMB_Flash_Channel*0x40)
#define SMB_CTL1    PORT_BYTE   (SMB1CTL1_ADDR  + SMB_Flash_Channel*0x40)
#define SMB_ADDR1   PORT_BYTE   (SMB1ADDR1_ADDR + SMB_Flash_Channel*0x40)
#define SMB_CTL2    PORT_BYTE   (SMB1CTL2_ADDR  + SMB_Flash_Channel*0x40)
#define SMB_ADDR2   PORT_BYTE   (SMB1ADDR2_ADDR + SMB_Flash_Channel*0x40)
#define SMB_CTL3    PORT_BYTE   (SMB1CTL3_ADDR  + SMB_Flash_Channel*0x40)
#define SMB_CTL4    PORT_BYTE   (SMB1CTL4_ADDR  + SMB_Flash_Channel*0x40)



/******************************************************************************
 * Start of SMBus polling code
 *****************************************************************************/
/*-----------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/

BYTE    SMB_Transmit_Cnt;
BYTE    SMB_Transmit_Index;
BYTE    SMB_Receive_Cnt;
BYTE    SMB_Direction;
BYTE    SMB_Operation_State;
BYTE    SMB_Address;
WORD    SMB_Checksum;
//BYTE    SMB_Flupdate_Sts;

typedef void (* BOOT_BLOCK_func_ptr)(void);

void FLASH_UPDATE_SMB_polling(void);
void SMB_Read_Command_Proc(void);
void Flash_WCB_Write_Proc(void);

/*-----------------------------------------------------------------------------
 * Main SMBus code
 *---------------------------------------------------------------------------*/
void Enter_Flash_Mode(void)
{
    BYTE   index;

    SMB_Flupdate_Sts = SMBFL_STATUS_OK;
    SMB_Receive_Cnt = 0x00;
    SMB_Transmit_Cnt = 0x00;
    SMB_Transmit_Index = 0x00;
    SMB_Direction = 0x00;
    SMB_Address = 0x00;
    SMB_Operation_State = SMB_IDLE;
    SMB_Checksum = 0x0000;

    /* S>[MCHT 2013/01/03] */
    // clean ??
    // SMB_FL_Rbuf[SMB_FL_Buf_Size];
    // SMB_FL_Wbuf[SMB_FL_Buf_Size];
    /* E<[MCHT 2013/01/03] */

    //FL_Event_Inactive;
    // re-enable the salve address
    SMB_ADDR1  &= ~SMBADDR_SAEN; /* [MCHT 2012/07/16] */
    SMB_ADDR2 |= SMBADDR_SAEN;

    /* [MCHT 2012/05/25] */ //SMB_ST = SMBST_NMATCH | SMBST_BER | SMBST_SLVSTP | SMBST_NEGACK;

    FLASH_UPDATE_SMB_polling();
}

void FLASH_UPDATE_SMB_polling(void)
{
    BYTE smb_data;

    while(1)
    {
        if (SMB_Operation_State == SMB_IDLE)
        {   /* A Slave Address Match has been identified */
            while (!(SMB_ST & SMBST_NMATCH))
            {
                ;
            }
        }

        if((SMB_ST & SMBST_NMATCH))
        {   /* A Slave Address Match has been identified */
            if((SMB_ST & SMBST_XMIT)) SMB_Direction = SMB_Data_Out;
            else                      SMB_Direction = SMB_Data_In;

            SMB_Address = SMB_SDA & 0xFE; // clear the r/w bit
            SMB_ST = SMBST_NMATCH | SMBST_BER;

            if (SMB_Operation_State == SMB_IDLE)
            {
                SMB_Operation_State = SMB_NEW_MATCH;
                SMB_Receive_Cnt = 0;
            }
            else // repeatd start for process call/read byte/read word/read block protocol
            {
                // will transmit data to master
                SMB_Transmit_Cnt = 0;
                SMB_Transmit_Index = 0;

                if(SMB_Operation_State == SMB_DATA_RECEIVE)
                {
                    SMB_Read_Command_Proc();
                }

                SMB_Operation_State = SMB_REPEATED_START;
            }
        }

        if ((SMB_ST & SMBST_NEGACK))
        {   /* A negative acknowledge has occurred */
            SMB_ST = SMBST_NEGACK;

            // check the all data has transmit to master or not
            // or block with some eror
            // wait for STOP condition

            //if(SMB_Operation_State == SMB_DATA_TRANSMIT)
            //{
                // check the data tramit is done or not
                // for Receive byte, Read byte/word, Block read, Process call
            //}

            SMB_Operation_State = SMB_NEGACK;
        }

        if ((SMB_ST & SMBST_SLVSTP))
        {   /* A Slave Stop Condition has been identified */
            SMB_ST = SMBST_SLVSTP;

            if(SMB_Operation_State == SMB_DATA_RECEIVE)
            {
                Flash_WCB_Write_Proc();
            }

            SMB_Operation_State = SMB_IDLE;

            if(FLASH_UPDATE_active == FALSE) return;
        }

        if ((SMB_ST & SMBST_BER))
        {   /* A Bus Error has been identified */
            SMB_ST = SMBST_BER;

            // bus error --- need to recover the bus ??

            SMB_Flupdate_Sts |= FU_SEM_ERR;
            SMB_Operation_State = SMB_IDLE;
            //SMB_Protocol_Done();

            if(FLASH_UPDATE_active == FALSE) return;
        }

        /* SDA status is set - transmit or receive, master or slave */
        if ((SMB_ST & SMBST_SDAST))
        {
            if (SMB_Direction == SMB_Data_In) // read data from SMBus buffer
            {
                if (SMB_Receive_Cnt < SMB_FL_Buf_Size)
                {
                    SMB_FL_Rbuf[SMB_Receive_Cnt] = SMB_SDA;
                    SMB_Receive_Cnt++;
                }
                else
                {
                    smb_data = SMB_SDA;
                }

                SMB_Operation_State = SMB_DATA_RECEIVE;
            }
            else    // write data to SMBus buffer
            {
                if (SMB_Transmit_Cnt)
                {
                    SMB_SDA = SMB_FL_Wbuf[SMB_Transmit_Index];
                    SMB_Transmit_Index++;
                    SMB_Transmit_Cnt--;
                }
                else
                {   // No specific data need to send, dummy return directly.
                    SMB_SDA = 0xFF;
                }

                SMB_Operation_State = SMB_DATA_TRANSMIT;
            }
        }
    }
}

void SMB_Read_Command_Proc(void)
{
    BYTE index;
    BYTE fl_size;

    if(SMB_Address == SMB_Flash_Addr)
    {
        switch(SMB_FL_Rbuf[OFFSET_SMBR_OPERATION]) // command byte
        {
            case FLASH_STS_READ:
                // handle the Read byte protocol
                // return data will store at write_command_buffer[0] -- status
                SMB_FL_Wbuf[OFFSET_SMBW_STATUS] = SMB_Flupdate_Sts;
                SMB_Transmit_Cnt = 1;
                SMB_Flupdate_Sts = SMBFL_STATUS_OK;

                //FL_Event_Inactive;
                break;

            case (READ_OP | 0x00): // 1  byte
            case (READ_OP | 0x01): // 2  bytes
            case (READ_OP | 0x02): // 4  bytes
            case (READ_OP | 0x03): // 8  bytes
            case (READ_OP | 0x04): // 16 bytes
            case (READ_OP | 0x05): // 32 bytes
                fl_size = 1 << (SMB_FL_Rbuf[OFFSET_SMBR_OPERATION] & 0x07);
                FLASH_UPDATE_read(fl_size, (BYTE *)(&SMB_FL_Wbuf[OFFSET_SMBW_DATA]));
                SMB_FL_Wbuf[OFFSET_SMBW_COUNT] = fl_size;
                SMB_Transmit_Cnt = fl_size + 1;
                break;

            case GET_CHECKSUM_OP:
                SMB_FL_Wbuf[0] = SMB_Checksum & 0xFF;
                SMB_FL_Wbuf[1] = SMB_Checksum >> 8;
                SMB_Transmit_Cnt = 2;
                break;

        }
    }
}

/******************************************************************************
 * End of SMBus polling code
 *****************************************************************************/

/******************************************************************************
 * Start of Flash Update code
 *****************************************************************************/

void Flash_WCB_Write_Proc(void)
{
    bool   success = FALSE;
    DWORD fl_addr;
    int index;

    if(SMB_Address == SMB_Flash_Addr)
    {
        // disable the salve address before service WCB command
        /* [MCHT 2012/07/16] */ //SMB_ADDR  &= ~SMBADDR_SAEN;
        SMB_ADDR2 &= ~SMBADDR_SAEN; /* [MCHT 2012/07/16] */

        SMB_FL_Wbuf[OFFSET_SMBW_COUNT] = 0x00;
        SMB_Flupdate_Sts = SMBFL_STATUS_BUSY;

        fl_addr = *(DWORD*)(&SMB_FL_Rbuf[OFFSET_SMBR_ADDRESS]) & (FLASH_device_size -1);

        /*---------------------------------------------------------------------------------*/
        /* Flash Update in progress. Host issued request - retrieve the request contents                             */
        /*---------------------------------------------------------------------------------*/
        switch(SMB_FL_Rbuf[OFFSET_SMBR_OPERATION])
        {
            case FLASH_COMMANDS_INIT_OP: //amit
                for(index = read_device_id; index <= last_command; index++) {
                    FLASH_commands[index] = SMB_FL_Rbuf[index+OFFSET_SMBR_DATA];
                }
                success = TRUE; /* Indicate end of operation */
                break;

            case ADDRESS_SET_OP:
                /*---------------------------------------------------------------------------------*/
                /* Set Addreses operation                                        FFF00000          */
                /*---------------------------------------------------------------------------------*/
                if(SMB_FL_Rbuf[OFFSET_SMBR_COUNT] == 4)
                {
                    FLASH_UPDATE_address = fl_addr;

                    success = TRUE;
                }
                break;

            case CHECKSUM_OP:
                /*---------------------------------------------------------------------------------*/
                /* Perform Checksum                                                                */
                /*---------------------------------------------------------------------------------*/
                if(SMB_FL_Rbuf[OFFSET_SMBR_COUNT] == 8)
                {
                    FLASH_UPDATE_checksum(fl_addr,
                                          *(DWORD*)(&SMB_FL_Rbuf[OFFSET_SMBR_READ_SIZE]),
                                           (WORD*)(&SMB_Checksum));
                    success = TRUE;
                }
                break;

            case SECTOR_ERASE_OP:
                /*--------------------------------------------------------------------------*/
                /* Sector Erase                                                             */
                /*--------------------------------------------------------------------------*/
                if(SMB_FL_Rbuf[OFFSET_SMBR_COUNT] == 4)
                {
                    FLASH_erase(fl_addr);
                    success = TRUE;
                }
                break;  // Sector Erase Operation

            case PROGRAM_OP:
                /*---------------------------------------------------------------------------------*/
                /* Program operation                                                               */
                /*---------------------------------------------------------------------------------*/
                if((SMB_FL_Rbuf[OFFSET_SMBR_COUNT] >= 1) &&
                   (SMB_FL_Rbuf[OFFSET_SMBR_COUNT] <= 32))
                {
                    FLASH_write_bytes_ex(FLASH_UPDATE_address, (void *)&SMB_FL_Rbuf[OFFSET_SMBR_DATA], SMB_FL_Rbuf[OFFSET_SMBR_COUNT]);
                    FLASH_UPDATE_address += SMB_FL_Rbuf[OFFSET_SMBR_COUNT];
                    success = TRUE;
                }
                break;

            case EXIT_OP:
                /*---------------------------------------------------------------------------------*/
                /* Exit Flash Update Indication                                                    */
                /*---------------------------------------------------------------------------------*/
                FLASH_UPDATE_active = FALSE;
                //FLASH_complete_burst_write(); /* Complete  burst write */
                success = TRUE;
                break;

            case RESET_EC_OP:
            //case GOTO_BOOT_BLOCK_OP:    /* [MCHT 2012/05/07] */
                /*---------------------------------------------------------------------------------*/
                /* Exit Flash Update Indication                                                    */
                /*---------------------------------------------------------------------------------*/
                //FLASH_complete_burst_write(); /* Complete  burst write */
                WDSDM = 0x87;
                WDSDM = 0x61;
                WDSDM = 0x63;
                SET_FIELD(TWCFG,WDRST_MODE, 2 /*COLD_RESET_WITH_VCC_POR*/);
                WDCNT = 1;
                while(1);
                break;

            case GOTO_BOOT_BLOCK_OP:
                /*---------------------------------------------------------------------------------*/
                /* Go to Boot Block Indication                                                     */
                /*---------------------------------------------------------------------------------*/
                //FLASH_complete_burst_write(); /* Complete  burst write */
                // Exit cleanly - although world will start over
                FLASH_UPDATE_active = FALSE;
                //FLASH_jbb = 0xAA55;

                 __asm__("cinv [i]");                          // invalidates the instruction cash
                ((BOOT_BLOCK_func_ptr) &BOOT_BLOCK_ADDRESS)();   // Jump to Boot Block in ROM
                break;
        }  // switch operation type


        /*--------------------------------------------------------------------------------------------*/
        /* Mark operation completed and error if happened                                             */
        /*--------------------------------------------------------------------------------------------*/

        SMB_Flupdate_Sts = SMBFL_STATUS_OK;

        if (!success) SMB_Flupdate_Sts = SMBFL_STATUS_ERROR;

        //FL_Event_Active;
        // re-enable the salve address
        SMB_ADDR2  |= SMBADDR_SAEN;  /* [MCHT 2012/07/16] */ //SMB_ADDR  |= SMBADDR_SAEN;
        SMB_ST = SMBST_NMATCH | SMBST_BER | SMBST_SLVSTP | SMBST_NEGACK;
    }
}

#endif //SMB_FLUPDATE_SUPPORTED

