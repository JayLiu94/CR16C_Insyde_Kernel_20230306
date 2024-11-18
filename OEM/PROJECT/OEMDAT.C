/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMDAT.C - Variables definition.
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/


/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "types.h"
#include "com_defs.h"
#include "swtchs.h"

BYTE    ConfigHostModule;

BYTE    SysState;        /* Current System State */

DWORD  PS_Ticker;       /* Power Sequence ticker */

BITS_8  PwrState;        /* (BYTE) EC space 0xA2 */

FUNCT_PTR_V_V pwrseq_handle = NULL;

BITS_8 SMBCtrl;




