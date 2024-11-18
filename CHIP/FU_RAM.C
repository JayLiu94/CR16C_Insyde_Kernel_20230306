/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     fu_ram.c - Flash Update runtime implementation
 *
 * Project:
 *     Sample Fimrware for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "types.h"
#include "com_defs.h"
#include "fu.h"
#include "fu_utils.h"
#include "shm.h"
#include "timers.h"
#include "swtchs.h"
#include "regs.h"

#if RAM_BASED_FLASH_UPDATE

#undef USE_SHM_INTERRUPT

extern volatile BYTE FLASH_commands[WCB_SIZE];
extern volatile BYTE write_command_buffer[WCB_SIZE];
void FLASH_UPDATE_handle_command(void);

extern BYTE Crisis_Flags;

/*------------------------------------------------------------------------------
 * Function:
 *    FLASH_UPDATE_callback
 *
 * Description:
 *    Flash Update callback - called from Shared Memory Interrupt handler upon
 *    every shared memory interrupt.
 *
 * Parameters:
 *    smc_sts   Current value of Shared Memory Core Status register. See
 *              datasheet for description of SMC_STS register.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
#ifdef USE_SHM_INTERRUPT
void _FLASH_UPDATE_callback(BYTE smc_sts)
{

    if (!IS_BIT_SET(smc_sts, HSEM2W)) {
        return;
    }

    FLASH_UPDATE_handle_command();


}
#else
void _FLASH_UPDATE_handle(void)
{
    unsigned int tpsr = 0;

    if (IS_BIT_CLEAR(SMC_STS, HSEM2W)) {
        return;
    }

    // save psr and disable interrupts
    _spr_("psr", tpsr);
    _di_();


    do
    {
        while(IS_BIT_CLEAR(SMC_STS, HSEM2W)); // wait for command.
        FLASH_UPDATE_handle_command();
    } while (FLASH_UPDATE_active);

    // restore psr
    _lpr_("psr",tpsr);

}
#endif //USE_SHM_INTERRUPT

/*------------------------------------------------------------------------------
 * Function:
 *    FLASH_UPDATE_read
 *
 * Description:
 *
 * Parameters:
 *    start
 *    size
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_UPDATE_read(BYTE size, BYTE *data)
{
    while(size)
    {
        //*data = read_byte();
        *data = *(BYTE *)(FLASH_UPDATE_address+0x20000);
        FLASH_UPDATE_address++;
        size--;
        data++;
    }
}



void FLASH_UPDATE_handle_command(void)
{
    void   *data;
    bool    success = FALSE;
    BYTE  operation;
    int    index;
    int PageSize; // default value for 256 page size

    SHM_STALL;

    operation = write_command_buffer[OFFSET_OPERATION];

    SHM_CLEAR_STATUS(HSEM2W);
    /*---------------------------------------------------------------------------------*/
    /* Handle end of protocol even if not activated - for post-Exit semaphore write    */
    /*---------------------------------------------------------------------------------*/
    if (IS_BIT_CLEAR(SHM_GET_SEM2, FU_SEM_EXE) && IS_BIT_SET(SHM_GET_SEM2, FU_SEM_RDY))
    {
        /*---------------------------------------------------------------------------------*/
        /* End of shared memory access protocol (EXE bit cleared by host) - clear RDY bit  */
        /*---------------------------------------------------------------------------------*/
        SHM_CLEAR_SEM2(FU_SEM_ERR | FU_SEM_RDY);
    }

    /*---------------------------------------------------------------------------------*/
    /* Semaphore of Flash Update window was written                                    */
    /*---------------------------------------------------------------------------------*/
    else if (IS_BIT_SET(SHM_GET_SEM2, FU_SEM_EXE))
    {
        /*---------------------------------------------------------------------------------*/
        /* Check whether Flash Update Mode is being initiated                                    */
        /*---------------------------------------------------------------------------------*/
        if ((operation == ENTER_OP) &&
            (*(DWORD *)&write_command_buffer[OFFSET_OPERATION + 1] == 0xBECDAA55))
            {
                /*-----------------------------------------------------------------------------*/
                /* A valid Enter command - prepare to run Flash update                         */
                /*-----------------------------------------------------------------------------*/
                FLASH_UPDATE_active = TRUE;
                FLASH_UPDATE_burst_active = FALSE;
                FLASH_write_status_reg(FLASH_commands[status_reg_val]);
                /* Indicate end of operation */
                success = TRUE;
            } // Enter operation command
            else if (operation == FLASH_COMMANDS_INIT_OP)
            {
            for(index = read_device_id; index <= last_command; index++) {
                FLASH_commands[index] = write_command_buffer[index+4];
            }
                success = TRUE; /* Indicate end of operation */
        }
          else if (operation == READ_IDS_OP)
            {
            /*---------------------------------------------------------------------------------*/
            /* Read Manufacturer and device IDs Operation                                      */
            /*---------------------------------------------------------------------------------*/
            FLASH_read_ids((BYTE *)(&write_command_buffer[OFFSET_DATA]));
                success = TRUE;
            } // Read IDs Operation
        else if (operation == ADDRESS_SET_OP)
        {
            /*---------------------------------------------------------------------------------*/
            /* Set Addreses operation                                        FFF00000                       */
            /*---------------------------------------------------------------------------------*/
            FLASH_UPDATE_address = *(DWORD*)(&write_command_buffer[OFFSET_ADDRESS]) & (FLASH_device_size -1);
            FLASH_complete_burst_write(); /* complete burst write */
            success = TRUE;
        } // set address
        else if ((operation & 0xF0) == READ_OP)
        {
            if (operation <= 0xDC)
            {
                FLASH_UPDATE_read((operation&0x0F), (BYTE *)(&write_command_buffer[OFFSET_DATA]));
                success = TRUE;
            }
        } // read operation
        else if (FLASH_UPDATE_active)
          {
            /*---------------------------------------------------------------------------------*/
            /* Flash Update in progress. Host issued request - retrieve the request contents                             */
            /*---------------------------------------------------------------------------------*/
            switch (operation)
            {
                case SECTOR_ERASE_OP:
                    /*--------------------------------------------------------------------------*/
                    /* Sector Erase                                                             */
                    /*--------------------------------------------------------------------------*/
                    FLASH_UPDATE_erase_address = *(DWORD*)(&write_command_buffer[OFFSET_ADDRESS]) & (FLASH_device_size -1);
                    FLASH_erase(FLASH_UPDATE_erase_address);
                    success = TRUE;
                    break;  // Sector Erase Operation

                case PROGRAM_OP | 0x1:
                    case PROGRAM_OP | 0x2:
                    case PROGRAM_OP | 0x3:
                    case PROGRAM_OP | 0x4:
                    case PROGRAM_OP | 0x5:
                    case PROGRAM_OP | 0x6:
                    case PROGRAM_OP | 0x7:
                    case PROGRAM_OP | 0x8:
                    /*---------------------------------------------------------------------------------*/
                    /* Program operation                                                               */
                    /*---------------------------------------------------------------------------------*/
                    data = (void *)&write_command_buffer[OFFSET_DATA];
                    if (FLASH_commands[program_unit_size] == 0xFF) { // 256 burst write
                        PageSize = (1 << FLASH_commands[page_size]) - 1;
                        if (FLASH_UPDATE_burst_active == FALSE)
                        {
                            FLASH_initiate_burst_write(FLASH_UPDATE_address); /* Initiate burst write */
                        }
                        FLASH_burst_write(data, (operation & OPERATION_SIZE_MASK));
                        FLASH_UPDATE_address += operation & OPERATION_SIZE_MASK;

                        if ((FLASH_UPDATE_address & PageSize) == 0)
                        {
                            FLASH_complete_burst_write(); /* complete burst write */
                        }
                    } else {
                        FLASH_write_bytes(FLASH_UPDATE_address, data, (operation & OPERATION_SIZE_MASK));
                        FLASH_UPDATE_address += (operation & OPERATION_SIZE_MASK);
                    }
                              success = TRUE;
                            break;

                case EXIT_OP:
                    /*---------------------------------------------------------------------------------*/
                    /* Exit Flash Update Indication                                                    */
                    /*---------------------------------------------------------------------------------*/
                    FLASH_UPDATE_active = FALSE;
                    FLASH_complete_burst_write(); /* Complete  burst write */
                    success = TRUE;
                    break;

                case RESET_EC_OP:
                    /*---------------------------------------------------------------------------------*/
                    /* Exit Flash Update Indication                                                    */
                    /*---------------------------------------------------------------------------------*/
                    FLASH_complete_burst_write(); /* Complete  burst write */
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
                    FLASH_complete_burst_write(); /* Complete  burst write */
                            // Exit cleanly - although world will start over
                    SHM_ENABLE;
                    FLASH_UPDATE_active = FALSE;
                    FLASH_jbb = 0xAA55;
                    SHM_SET_SEM2(FU_SEM_RDY);
                    while(!IS_BIT_SET(SMC_STS, HSEM2W)); // Wait for Host to clear EXE
                    SHM_CLEAR_SEM2(FU_SEM_RDY);
                    SHM_CLEAR_STATUS(HSEM2W);
                    __asm__("cinv[i]");                          // invalidates the instruction cash
                    ((BOOT_BLOCK_func_ptr)BOOT_BLOCK_ADDRESS)(); // Jump to Boot Block in ROM
                    break;
               }  // switch operation type

                /*--------------------------------------------------------------------------------------------*/
                /* Mark operation completed and error if happened                                             */
                /*--------------------------------------------------------------------------------------------*/
            } // Flash Update already activated
        if (!success)
            SHM_SET_SEM2(FU_SEM_RDY | FU_SEM_ERR);
        else
            SHM_SET_SEM2(FU_SEM_RDY);
    } // EXE bit set

  SHM_ENABLE;

}

#endif // RAM_BASED_FLASH_UPDATE

