/* ----------------------------------------------------------------------------
 * MODULE PURINI.H
 *
 * Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started May 20, 1996 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * EDH
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * ------------------------------------------------------------------------- */

#ifndef PURINI_H
#define PURINI_H

void Reset_Initialization(void);
void Setup_Ext_Cbx(void);
void Reset_Int_Mouse (void);
//void Sample_ADC(BYTE VC1SELIN, BYTE VC2SELIN, BYTE VC3SELIN);
void CalibrateADC(void);
void InitADC( BYTE ChNum1, BYTE ChNum2, BYTE ChNum3);
#endif

