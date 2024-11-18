/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   HID_I2C.H                                                                                             */
/*            This file contains HID over I2C implementation.                                              */
/* Project:                                                                                                */
/*            HID over I2C interface.                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include "purdat.h"
#include "swtchs.h"
#include "types.h"
#include "com_defs.h"
#include "purxlt.h"
#include "i2c.h"
#if SHL_SUPPORTED
#include "sensorhub.h"
#endif // SHL_SUPPORTED
#include "hid_i2c.h"
#include "regs.h"

#if HID_OV_I2C_SUPPORTED
#define HID_I2C_TEST_INPUT_ORDER    1   /* to test code RAM/ROM size, etc. */
#define HID_I2C_DBG_ERR             0   /* for debug purpose */



/* ========================================================================= */
/* ========================================================================= */

#if HID_I2C_DBG_ERR
BYTE Hidi2c_Err;
#endif

BYTE Hidi2c_Slave_State;
BYTE GS_report_EN;

BYTE Hidi2c_Slave_Buf[HID_I2C_BUF_SIZE];
BYTE Hidi2c_ReportID;
BYTE Hidi2c_ReportType;


// merge with Hidi2c_Slave_Buf later
struct
{
    BYTE report[HID_DESC_MAX_INPUT_LENGTH+2];
	WORD len;
} Hidi2c_Slave_InputReportBuf[2];
BYTE Hidi2c_Slave_InputReportBufIdx;

static volatile bool Hidi2c_Slave_RequestToSendInputReport;
void Hidi2c_Slave_InitReportFeature(void);
void Hidi2c_Slave_InitReportInput(void);
static void Hidi2c_Slave_Init_Irq(void);
static void Hidi2c_Slave_Assert_Irq(void);
static void Hidi2c_Slave_Release_Irq(void);
static void Hidi2c_Slave_HIR_DIR(void);
static void Hidi2c_Slave_SetPower(bool state);
static int  Hidi2c_Slave_GetReportInput(BYTE rid, BYTE * buff, WORD * size);
static int  Hidi2c_Slave_GetReportFeature(BYTE rid, BYTE * buff, WORD * size);
static int  Hidi2c_Slave_SetReportFeature(BYTE rid, BYTE * buff, WORD size);
static void memcpy(BYTE * dest, BYTE * src, WORD size);

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_Init_Irq
 * - Based on MS HID Over I2C spec, interrupt is active low, level trigger.
 *---------------------------------------------------------------------------*/
static void Hidi2c_Slave_Init_Irq(void)
{
    // init HID Over I2C INT (low active) - GPIO81
    //DEVALTA &= ~0x80;
    //P5DOUT  |= 0x08;
    P8DOUT &= ~0x02;
    P8DIR  &= ~0x02;
}

static void Hidi2c_Slave_Assert_Irq(void)
{
    P8DIR |= 0x02;
    //P5DOUT |= 0x08;
}

static void Hidi2c_Slave_Release_Irq(void)
{
    P8DIR &= ~0x02;
    //P5DOUT &= ~0x08;
}


/*-----------------------------------------------------------------------------
 * memcpy - copy "size" bytes from memory location src to dest
 *---------------------------------------------------------------------------*/
static void memcpy(BYTE * dest, BYTE * src, WORD size)
{
    WORD index;
    for (index = 0; index < size; index++)
    {
        *(dest+index) = *(src+index);
    }
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_HIR_DIR
 * - Response to HIR (Host Initiated Reset) or do DIR (Device Initiated Reset)
 *---------------------------------------------------------------------------*/
static void Hidi2c_Slave_HIR_DIR(void)
{
	Hidi2c_Slave_State = HIDI2C_STATE_INIT;
	Hidi2c_Slave_RequestToSendInputReport = FALSE;
	Hidi2c_Slave_Assert_Irq();
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_SendInputReport
 * - HID Over I2C Slave Request to send asynchronous input report over I2C
 *---------------------------------------------------------------------------*/
int Hidi2c_Slave_SendInputReport(BYTE rid, BYTE * report, WORD len)
{
    #if HID_I2C_TEST_INPUT_ORDER
    WORD rSize;
    BYTE dataIdx;
    #endif

    /* send new input data only if previous one was sent */
	if (!Hidi2c_Slave_RequestToSendInputReport)
	{
		ASSERT(len < (sizeof(Hidi2c_Slave_InputReportBuf[0].report) - 2));

		Hidi2c_Slave_RequestToSendInputReport = TRUE;

        #if HID_I2C_TEST_INPUT_ORDER

        if (rid == 1)
        {
            rSize = len + HID_INPUT_DATA_IDX_RID1;
            dataIdx = HID_INPUT_DATA_IDX_RID1;
            // Edward 2015/3/26 Masked.
            //Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[5] = HIDSENSOR_DELTA_NEW_ASYNC_EVENT;
            // End of Edward 2015/3/26 Masked.
        }
        else if (rid == 2)
        {
            rSize = len + HID_INPUT_DATA_IDX_RID2;
            dataIdx = HID_INPUT_DATA_IDX_RID2;
        }

        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].len = rSize;
        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[0] = (rSize & 0x00FF);
        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[1] = (rSize & 0xFF00) >> 8;
        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[2] = rid;
        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[3] = HID_USAGE_SENSOR_STATE_READY;
        Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[4] = HID_USAGE_SENSOR_EVENT_DATA_UPDATED;

        memcpy(&Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[dataIdx], report, len);
        #else   //HID_I2C_TEST_INPUT_ORDER

        if (rid == 1)
        {
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].len = len + 6;
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[0] = ((len + 6) & 0x00FF);
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[1] = ((len + 6) & 0xFF00) >> 8;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[2] = rid;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[3] = HID_USAGE_SENSOR_STATE_READY;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[4] = HID_USAGE_SENSOR_EVENT_DATA_UPDATED;

		    memcpy(&Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[5], report, len);
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[11] = HIDSENSOR_DELTA_NEW_ASYNC_EVENT;
        }
        else if (rid == 2)
        {
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].len = len + 5;
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[0] = ((len + 5) & 0x00FF);
		    Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[1] = ((len + 5) & 0xFF00) >> 8;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[2] = rid;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[3] = HID_USAGE_SENSOR_STATE_READY;
            Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[4] = HID_USAGE_SENSOR_EVENT_DATA_UPDATED;
		    memcpy(&Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report[5], report, len);
        }

        #endif //HID_I2C_TEST_INPUT_ORDER

		Hidi2c_Slave_Assert_Irq();
		return 0;
	}

	return -1;
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_SetPower - Set Power Request
 *---------------------------------------------------------------------------*/
static void Hidi2c_Slave_SetPower(bool state)
{
	if (!state)
	{   // HID_I2C_POWER_STATE_SLEEP
	    // stop all hid sensor by hand
		// Hidi2c_StopSensor();
//		Hidi2c_Slave_State = HIDI2C_STATE_OFF;
//		GS_report_EN = 0;
        HID_I2C_Init();
	}
	else
	{   //HID_I2C_POWER_STATE_ON
		// nothing to do, waiting for host command ??
		GS_report_EN = 1;
	}
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_GetReportFeature - Get Feature Report
 *---------------------------------------------------------------------------*/
static int Hidi2c_Slave_GetReportFeature(BYTE rid, BYTE * buff, WORD * size)
{
    #if USE_ACCE_SENSOR
    if (rid == RID_ACCELEROMETER)
    {
		memcpy(buff, (BYTE *)&Hidi2c_RID1_FeatureReport, HID_FEATURE_REPORT_SIZE_RID1);
		*size = HID_FEATURE_REPORT_SIZE_RID1;
		return 0;
    }
    #endif //USE_ACCE_SENSOR
    #if USE_CUSTOM_SENSOR
    else if (rid == RID_CUSTOM_SENSOR)
    {
		memcpy(buff, (BYTE *)&Hidi2c_RID2_FeatureReport, HID_FEATURE_REPORT_SIZE_RID2);
		*size = HID_FEATURE_REPORT_SIZE_RID2;
		return 0;
    }
    #endif
	return -1;
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_GetReportInput - Get Input Report
 *---------------------------------------------------------------------------*/
static int Hidi2c_Slave_GetReportInput(BYTE rid, BYTE * buff, WORD * size)
{
    #if USE_ACCE_SENSOR
    if (rid == RID_ACCELEROMETER)
    {
		memcpy(buff, (BYTE *)&Hidi2c_RID1_InputReport, HID_INPUT_REPORT_SIZE_RID1);
		*size = HID_INPUT_REPORT_SIZE_RID1;
		return 0;
    }
    #endif //USE_ACCE_SENSOR
    #if USE_CUSTOM_SENSOR
    else if (rid == RID_CUSTOM_SENSOR)
    {
		memcpy(buff, (BYTE *)&Hidi2c_RID2_InputReport, HID_INPUT_REPORT_SIZE_RID2);
		*size = HID_INPUT_REPORT_SIZE_RID2;
		return 0;
    }
    #endif

	return -1;
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_SetReportFeature - Set Feature Report
 *---------------------------------------------------------------------------*/
static int Hidi2c_Slave_SetReportFeature(BYTE rid, BYTE * buff, WORD size)
{
    BYTE * ptr;

    #if USE_ACCE_SENSOR
    if (rid == RID_ACCELEROMETER)
    {
        ptr = (BYTE *)&Hidi2c_RID1_FeatureReport;
		memcpy(ptr, buff, size);
		return 0;
    }
    #endif //USE_ACCE_SENSOR
    #if USE_CUSTOM_SENSOR
    else if (rid == RID_CUSTOM_SENSOR)
    {
        ptr = (BYTE *)&Hidi2c_RID2_FeatureReport;
		memcpy(ptr, buff, size);
		return 0;
    }
    #endif
	return -1;
}


/******************************************************************************
 ******************************************************************************
 *  Hid Over I2C Global Functions
 ******************************************************************************
 *****************************************************************************/

/*-----------------------------------------------------------------------------
 * HID_I2C_Init
 * - called is when power on ??
 *---------------------------------------------------------------------------*/
void HID_I2C_Init(void)
{
    Hidi2c_Slave_Init_Irq();

    Hidi2c_Slave_State = HIDI2C_STATE_OFF;

    Hidi2c_Slave_RequestToSendInputReport = FALSE;

    Hidi2c_ReportID = 0;
    Hidi2c_ReportType = 0;

    Hidi2c_Slave_InitReportFeature();
    Hidi2c_Slave_InitReportInput();

    GS_report_EN = 0x00;

    #if HID_I2C_DBG_ERR
    Hidi2c_Err = 0;
    #endif
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_Cfg_BufPntr
 * Set I2C slave buffer read/write pointer.
 *---------------------------------------------------------------------------*/
void Hidi2c_Slave_Cfg_BufPntr(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    I2c_Var_Pntr->i2c_slave_rbuf = (BYTE *) Hidi2c_Slave_Buf;
    I2c_Var_Pntr->i2c_slave_wbuf = (BYTE *) Hidi2c_Slave_Buf;
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_Rx - Slave Receive (Rx) from Host (i.e. Host "Write" to device).
 *---------------------------------------------------------------------------*/
void Hidi2c_Slave_Rx(const BYTE * rcv_buff, WORD rcv_size)
{
	if (rcv_size >= 2)
    {
        /* Check commands/requests from Host */
		const WORD regValue = (((WORD)rcv_buff[1]) << 8) | rcv_buff[0];
		switch (regValue)
	    {
	    	case HID_DESC_REG:
	    		Hidi2c_Slave_State = HIDI2C_STATE_SEND_HIDDESC;
	    		break;

			case HID_DESC_REPORT_DESC_REG:
				Hidi2c_Slave_State = HIDI2C_STATE_SEND_REPORTDESC;
				break;

	        case HID_DESC_INPUT_REG:
	            Hidi2c_Slave_State = HIDI2C_STATE_SEND_INPUTREPORT;
				break;

			case HID_DESC_OUTPUT_REG:
				/* currently not supported */
	    		break;

			case HID_DESC_CMD_REG:
			{
				WORD idx = 4;

	            Hidi2c_ReportID   = (rcv_buff[2] & HID_I2C_REQ_RID);
	            Hidi2c_ReportType = ((rcv_buff[2] & HID_I2C_REQ_REPORT_TYPE) >> 4);

	            // check special case (i.e. rid > 15)
	            if (Hidi2c_ReportID == 0x0F)
	            {
	            	++idx;
	            	Hidi2c_ReportID = rcv_buff[4];
	            }

                /* check op code */
	            switch((rcv_buff[3] & HID_I2C_REQ_OP))
	            {
	                /* can only be "set report", since in receiver mode */
					case HID_I2C_REQ_OP_SET_REPORT:
					{
						const WORD dataRegValue = (((WORD)rcv_buff[idx+1]) << 8) | rcv_buff[idx];

						if (dataRegValue == HID_DESC_DATA_REG)
	                    {
	                    	WORD reportlen = (((WORD)rcv_buff[idx+3]) << 8) | rcv_buff[idx+2];

	                    	if (rcv_size == reportlen + idx + 2)
	                    	{
                                /* total # of bytes (in the first 2 bytes) = appending 2 bytes to the length of the report being exchanged in the Data Register, therefore we should substract 2 to reportlen */
								reportlen -= 2;

	                    		/* set report fully received, now we can propagate it to other layers */
	                    		if(Hidi2c_ReportType == HID_I2C_REQ_REPORT_TYPE_FEATURE)
	                    		{
                                    Hidi2c_Slave_SetReportFeature(Hidi2c_ReportID, (BYTE *)&rcv_buff[idx+4], reportlen);
	                    		}

				                break;
		                   	}
						}

                        #if HID_I2C_DBG_ERR
                        Hidi2c_Err = 1;
                        #endif
						goto error;
					}

					case HID_I2C_REQ_OP_GET_REPORT:
						Hidi2c_Slave_State = HIDI2C_STATE_SEND_GETREPORT;
						break;

					/* POWER request */
	            	case HID_I2C_REQ_OP_SET_POWER:
                        Hidi2c_Slave_SetPower(((rcv_buff[2] & 0x03) == HID_I2C_POWER_STATE_ON));
	            		break;

	                /* RESET request (HIR) */
	            	case HID_I2C_REQ_OP_RESET:
						Hidi2c_Slave_HIR_DIR();
	            		break;

	            	default:
                        #if HID_I2C_DBG_ERR
                        Hidi2c_Err = 2;
                        #endif
	                	goto error;
	            }
				break;
			}

	        default:
                #if HID_I2C_DBG_ERR
                Hidi2c_Err = 3;
                #endif
				goto error;
		}
	}

    return;

error:
    /* Trigger a DIR if any error */
    Hidi2c_Slave_HIR_DIR();
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_Tx - Slave Transmit (Tx) to Host (i.e. Host reads from device).
 *---------------------------------------------------------------------------*/
void Hidi2c_Slave_Tx(const BYTE ** buff, WORD * size)
{

	switch(Hidi2c_Slave_State)
	{

		case HIDI2C_STATE_SEND_HIDDESC:
			*buff = (const BYTE *)&HID_Descriptor;
			*size = HID_DESC_HIDDESC_LENGTH;
			break;

		case HIDI2C_STATE_SEND_REPORTDESC:
			*buff = (const BYTE *)&Hidi2c_ReportDescriptor;
			*size = Hidi2c_Slave_GetReportSize();
			break;

		case HIDI2C_STATE_SEND_GETREPORT:
		{
			int (*pGetReport)(BYTE, BYTE *, WORD *);

			switch(Hidi2c_ReportType)
			{
				case HID_I2C_REQ_REPORT_TYPE_FEATURE:
							pGetReport = Hidi2c_Slave_GetReportFeature;
				break;

				case HID_I2C_REQ_REPORT_TYPE_INPUT:
							pGetReport = Hidi2c_Slave_GetReportInput;
				break;

				default:
					#if HID_I2C_DBG_ERR
							Hidi2c_Err = 4;
					#endif
					goto error;
			}

			*size = (pGetReport(Hidi2c_ReportID, &Hidi2c_Slave_Buf[2], size) == 0) ? *size + 2 : 2;
			Hidi2c_Slave_Buf[0] = (*size & 0x00FF);
			Hidi2c_Slave_Buf[1] = (*size & 0xFF00) >> 8;
			*buff = Hidi2c_Slave_Buf;
		}
		break;

        case HIDI2C_STATE_SEND_INPUTREPORT:
		default: // for host issue read without writing to INPUTREG
        	if (Hidi2c_Slave_RequestToSendInputReport)
        	{
				*buff = Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].report;
				*size = Hidi2c_Slave_InputReportBuf[Hidi2c_Slave_InputReportBufIdx].len;

				/* increment idx of double buffer to avoid data corruption if
					new request to send data occurs before all data were really sent */
				Hidi2c_Slave_InputReportBufIdx = (Hidi2c_Slave_InputReportBufIdx+1)%2;

				/* reset flag to allow main programm to prepare a new report */
				Hidi2c_Slave_RequestToSendInputReport = FALSE;

				/* call back could request to send another input report */
                Hidi2c_Slave_Release_Irq();
		   	}
			else
			{   // HID_I2C_REQ_OP_RESET or no data
                Hidi2c_Slave_Buf[0] = 0;
                Hidi2c_Slave_Buf[1] = 0;
				*buff = Hidi2c_Slave_Buf;
				*size = 2;
				Hidi2c_Slave_Release_Irq();

			}
			break;
	}


	Hidi2c_Slave_State = HIDI2C_STATE_INIT;


	return;

error:
    Hidi2c_Slave_HIR_DIR();
}

#endif  // SHL_SUPPORTED
