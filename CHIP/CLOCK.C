/*------------------------------------------------------------------------------
 * Copyright (c) 2017 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     Clock.c - Core Domain Clock Generator - Initialization and get core clock
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#include "types.h"
#include "regs.h"
#include "hfcg.h"

void clock_init(void)
{
    HFCGP = (HFCG_FMCLK/HFCG_CORE_CLK - 1);
}

DWORD fm_clock_get_freq(void) {
    DWORD fm_clk = 40000000l;
    if ( 0x0BEC == (((WORD)HFCGMH << 8) | HFCGML)) {
        fm_clk = 50000000l;
    } else if ( 0x0B72 == (((WORD)HFCGMH << 8) | HFCGML)) {
        fm_clk = 48000000l;
    } else if ( 0x07DE == (((WORD)HFCGMH << 8) | HFCGML)) {
        fm_clk = 33000000l;
    }
    return fm_clk;
}

/*
 * Return the current clock frequency in Hz.
 */
DWORD clock_get_freq(void)
{
    BYTE core_div = (HFCGP & 0x0F) + 1;
	return fm_clock_get_freq()/core_div;
}


