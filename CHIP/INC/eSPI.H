/* ---------------------------------------------------------------
 * MODULE HOST_IF.H
 *
 *          Copyright 1996-1999, all rights reserved
 *          Insyde Software Corp.
 *
 * Host interface.
 *
 * Revision History (Started July 10, 1996 by GLP)
 *
 * GLP
 * 26 Nov 96   Added Read_Host_Status_Reg2 and
 *             Write_Host_Status_Reg2.
 * 06 May 97   Added status register definitions for secondary
 *             Host interface for ACPI.
 * 27 Feb 98   Removed references to EMUL.
 * 17 Mar 98   Added prototype.
 * 20 Mar 98   Removed definitions for maskSTATUS_PORT2_ALARM,
 *             maskSTATUS_PORT2_BUSY, and maskSTATUS_PORT2_SMI.
 * 24 Apr 98   Updated Data_To_Host2 to save a copy of the data.
 * 19 Aug 99   Updated copyright.
 * EDH
 * 15 Feb 05   Added Read_Host_Status_Reg3 and Write_Host_Status_Reg3.
 *             Added status register definitions for third Host interface
 *             for SMI.
 * --------------------------------------------------------------- */

#ifndef ESPI_H
#define ESPI_H

#include "regs.h"

#define ENABLE      1
#define DISABLE     0
#define ESPI_ACCESS                     MEM
#define ESPI_HOST_ACCESS                SIB
#define ESPI_INTERRUPT					        24

enum{
	VW_IDX_4,
	VW_IDX_5,
	VW_IDX_6,
	VW_IDX_40,
	VW_IDX_45,
	VW_IDX_46,
	VW_IDX_2,
	VW_IDX_3,
	VW_IDX_7,
	VW_IDX_41,
	VW_IDX_42,
	VW_IDX_43,
	VW_IDX_44,
	VW_IDX_47,
};

typedef struct
{
	DWORD   header;
	DWORD   Send_data;
} ESPI_PARAMS;

typedef enum
{
    FLASH_READ		            = 0,
    FLASH_WRITE                 = 1,
    FLASH_ERASE                 = 2
} ESPI_FLASH_T;



typedef enum
{
    ESPI_CHANNEL_PERIPHERAL		= 0,
    ESPI_CHANNEL_VW    			= 1,
    ESPI_CHANNEL_OOB    		= 2,
    ESPI_CHANNEL_FLASH    		= 3
} ESPI_DIR_T;

typedef enum
{
    OPER_FREQ_20MHZ		        = 0,
    OPER_FREQ_25MHZ             = 1,
    OPER_FREQ_33MHZ             = 2,
    OPER_FREQ_50MHZ             = 3,
    OPER_FREQ_66MHZ             = 4
} ESPI_OPER_FREQ_T;

typedef enum
{
    IO_MODE_SINGAL		        = 0,
    IO_MODE_DUAL                = 1,
    IO_MODE_QUAD                = 2
} ESPI_IO_MODE_T;

#define IDX2_SLP_S3             (1 << 0)
#define IDX2_SLP_S4             (1 << 1)
#define IDX2_SLP_S5             (1 << 2)

#define IDX3_SUS_STAT           (1 << 0)
#define IDX3_PLTRST             (1 << 1)
#define IDX3_OOB_RST_WARN       (1 << 2)

#define IDX4_OOB_RST_ACK        (1 << 0)
#define IDX4_WAKE               (1 << 2)
#define IDX4_PME                (1 << 3)

#define IDX6_SCI                (1 << 0)
#define IDX6_SMI                (1 << 1)
#define IDX6_RCIN               (1 << 2)
#define IDX6_HOST_RST_ACK       (1 << 3)

#define IDX7_HOST_RST_WARN      (1 << 0)

#define IDX5_BOOT_LOAD_DONE     (1 << 0)
#define IDX5_ERROR_FATAL        (1 << 1)
#define IDX5_ERROR_NONFATAL     (1 << 2)
#define IDX5_BOOT_LOAD_STATUS   (1 << 3)

#define IDX40_SUSACK            (1 << 0)

#define IDX41_SUS_WARN          (1 << 0)
#define IDX41_SUS_PWRDN_ACK     (1 << 1)
#define IDX41_SLP_A             (1 << 3)

#define IDX42_SLP_LAN           (1 << 0)
#define IDX42_SLP_WLAN          (1 << 1)

#define IDX47_HOST_C10          (1 << 0)

void eSPI_VM_Write(BYTE eSPI_Idx, BYTE data);
DWORD eSPI_VM_Read(BYTE index);
void eSPI_Configuration(ESPI_DIR_T eSPI_ch, FLAG en_flag);
void eSPI_Init(void);
void eSPI_OOB_Test(void);
void eSPI_OOB_Rcvd(void);
void eSPI_FLASH_Test(ESPI_FLASH_T type, DWORD addr, WORD leng);
void eSPI_FLASH_Rcvd(void);
/* Edward 12/18/2017 Added for eSPI supported. */
#if ESPI_SUPPORTED
void eSPI_IntHandler(void);
#endif /* ESPI_SUPPORTED */
/* End of Edward 12/18/2017 Added. */

#endif /* ifndef ESPI_H */

