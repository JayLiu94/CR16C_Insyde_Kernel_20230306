/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMCEIR.H - OEM CEIR routines.
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef OEMCEIR_H
#define OEMCEIR_H

#include "swtchs.h"

void ceir_init();
void ceir_periodic();
void ceir_enter_S0();
void ceir_exit_S0();


#define CEIR_RECEIVE_ONLY_DRIVER        TRUE
#define CEIR_MCE_FULL_DRIVER            FALSE

// Change CEIR Receive pin according to system configuration
#define CEIR_MAIN_RECEIVE_PIN           CIRRM1

// Change CEIR signal configuration (inverted/not inverted) according to system configuration
#define CEIR_SIGNAL_INVERT              FALSE

// Change nominal stretch time added by IR reeciver in the demodulation process to Cell_1
// duration according to system configuration. See WPCE775x datasheet for more details.
// No change is needed if the system IR receiver is according to the WPCE775x reference design
#define CEIR_STR_NUM                    5


#endif /* ifndef OEMCEIR_H */


