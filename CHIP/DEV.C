/*-----------------------------------------------------------------------------
 * MODULE DEV.C
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
 * Contains AUX keyboard and AUX device serial send/receive
 * and lock/unlock code.  Based on PC87570 code version
 * 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 20, 2000 by GLP)
 *
 * EDH
 * 11 Dec 00 	Support the fourth PS/2 port as port 4. It will be
 * 				recognized as port 0 with Active PS/2 Multiplexing
 *					driver.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * --------------------------------------------------------------- ---------*/

#define DEV_C

#include "dev.h"
#include "ps2.h"
#include "proc.h"
#include "purdat.h"
#include "purdev.h"
#include "staticfg.h"
#include "purscn.h"
#include "purext.h"
#include "regs.h"
#include "scan.h"
#include "a_to_d.h"
#include "d_to_a.h"
#include "i2c.h"
#include "purxlt.h"
#include "id.h"

#define Short_Delay() Inhibit_Delay(); Disable_Irq()


#ifndef RAM_SCAN_TABLE
#error RAM_SCAN_TABLE switch is not defined.
#endif

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

/* ---------------------------------------------------------------
   Name: Do_Line_Test

   Tests the auxiliary keyboard or auxiliary device (mouse) lines.

   Input: Name of port to test:
            AUX_PORT1_CODE, AUX_PORT2_CODE, AUX_PORT3_CODE or AUX_PORT4_CODE.

   Returns: Error Code
               0 = no error
               1 = Clock line stuck low.
               2 = Clock line stuck high.
               3 = Data line stuck low.
               4 = Data line stuck high.
   --------------------------------------------------------------- */
typedef enum line_test_code
{
   TEST_OKAY,
   CLK_STUCK_LO, CLK_STUCK_HI,
   DAT_STUCK_LO, DAT_STUCK_HI,
   TEST_DONE
} LTC;

WORD Do_Line_Test(SMALL port)
{
    return (0);
}
