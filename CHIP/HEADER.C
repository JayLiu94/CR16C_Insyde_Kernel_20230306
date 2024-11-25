/*----------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<-------------------------------------------------------------------------
 * File Contents:
 *     header.c - BIOS header definition and initialization
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

typedef    unsigned long   DWORD;
typedef    unsigned short  WORD;
typedef    unsigned char   BYTE;

#include "hfcg.h"
#include "swtchs.h"

#pragma section(".header",flash_header)

typedef struct
{
/*-----------------------------------------------------------------------------
* Flash Signature
*----------------------------------------------------------------------------*/
/* 00 */ WORD  signature; // Flash Signature
/* 02 */ WORD  reserved1;

/*-----------------------------------------------------------------------------
* MSWC Core Registers Configuration
*----------------------------------------------------------------------------*/
/* 04 */ BYTE   HCBAL_DAT; // Host Configuration Base Address Low
/* 05 */ BYTE   HCBAH_DAT; // Host Configuration Base Address High
/* 06 */ WORD  reserved2;

/*-----------------------------------------------------------------------------
* High Frequency Clock Generator (HFCG) Module Configuration
*----------------------------------------------------------------------------*/
/* 08 */ BYTE   HFCGML_DAT; // HFCG, M value, Low
/* 09 */ BYTE   HFCGMH_DAT; // HFCG, M value, High
/* 0A */ BYTE   HFCGN_DAT;  // HFCG, N value
/* 0B */ BYTE   HFCGP_DAT;  // bits 7-4 - Flash Clock Divider
                             // bits 3-0 - Core Clock Divider
/* 0C */ WORD  reserved3;

/*-----------------------------------------------------------------------------
* Flash Interface Unit (FIU) Module Configuration
*----------------------------------------------------------------------------*/
/* 0E */ BYTE   FL_SIZE_DAT; // Flash size
/* 0F */ BYTE   BURST_DAT;   // bits 5-4 - Flash Write Burst
                              // bits 1-0 - Flash Read Burst
/* 10 */ BYTE   SPI_FL_DAT;  // bit    6 - Flash Fast Read (set if HFCG_FLASH_CLK is larger than 33MHz)
                              // bits 5-0 - Flash Device Size
/* 11 */ BYTE   MISC_CTL;    // bit 7    - Bypass on bad Firmware Checksum
/* 12 */ WORD  Reserved4;

/*-----------------------------------------------------------------------------
* General Settings
*----------------------------------------------------------------------------*/
/* 14 */ DWORD  FIRMW_START;  // Firmware Start Address
/* 18 */ DWORD  FIRMW_SIZE;   // Firmware Size (in words)
/* 1C */ WORD  Reserved5[18];
/* 40 */ WORD  HEADER_CKSM;  // Checksum of Header (bytes 0h to 40h)
/* 42 */ WORD  Reserved6;
/* 44 */ DWORD  FIRMW_CKSM;   // Checksum of Firmware
/* 48 */ DWORD  LFCG_FRCDIV;  // Low Freq Clock Divisor
} Flash_header_t;



const Flash_header_t flash_header =
{
/*-----------------------------------------------------------------------------
* Flash Signature
*----------------------------------------------------------------------------*/
#if RUN_FROM_MRAM
/* 00 */ 0x1678, // MRAM header Signature
#else
/* 00 */ 0x8761, // NO MRAM header Signature
#endif

/* 02 */ 0x0000, // reserved1

/*-----------------------------------------------------------------------------
* MSWC Core Registers Configuration
*----------------------------------------------------------------------------*/
/* 04 */ 0x4E,   // HCBAL_DAT
/* 05 */ 0x16,   // HCBAH_DAT
/* 06 */ 0x0000, // reserved2

/*-----------------------------------------------------------------------------
* High Frequency Clock Generator (HFCG) Module Configuraion
* Please don't change the clock registers values here. The values are defined
* in hfcg.h and they depend on the values of HFCG_FMCLK, HFCG_FLASH_CLK and
* HFCG_CORE_CLK which may be customized.
*----------------------------------------------------------------------------*/
/* 08 */ FMCLK_HFCGML,   // HFCGML_DAT
/* 09 */ FMCLK_HFCGMH,   // HFCGMH_DAT
/* 0A */ FMCLK_HFCGN,    // HFCGN_DAT
/* 0B */ HFCGP_VAL,      // HFCGP_DAT   bits 7-4 - FPRED
                         //             bits 3-0 - CPRED
/* 0C */ 0x0000, // reserved3

/*-----------------------------------------------------------------------------
* Flash Interface Unit (FIU) Module Configuration
*----------------------------------------------------------------------------*/
/* 0E */ 0x09,   // FL_SIZE_P1
/* 0F */ 0x03,   // bits 5-4 - W_BURST
                 // bits 1-0 - R_BURST
/* 10 */ (FAST_READ_MASK | 0x08),
                 // bit    6 - FREAD (set if HFCG_FLASH_CLK is larger than 33MHz)
                 // bits 0-5 - DEV_SIZE
/* 11 */ 0x7f,   // MISC_CTL
                 // bit 7    - BAD_FCKSM_BYP
/* 12 */ 0x0000, // Reserved4

/*-----------------------------------------------------------------------------
* General Settings
*----------------------------------------------------------------------------*/
/* 14 */ 0x00020100, // FIRMW_START
/* 18 */ 0x00000000, // FIRMW_SIZE
/* 1C */ {0x0000},   // Reserved5[18]
/* 40 */ 0x0000,     // HEADER_CKSM
/* 42 */ 0x0000,     // Reserved6;
/* 44 */ 0x00000000, // FIRMW_CKSM
/* 48 */ 0xFFFFFFFF  // LFCG_FRCDIV

};

