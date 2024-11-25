/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     PWRSEQ.C - Power Sequence & System State Handle
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/


/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "purdat.h"
#include "host_if.h"
#include "com_defs.h"
#include "types.h"
#include "regs.h"
#include "oemmain.h"
#include "pindef.h"
#include "pwrseq.h"
#include "proc.h"
#include "oemdat.h"
#include "oem.h"
#include "raminit.h"

extern void HostModuleInit(void);
void SysStateChanged(BYTE State);
void SystemOffInit(void);
void SystemOnInit(void);
void StandbyIO(void);
bool OEM_check_reference_clock(void);

/*------------------------------------------------------------------------------
 * Function:
 *      PwrSeq_Init
 *
 * Description:
 *      Module Initialization. Keeps power sequence to be called from OEM timer
 *      ticker (HandlePwrSeq)
 *
 * Parameters:
 *      pwrseq_handle to be called after ticker msec
 *
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void PwrSeq_Init(FUNCT_PTR_V_V power_sequence_handle, BYTE ticker)
{
    pwrseq_handle = power_sequence_handle;
    PS_Ticker = ticker;
}

/*------------------------------------------------------------------------------
 * Function:
 *      HandlePwrSeq
 *
 * Description:
 *      Handle and dispatch power sequence and monitor system state
 *      called every 1mS
 *
 * Parameters:
 *      None
 *
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void HandlePwrSeq(void)
{
    if (pwrseq_handle != NULL)
    {
        // check if the time had come
        if (Ticker_Update_32(&PS_Ticker))
        {
            // call next Power Sequence step
            pwrseq_handle();
        }
    }
}

/*------------------------------------------------------------------------------
 * Function:
 *      SeqNullHandle
 *
 * Description:
 *      Power Sequence null handle routine for unexpected entry.
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void SeqNullHandle(void)
{
    ;
}

/*------------------------------------------------------------------------------
 * Function:
 *      Seq_Step1
 *
 * Description:
 *      Turn on required power ports, set delay time and next step
 *
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void Seq_Step1(void)
{
    // Turn on required power ports
    // Extended_Write_Port(...)

    // set next power sequence step
    pwrseq_handle = Seq_Step2;
    // set delay (in millisecond) till next step
    PS_Ticker = 5;
}

/*------------------------------------------------------------------------------
 * Function:
 *      Seq_Step2
 *
 * Description:
 *      Wait for some value at some port
 *      Turn on required power ports, set delay time and next step
 *
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void Seq_Step2(void)
{
    // wait till some port get some value, then proceed
    // if (Read_Port_Pin(...))
    {
        // write required ports
        // Extended_Write_Port(...)

        // set next power sequence step
        pwrseq_handle = Seq_Step3;
        // set delay (in millisecond) till next step
        PS_Ticker = 10;
    }
    // else // check in next millisecond
    {
        PS_Ticker = 1;
    }
}

/*------------------------------------------------------------------------------
 * Function:
 *      Seq_Step3
 *
 * Description:
 *      Turn on required power ports, set delay time and next step according to
 *      SysState
 *
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void Seq_Step3(void)
{
    // Turn on required power ports
    // Extended_Write_Port(...)

    // set next power sequence step according to SysState
    // if (SysState == InSx)
    //      pwrseq_handle = Seq_Next;
    // else
    //      pwrseq_handle = Seq_Next1;

    // set delay (in millisecond) till next step
    PS_Ticker = 5;
}

/*------------------------------------------------------------------------------
 * Function:
 *      Seq_Active
 *
 * Description:
 *      some of the final steps in power sequence - enter S0
 *      init what needed
 *      for next step call function to monitor system state
 *
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void Seq_Active(void)
{
    SysState = InS0;
    SysStateChanged(ResumeS0);

    // perform system On initializations
    // OEM_A20_High();
    // OEM_Host_Reset_High();
    // init Fan and Thermal data ...

    pwrseq_handle = Seq_Monitor_SysState;
    PS_Ticker = 1;
}

/*------------------------------------------------------------------------------
 * Function:
 *      Seq_Monitor_SysState
 *
 * Description:
 *      monitor system state
 *
 * Parameters:
 *      None
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void Seq_Monitor_SysState(void)
{
    if (( SysState == InS0) && !Read_Port_Pin(PM_SLP_S3))       //from S0 to S3/S4
    {
        // PwrSeq_Init(...);
    }
    else if ((SysState == InS3) && !Read_Port_Pin(PM_SLP_S4))   //from S3 to S4/S5
    {
        // PwrSeq_Init(...);
    }
    else if ((SysState == InS3) && Read_Port_Pin(PM_SLP_S3))    //from S3 to S0
    {
        // PwrSeq_Init(...);
    }
    else if ((SysState == InS4) && Read_Port_Pin(PM_SLP_S3))    //from S4 to S0
    {
        // PwrSeq_Init(...);
    }

    PS_Ticker = 1;
}

/*------------------------------------------------------------------------------
 * Function:
 *      SysStateChanged
 *
 * Description:
 *      called on system state change
 *
 * Parameters:
 *      State - the new state
 *
 * Return Value:
 *      None
 *----------------------------------------------------------------------------*/
void SysStateChanged(BYTE State)
{
    switch (State)
    {

//////////  S0  //////////
        case ResumeS0:
            SysState = InS0;

            // prevent host module reset on Lreset
            SET_BIT(RSTCTL, LRESET_MODE);

            HostModuleInit();

            // release Host wait
            SET_BIT(SMC_CTRL, HOSTWAIT);

            SystemOnInit();

            // OEM_A20_High/Low();
            // OEM_Host_Reset_High/Low();

            Ccb42.bit.SYS_FLAG = 0;
            Write_System_Flag(0);

            // reset PS2 state machine
            Aux_Port1.sequence = 0;
            #if AUX_PORT2_SUPPORTED
            Aux_Port2.sequence = 0;
            #endif // AUX_PORT2_SUPPORTED
            #if AUX_PORT3_SUPPORTED
            Aux_Port3.sequence = 0;
            #endif // AUX_PORT3_SUPPORTED

            // reset mouse params
            Mouse_State[0] = Mouse_State0_Init;
            Mouse_State[1] = Mouse_State1_Init;
            Mouse_State[2] = Mouse_State2_Init;


            // check if reference clock is available
            if (OEM_check_reference_clock() == TRUE)
            {
                // reference clock is available
                CLEAR_BIT(LFCGCTL2, STOPCAL);
            }
            break;


//////////  S3  //////////
        case EntryS3:

            // stop automatic LFCG calibration before stoping external clock
            SET_BIT(LFCGCTL2, STOPCAL);
            // clear OBF indication (Host can not read data in S3)
            HIPM1CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM2CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM3CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM4CTL2 |= HIPMnCTL2_FW_OBF;

            StandbyIO();
            SysState = InS3;

            break;


//////////  S4  //////////
        case EntryS4:

            // stop automatic LFCG calibration before stoping external clock
            SET_BIT(LFCGCTL2, STOPCAL);
            // clear OBF indication (Host can not read data in S4)
            HIPM1CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM2CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM3CTL2 |= HIPMnCTL2_FW_OBF;
            HIPM4CTL2 |= HIPMnCTL2_FW_OBF;

            SystemOffInit();
            StandbyIO();

            SysState = InS4;

            break;
    }
}

/* ---------------------------------------------------------------
   Name: SystemOffInit

   Initialized variables and registers when system entry S4/S5.

   Input: None.
   Returns: None.
   --------------------------------------------------------------- */
void SystemOffInit(void)
{
    /* Initialized variables for system off or entry Hibration. */
}

/* ---------------------------------------------------------------
   Name: SystemOnInit

   Initialized variables and registers when system entry S0.

   Input: None.
   Returns: None.
   --------------------------------------------------------------- */
void SystemOnInit(void)
{
    /* Initialized variables and IOs for system on. */
}

/* ---------------------------------------------------------------
   Name: StandbyIO

   Initialized GPO pins when system entry S3.

   Input: None.
   Returns: None.
   --------------------------------------------------------------- */
void StandbyIO(void)
{
    /* served the power sequence. */
    // Extended_Write_Port ...
}



