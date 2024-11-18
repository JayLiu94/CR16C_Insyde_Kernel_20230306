/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMMAIN.H - OEM main routines.
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef OEMMAIN_H
#define OEMMAIN_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

bool Ticker_Update_32(DWORD *pntr);
bool Ticker_Update_8(BYTE *pntr);
FLAG Read_Port_Pin(BYTE data);
void Service_OEM_10mS(void);
void Service_OEM_100mS(void);
void Service_OEM_500mS(void);
void Service_OEM_1000mS(void);

#endif /* ifndef OEMMAIN_H */


