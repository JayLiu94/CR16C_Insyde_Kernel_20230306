/*-----------------------------------------------------------------------------
 * MODULE A_TO_D.H
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Analog to Digital converter.
 *
 * Revision History (Started April 30, 1998 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef A_TO_D_H
#define A_TO_D_H

#include "types.h"

WORD Read_AtoD(BYTE data);
void inital_ADC(BYTE divitor);

#endif /* ifndef A_TO_D_H */
