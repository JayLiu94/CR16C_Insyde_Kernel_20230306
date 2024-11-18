/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMDAT.H - OEM data definition.
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/
#ifndef OEMDAT_H
#define OEMDAT_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "types.h"
#include "swtchs.h"

extern BYTE SysState;
extern DWORD PS_Ticker;

extern FUNCT_PTR_V_V pwrseq_handle;

extern BITS_8 PwrState;
#define BATFCHG         bit3    /* Battery in fully charging. */
#define BATCHG          bit4    /* Battery in Charging. */
#define LIDSTS          bit5    /* Lid status 0: Closed, 1: Open */
#define BATPres          bit6    /* Battery present. */
#define ACPres           bit7    /* AC present. */

extern BYTE             ConfigHostModule;

extern BITS_8 SMBCtrl;
#define SMB0_Active     bit0
#define SMB1_Active     bit1
#define SMB2_Active     bit2
#define SMB3_Active     bit3

#define SMB_Active      bit4    /* SMBus is active. */
#define SMB_BAT         bit5    /* SMBus active for battery. */
#define SMB_THML        bit6    /* SMBus active for thermal. */




#endif /* ifndef OEMDAT_H */
