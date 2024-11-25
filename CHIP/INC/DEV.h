/*-----------------------------------------------------------------------------
 * MODULE DEV.H
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started August 13, 1996 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef DEV_H
#define DEV_H

#include "types.h"

WORD Aux_Data_In(SMALL port);
WORD Do_Line_Test(SMALL dev);
void Write_Config_Table(BYTE data, WORD offset);
BYTE Read_Config_Table(WORD offset);



#endif /* ifndef DEV_H */
