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
#include "types.h"
#include "com_defs.h"
#include "fu_utils.h"
#include "swtchs.h"
#include "regs.h"
#include "ecdthif.h"
#include "fu.h"
#include "shm.h"
#include "puracpi.h"
#include "purdat.h"


/*------------------------------------------------------------------------------
 * WRITE COMMAND BUFFER (WCB) Interface Definitions
 *----------------------------------------------------------------------------*/
#define WCB_SIZE                 16

/* Command buffer offsets */
#define ECDT_HIF_OFFSET_SEMAPHORE    0
#define ECDT_HIF_OFFSET_OPERATION    3
#define ECDT_HIF_OFFSET_DATA         5
#define ECDT_HIF_OFFSET_ADDRESS      5


/* Op codes */
#define ECDT_HIF_MAJOR_CMD_CODE    0xE0
#define ECDT_HIF_ADDR_SET_OP       0x00
#define ECDT_HIF_WRITE_MEM_OP      0x10
#define ECDT_HIF_READ_MEM_OP       0x20
#define ECDT_HIF_WRITE_EC_SPACE_OP 0x30
#define ECDT_HIF_READ_EC_SPACE_OP  0x40

#define LEGACY_WRITE_EC_SPACE_OP   0x91

#define OPERATION_TYPE_MASK     0xF0
#define WCB_MAX_DATA_SIZE       (WCB_SIZE - ECDT_HIF_OFFSET_DATA)


/*------------------------------------------------------------------------------
 * Global declarations
 *----------------------------------------------------------------------------*/
/* Holds the address referred by the ECDT HIF commands */
DWORD ECDT_HIF_Address;

#if !RAM_BASED_FLASH_UPDATE
extern volatile BYTE write_command_buffer[WCB_SIZE];
#endif // RAM_BASED_FLASH_UPDATE

#ifdef ECDT_DEBUG
extern BYTE TestECSpace[256];
#endif // ECDT_DEBUG

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
bool ECDT_SHM_HIF_handle(void)
{
    void   *data;
    bool    success = FALSE;
    BYTE  operation, DataSize;
    int    index, i;
    int PageSize; // default value for 256 page size

    if (!(SMC_STS & HSEM2W)) {
        return;
    }


    if ((write_command_buffer[ECDT_HIF_OFFSET_OPERATION] != ECDT_HIF_MAJOR_CMD_CODE) &&
         ((write_command_buffer[ECDT_HIF_OFFSET_OPERATION] & ~0x01) != 0x38)) {
        return FALSE;
    }

    SHM_STALL;

    operation = write_command_buffer[ECDT_HIF_OFFSET_OPERATION + 1];

    SHM_CLEAR_STATUS(HSEM2W);
    /*---------------------------------------------------------------------------------*/
    /* Handle end of protocol even if not activated - for post-Exit semaphore write    */
    /*---------------------------------------------------------------------------------*/
    if (!IS_BIT_SET(SHM_GET_SEM2, FU_SEM_EXE) && IS_BIT_SET(SHM_GET_SEM2, FU_SEM_RDY))
    {
        SHM_ENABLE; // release HOST before RDY clear

        /*---------------------------------------------------------------------------------*/
        /* End of shared memory access protocol (EXE bit cleared by host) - clear RDY bit  */
        /*---------------------------------------------------------------------------------*/
        SHM_CLEAR_SEM2(FU_SEM_ERR | FU_SEM_RDY);
        success = TRUE;
    }

    /*---------------------------------------------------------------------------------*/
    /* Semaphore of Flash Update window was written                                    */
    /*---------------------------------------------------------------------------------*/
    else if (IS_BIT_SET(SHM_GET_SEM2, FU_SEM_EXE))
    {
        if (write_command_buffer[ECDT_HIF_OFFSET_OPERATION] == 0x38)
        {
            SHM_RC_STATUS.byte = 0x00;
            if(write_command_buffer[4] == 1) SHM_RC_STATUS.bit.DIA_MODE_ACTIVE = 1;
            else SHM_RC_STATUS.bit.DIA_MODE_ACTIVE = 0;
            success = TRUE;
        }
        if (write_command_buffer[ECDT_HIF_OFFSET_OPERATION] == 0x39)
        {
            write_command_buffer[4] = SHM_RC_STATUS.bit.NEW_KEY_PRESSED;
            write_command_buffer[5] = SHM_RC_VALUE;
            SHM_RC_STATUS.bit.NEW_KEY_PRESSED = 0;
            success = TRUE;
        }
        else
        {
            DataSize = operation & 0x0F;
            /*---------------------------------------------------------------------------------*/
            /* According to protocol limiation - max data size is 12                           */
            /*---------------------------------------------------------------------------------*/
            if (DataSize <= WCB_MAX_DATA_SIZE)
            {
                switch(operation & OPERATION_TYPE_MASK)
                {
                    case ECDT_HIF_ADDR_SET_OP:
                        /*---------------------------------------------------------------------------------*/
                        /* Sets the adress for the next commands                                           */
                        /*---------------------------------------------------------------------------------*/
                        ECDT_HIF_Address = *(DWORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]);
                        success = TRUE;
                        break;
                    case ECDT_HIF_WRITE_MEM_OP:
                        /*---------------------------------------------------------------------------------*/
                        /* Write to memory with the following access rules:                                */
                        /* Size 2: word access                                                             */
                        /* Size 4: double word access                                                      */
                        /* Otherwise: byte access                                                          */
                        /*---------------------------------------------------------------------------------*/
                        switch(DataSize)
                        {
                            case 2:
                                 *(WORD*)ECDT_HIF_Address = *(WORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]);
                                 break;
                            case 4:
                                 *(DWORD*)ECDT_HIF_Address = *(DWORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]);
                                 break;
                            default:
                                 for(i = 0;i < DataSize;i++)
                                 {
                                     *(BYTE*)(ECDT_HIF_Address+i) = *(BYTE*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS+i]);
                                 }
                                 break;
                        }
                        success = TRUE;
                        break;
                    case ECDT_HIF_READ_MEM_OP:
                        /*---------------------------------------------------------------------------------*/
                        /* Read from memory with the following access rules:                               */
                        /* Size 2: word access                                                             */
                        /* Size 4: double word access                                                      */
                        /* Otherwise: byte access                                                          */
                        /*---------------------------------------------------------------------------------*/
                        switch(DataSize)
                        {
                            case 2:
                                 *(WORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]) = *(WORD*)ECDT_HIF_Address;
                                 break;
                            case 4:
                                 *(DWORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]) = *(DWORD*)ECDT_HIF_Address;
                                 break;
                            default:
                                 for(i = 0;i < DataSize;i++)
                                 {
                                     *(BYTE*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS+i]) = *(BYTE*)(ECDT_HIF_Address+i);
                                 }
                                 break;
                        }
                        success = TRUE;
                        break;
                    case ECDT_HIF_WRITE_EC_SPACE_OP:
                        /*---------------------------------------------------------------------------------*/
                        /* Write to EC Space                                                               */
                        /*---------------------------------------------------------------------------------*/
                        for(i = 0;i < DataSize;i++)
                        {
                            #if ACPI_SUPPORTED
                            Write_EC_Space(LEGACY_WRITE_EC_SPACE_OP, write_command_buffer[ECDT_HIF_OFFSET_DATA+i], HIF_PM1);
                            #endif
                            #ifdef ECDT_DEBUG
                            TestECSpace[(BYTE)ECDT_HIF_Address+i] = write_command_buffer[ECDT_HIF_OFFSET_DATA+i];
                            #endif // ECDT_DEBUG
                        }
                        success = TRUE;
                        break;
                    case ECDT_HIF_READ_EC_SPACE_OP:
                        /*---------------------------------------------------------------------------------*/
                        /* Read from EC Space                                                              */
                        /*---------------------------------------------------------------------------------*/
                        for(i = 0;i < DataSize;i++)
                        {
                            #if ACPI_SUPPORTED
                            *(BYTE*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS+i]) = (BYTE)Read_EC_Space((BYTE)ECDT_HIF_Address+i);
                            #endif
                            #ifdef ECDT_DEBUG
                            *(BYTE*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS+i]) = *(BYTE*)(&TestECSpace[(BYTE)ECDT_HIF_Address+i]);
                            #endif // ECDT_DEBUG
                        }
                        success = TRUE;
                        break;
                     default:
                        #ifdef ECDT_DEBUG
                        *(DWORD*)(&write_command_buffer[ECDT_HIF_OFFSET_ADDRESS]) = 0x11223344;
                        #endif // ECDT_DEBUG
                        success = FALSE;
                        break;
                }
            }
        }


        SHM_ENABLE; // release HOST before RDY set

        if (!success)
        {
            SHM_SET_SEM2(FU_SEM_RDY | FU_SEM_ERR);
        }
        else
        {
            ECDT_HIF_Address += DataSize;
            SHM_SET_SEM2(FU_SEM_RDY);
        }
    } // EXE bit set

    SHM_ENABLE;

    return TRUE; //this is a ECDT command.

}





