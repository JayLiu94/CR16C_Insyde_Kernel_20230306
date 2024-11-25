/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     hfcg.h - High Frequency Clock Generator driver
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef HFCG_H
#define HFCG_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Custom definitions
 * These definitions may be cusotmized. Please note that this is the only
 * place where these values may be modified. The values are used by other
 * drivers and booter header
 *----------------------------------------------------------------------------*/
#define HFCG_FMCLK      50000000UL // FMCLK Clock Frequency in Hz units (default 40MHz)
#define HFCG_FLASH_CLK  50000000UL // Flash Clock Frequency in Hz units (default 13.3MHz)
#define HFCG_CORE_CLK   25000000UL // Core Clock Frequency in Hz units  (default 6.67MHz)

/*-----------------------------------------------------------------------------
 * Frequency multiplier values definition according to the requested
 * FMCLK Clock Frequency
 *-----------------------------------------------------------------------------*/
#define FMCLK_HFCGN    0x02
#if (HFCG_FMCLK == 50000000UL)
#define FMCLK_HFCGMH   0x0B
#define FMCLK_HFCGML   0xEC
#elif (HFCG_FMCLK == 40000000UL)
#define FMCLK_HFCGMH   0x09
#define FMCLK_HFCGML   0x89
#elif (HFCG_FMCLK == 33000000UL)
#define FMCLK_HFCGMH   0x07
#define FMCLK_HFCGML   0xDE
#else
#error "hfcg.h: wrong FMCLK Clock Frequency"
#endif

/*----------------------------------------------------------------------------
 * Calculate flash prescaler divider according to the requested Flash
 * Clock Frequency, verify the correct values range
 *---------------------------------------------------------------------------*/
#if ((HFCG_FLASH_CLK > 50000000UL)||(HFCG_FLASH_CLK < 8250000UL))
#error "hfcg.h: Flash Clock Frequency is out the range of 8.25 MHz to 50 MHz."
#endif

#if (HFCG_FLASH_CLK > 33000000UL) // if Flash clock is larger than 33MHz - set fast read bit (SPI_FL_CFG.F_READ)
#define FAST_READ_MASK  0x40
#else
#define FAST_READ_MASK  0x00
#endif

#define FLASH_DIVIDER   \
(HFCG_FMCLK%HFCG_FLASH_CLK) > HFCG_FLASH_CLK/2 ? HFCG_FMCLK/HFCG_FLASH_CLK+1 : HFCG_FMCLK/HFCG_FLASH_CLK

/*----------------------------------------------------------------------------
 * Calculate core prescaler divider according to the requested Core
 * Clock Frequency, verify the correct values range
 *---------------------------------------------------------------------------*/

#if ((HFCG_CORE_CLK > 25000000UL)||(HFCG_CORE_CLK < 4000000UL))
#error "hfcg.h: Core Clock Frequency is out the range of 4 MHz to 25 MHz."
#endif

#define CORE_DIVIDER    \
(HFCG_FLASH_CLK%HFCG_CORE_CLK)  > HFCG_CORE_CLK/2 ? HFCG_FLASH_CLK/HFCG_CORE_CLK+1 : HFCG_FLASH_CLK/HFCG_CORE_CLK

/*----------------------------------------------------------------------------
 * HFCG Prescaler Register value definition according to the requested Flash
 * and Core Clocks Frequency
 *---------------------------------------------------------------------------*/
#define HFCGP_VAL   \
    (((FLASH_DIVIDER) - 1) << 4) + ((CORE_DIVIDER) -1)

#define FRCLK 930000UL


#endif // HFCG_H
