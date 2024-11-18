/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents: 
 *     fu_utils.c - Flash Update Utils
 *
 * Project:
 *     Firmware for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/
 
/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "types.h"
#include "com_defs.h"
#include "fu_utils.h"
#include "fu.h"
#include "shm.h"
#include "swtchs.h"
#include "regs.h"

#if RAM_BASED_FLASH_UPDATE

void Execute_SPI_Command(BYTE SPI_CODE, BYTE SPI_CTS){
    UMA_CODE = SPI_CODE;                      // set UMA_CODE
    UMA_CTS  = SPI_CTS;                       // execute UMA flash transaction
    while (IS_BIT_SET(UMA_CTS, EXEC_DONE));   // wait for UMA to complete. 
}

void wait_flash_ready(){
    do
    {
        Execute_SPI_Command(FLASH_commands[read_status_reg], EXEC_DONE | 1);
    } while (UMA_DB0 & 0x1);                       // wait for status not busy
}

void flash_write_enable(void)
{
    Execute_SPI_Command(FLASH_commands[write_enable], EXEC_DONE);
}

void set_address(DWORD dest)
{
    BYTE * addr = (BYTE *)&dest;
    // Write address 
    UMA_AB2 = addr[2]; 
    UMA_AB1 = addr[1]; 
    UMA_AB0 = addr[0]; 
}
/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_erase
 *
 * Description:
 *    Erases a fragment of the flash.
 *
 * Parameters:
 *    address: An address in the fragment to be erased.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_erase(DWORD address)
{
//    SHM_STALL;

    flash_write_enable();
    set_address(address);
    Execute_SPI_Command(FLASH_commands[sector_erase], EXEC_DONE | A_SIZE);
	  wait_flash_ready();

	// Now shared memory interrupts can be allowed
//    SHM_ENABLE;
}


/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_initiate_burst_write
 *
 * Description:
 *    Initiates the flash and chip for burst
 *     write of up to 256 bytes .
 *
 * Parameters:
 *    dest: A start address for writing
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/

void FLASH_initiate_burst_write(DWORD dest)
{
    flash_write_enable();      
    UMA_ECTS = 0x02;     // Chip Select down.
    set_address(dest);
    Execute_SPI_Command(FLASH_commands[flash_program],
                        RD_WR | EXEC_DONE | A_SIZE);
    FLASH_UPDATE_burst_active = TRUE;
}

/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_complete_burst_write
 *
 * Description:
 *    Completes the process of burst write of up to 256 bytes .
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_complete_burst_write(void)
{
    UMA_ECTS = 0x3 ;    // Chip Select up indicating end of transaction
    wait_flash_ready();
    FLASH_UPDATE_burst_active = FALSE;
}

/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_burst_write
 *
 * Description:
 *    Performs write of up to 8 bytes within burst
 *    write of up to 256 bytes .
 *
 * Parameters:
 *    source: Pointer to data to write.
 *    length : Length of data 
 *
 * Return value:
 *    None.
 *
 * NOte:
 * Only 1,2,4, and 8 bytes are supported
------------------------------------------------------------------------------*/
void FLASH_burst_write(BYTE *source, BYTE length) 
{
    
    while(length) {
        Execute_SPI_Command(*source, RD_WR | EXEC_DONE);
        source++;
        length--;
    } 
}

/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_write_bytes
 *
 * Description:
 *    Writes specified number of bytes into the flash, taking into account
 *    flash device capabilities.
 *
 * Parameters:
 *    dest:      address of destination to be written
 *    source:    pointer to source data
 *    num_bytes: number of bytes to write
 *
 * Return value:
 *    TRUE: the operation completed successfully
 *    FALSE: otherwise (the operation is illegal, or the write failed)
------------------------------------------------------------------------------*/
void FLASH_write_bytes(DWORD dest,
                       BYTE* source,
                       BYTE num_bytes)
{
    BYTE size = FLASH_commands[program_unit_size];
//    SHM_STALL;
	
    while (num_bytes)
    {
        FLASH_initiate_burst_write(dest);
        FLASH_burst_write(source, size);
    		FLASH_complete_burst_write();
    
    		num_bytes -= size;
    		source += size;
    		dest += size;
    }  // while bytes left to program
    
//    SHM_ENABLE;
}

/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_read_ids
 *
 * Description:
 *    Read Manuafacturer and Devices IDs from the flash device.
 *
 * Parameters:
 *    read_ids: Destination where read ids should be written to.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_read_ids(BYTE * read_ids)
{
//	  SHM_STALL;
    
    set_address(0);
    
    if (FLASH_commands[read_dev_id_type] == READ_DEV_ID_WITHOUT_DUMMY)
    {
        // read Device ID without dummy bytes
        Execute_SPI_Command(FLASH_commands[read_device_id], EXEC_DONE | 4 );
    }
    else
    {
        Execute_SPI_Command(FLASH_commands[read_device_id], EXEC_DONE | 4 | A_SIZE);
    }
    
	// Get read transaction results
    *read_ids = UMA_DB0;
    *(read_ids + 1) = UMA_DB1;
    *(read_ids + 2) = UMA_DB2;
    *(read_ids + 3) = UMA_DB3;
    
//    SHM_ENABLE;
}

/*------------------------------------------------------------------------------
 * Function: 
 *    FLASH_write_status_reg
 *
 * Description:
 *    Write specified data to the flash device's status register.
 *
 * Parameters:
 *    status_data: Value to be written to the status register.
 *
 * Return value:
 *    None.
------------------------------------------------------------------------------*/
void FLASH_write_status_reg(BYTE status_data)
{
//    SHM_STALL;

    // Enable write to Status Register
    Execute_SPI_Command(FLASH_commands[write_status_enable], EXEC_DONE);

    // Write to the Status Register
  	UMA_DB0 = status_data;
    Execute_SPI_Command(FLASH_commands[write_status_reg], EXEC_DONE | RD_WR | 1);
	  wait_flash_ready();

//    SHM_ENABLE;
}

#endif // RAM_BASED_FLASH_UPDATE

