/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   peci_drv.c                                                                                            */
/*            This file contains Platform Environment Control Interface (PECI) driver implementation       */
/* Project:                                                                                                */
/*            SWC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "types.h"
#include "swtchs.h"
#include "peci.h"
#include "hfcg.h"
#include "regs.h"


#if PECI_SUPPORTED

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TABLES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* PECI CRC8 table (Polynomial: x8 + x2 + x + 1)                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static const BYTE PECI_crc8_table[256] =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
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



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Control Status Error Mask                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
//#define PECI_CTL_STS_ERROR_MASK (ABRT_CTN | ABRT_ERR | CRC_ERR | AVL_ERR)
#define PECI_CTL_STS_ERROR_MASK (ABRT_ERR | CRC_ERR | AVL_ERR)

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Host ID Retry                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_HOST_ID_RETRY(host_id, retry)      (DWORD)(host_id << 1 | retry)

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Maximum Retry Trials                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define PECI_RETRY_MAX_COUNT        3

/*---------------------------------------------------------------------------------------------------------*/
/* Zero-data transaction                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define NO_DATA                     0L

/*---------------------------------------------------------------------------------------------------------*/
/* CRC Polynom used                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CRC_POLYNOM                 0xE0

/*---------------------------------------------------------------------------------------------------------*/
/* CRC reset value                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define CRC_PRESET                  0x00



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                            GLOBAL VARIABLES                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Current PECI Command being handled                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static PECI_COMMAND_T   PECI_current_command;

/*---------------------------------------------------------------------------------------------------------*/
/* Current PECI Domain being handled                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
static PECI_DOMAIN_T    PECI_current_domain;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Client Address                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static BYTE            PECI_client_address;

/*---------------------------------------------------------------------------------------------------------*/
/* Latest PECI Completion Code                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static PECI_CC_T        PECI_cc;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Retry Counter                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static BYTE            PECI_retry_count;

/*---------------------------------------------------------------------------------------------------------*/
/* User callback to be called from PECI interrupt                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static PECI_CALLBACK_T  PECI_callback;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Data Buffer                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static DWORD           PECI_buff[2];



BYTE LAST_AWFCS;
BYTE LAST_COMPLETION_CODE;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                  LOCAL FUNCTIONS FORWARD DECLARATIONS                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void    PECI_Trans (BYTE wr_length, BYTE rd_length, PECI_COMMAND_T cmd_code,
                            DWORD data_0, DWORD data_1);
static  void    PECI_StartTrans (void);
static  BYTE   PECI_CalcAWFCS  (void);
        void    PECI_IntHandler (void);



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_Init                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback  - Application callback to be called from PECI interrupt handler.             */
/*                  peci_freq - PECI bit rate (in Hertz)                                                   */
/*                              If requested frequency is 0, default settings are used.                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes the PECI module.                                              */
/*---------------------------------------------------------------------------------------------------------*/
void PECI_Init (
    PECI_CALLBACK_T callback,
    DWORD          peci_freq
)
{
    BYTE peci_max_bit_rate;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize SW driver                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_callback           = callback;
    PECI_current_command    = PECI_COMMAND_NONE;
    PECI_current_domain     = PECI_DOMAIN_0;
    PECI_cc                 = PECI_CC_NONE;
    PECI_retry_count        = PECI_RETRY_MAX_COUNT;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set the maximum bit rate used by the PECI module during both Address Timing Negotiation and Data    */
    /* Timing Negotiation.                                                                                 */
    /* The resulting maximum bit rate MAX_BIT_RATE in decimal is according to the following formula:       */
    /*                                                                                                     */
    /*      MAX_BIT_RATE [d] = (FMCLK / (4 * freq)) - 1                                                    */
    /*                                                                                                     */
    /* Maximum bit rate should not extend the field's boundaries.                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (peci_freq != 0)
    {
        peci_max_bit_rate = (HFCG_FMCLK / (4 * peci_freq)) - 1;
        peci_max_bit_rate = MIN(peci_max_bit_rate, GET_FIELD(PECI_RATE, PECI_RATE_MAX_BIT_RATE));

        SET_FIELD(PECI_RATE, PECI_RATE_MAX_BIT_RATE, peci_max_bit_rate);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable PECI pin                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_BIT(DEVCNT, PECI_EN);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable PECI Done Interrupt                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (PECI_callback != NULL)
    {
        SET_BIT(PECI_CTL_STS, DONE_EN);
    }


}



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
void PECI_SetAddress (BYTE client_address)
{
    PECI_client_address = client_address;
}



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
void PECI_SetDomain (PECI_DOMAIN_T domain)
{
    PECI_current_domain = domain;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_GetComplitionCode                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         PECI Completion Code.                                                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the Completion Code of the last PECI transaction.               */
/*---------------------------------------------------------------------------------------------------------*/
PECI_CC_T PECI_GetComplitionCode (void)
{
    return PECI_cc;
}



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
void PECI_Ping (void)
{
    PECI_Trans(0x00, 0x00, PECI_COMMAND_PING, NO_DATA, NO_DATA);
}



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
void PECI_GetDIB (void)
{
    PECI_Trans(0x01, 0x08, PECI_COMMAND_GET_DIB, NO_DATA, NO_DATA);
}



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
void PECI_GetTemp (void)
{
    PECI_Trans(0x01, 0x02, PECI_COMMAND_GET_TEMP, NO_DATA, NO_DATA);
}



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
void PECI_RdPkgConfig (
    PECI_DATA_SIZE_T read_data_size,
    BYTE            host_id,
    bool          retry,
    BYTE            index,
    WORD           parameter
)
{
    PECI_Trans(
        0x05,
        read_data_size + 1,
        PECI_COMMAND_RD_PKG_CFG,
        PECI_HOST_ID_RETRY(host_id, retry) | (DWORD)index << 8 | (DWORD)parameter << 16,
        NO_DATA);
}



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
void PECI_WrPkgConfig (
    PECI_DATA_SIZE_T write_data_size,
    BYTE            host_id,
    bool          retry,
    BYTE            index,
    WORD           parameter,
    DWORD           data
)
{
    PECI_Trans(
        write_data_size + 6,
        0x01,
        PECI_COMMAND_WR_PKG_CFG,
        PECI_HOST_ID_RETRY(host_id, retry) | (DWORD)index << 8 | (DWORD)parameter << 16,
        data);
}



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
void PECI_RdIAMSR (
    PECI_DATA_SIZE_T read_data_size,
    BYTE            host_id,
    bool          retry,
    BYTE            processor_id,
    WORD           msr_address
)
{
    PECI_Trans(
        0x05,
        read_data_size + 1,
        PECI_COMMAND_RD_IAMSR,
        PECI_HOST_ID_RETRY(host_id, retry) | (DWORD)processor_id << 8 | (DWORD)msr_address << 16,
        NO_DATA);
}



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
void PECI_RdPCIConfig (
    PECI_DATA_SIZE_T read_data_size,
    BYTE            host_id,
    bool          retry,
    DWORD           pci_config_address
)
{
    PECI_Trans(
        0x06,
        read_data_size + 1,
        PECI_COMMAND_RD_PCI_CFG,
        PECI_HOST_ID_RETRY(host_id, retry) | (DWORD)pci_config_address << 8,
        (DWORD)LSN(MSB0(pci_config_address)));
}



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
void PECI_RdPCIConfigLocal (
    PECI_DATA_SIZE_T read_data_size,
    BYTE            host_id,
    bool            retry,
    DWORD           pci_config_address
)
{
    PECI_Trans(
        0x05,
        read_data_size + 1,
        PECI_COMMAND_RD_PCI_CFG_LOCAL,
        PECI_HOST_ID_RETRY(host_id, retry) | pci_config_address << 8,
        NO_DATA);
}



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
void PECI_WrPCIConfigLocal (
    PECI_DATA_SIZE_T write_data_size,
    BYTE            host_id,
    bool          retry,
    DWORD           pci_config_address,
    DWORD           data
)
{
    PECI_Trans(
        write_data_size + 6,
        0x01,
        PECI_COMMAND_WR_PCI_CFG_LOCAL,
        PECI_HOST_ID_RETRY(host_id, retry) | pci_config_address << 8,
        data);
}



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
)
{
    // check that there is NO PECI transaction in progress
    if (IS_BIT_SET(PECI_CTL_STS, START_BUSY))
    {
        // PECI transaction is in progress - can not initiate a new one
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Save partial transaction parameters for later use                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_current_command = cmd_code + PECI_current_domain;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize retry counter                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_retry_count = PECI_RETRY_MAX_COUNT;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Save data in internal buffer                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_buff[0] = data_0;
    PECI_buff[1] = data_1;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set basic transaction parameters                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_ADDR      = PECI_client_address;
    PECI_WR_LENGTH = wr_length;
    PECI_RD_LENGTH = rd_length;
    PECI_CMD       = PECI_current_command;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initiate the transaction                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_StartTrans();
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_StartTrans                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine starts a PECI transaction.                                                */
/*---------------------------------------------------------------------------------------------------------*/
static void PECI_StartTrans (void)
{
    BYTE  i;
    BYTE buff_copy_size;

    BYTE wr_length = PECI_WR_LENGTH;
    BYTE cmd_code  = PECI_CMD;

    /*-----------------------------------------------------------------------------------------------------*/
    /* The following calculates the amount of data to copy to PECI DATA_OUT buffer:                        */
    /* ?In case wr_length is 0, return 0 (as in case of Ping command);                                    */
    /* ?Otherwise, decrease the command code byte (allowing maximum of 8 bytes)                           */
    /*-----------------------------------------------------------------------------------------------------*/
    buff_copy_size = (wr_length != 0) * MIN((wr_length - 1), 8);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set extended transaction parameters                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < buff_copy_size; i++)
    {
        PECI_DATA_OUT(i) = ((BYTE*)PECI_buff)[i];
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate AW FCS if this is a write transaction and write it to the data out buffer                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((cmd_code == PECI_COMMAND_WR_PKG_CFG) || (cmd_code == PECI_COMMAND_WR_PCI_CFG_LOCAL))
    {
        if (! IS_BIT_SET(PECI_CTL_STS, AWFCS_EN))
        {
            PECI_DATA_OUT(wr_length - 2) = PECI_CalcAWFCS();
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Start the PECI transaction                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_BIT(PECI_CTL_STS, START_BUSY);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_CalcAWFCS                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates an Assured Write FCS, which is required in write transactions. */
/*                  AW FCS provides the processor client a high degree of confidence that the data it      */
/*                  received from the host is correct.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static BYTE PECI_CalcAWFCS (void)
{
    BYTE    i;
    BYTE   aw_fcs    = CRC_PRESET;
    BYTE   wr_length = PECI_WR_LENGTH;

    aw_fcs ^= PECI_ADDR;
    aw_fcs = PECI_crc8_table[aw_fcs];

    aw_fcs ^= wr_length;
    aw_fcs = PECI_crc8_table[aw_fcs];

    aw_fcs ^= PECI_RD_LENGTH;
    aw_fcs = PECI_crc8_table[aw_fcs];

    aw_fcs ^= PECI_CMD;
    aw_fcs = PECI_crc8_table[aw_fcs];

    for (i = 0; i < wr_length - 2; i++) /* cnt = number of protocol bytes without CRC */
    {
        aw_fcs ^= PECI_DATA_OUT(i);
        aw_fcs = PECI_crc8_table[aw_fcs];
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* The MSb of this result must be inverted to avoid a Write FCS of 0x00                                */
    /*-----------------------------------------------------------------------------------------------------*/
    aw_fcs ^= 0x80;

    LAST_AWFCS = aw_fcs;

    return aw_fcs;
}


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
void PECI_Enable_HwAWFCS(void)
{
    // set PECI_CTL_STS.AWFCS_EN
    SET_BIT(PECI_CTL_STS, AWFCS_EN);
}


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
void PECI_Disable_HwAWFCS(void)
{
    // clear PECI_CTL_STS.AWFCS_EN
    CLEAR_BIT(PECI_CTL_STS, AWFCS_EN);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_IntHandler                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  PECI handler function - this routine performs the following:                           */
/*                  ?Retry to send the PECI transaction in cases of errors.                               */
/*                  ?Inform upper-layer on error type in case all retries fail.                           */
/*                  ?Fill the PECI data buffer for the user's application use.                            */
/*                  ?Invoke the application callback function and clear the interrupt in ICU.             */
/*---------------------------------------------------------------------------------------------------------*/
#pragma interrupt (PECI_IntHandler)
void PECI_IntHandler (void)
{
    BYTE                i;
    PECI_TRANS_DONE_T   trans_done_sts;
    PECI_CC_T           cc = PECI_CC_NONE;
    BYTE               dataOffset;   /* Byte offset where data begins (excludes the Completion Code) */
    BYTE               peci_ctl_sts = PECI_CTL_STS;
    BYTE               peci_ctl_sts_error_masked = (peci_ctl_sts & PECI_CTL_STS_ERROR_MASK);
    BYTE               data_size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve the commnd completion code (applicable for specific commands only)                         */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (PECI_current_command - PECI_current_domain)
    {
    case PECI_COMMAND_PING:
    case PECI_COMMAND_GET_DIB:
    case PECI_COMMAND_GET_TEMP:
        /*-------------------------------------------------------------------------------------------------*/
        /* Completion Code is not available in these commands - data starts at offset 0                    */
        /*-------------------------------------------------------------------------------------------------*/
        dataOffset = 0;
        break;

    default:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Completion Code only when there is no HW error (otherwise CC is not sent)              */
        /*-------------------------------------------------------------------------------------------------*/
        if (! peci_ctl_sts_error_masked)
        {
            cc = PECI_DATA_IN(0);
            LAST_COMPLETION_CODE = cc;
        }
        /*-------------------------------------------------------------------------------------------------*/
        /* Data starts at offset 1 (after the Completion Code)                                             */
        /*-------------------------------------------------------------------------------------------------*/
        dataOffset = 1;
        break;
    }

    PECI_cc = cc;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve the amount of data to read (excluding the Completion Code, if applicable)                  */
    /*-----------------------------------------------------------------------------------------------------*/
    data_size = PECI_RD_LENGTH - dataOffset;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear PECI error bits                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    PECI_CTL_STS = peci_ctl_sts;

    if ((cc & 0x90) == 0x90)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Abort the PECI transaction and inform on error in the following cases:                          */
        /* ?CC: 0x9x                                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        PECI_callback(PECI_current_command, 0, PECI_TRANS_DONE_CC_ERROR, NO_DATA, NO_DATA);
    }
    else if (peci_ctl_sts_error_masked || ((cc & 0x80) == 0x80))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Retry to send the PECI transaction in the following cases:                                      */
        /* ?Bad FCS                                                                                       */
        /* ?Abort FCS                                                                                     */
        /* ?CC: 0x8x                                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        PECI_retry_count--;

        if (PECI_retry_count > 0)
        {
            //[-START-], nuvoTon-Steve Huang, 2017/04/27, add workaround for Intel KBL Y/U/S/H PECI issue(sighting 4712792)
            if( (cc & 0x80) == 0x80 )
            {
              PECI_buff[0] |= 0x0001;  //set retry bit for retry
            }
            //[--END--], nuvoTon-Steve Huang, 2017/04/27, add workaround for Intel KBL Y/U/S/H PECI issue(sighting 4712792)
            /*---------------------------------------------------------------------------------------------*/
            /* Re-start the failed transaction                                                             */
            /*---------------------------------------------------------------------------------------------*/
            PECI_StartTrans();
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* PECI transaction failed for [PECI_RETRY_MAX_COUNT] times - Initialize retry counter         */
            /*---------------------------------------------------------------------------------------------*/
            PECI_retry_count = PECI_RETRY_MAX_COUNT;

            trans_done_sts =
                (peci_ctl_sts_error_masked) ? peci_ctl_sts_error_masked : PECI_TRANS_DONE_CC_ERROR;

            /*---------------------------------------------------------------------------------------------*/
            /* Abort the PECI transaction and inform on error                                              */
            /*---------------------------------------------------------------------------------------------*/
            PECI_callback(PECI_current_command, 0, trans_done_sts, NO_DATA, NO_DATA);
        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Copy the desired data to return                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        for (i = 0; i < data_size; i++)
        {
            ((BYTE*)PECI_buff)[i] = PECI_DATA_IN((i + dataOffset));
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Transaction is completed successfully                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        PECI_callback(PECI_current_command, (PECI_DATA_SIZE_T)data_size, PECI_TRANS_DONE_OK,
                        PECI_buff[0], PECI_buff[1]);
    }
}



#endif // PECI_SUPPORTED


