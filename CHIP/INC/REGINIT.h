/*-----------------------------------------------------------------------------
 * MODULE REGINIT.H
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
 * Definitions for the variables in the register initialization table.
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started April 3, 2000 by GLP)
 *
 * ------------------------------------------------------------------------- */

#ifndef REGINIT_H
#define REGINIT_H

#include "types.h"

#define Word_type   2
#define Byte_type   1

typedef struct _REG_INIT_DESCRIPTOR
{
    DWORD   Reg_Addr;
    BYTE    Reg_Type;
    WORD    Reg_Value;
} REG_INIT_DESCRIPTOR;

extern const REG_INIT_DESCRIPTOR Reg_Init[];

void Init_Regs(void);


#endif /* ifndef REGINIT_H */
