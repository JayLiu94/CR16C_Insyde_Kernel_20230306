/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   peci_if.h                                                                                             */
/*            This file contains Platform Environment Control Interface (PECI) interface                   */
/*  Project:                                                                                               */
/*            SWC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _PECI_IF_H
#define _PECI_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#include "com_defs.h"
#include "types.h"

/*---------------------------------------------------------------------------------------------------------*/
/*                         Platform Environment Control Interface (PECI) Definitions                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* PECI Command Code                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    PECI_COMMAND_PING               = 0x00,
    PECI_COMMAND_GET_DIB            = 0xF7,
    PECI_COMMAND_GET_TEMP           = 0x01,
    PECI_COMMAND_RD_PKG_CFG         = 0xA1,
    PECI_COMMAND_WR_PKG_CFG         = 0xA5,
    PECI_COMMAND_RD_IAMSR           = 0xB1,
    PECI_COMMAND_RD_PCI_CFG         = 0x61,
    PECI_COMMAND_RD_PCI_CFG_LOCAL   = 0xE1,
    PECI_COMMAND_WR_PCI_CFG_LOCAL   = 0xE5,
    PECI_COMMAND_NONE               = 0xFF
} PECI_COMMAND_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Domain Number                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    PECI_DOMAIN_0   = 0x00,
    PECI_DOMAIN_1   = 0x01
} PECI_DOMAIN_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Data Size                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    PECI_DATA_SIZE_NONE     = 0x00,
    PECI_DATA_SIZE_BYTE     = 0x01,
    PECI_DATA_SIZE_WORD     = 0x02,
    PECI_DATA_SIZE_DWORD    = 0x04,
    PECI_DATA_SIZE_QWORD    = 0x08
} PECI_DATA_SIZE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Hardware Completion Code                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    PECI_CC_NONE                                                        = 0x00,
    PECI_CC_COMMAND_PASSED_DATA_VALID                                   = 0x40,
    PECI_CC_RESPONSE_TIMEOUT_PROCESSOR_NOT_ABLE_TO_GENERATE_RESPONSE    = 0x80,
    PECI_CC_RESPONSE_TIMEOUT_PROCESSOR_NOT_ABLE_TO_ALLOCATE_RESOURCES   = 0x81,
    PECI_CC_RESURCES_IN_LOW_POWER_MODE                                  = 0x82,
    PECI_CC_UNKNOWN_INVALID_ILLEGAL_REQUEST                             = 0x90,
    PECI_CC_HW_FW_ASSOC_LOGIC_ERROR                                     = 0x91
} PECI_CC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Transaction Completion Code                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    PECI_TRANS_DONE_OK          = 0x02,     /* DO NOT CHANGE ENUM NUMBERS */
    PECI_TRANS_DONE_AVL_ERR     = 0x04,     /* DO NOT CHANGE ENUM NUMBERS */
    PECI_TRANS_DONE_CRC_ERR     = 0x08,     /* DO NOT CHANGE ENUM NUMBERS */
    PECI_TRANS_DONE_ABORT_ERR   = 0x10,     /* DO NOT CHANGE ENUM NUMBERS */
    PECI_TRANS_DONE_ABORT_CTN   = 0x20,     /* DO NOT CHANGE ENUM NUMBERS */
    PECI_TRANS_DONE_CC_ERROR    = 0x40      /* DO NOT CHANGE ENUM NUMBERS */
} PECI_TRANS_DONE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Callback function                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*PECI_CALLBACK_T)(PECI_COMMAND_T command, PECI_DATA_SIZE_T data_size, PECI_TRANS_DONE_T sts,
                                DWORD data_low, DWORD data_high);

/*---------------------------------------------------------------------------------------------------------*/
/* PCI Config Address Fields                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_PCI_CFG_ADDRESS_REGISTER_FIELD         0, 12
#define PECI_PCI_CFG_ADDRESS_FUNCTION_FIELD         12, 3
#define PECI_PCI_CFG_ADDRESS_DEVICE_FIELD           15, 5
#define PECI_PCI_CFG_ADDRESS_BUS_FIELD              20, 8

/*---------------------------------------------------------------------------------------------------------*/
/* PCI Local Config Address Fields                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_PCI_LOCAL_CFG_ADDRESS_REGISTER_FIELD   PECI_PCI_CFG_ADDRESS_REGISTER_FIELD
#define PECI_PCI_LOCAL_CFG_ADDRESS_FUNCTION_FIELD   PECI_PCI_CFG_ADDRESS_FUNCTION_FIELD
#define PECI_PCI_LOCAL_CFG_ADDRESS_DEVICE_FIELD     PECI_PCI_CFG_ADDRESS_DEVICE_FIELD
#define PECI_PCI_LOCAL_CFG_ADDRESS_BUS_FIELD        20, 4

/*---------------------------------------------------------------------------------------------------------*/
/* Build PCI Config Address                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_PCI_ADDRESS(register, function, device, bus)             \
    (BUILD_FIELD_VAL(PECI_PCI_CFG_ADDRESS_REGISTER_FIELD, register) | \
     BUILD_FIELD_VAL(PECI_PCI_CFG_ADDRESS_FUNCTION_FIELD, function) | \
     BUILD_FIELD_VAL(PECI_PCI_CFG_ADDRESS_DEVICE_FIELD, device)     | \
     BUILD_FIELD_VAL(PECI_PCI_CFG_ADDRESS_BUS_FIELD, bus))

/*---------------------------------------------------------------------------------------------------------*/
/* Build PCI Local Config Address                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_PCI_LOCAL_ADDRESS(register, function, device, bus)             \
    (BUILD_FIELD_VAL(PECI_PCI_LOCAL_CFG_ADDRESS_REGISTER_FIELD, register) | \
     BUILD_FIELD_VAL(PECI_PCI_LOCAL_CFG_ADDRESS_FUNCTION_FIELD, function) | \
     BUILD_FIELD_VAL(PECI_PCI_LOCAL_CFG_ADDRESS_DEVICE_FIELD, device)     | \
     BUILD_FIELD_VAL(PECI_PCI_LOCAL_CFG_ADDRESS_BUS_FIELD, bus))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Init                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback - Application callback to be called from PECI interrupt handler.              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes the PECI module.                                              */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Init (PECI_CALLBACK_T callback, DWORD peci_freq);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_SetAddress                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  client_address - PECI Client Address.                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the value of the address frame of the next PECI transaction.         */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_SetAddress (BYTE client_address);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_SetDomain                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  domain - PECI domain number.                                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the value of the domain to access in the next PECI transaction.      */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_SetDomain (PECI_DOMAIN_T domain);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_GetComplitionCode                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         PECI Completion Code.                                                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the Completion Code of the last PECI transaction.               */
/*---------------------------------------------------------------------------------------------------------*/
PECI_CC_T PECI_GetCompletionCode (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Ping                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a Ping command.                                                     */
/*                  This command is used to enumerate devices or determine if a device has been removed,   */
/*                  been powered-off, etc'.                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Ping (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_GetDIB                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a GetDIB command.                                                   */
/*                  This command provides information regarding client revision number and the number of   */
/*                  supported domains.                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_GetDIB (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_GetTemp                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a GetTemp command.                                                  */
/*                  This command is used to retrieve the maximum die temperature from a target PECI        */
/*                  address.                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_GetTemp (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_RdPkgConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size - The desired data return size (BYTE/WORD/DWORD).                       */
/*                  host_id        - Host ID.                                                              */
/*                  retry          - TRUE to retry in case of a failure; FALSE otherwise.                  */
/*                  index          - Requested service.                                                    */
/*                  parameter      - Service parameter value.                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPkgConfig command.                                              */
/*                  This command provides read access to the Package Configuration Space (PCS) within the  */
/*                  processor, including various power and thermal management functions.                   */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_RdPkgConfig (PECI_DATA_SIZE_T read_data_size, BYTE host_id, bool retry,
                       BYTE index, WORD parameter);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_WrPkgConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  write_data_size - The desired write granularity (BYTE/WORD/DWORD).                     */
/*                  host_id         - Host ID.                                                             */
/*                  retry           - TRUE to retry in case of a failure; FALSE otherwise.                 */
/*                  index           - Requested service.                                                   */
/*                  parameter       - Service parameter value.                                             */
/*                  data            - Data to write to the processor Package Configuration Space.          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a WrPkgConfig command.                                              */
/*                  This command provides write access to the Package Configuration Space (PCS) within the */
/*                  processor, including various power and thermal management functions.                   */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_WrPkgConfig (PECI_DATA_SIZE_T write_data_size, BYTE host_id, bool retry,
                       BYTE index, WORD parameter, DWORD data);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_RdIAMSR                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size - The desired data return size (BYTE/WORD/DWORD/QWORD).                 */
/*                  host_id        - Host ID.                                                              */
/*                  retry          - TRUE to retry in case of a failure; FALSE otherwise.                  */
/*                  processor_id   - Logical processor ID within the CPU.                                  */
/*                  msr_address    - Model Specific Register address.                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdIAMSR command.                                                  */
/*                  This command provides read access to the Model Specific Registers (MSRs) defined in    */
/*                  processor's Intel Architecture (IA).                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_RdIAMSR (PECI_DATA_SIZE_T read_data_size, BYTE host_id, bool retry,
                   BYTE processor_id, WORD msr_address);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_RdPCIConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size     - The desired data return size (BYTE/WORD/DWORD).                   */
/*                  host_id            - Host ID.                                                          */
/*                  retry              - TRUE to retry in case of a failure; FALSE otherwise.              */
/*                  pci_config_address - PCI configuration address (28-bit).                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPCIConfig command.                                              */
/*                  This command provides sideband read access to the PCI configuration space maintained   */
/*                  in downstream devices external to the processor.                                       */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_RdPCIConfig (PECI_DATA_SIZE_T read_data_size, BYTE host_id, bool retry,
                       DWORD pci_config_address);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_RdPCIConfigLocal                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size     - The desired data return size (BYTE/WORD/DWORD).                   */
/*                  host_id            - Host ID.                                                          */
/*                  retry              - TRUE to retry in case of a failure; FALSE otherwise.              */
/*                  pci_config_address - PCI configuration address for local accesses (24-bit).            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPCIConfigLocal command.                                         */
/*                  This command provides sideband read access to the PCI configuration space that resides */
/*                  within the processor.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_RdPCIConfigLocal (PECI_DATA_SIZE_T read_data_size, BYTE host_id, bool retry,
                            DWORD pci_config_address);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_WrPCIConfigLocal                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  write_data_size    - The desired write granularity (BYTE/WORD/DWORD).                  */
/*                  host_id            - Host ID.                                                          */
/*                  retry              - TRUE to retry in case of a failure; FALSE otherwise.              */
/*                  pci_config_address - PCI configuration address for local accesses (24-bit).            */
/*                  data               - Data to write to the PCI configuration Space within the processor.*/
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a WrPCIConfigLocal command.                                         */
/*                  This command provides sideband write access to the PCI configuration space that        */
/*                  resides within the processor.                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_WrPCIConfigLocal (PECI_DATA_SIZE_T write_data_size, BYTE host_id, bool retry,
                            DWORD pci_config_address, DWORD data);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Trans                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  wr_length - Number of bytes to write.                                                  */
/*                  rd_length - Number of bytes to read.                                                   */
/*                  cmd_code  - Command code.                                                              */
/*                  data_0    - Low 32-bit data to write (when applicable).                                */
/*                  data_1    - High 32-bit data to write (when applicable).                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initiates the parameters of a PECI transaction.                           */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Trans (
    BYTE          wr_length,
    BYTE          rd_length,
    PECI_COMMAND_T cmd_code,
    DWORD         data_0,
    DWORD         data_1
);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Enable_HwAWFCS                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables HW generation of an Assured Write FCS byte.                       */
/*                  AW FCS provides the processor client a high degree of confidence that the data it      */
/*                  received from the host is correct.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Enable_HwAWFCS(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Disable_HwAWFCS                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disables HW generation of an Assured Write FCS byte.                      */
/*                  AW FCS provides the processor client a high degree of confidence that the data it      */
/*                  received from the host is correct.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Disable_HwAWFCS(void);


#endif  /* _PECI_IF_H */

