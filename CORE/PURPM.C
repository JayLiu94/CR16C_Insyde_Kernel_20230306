/* ----------------------------------------------------------------------------
 * MODULE PURPM1.C
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
 *
 * Contains standard 8042 like embedded controller commands.
 *
 * Revision History (Started May 12, 2015 by EDH)
 *
 * EDH
 * 12 May 15   Moved PM2 command/data for seperate Host infterface handling.
 * ------------------------------------------------------------------------- */

#define PURPM_C

#include "swtchs.h"
#include "purcmd.h"
#include "purext.h"
#include "defs.h"
#include "types.h"
#include "purdat.h"
#include "host_if.h"
#include "proc.h"
#include "ps2.h"
#include "purscn.h"
#include "purxlt.h"
#include "purhook.h"
#include "purdev.h"
#include "oem.h"
#include "ini.h"
#include "selftest.h"
#include "dev.h"
#include "puracpi.h"
#include "timers.h"
#include "staticfg.h"


#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif


/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static WORD pmx_cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd3(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd7(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmd9(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif);
static WORD pmx_dat0(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat1(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat2(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat3(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat4(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat5(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat6(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat7(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat8(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_dat9(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_data(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_datb(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_datc(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_datd(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_date(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);
static WORD pmx_datf(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif);

/* ---------------------------------------------------------------
   Name: Srvc_Pm1cmd

   Handle incoming command from Host write to port 66h Host interface.
   Dispatch command to one of 16 functions based on the command #.

   Input: Command #

   On return, "Cmd_Byte" will be non-zero if the command
   requires more data to complete processing of the command.
   --------------------------------------------------------------- */

   /* For the right shift to work, "command_num" MUST be unsigned.
      (Right shifting an unsigned quantity fills vacated bits with
      zeros.  Right shifting a signed quantity will fill vacated
      bits with sign bits on some machines and zero bits on others.) */

   /* The functions in the table take an WORD
      (the command #) as a parameter.

      They also return a WORD value with all bits set if
      no data is to be sent to the host.  Otherwise, the low
      byte of the WORD that is returned is the data to
      be sent to the host (the high bits are zero). */

    const FUNCT_PTR_W_HPBB pm1cmd_table[16] =
    {
        pmx_cmd0,    /* [0x] Extended commands */
        pmx_cmd1,    /* [1x] OEM custom commands */
        pmx_cmd2,    /* [2x] OEM custom commands */
        pmx_cmd3,    /* [3x] OEM custom commands */
        pmx_cmd4,    /* [4x] OEM custom commands */
        pmx_cmd5,    /* [5x] OEM custom commands */
        pmx_cmd6,    /* [6x] OEM custom commands */
        pmx_cmd7,    /* [7x] OEM custom commands */
        #if EXTEND_PM1_SUPPORTED
        Ext_Cmd8,     /* [8x] Extended commands */
        Ext_Cmd8,     /* [9x] Extended commands */
        #else // EXTEND_PM1_SUPPORTED
        pmx_cmd8,    /* [8x] OEM custom commands */
        pmx_cmd9,    /* [9x ]OEM custom commands */
        #endif // EXTEND_PM1_SUPPORTED
        pmx_cmda,    /* [Ax] OEM custom commands */
        #if EXTEND_PM1_SUPPORTED
        Ext_Cmdb,
        #else // EXTEND_PM1_SUPPORTED
        pmx_cmdb,    /* [Bx] OEM custom commands */
        #endif // EXTEND_PM1_SUPPORTED
        pmx_cmdc,    /* [Cx] OEM custom commands */
        pmx_cmdd,    /* [Dx] OEM custom commands */
        pmx_cmde,    /* [Ex] Extended commands */
        pmx_cmdf     /* [Fx] OEM custom commands */
    };

    const FUNCT_PTR_W_HPBBB pm1dat_table[16] =
    {
        pmx_dat0,    /* [0x] Extended commands */
        pmx_dat1,    /* [1x] OEM custom commands */
        pmx_dat2,    /* [2x] OEM custom commands */
        pmx_dat3,    /* [3x] OEM custom commands */
        pmx_dat4,    /* [4x] OEM custom commands */
        pmx_dat5,    /* [5x] OEM custom commands */
        pmx_dat6,    /* [6x] OEM custom commands */
        pmx_dat7,    /* [7x] OEM custom commands */
        #if EXTEND_PM1_SUPPORTED
        Ext_Dat8,     /* [8x] Extended commands */
        Ext_Dat8,     /* [9x] Extended commands */
        #else // EXTEND_PM1_SUPPORTED
        pmx_dat8,    /* [8x] OEM custom commands */
        pmx_dat9,    /* [9x] OEM custom commands */
        #endif // EXTEND_PM1_SUPPORTED
        pmx_data,    /* [Ax] OEM custom commands */
        #if EXTEND_PM1_SUPPORTED
        Ext_Datb,     /* [Bx] Extended commands */
        #else // EXTEND_PM1_SUPPORTED
        pmx_datb,    /* [Bx] Extended commands */
        #endif // EXTEND_PM1_SUPPORTED
        pmx_datc,    /* [Cx] OEM custom commands */
        pmx_datd,    /* [Dx] OEM custom commands */
        pmx_date,    /* [Ex] Extended commands */
        pmx_datf     /* [Fx] OEM custom commands */
    };

    const FUNCT_PTR_W_HPBB pm2cmd_table[16] =
    {
        pmx_cmd0,    /* [0x] Extended commands */
        pmx_cmd1,    /* [1x] OEM custom commands */
        pmx_cmd2,    /* [2x] OEM custom commands */
        pmx_cmd3,    /* [3x] OEM custom commands */
        pmx_cmd4,    /* [4x] OEM custom commands */
        pmx_cmd5,    /* [5x] OEM custom commands */
        pmx_cmd6,    /* [6x] OEM custom commands */
        pmx_cmd7,    /* [7x] OEM custom commands */
        #if EXTEND_PM2_SUPPORTED
        Ext_Cmd8,     /* [8x] Extended commands */
        Ext_Cmd8,     /* [9x] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_cmd8,    /* [8x] OEM custom commands */
        pmx_cmd9,    /* [9x ]OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_cmda,    /* [Ax] OEM custom commands */
        #if EXTEND_PM2_SUPPORTED
        Ext_Cmdb,
        #else // EXTEND_PM2_SUPPORTED
        pmx_cmdb,    /* [Bx] OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_cmdc,    /* [Cx] OEM custom commands */
        pmx_cmdd,    /* [Dx] OEM custom commands */
        pmx_cmde,    /* [Ex] Extended commands */
        pmx_cmdf     /* [Fx] OEM custom commands */
    };

    const FUNCT_PTR_W_HPBBB pm2dat_table[16] =
    {
        pmx_dat0,    /* [0x] Extended commands */
        pmx_dat1,    /* [1x] OEM custom commands */
        pmx_dat2,    /* [2x] OEM custom commands */
        pmx_dat3,    /* [3x] OEM custom commands */
        pmx_dat4,    /* [4x] OEM custom commands */
        pmx_dat5,    /* [5x] OEM custom commands */
        pmx_dat6,    /* [6x] OEM custom commands */
        pmx_dat7,    /* [7x] OEM custom commands */
        #if EXTEND_PM2_SUPPORTED
        Ext_Dat8,     /* [8x] Extended commands */
        Ext_Dat8,     /* [9x] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_dat8,    /* [8x] OEM custom commands */
        pmx_dat9,    /* [9x] OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_data,    /* [Ax] OEM custom commands */
        #if EXTEND_PM2_SUPPORTED
        Ext_Datb,     /* [Bx] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_datb,    /* [Bx] Extended commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_datc,    /* [Cx] OEM custom commands */
        pmx_datd,    /* [Dx] OEM custom commands */
        pmx_date,    /* [Ex] Extended commands */
        pmx_datf     /* [Fx] OEM custom commands */
    };

    const FUNCT_PTR_W_HPBB pm3cmd_table[16] =
    {
        pmx_cmd0,    /* [0x] Extended commands */
        pmx_cmd1,    /* [1x] OEM custom commands */
        pmx_cmd2,    /* [2x] OEM custom commands */
        pmx_cmd3,    /* [3x] OEM custom commands */
        pmx_cmd4,    /* [4x] OEM custom commands */
        pmx_cmd5,    /* [5x] OEM custom commands */
        pmx_cmd6,    /* [6x] OEM custom commands */
        pmx_cmd7,    /* [7x] OEM custom commands */
        #if EXTEND_PM3_SUPPORTED
        Ext_Cmd8,     /* [8x] Extended commands */
        Ext_Cmd8,     /* [9x] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_cmd8,    /* [8x] OEM custom commands */
        pmx_cmd9,    /* [9x ]OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_cmda,    /* [Ax] OEM custom commands */
        #if EXTEND_PM3_SUPPORTED
        Ext_Cmdb,
        #else // EXTEND_PM2_SUPPORTED
        pmx_cmdb,    /* [Bx] OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_cmdc,    /* [Cx] OEM custom commands */
        pmx_cmdd,    /* [Dx] OEM custom commands */
        pmx_cmde,    /* [Ex] Extended commands */
        pmx_cmdf     /* [Fx] OEM custom commands */
    };

    const FUNCT_PTR_W_HPBBB pm3dat_table[16] =
    {
        pmx_dat0,    /* [0x] Extended commands */
        pmx_dat1,    /* [1x] OEM custom commands */
        pmx_dat2,    /* [2x] OEM custom commands */
        pmx_dat3,    /* [3x] OEM custom commands */
        pmx_dat4,    /* [4x] OEM custom commands */
        pmx_dat5,    /* [5x] OEM custom commands */
        pmx_dat6,    /* [6x] OEM custom commands */
        pmx_dat7,    /* [7x] OEM custom commands */
        #if EXTEND_PM3_SUPPORTED
        Ext_Dat8,     /* [8x] Extended commands */
        Ext_Dat8,     /* [9x] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_dat8,    /* [8x] OEM custom commands */
        pmx_dat9,    /* [9x] OEM custom commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_data,    /* [Ax] OEM custom commands */
        #if EXTEND_PM3_SUPPORTED
        Ext_Datb,     /* [Bx] Extended commands */
        #else // EXTEND_PM2_SUPPORTED
        pmx_datb,    /* [Bx] Extended commands */
        #endif // EXTEND_PM2_SUPPORTED
        pmx_datc,    /* [Cx] OEM custom commands */
        pmx_datd,    /* [Dx] OEM custom commands */
        pmx_date,    /* [Ex] Extended commands */
        pmx_datf     /* [Fx] OEM custom commands */
    };


void Srvc_Pm1cmd(BYTE command_num)
{
    WORD data;

    Hif_Var[HIF_PM1].Cmd_Byte = 0;  /* Clear any previous command. */
    Hif_Var[HIF_PM1].Tmp_Load = 0;  /* Initialize Tmp_Load. */

    DEBUG_PRINTF("PM1 Cmd: 0x%02x\n", command_num);

    /* Dispatch to command handler. */
    #if ACPI_SUPPORTED
    if ( (command_num >= FIRST_ACPI_CMD) && (command_num <= LAST_ACPI_CMD) )
    {
        /* For ACPI Embedded Controller Commands. */
        data = ACPI_Cmd((BYTE) command_num);
        if (data != (WORD) ~0)
        {
            Data_To_Host2((BYTE) data);/* Send data to Host. */
            ACPI_Gen_Int();            /* Generate interrupt. */
        }
    }
    else
    #endif
    {
        data = (pm1cmd_table[command_num >> 4])(&Hif_Var[HIF_PM1], (BYTE) command_num, HIF_PM1);

        if (data != (WORD) ~0)
        {
            Data_To_Host2((BYTE) data);/* Send data to Host. */
        }
    }
}

void Srvc_Pm2cmd(BYTE command_num)
{
    WORD data;

    Hif_Var[HIF_PM2].Cmd_Byte = 0;  /* Clear any previous command. */
    Hif_Var[HIF_PM2].Tmp_Load = 0;  /* Initialize Tmp_Load. */

    DEBUG_PRINTF("PM2 Cmd: 0x%02x\n", command_num);

    /* Dispatch to command handler. */
    data = (pm2cmd_table[command_num >> 4])(&Hif_Var[HIF_PM2], (BYTE) command_num, HIF_PM2);

    if (data != (WORD) ~0)
    {
        Data_To_Host3((BYTE) data);/* Send data to Host. */
    }
}

void Srvc_Pm3cmd(BYTE command_num)
{
    WORD data;

    Hif_Var[HIF_PM3].Cmd_Byte = 0;  /* Clear any previous command. */
    Hif_Var[HIF_PM3].Tmp_Load = 0;  /* Initialize Tmp_Load. */

    DEBUG_PRINTF("PM3 Cmd: 0x%02x\n", command_num);

    /* Dispatch to command handler. */
    data = (pm3cmd_table[command_num >> 4])(&Hif_Var[HIF_PM3], (BYTE) command_num, HIF_PM3);

    if (data != (WORD) ~0)
    {
        Data_To_Host4((BYTE) data);/* Send data to Host. */
    }
}

/* ---------------------------------------------------------------
   Name: pmx_cmd0

   Handle "0x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd0(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd0(hvp, command_num, hif));
}


/* ---------------------------------------------------------------
   Name: pmx_cmd1

   Handle "1x" controller commands from HOST through PM2 Channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd1(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd1(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pc_cmd2

   Handle "2x" controller commands from Host through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd2(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd2(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pc_cmd3

   Handle "3x" controller commands from Host through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd3(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd3(hvp, command_num, hif));
}


/* ---------------------------------------------------------------
   Name: pmx_cmd4

   Handle "4x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd4(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd4(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmd5

   Handle "5x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd5(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd5(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmd6

   Handle "6x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd6(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd6(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmd7

   Handle "7x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd7(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd7(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmd8

   Handle "8x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd8(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd8(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmd9

   Handle "9x" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmd9(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmd9(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmda

   Handle "Ax" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmda(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmda(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmdb

   Handle "Bx" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmdb(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmdb(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmdc

   Handle "Cx" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmdc(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmdc(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmdd

   Handle "Dx" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmdd(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmdd(hvp, command_num, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_cmde

   Handle "Ex" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmde(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmde(hvp, command_num, hif));
}


/* ---------------------------------------------------------------
   Name: pmx_cmdf

   Handle "Fx" controller commands from HOST through PM2 channel.

   Input: Command #

   Returns: a value with all bits set if no data is to be sent to the host;
            otherwise, returns the data to be sent to the host.
   --------------------------------------------------------------- */
static WORD pmx_cmdf(HIF_VAR* hvp, BYTE command_num, BYTE hif)
{
   return(Gen_Hookc_Pmx_Cmdf(hvp, command_num, hif));
}


/* ---------------------------------------------------------------
   Name: Srvc_Pcdat2

   Handle incoming data byte from Host write to port 60h, 62h or from
   write to third Host interface.  The byte is either data
   that is required to complete processing of a controller command
   from the Host, or it is data that should be passed on to the
   keyboard (i.e., it is a keyboard command).

   Input: Data byte
            Cmd_Byte is non-zero if the data
            is part of a controller command.
   --------------------------------------------------------------- */
void Srvc_Pm1dat(BYTE data)
{
   BYTE command;
   WORD rval;


   command = Hif_Var[HIF_PM1].Cmd_Byte;  /* Get the command byte. */
   Hif_Var[HIF_PM1].Cmd_Byte = 0;        /* Clear out the command byte. */
   DEBUG_PRINTF("PM1 Cmd&Data: 0x%02x, 0x%02x\n", command, data);

   if (command == 0)
   {
      /* At this point, "data" is actually a command for the keyboard. */

      ;  /* Ignore it. */
   }
   else
   {
      /* At this point, "data" is the data part of a controller command. */

      #if ACPI_SUPPORTED
      if ( (command >= FIRST_ACPI_CMD) && (command <= LAST_ACPI_CMD) )
      {
         /* For ACPI Embedded Controller Commands. */
         rval = ACPI_Dat(command, data);
         if (rval != (WORD) ~0)
         {
            Data_To_Host2((BYTE) rval);/* Send data to Host. */
            ACPI_Gen_Int();            /* Generate interrupt. */
         }
      }
      else
      #endif
      {
         if ( (command >= FIRST_HIF2_CMD) && (command <= LAST_HIF2_CMD) )
         {
            /* For commands that are only accessed
               through the secondary Host interface. */
            rval = HIF2_Dat(command, data);
         }
         else
         {
            rval = (pm1dat_table[command >> 4])(&Hif_Var[HIF_PM1], (BYTE) command, (BYTE) data, HIF_PM1);
         }

         if (rval != (WORD) ~0)
         {
            Data_To_Host2((BYTE) rval);/* Send data to Host. */
         }
      }
   }
}

void Srvc_Pm2dat(BYTE data)
{
   BYTE command;
   WORD rval;

   command = Hif_Var[HIF_PM2].Cmd_Byte;  /* Get the command byte. */
   Hif_Var[HIF_PM2].Cmd_Byte = 0;        /* Clear out the command byte. */
   DEBUG_PRINTF("PM2 Cmd&Data: 0x%02x, 0x%02x\n", command, data);

   if (command == 0)
   {
      /* At this point, "data" is actually a command for the keyboard. */

      ;  /* Ignore it. */
   }
   else
   {
      /* At this point, "data" is the data part of a controller command. */

      if ( (command >= FIRST_HIF3_CMD) && (command <= LAST_HIF3_CMD) )
      {
         /* For commands that are only accessed
            through the third Host interface. */
         rval = HIF3_Dat(command, data);
      }
      else
      {
         rval = (pm2dat_table[command >> 4])(&Hif_Var[HIF_PM2], (BYTE) command, (BYTE) data, HIF_PM2);
      }

      if (rval != (WORD) ~0)
      {
         Data_To_Host3((BYTE) rval);/* Send data to Host. */
      }
   }
}

void Srvc_Pm3dat(BYTE data)
{
   BYTE command;
   WORD rval;

   command = Hif_Var[HIF_PM3].Cmd_Byte;  /* Get the command byte. */
   Hif_Var[HIF_PM3].Cmd_Byte = 0;        /* Clear out the command byte. */

   DEBUG_PRINTF("PM3 Cmd&Data: 0x%02x, 0x%02x\n", command, data);

   if (command == 0)
   {
      /* At this point, "data" is actually a command for the keyboard. */

      ;  /* Ignore it. */
   }
   else
   {
      /* At this point, "data" is the data part of a controller command. */

      if ( (command >= FIRST_HIF4_CMD) && (command <= LAST_HIF4_CMD))
      {
         /* For commands that are only accessed
            through the third Host interface. */
         rval = HIF4_Dat(command, data);
      }
      else
      {
         rval = (pm3dat_table[command >> 4])(&Hif_Var[HIF_PM3], (BYTE) command, (BYTE) data, HIF_PM3);
      }

      if (rval != (WORD) ~0)
      {
         Data_To_Host4((BYTE) rval);/* Send data to Host. */
      }
   }
}


/* ---------------------------------------------------------------
   Name: pmx_dat0

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 00h to 0Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat0(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat0(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat1

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 10h to 1Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat1(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat1(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat2

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 20h to 2Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat2(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat2(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat3

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 30h to 3Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat3(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat3(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat4

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 40h to 4Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat4(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat4(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat5

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 50h to 5Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat5(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat5(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat6

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 60h to 6Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat6(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat6(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat7

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 70h to 7Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat7(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat7(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat8

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 80h to 8Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat8(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat8(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_dat9

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of 90h to 9Fh extended commands.
   --------------------------------------------------------------- */
WORD pmx_dat9(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Dat9(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_data

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of A0h to AFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_data(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Data(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_datb

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of B0h to BFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_datb(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Datb(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_datc

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of C0h to CFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_datc(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Datc(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_datd

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of D0h to DFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_datd(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Datd(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_date

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of E0h to EFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_date(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Date(hvp, command_num, data, hif));
}

/* ---------------------------------------------------------------
   Name: pmx_datf

   Input: command == command number.
          data == data byte.

   At this point, Cmd_Byte == 0.

   Handle the data part of F0h to FFh extended commands.
   --------------------------------------------------------------- */
WORD pmx_datf(HIF_VAR* hvp, BYTE command_num, BYTE data, BYTE hif)
{
   return(Gen_Hookc_Pmx_Datf(hvp, command_num, data, hif));
}


