/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMPECI.C - OEM PECI
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#include "swtchs.h"
#include "types.h"
#include "peci.h"
#include "oem.h"


#if PECI_SUPPORTED



void OEM_PECI_CallBack (
    PECI_COMMAND_T    command, 
    PECI_DATA_SIZE_T  data_size,
    PECI_TRANS_DONE_T sts, 
    DWORD             data_low,
    DWORD             data_high);

// PECI Processor Socket Address                                                                           
#define PECI_CLIENT_ADDR        0x30

// PECI Domain                                                                                             
#define PECI_DOMAIN             0x0

BYTE ERROR_OCCUR;

// PECI mailbox buffer for receiving data from HOST and sending data to HOST
BYTE PECI_MAILBOX_BUF[30];

extern BYTE LAST_AWFCS;
extern BYTE LAST_COMPLETION_CODE;








void OEM_PECI_Init(void)
{
    // Initialize the PECI module                                                                          
    PECI_Init(OEM_PECI_CallBack, 0);

    // Configure PECI domain and client address                                                            
    PECI_SetDomain(PECI_DOMAIN);
    PECI_SetAddress(PECI_CLIENT_ADDR);    
}


void OEM_PECI_MAILBOX_Interface()
{
    DWORD repeat_cycle;
    BYTE  repeat_interval;
    BYTE  stop_on_error;    
    BYTE  write_length;
    BYTE  read_length;
    BYTE  command_code;
    DWORD data_0;
    DWORD data_1;

    ERROR_OCCUR = 0;
    
    // bytes 0->3 - Repeat Cycle - how many times to execute the PECI command
    repeat_cycle = (*(DWORD *)&PECI_MAILBOX_BUF[0]);

    if (repeat_cycle == 0)
    {
        return;
    }
    
    // byte 4 - Repeat Interval - delay (in 10ms) between each repetition of the PECI command
    repeat_interval = PECI_MAILBOX_BUF[4];
    
    // byte 5 - Stop On Error - whether to stop on error or continue all repeat_cycle
    stop_on_error = PECI_MAILBOX_BUF[5];
    
    // byte 6 - PECI Client Address - target PECI client address
    PECI_SetAddress(PECI_MAILBOX_BUF[6]);

    // byte 7 - Write Length - command write data block length
    write_length = PECI_MAILBOX_BUF[7];

    // byte 8 - Read Length - expected length for the return data from PECI client
    read_length = PECI_MAILBOX_BUF[8];
    
    // byte 9 - is the command code 
    command_code = PECI_MAILBOX_BUF[9];

    // bytes 10-13 - data_0
    data_0 = (*(DWORD *)&PECI_MAILBOX_BUF[10]);

    // bytes 14-17 - data_1
    data_1 = (*(DWORD *)&PECI_MAILBOX_BUF[14]);



    // send the PECI transaction repeat_cycle times -
    while (repeat_cycle)
    {
        repeat_cycle--;
        
        // send the PECI transaction
        PECI_Trans (
            write_length, 
            read_length, 
            command_code, 
            data_0, 
            data_1);
        
        // repeat_interval delay
        Microsecond_Delay(repeat_interval * 10000); // repeat_interval is in 10 milli sec units (10000 micro sec)
        
        // meanwhile OEM_PECI_callback is called from PECI interrupt -
        // if error - it sets ERROR_OCCUR
        // else - copy return data to PECI_MAILBOX_BUF buffer
        
        if (stop_on_error && ERROR_OCCUR) 
        {
            break;
        }       
        else
        {
            // send PECI_MAILBOX_BUF back to host
        }
    }

    
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        OEM_PECI_CallBack                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command   - PECI latest command sent.                                                  */
/*                  data_size - PECI received data size.                                                   */
/*                  sts       - PECI transaction status.                                                   */
/*                  data_low  - Low 32-bit data read.                                                      */
/*                  data_high - High 32-bit data read.                                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is called upon PECI HW transaction completion.                            */
/*                  It retrieves the latest PECI temperature reading.                                      */
/*                  The temperature representation is changed from +-512°C to +-128°C, where:              */
/*                  ?Integral (8-bit) part is in high byte.                                               */
/*                  ?Fractional (8-bit) part is in low byte.                                              */
/*---------------------------------------------------------------------------------------------------------*/
void OEM_PECI_CallBack (
    PECI_COMMAND_T    command, 
    PECI_DATA_SIZE_T  data_size,
    PECI_TRANS_DONE_T sts, 
    DWORD             data_low,
    DWORD             data_high 
)
{
    // byte 7 - Write Length 
    BYTE  write_length = PECI_MAILBOX_BUF[7];
    
        
    if (sts != PECI_TRANS_DONE_OK)
    {
        ERROR_OCCUR = 1;
    }
    else if ((command == PECI_COMMAND_WR_PKG_CFG) || (command == PECI_COMMAND_WR_PCI_CFG_LOCAL))
    {
        // need to add - Assured Write FCS and completion code        
        PECI_MAILBOX_BUF[write_length + 3] = LAST_AWFCS;
        PECI_MAILBOX_BUF[write_length + 4] = LAST_COMPLETION_CODE;
        
        // copy received data to PECI_MAILBOX_BUF[wr_length + 5]        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 5]) = data_low;        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 9]) = data_high;
        
    }
    else if ((command == PECI_COMMAND_PING) || (command == PECI_COMMAND_GET_DIB) || (command == PECI_COMMAND_GET_TEMP))
    {
        // no completion code, no Assured Write FCS
        
        // copy received data to PECI_MAILBOX_BUF[wr_length + 3]        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 3]) = data_low;        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 7]) = data_high;        
    }
    else
    {
        // need to add - completion code        
        PECI_MAILBOX_BUF[write_length + 3] = LAST_COMPLETION_CODE;
        
        // copy received data to PECI_MAILBOX_BUF[wr_length + 4]        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 4]) = data_low;        
        (*(DWORD *)&PECI_MAILBOX_BUF[write_length + 8]) = data_high;                
    }
}




#endif // PECI_SUPPORTED


