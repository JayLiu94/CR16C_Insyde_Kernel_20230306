/*-----------------------------------------------------------------------------
 * MODULE PURPM1.H
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
 * Contains prototypes for PURPM.C.
 *
 * Revision History (Started May 12, 2015 by EDH)
 * ------------------------------------------------------------------------- */

#ifndef PURPM_H
#define PURPM_H

#include "types.h"

void Srvc_Pm1cmd(BYTE command_num);
void Srvc_Pm1dat(BYTE data);
void Srvc_Pm2cmd(BYTE command_num);
void Srvc_Pm2dat(BYTE data);
void Srvc_Pm3cmd(BYTE command_num);
void Srvc_Pm3dat(BYTE data);

#endif /* ifndef PURPM1_H */

