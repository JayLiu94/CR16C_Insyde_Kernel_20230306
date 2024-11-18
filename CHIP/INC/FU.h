/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     fu.h - Flash Update definitions
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef FU_H
#define FU_H

#include "swtchs.h"

#if RAM_BASED_FLASH_UPDATE
/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * System-dependent configurable values
 *----------------------------------------------------------------------------*/
/* Address in host address space to which flash is mapped */
#define PC_FLASH_ADDRESS_START 0xFFF00000

/* Fixed Address for return to Boot Block after Flash Update */
#define BOOT_BLOCK_ADDRESS 0x21014

/* SPI_FL_CFG register bits fields */
#define DEV_SIZE_P          0x0
#define DEV_SIZE_S          0x6
#define F_READ              0x6
#define FL_DEV              0x7



/*------------------------------------------------------------------------------
 * WRITE COMMAND BUFFER (WCB) Interface Definitions
 *----------------------------------------------------------------------------*/
#define WCB_SIZE                 16

/* Command buffer offsets */
#define OFFSET_SEMAPHORE         0
#define OFFSET_OPERATION         3
#define OFFSET_ADDRESS           4
#define OFFSET_DATA              4    // Commands have either address or data
#define OFFSET_DATA_GENERIC      8    // Generic commands have both address and data
#define OFFSET_GENERIC_CODE      12
#define OFFSET_GENERIC_HAS_ADDR  13
#define OFFSET_GENERIC_NUM_BYTES 14
#define OFFSET_GENERIC_WRITE     15

// bits definitions for the above bit mask for SPI functions hooks
#define wait_flash_ready_bit            0x0001 // bit 0
#define flash_write_enable_bit          0x0002 // bit 1
#define set_address_bit                 0x0004 // bit 2
#define can_write_flash_addr_bit        0x0008 // bit 3
#define FLASH_erase_bit                 0x0010 // bit 4
#define FLASH_initiate_burst_write_bit  0x0020 // bit 5
#define FLASH_complete_burst_write_bit  0x0040 // bit 6
#define FLASH_burst_write_bit           0x0080 // bit 7
#define FLASH_write_bytes_bit           0x0100 // bit 8
#define FLASH_read_ids_bit              0x0200 // bit 9
#define FLASH_write_status_reg_bit      0x0400 // bit 10

#define OEM_SPI_Hooks_Mask              0x0000


/* defined in OEMBLD.MAK */ //#define FLASH_jbb *((volatile WORD  *) 0x00017FDE)
extern volatile WORD FLASH_jbb;
extern volatile BYTE write_command_buffer[16];
extern volatile BYTE FLASH_commands[16];

/* WCB Commands */
#define FLASH_COMMANDS_INIT_OP  0x5A
#define ENTER_OP                0x10
#define EXIT_OP                 0x20
#define RESET_EC_OP             0x21
#define GOTO_BOOT_BLOCK_OP      0x22
#define ERASE_OP                0x80
#define SECTOR_ERASE_OP         0x80
#define ADDRESS_SET_OP          0xA0
#define PROGRAM_OP              0xB0
#define READ_IDS_OP             0xC0
#define GENERIC_OP              0xD0
#define READ_OP                 0xD0 // 0xD1-0xDC


#define OPERATION_TYPE_MASK     0xF0
#define OPERATION_SIZE_MASK     0x0F

// FLASH_commands buffer definitions.
#define read_device_id        0
#define write_status_enable   1
#define write_enable          2
#define read_status_reg       3
#define write_status_reg      4
#define flash_program         5
#define sector_erase          6
#define status_busy_mask      7
#define status_reg_val        8
#define program_unit_size     9
#define page_size             10
#define read_dev_id_type      11
#define last_command          11

// read_dev_id_type commannd
#define READ_DEV_ID_WITHOUT_DUMMY  0x47

/*------------------------------------------------------------------------------
 * Global variables
 *----------------------------------------------------------------------------*/

BYTE FLASH_UPDATE_active;
bool FLASH_UPDATE_burst_active;
DWORD FLASH_device_size;
DWORD FLASH_UPDATE_address;
DWORD FLASH_UPDATE_erase_address;
typedef void (* BOOT_BLOCK_func_ptr)(void);
BOOT_BLOCK_func_ptr BOOT_BLOCK_routine;

extern volatile BYTE write_command_buffer[WCB_SIZE];
extern volatile BYTE FLASH_commands[WCB_SIZE];
extern volatile WORD FLASH_jbb;

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/

#else // ROM BASED FLASH UPDATE

/*------------------------------------------------------------------------------
 * System-dependent values
 *----------------------------------------------------------------------------*/
#define FU_init_ADDR                    (*(volatile unsigned long *)0x20)   // FLASH_UPDATE_init ROM address
#define FU_handle_ADDR                  (*(volatile unsigned long *)0x24)   // FLASH_UPDATE_handle ROM address
#define wait_flash_ready_ADDR           (*(volatile unsigned long *)0x28)   // wait_flash_ready ROM address
#define FU_handle_command_ADDR          (*(volatile unsigned long *)0x2C)   // FLASH_UPDATE_handle_command ROM address
#define Execute_SPI_Command_ADDR        (*(volatile unsigned long *)0x30)   // Execute_SPI_Command ROM address
#define FL_write_status_reg_ADDR        (*(volatile unsigned long *)0x34)   // FLASH_write_status_reg ROM address
#define FL_read_ids_ADDR                (*(volatile unsigned long *)0x38)   // FLASH_read_ids ROM address
#define FU_checksum_ADDR                (*(volatile unsigned long *)0x3C)   // FLASH_UPDATE_checksum ROM address
#define FL_complete_burst_write_ADDR    (*(volatile unsigned long *)0x40)   // FLASH_complete_burst_write ROM address
#define FU_read_ADDR                    (*(volatile unsigned long *)0x44)   // FLASH_UPDATE_read ROM address
#define FL_erase_ADDR                   (*(volatile unsigned long *)0x48)   // FLASH_erase ROM address
#define FL_initiate_burst_write_ADDR    (*(volatile unsigned long *)0x4C)   // FLASH_initiate_burst_write ROM address
#define FL_burst_write_ADDR             (*(volatile unsigned long *)0x50)   // FLASH_burst_write ROM address
#define FL_power_down_ADDR              (*(volatile unsigned long *)0x54)   // FLASH_power_down ROM address
#define FL_write_bytes_ADDR             (*(volatile unsigned long *)0x58)   // FLASH_write_bytes ROM address
#define INIT_oem_spi_hooks_ADDR         (*(volatile unsigned long *)0x5C)   // INIT_oem_spi_hooks ROM address
#define FU_set_write_win_ADDR           (*(volatile unsigned long *)0x60)   // FLASH_UPDATE_set_write_win ROM address
#define FL_write_bytes_ex_ADDR          (*(volatile unsigned long *)0x64)   // FLASH_write_bytes_ex ROM address


#define FU_write_bytes        (*(volatile unsigned long *)0x58)     // FLASH_write_bytes ROM address

extern  DWORD BOOT_BLOCK_ADDRESS;            // Fixed Address for return to Boot Block after Flash Update


/*------------------------------------------------------------------------------
 * Type definitions
 *----------------------------------------------------------------------------*/

typedef BYTE (* HOOK_OP_func_ptr)(BYTE, BYTE);
typedef BYTE (* HOOK_SPI_OP_func_ptr)(WORD, DWORD, DWORD, BYTE);

typedef void (*FU_init_func_ptr)(DWORD address, HOOK_OP_func_ptr func);
typedef void (*FU_handle_func_ptr)(void);
typedef void (*wait_flash_ready_ptr)(void);
typedef void (*FU_handle_cmd_ptr)(void);
typedef void (*Execute_SPI_cmd_ptr)(BYTE SPI_CODE, BYTE SPI_CTS);
typedef void (*FL_write_status_reg_ptr)(BYTE  status_data);
typedef void (*FL_read_ids_ptr)(BYTE * read_ids);
typedef void (*FU_checksum_ptr)(DWORD addr, DWORD size, WORD *res);
typedef void (*FL_complete_burst_write_ptr)(void);
typedef void (*FU_read_ptr)(BYTE size, BYTE * data);
typedef void (*FL_erase_ptr)(DWORD address);
typedef void (*FL_initiate_burst_write_ptr)(DWORD dest);
typedef void (*FL_burst_write_ptr)(BYTE *source, BYTE length);
typedef void (*FL_power_down_ptr)(BYTE enter, WORD exit, WORD delay);
typedef void (*FL_write_bytes_ptr)(DWORD dest, BYTE* source, BYTE num_bytes);
typedef void (*INIT_oem_spi_hooks_ptr)(HOOK_SPI_OP_func_ptr func, WORD mask);
typedef void (*FU_set_write_win_ptr)(DWORD w_start, DWORD w_size);
typedef void (*FL_write_bytes_ex_ptr)(DWORD dest, BYTE* source, WORD num_bytes);

#if SMB_FLUPDATE_SUPPORTED
#define OFFSET_SMBR_OPERATION       0
#define OFFSET_SMBR_COUNT           1
#define OFFSET_SMBR_ADDRESS         2
#define OFFSET_SMBR_DATA            2
#define OFFSET_SMBR_READ_SIZE       6

#define OFFSET_SMBW_STATUS          0
#define OFFSET_SMBW_COUNT           0
#define OFFSET_SMBW_DATA            1


// bits definitions for the above bit mask for SPI functions hooks
#define wait_flash_ready_bit            0x0001 // bit 0
#define flash_write_enable_bit          0x0002 // bit 1
#define set_address_bit                 0x0004 // bit 2
#define can_write_flash_addr_bit        0x0008 // bit 3
#define FLASH_erase_bit                 0x0010 // bit 4
#define FLASH_initiate_burst_write_bit  0x0020 // bit 5
#define FLASH_complete_burst_write_bit  0x0040 // bit 6
#define FLASH_burst_write_bit           0x0080 // bit 7
#define FLASH_write_bytes_bit           0x0100 // bit 8
#define FLASH_read_ids_bit              0x0200 // bit 9
#define FLASH_write_status_reg_bit      0x0400 // bit 10

#define OEM_SPI_Hooks_Mask              0x0000

#define OEM_HOOK_OP_func              *((volatile DWORD *) 0x00017fb4)
#define FLASH_device_size             *((volatile DWORD *) 0x00017fc0)
#define FLASH_UPDATE_address          *((volatile DWORD *) 0x00017fc4)
#define OEM_HOOK_SPI_OP_mask          *((volatile WORD  *) 0x00017fcc)
#define OEM_HOOK_SPI_OP_func          *((volatile DWORD *) 0x00017fc8)
#define FLASH_UPDATE_active           *((volatile BYTE  *) 0x00017fce)
#define FLASH_address_is_protected    *((volatile BYTE  *) 0x00017fcf)
#define FLASH_UPDATE_burst_active     *((volatile BYTE  *) 0x00017fd0)
#define FLASH_UPDATE_write_win_start  *((volatile DWORD *) 0x00017fb8)
#define FLASH_UPDATE_write_win_end    *((volatile DWORD *) 0x00017fbc)

/* defined in OEMBLD.MAK */ //#define FLASH_jbb *((volatile WORD  *) 0x00017FDE)
//extern volatile WORD FLASH_jbb;
extern volatile BYTE write_command_buffer[16];
extern volatile BYTE FLASH_commands[16];
extern BYTE BOOT_BLOCK_ADDRESS;


/*------------------------------------------------------------------------------
 * Function/Macro definitions
 *----------------------------------------------------------------------------*/
/* WCB Commands */
#define FLASH_COMMANDS_INIT_OP  0x5A
#define ENTER_OP                0x10
#define EXIT_OP                 0x20
#define RESET_EC_OP             0x21
#define GOTO_BOOT_BLOCK_OP      0x22
#define ERASE_OP                0x80
#define SECTOR_ERASE_OP         0x80
#define ADDRESS_SET_OP          0xA0
#define PROGRAM_OP              0xB0
#define READ_IDS_OP             0xC0
#define SET_WRITE_WIN_OP        0xC5
#define CHECKSUM_OP             0xC6
#define GET_CHECKSUM_OP         0xC7
#define READ_OP                 0xD0 // 0xD1-0xDC

// FLASH_commands buffer definitions.
#define read_device_id        0
#define write_status_enable   1
#define write_enable          2
#define read_status_reg       3
#define write_status_reg      4
#define flash_program         5
#define sector_erase          6
#define status_busy_mask      7
#define status_reg_val        8
#define program_unit_size     9
#define page_size             10
#define read_dev_id_type      11
#define last_command          11

#endif // SMB_FLUPDATE_SUPPORTED

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/




#define FLASH_UPDATE_init(addr, func)                   ((FU_init_func_ptr) FU_init_ADDR)(addr, func)
#define FLASH_UPDATE_handle()                           ((FU_handle_func_ptr) FU_handle_ADDR)()
#define wait_flash_ready()                              ((wait_flash_ready_ptr) wait_flash_ready_ADDR)()
#define FLASH_UPDATE_handle_command()                   ((FU_handle_cmd_ptr) FU_handle_command_ADDR)()
#define Execute_SPI_Command(SPI_CODE, SPI_CTS)          ((Execute_SPI_cmd_ptr) Execute_SPI_Command_ADDR)(SPI_CODE, SPI_CTS)
#define FLASH_write_status_reg(status_data)             ((FL_write_status_reg_ptr) FL_write_status_reg_ADDR)(status_data)
#define FLASH_read_ids(read_ids)                        ((FL_read_ids_ptr) FL_read_ids_ADDR)(read_ids)
#define FLASH_UPDATE_checksum(addr, size, res)          ((FU_checksum_ptr) FU_checksum_ADDR)(addr, size, res)
#define FLASH_complete_burst_write()                    ((FL_complete_burst_write_ptr) FL_complete_burst_write_ADDR)()
#define FLASH_UPDATE_read(size, data)                   ((FU_read_ptr) FU_read_ADDR)(size, data)
#define FLASH_erase(address)                            ((FL_erase_ptr) FL_erase_ADDR)(address)
#define FLASH_initiate_burst_write(dest)                ((FL_initiate_burst_write_ptr) FL_initiate_burst_write_ADDR)(dest)
#define FLASH_burst_write(source, length)               ((FL_burst_write_ptr) FL_burst_write_ADDR)(source, length)
#define FLASH_power_down(enter, exit, delay)            ((FL_power_down_ptr) FL_power_down_ADDR)(enter, exit, delay)
#define FLASH_write_bytes(dest, source, num_bytes)      ((FL_write_bytes_ptr) FL_write_bytes_ADDR)(dest, source, num_bytes)
#define INIT_oem_spi_hooks(func, mask)                  ((INIT_oem_spi_hooks_ptr)INIT_oem_spi_hooks_ADDR)(func, mask)
#define FLASH_UPDATE_set_write_win(w_start, w_size)     ((FU_set_write_win_ptr) FU_set_write_win_ADDR)(w_start, w_size)
#define FLASH_write_bytes_ex(dest, source, num_bytes)   ((FL_write_bytes_ex_ptr) FL_write_bytes_ex_ADDR)(dest, source, num_bytes)


#endif // RAM/ROM BASED_FLASH_UPDATE
#endif // FU_H

