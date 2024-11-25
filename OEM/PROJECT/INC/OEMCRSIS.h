/* ----------------------------------------------------------------------------
 * MODULE OEMCRSIS.H
 *
 * Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started August 5, 1998 by GLP)
 *
 * GLP
 * 24 Jan 00   Allowed OEM_crisis_check and OEM_entering_crisis
 *             to be defined even if the INCLUDE_CRISIS switch is
 *             off.  The crisis module is compiled if the
 *             INCLUDE_CRISIS switch is on or off.  The module is
 *             linked in only if the INCLUDE_CRISIS switch is on.
 * ---------------------------------------------------------------------------- */

#ifndef OEMCRSIS_H
#define OEMCRSIS_H

#include "swtchs.h"
#include "com_defs.h"

WORD OEM_detect_crisis(void);
WORD OEM_crisis_check(void);
void OEM_entering_crisis(void);
void OEM_Hookc_Crisis_1ms(void);
bool OEM_Hookc_controller_cmd(BYTE host_byte);
void OEM_Hookc_Crisis_Init_Cmd_Hndlr(void);
void OEM_Hookc_Handle_PM_Data(void);


#define OEM_Hookc_PM_Buffer_Full()  0
// can be defined to get data from PM, e.g -
// #define OEM_Hookc_PM_Buffer_Full()  Input_Buffer3_Full()

#endif /* ifndef OEMCRSIS_H */
