/* ----------------------------------------------------------------------------
 * MODULE PURACPI.H
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
 * Revision History (Started March 19, 1998 by GLP)
 *
 * GLP
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef PURACPI_H
#define PURACPI_H

#include "types.h"

#define FIRST_ACPI_CMD 0x80
#define LAST_ACPI_CMD  0x84

void ACPI_Init(void);
void Handle_ACPI(void);
WORD ACPI_Cmd(BYTE command_num);
WORD ACPI_Dat(BYTE command_num, BYTE data);
WORD Read_EC_Space(BYTE data);
FLAG Write_EC_Space(BYTE command_num, BYTE data, BYTE hif);
void Set_SCI_Cause(BYTE number);
void ACPI_Gen_Int(void);

extern BYTE ACPI_Timer;


#endif /* ifndef PURACPI_H */

