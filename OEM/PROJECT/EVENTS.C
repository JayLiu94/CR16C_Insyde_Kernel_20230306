/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     EVENTS.C - OEM Events Handling
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/


/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "types.h"
#include "com_defs.h"
#include "proc.h"
#include "pindef.h"
#include "oemmain.h"
#include "events.h"
#include "oemdat.h"
#include "pwrseq.h"
#include "d_to_a.h"
#include "purxlt.h"
#include "purscn.h"
#include "purfunct.h"


typedef volatile struct _EVENT_DESCRIPTOR
{
    BYTE PortPin;    /* Port Pin defined following kernel code. */
    BYTE Bounce;     /* Debounce time base is 10mS. */
} EVENT_DESCRIPTOR;


static const EVENT_DESCRIPTOR Events_Table[] =
{
    {EC_PWRSW, 20},
    {BAT_IN, 10},
    {SW_AUD_VU, 10},
    {SW_AUD_VD, 10},
    {SW_AUD_STOP, 10},
    {SW_AUD_PAUSE, 10},
    {SW_AUD_MUTE, 10},
    {WIRELESS_EN, 10},
    {INTERNET_KEY, 10},
    {RF_KILL, 10},
    {LIDSW, 10},
    {AC_IN, 10},
    {TEMP_ALERT, 0}
};

#define Table_Size (sizeof(Events_Table) / sizeof(EVENT_DESCRIPTOR))

BYTE EventBounce[Table_Size];

BYTE PreEventState[(Table_Size >> 3) + 1];


/* ---------------------------------------------------------------
   Name: EventsInit

   Events handle initialized.

   Purpose: Service the OEM buttons.
   --------------------------------------------------------------- */
void EventsInit(void)
{
    PreEventState[0] = 0x03;    //bit 0 --> EC_PWRSW
}


/* ---------------------------------------------------------------
   Name: Handle_Events

   This routine is called at a ten millisecond rate.

   Purpose: Service the OEM buttons.
   --------------------------------------------------------------- */
void Handle_Events(void)
{
    SMALL index;
    FLAG pin_state;
    index = 0;
    FLAG prepin_state;

    do
    {
        pin_state = Read_Port_Pin(Events_Table[index].PortPin);
        prepin_state = PreEventState[index >> 3] & Byte_Mask((SMALL) (index & 7));
        // check if there is a change
        if ((pin_state << (index & 7))
            != (PreEventState[index >> 3] & Byte_Mask((SMALL) (index & 7))))
        {
            // updated pin status
            PreEventState[index >> 3] &= (~Byte_Mask((SMALL) (index & 7)));
            PreEventState[index >> 3] |= (pin_state << (index & 7));

            if (Events_Table[index].Bounce == 0)
            {
                // no need to debounce - call ProcEventActive to handle
                ProcEventActive(Events_Table[index].PortPin, pin_state);
            }
            else
            {
                // need to debounce
                EventBounce[index] = Events_Table[index].Bounce;
            }
        }
        else if (Ticker_Update_8(&EventBounce[index]))
        {
            // debounce end - call ProcEventActive to handle
            ProcEventActive(Events_Table[index].PortPin, pin_state);
        }
        index++;
    }
    while(index < Table_Size);
}



/* ---------------------------------------------------------------
   Name: ProcEventActive

   This is used for the button function description when it actived.

   Purpose: Described the button function.
   Input:
        data  -> Button active now.
        state -> Pin state (high or low)
   --------------------------------------------------------------- */
void ProcEventActive(BYTE data, FLAG state)
{
    WORD Event = 0;
    BYTE SendKey = 0;
    switch (data)
    {
        case EC_PWRSW:
            // make state - Power button state is active
            if (SysState >= InS4)
            {
                // system in S4/S5 (turn on)
                PwrSeq_Init(Seq_Step1, 5);
            }
            else
            {
                // system in S0 or S3 (turn off)
                // ...
            }
            break;
        case BAT_IN:
            if (!state)
            {
                // make state
                PwrState.bit.BATPres = 1;
            }
            else
            {
                // break state
                PwrState.bit.BATPres = 0;
            }
            break;
        case SW_AUD_VU:
            // SendKey = MVOL_UP;
            break;
        case SW_AUD_VD:
            // SendKey = MVOL_DN;
            break;
        case SW_AUD_STOP:
            // SendKey = MPAUSE;
            break;
        case SW_AUD_PAUSE:
            // SendKey = MPAUSE;
            break;
        case SW_AUD_MUTE:
            // SendKey = MMUTE;
            break;
        case WIRELESS_EN:
            // Event = BT_WLAN_EVENT;
            break;
        case INTERNET_KEY:
            // SendKey = WEB_NET;
            break;
        case RF_KILL:
            // Event = RFKill_EVENT;
            break;
        case LIDSW:
            // Event = LID_EVENT;
            break;
        case AC_IN:
            /* AC adaptor status changed. */
            // Event = AC_EVENT;
            if (state)
            {
                PwrState.bit.ACPres = 1;
            }
            else
            {
                PwrState.bit.ACPres = 0;
            }
            break;
        case TEMP_ALERT:
            // Event = ALERT_EVENT;
            break;
    }

    if (SysState == InS0)
    {
        if (SendKey != 0)
        {   /* Scan Code need to be sent out. */
            Send_OEM_Key(SendKey, state);
        }

        if (Event != 0)
        {   /* SCI event need to be send out to OS. */
            System_Control_Function(Event, MAKE_EVENT);
        }
    }
}


void Send_OEM_Key(BYTE key, BYTE Event)
{
    if ((Event == MAKE_EVENT) | (Event == REPEAT_EVENT))
        Send_Key((BYTE) (key & 0xFF), MAKE_EVENT);
    else
        Send_Key((BYTE) (key & 0xFF), BREAK_EVENT);

    Disable_Irq();
    if (Timer_B.bit.SEND_ENABLE == 0)   /* 1 = Device send request.
                                                          (Request already set.) */
    {
        if (Check_Scan_Transmission())   /* Is data available? */
            Start_Scan_Transmission();    /* Yes, start new transmission. */
    }
    Enable_Irq();
}




