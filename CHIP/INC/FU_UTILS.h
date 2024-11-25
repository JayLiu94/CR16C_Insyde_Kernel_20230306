/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents: 
 *     fu_utils.h - Flash Device Access for BIOS update
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef FU_UTILS_H
#define FU_UTILS_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

#define DEV_SIZE_UNIT                   0x20000 // 128 Kb

#define CREATE_MASK(bit_position)      (0x1 << (bit_position))

/*------------------------------------------------------------------------------
 * Custom Definitions
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/
#if RAM_BASED_FLASH_UPDATE

void FLASH_erase(DWORD address);
void FLASH_write_bytes(DWORD dest,
                       BYTE* source,
                       BYTE num_bytes);

bool FLASH_execute_command(BYTE   command_code,
						   bool    has_addr,
                           DWORD  addr,
                           BYTE   num_bytes,
						   bool    write,
						   BYTE * data);
void FLASH_read_ids(BYTE * read_ids);
void FLASH_write_status_reg(BYTE status_data);
extern void FLASH_initiate_burst_write(DWORD dest);
extern void FLASH_complete_burst_write(void);
extern void FLASH_burst_write(BYTE *source, BYTE length);

#endif //RAM_BASED_FLASH_UPDATE

#endif  // FU_UTILS_H

