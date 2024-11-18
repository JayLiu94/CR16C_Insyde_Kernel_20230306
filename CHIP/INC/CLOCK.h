/*------------------------------------------------------------------------------
 * Copyright (c) 2017 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     Clock.h - Core Domain Clock Generator - Initialization and get core clock
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef CLOCK_H
#define CLOCK_H

#include "types.h"
void clock_init(void);
DWORD clock_get_freq(void);
DWORD fm_clock_get_freq(void);

#endif /* ifndef CLOCK_H */

