/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMMAIN.C - OEM Main Tasks Handler
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#define OEMMAIN_C
/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "swtchs.h"
#include "types.h"
#include "com_defs.h"
#include "proc.h"
#include "regs.h"
#include "oem.h"
#include "host_if.h"
#include "purhook.h"
#include "purdat.h"
#include "oemmain.h"
#include "pwrseq.h"
#include "pindef.h"
#include "smb_tran.h"
#include "oemdat.h"
#include "events.h"
#include "d_to_a.h"
#include "regs.h"
#include "scan.h"
#include "i2c.h"
#include "cr_uart.h"

#if PECI_SUPPORTED
void PECI_GetTemp(void);
#endif

extern MS_TIMER millisecond_timer;
extern void HostModuleInit(void);

/* ---------------------------------------------------------------
   Name: Hookc_Service_OEM_0

   To cause this hook to be called, post
   an OEM service 0 request as follows:

      Service.bit.OEM_SRVC_0 = 1;

   --------------------------------------------------------------- */
void Hookc_Service_OEM_0(void)
{
   /* At this point, the service flag has already been cleared. */
   Service_OEM_10mS();
}

/* ---------------------------------------------------------------
   Name: Hookc_Service_OEM_1

   To cause this hook to be called, post
   an OEM service 1 request as follows:

      Service.bit.OEM_SRVC_1 = 1;

   --------------------------------------------------------------- */
void Hookc_Service_OEM_1(void)
{
   /* At this point, the service flag has already been cleared. */
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Service_OEM_2

   To cause this hook to be called, post
   an OEM service 2 request as follows:

      Service.bit.OEM_SRVC_2 = 1;

   --------------------------------------------------------------- */
void Hookc_Service_OEM_2(void)
{
   /* At this point, the service flag has already been cleared. */
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Service_1mS

   This hook is called at a one millisecond rate.
   --------------------------------------------------------------- */
BYTE ReadFromSIB(WORD module, BYTE port);

void Hookc_Service_1mS(void)
{
    int index;

    if (millisecond_timer.field.y >= 3)
    {  /* 10mS elapse. */
        Service_OEM_10mS();
    }
    HandlePwrSeq();

    // check if Lreset occurred
    if(IS_BIT_SET(WKPND5, 7))
    {
        WKPCL5 = MASK(bit7);  // Clear pending MIU57
        ConfigHostModule = 1; // need to re-config host modules
    }
    if (ConfigHostModule == 1)
    {
        // Wait for VDD ON and LRESET OFF then config host modules
        if ((MSWCTL1& 0x06) == 2)
        {
            // re-config host modules
            HostModuleInit();
            //SysStateChanged(ResumeS0);
            ConfigHostModule = 0;
        }
    }
#if CR_UART_SUPPORTED
    ConsoleCommandHandling();
#endif
}

/* ---------------------------------------------------------------
   Name: Service_OEM_10mS

   --------------------------------------------------------------- */
void Service_OEM_10mS(void)
{
    WORD KBSOUT0_val;

    if (millisecond_timer.field.y >= 4)
    {  /* 100mS elapse. */
        Service_OEM_100mS();
    }
    Handle_Events();
}

/* ---------------------------------------------------------------
   Name: Service_OEM_100mS

   --------------------------------------------------------------- */
void Service_OEM_100mS(void)
{
    if (millisecond_timer.field.y >= 5)
    {  /* 500mS elapse. */
        Service_OEM_500mS();
    }
}

/* ---------------------------------------------------------------
   Name: Service_OEM_500mS

   --------------------------------------------------------------- */
void Service_OEM_500mS(void)
{
    if (millisecond_timer.field.y >= 6)
    {  /* 1000mS elapse. */
        Service_OEM_1000mS();
    }
}

/* ---------------------------------------------------------------
   Name: Service_OEM_1000mS

   --------------------------------------------------------------- */
void Service_OEM_1000mS(void)
{
    BYTE i;
#if PECI_SUPPORTED

    PECI_GetTemp();

#endif // PECI_SUPPORTED

}

/* ---------------------------------------------------------------
   Name: Hookc_Service_Ext_Irq

   This hook is called in the main loop when an external interrupt
   has occurred.
   --------------------------------------------------------------- */
void Hookc_Service_Ext_Irq(void)
{

}

/*------------------------------------------------------------------------------
 * Function:
 *      Ticker_Update
 *
 * Description:
 *      Updated the power sequence time ticker.
 *
 * Parameters:
 *      None
 *
 * Return Value:
 *      bool 0 - Not timeout or already timeout.
 *      bool 1 - Timeuot now.
 *----------------------------------------------------------------------------*/
bool Ticker_Update_32(DWORD *pntr)
{
    bool TMOUT = FALSE;
    if (*pntr)
    {
        *pntr-=1;
        if (*pntr == 0)
            TMOUT = TRUE;
    }
    return (TMOUT);
}

/*------------------------------------------------------------------------------
 * Function:
 *      Ticker_Update
 *
 * Description:
 *      Updated the power sequence time ticker.
 *
 * Parameters:
 *      None
 *
 * Return Value:
 *      bool 0 - Not timeout or already timeout.
 *      bool 1 - Timeuot now.
 *----------------------------------------------------------------------------*/
bool Ticker_Update_8(BYTE *pntr)
{
    bool TMOUT = FALSE;
    if (*pntr)
    {
        *pntr-=1;
        if (*pntr == 0)
            TMOUT = TRUE;
    }
    return (TMOUT);
}

/* ---------------------------------------------------------------
   Name: Read_Port_Pin

   Read IO port pin selected by the data byte.

   Input: Bits 7 through 4 of data byte contains the port code.
          Bits 2 through 0 of data byte contains the pin number.

   Returns: the IO port pin read in LSB.
   --------------------------------------------------------------- */
FLAG Read_Port_Pin(BYTE data)
{
    BYTE Port_State, mask;
    FLAG rval;

    Port_State = Extended_Read_Port((data >> 4), 0);
    mask = Byte_Mask((SMALL) (data & 7));

    rval = ((Port_State & mask) >> (data & 7));

    return(rval);
}


/* ---------------------------------------------------------------
   Name: OEM_check_reference_clock

   Check that reference clock for LFCG calibration is exist
   --------------------------------------------------------------- */
bool OEM_check_reference_clock(void)
{
    return TRUE;
}
