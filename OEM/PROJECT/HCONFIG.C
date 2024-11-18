/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     SHARED.C - HOST modules configuration
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/


/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "types.h"
#include "com_defs.h"
#include "sib.h"
#include "regs.h"

#define SHM_IO_H    0x02    /* Base Address MSB. */
#define SHM_IO_L    0x00    /* Base Address LSB. */

#define LDN_ACTIVE      0x30    /* Activate. */

typedef struct _SIB_DESCRIPTOR
{
    BYTE addr;      /* Indirect Host I/O Address. */
    BYTE data;      /* Indirect Host I/O Data. */
} SIB_DESCRIPTOR;

const SIB_DESCRIPTOR InitSIBTable[] =
{
  {LDN, LDN_MOUSE},
  {LDN_ACTIVE, 0x01},

  {LDN, LDN_KBD},
  {LDN_ACTIVE, 0x01},

  {LDN, LDN_PM1},
  {0x70, 0x00},
  {LDN_ACTIVE, 0x01},

  {LDN, LDN_PM2},
  {0x70, 0x00},
  {LDN_ACTIVE, 0x01},

  {LDN, LDN_SHM},
  {BASE_ADDR, SHM_IO_H},
  {BASE_ADDR+1, SHM_IO_L},
  {WIN_CFG, 0x0A},
  {LDN_ACTIVE, 0x01}
};
#define SIB_Size  (sizeof(InitSIBTable) / sizeof(SIB_DESCRIPTOR))


void HostModuleInit(void)
{
    BYTE index;

	SIB_set_lock(SIB_CONFIG_DEV);
    for (index = 0; index < SIB_Size; index++)
    {
        SIO_CONFIG_WRITE(InitSIBTable[index].addr, InitSIBTable[index].data);
    }
	SIB_clear_lock(SIB_CONFIG_DEV);
}



