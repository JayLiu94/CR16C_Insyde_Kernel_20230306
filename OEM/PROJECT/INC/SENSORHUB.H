/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   SENSORHUB.H                                                                                           */
/*            This file contains Sensor Hub Lite for Angle calculated implementation.                      */
/* Project:                                                                                                */
/*            Simple Sensor Hub Lite Firmware for uRider Notebook Embedded Controller Peripherals.         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef SENSORHUB_H
#define SENSORHUB_H


#include "swtchs.h"
#include "types.h"


#define ADC_SENSOR 0
#define USE_ACCE_SENSOR     1   /* standard Accelerometer sensor */
#define USE_CUSTOM_SENSOR   1   /* customer sensor */

#define HID_OVER_I2C_GSENSOR_SUPPORTED 1

// 2014/7/25 add for two G-sensor HID over I2C function.
/* ========================================================================= */
/* ========================================================================= */
#if HID_OVER_I2C_GSENSOR_SUPPORTED

extern BYTE Hidi2c_Slave_State;
extern BYTE GS_report_EN;


/*-----------------------------------------------------------------------------
 * HID_Descriptor: HID Descriptor
 *---------------------------------------------------------------------------*/

#define HID_DESC_HIDDESC_LENGTH     0x001E                      // wHIDDescLength (Fixed to 0x1E)
#define HID_DESC_BCD_VER            0x0100                      // bcdVersion (Compliant with 1.00)
#define HID_DESC_REPORT_DESC_LENGTH HID_REPORT_DESC_LENGTH      // wReportDescLength
#define HID_DESC_REPORT_DESC_REG    0xDD01                      // wReportDescRegister
#define HID_DESC_INPUT_REG          0xDD02                      // wInputRegister
#define HID_DESC_MAX_INPUT_LENGTH   (HID_INPUT_REPORT_SIZE_RID2+2)  // wMaxInputLength - Use the RIDn with the "MAX" size
#define HID_DESC_OUTPUT_REG         0                           // wOutputRegister
#define HID_DESC_MAX_OUTPUT_LENGTH  0                           // wMaxOutputLength
#define HID_DESC_CMD_REG            0xDD03                      // wCommandRegister
#define HID_DESC_DATA_REG           0xDD04                      // wDataRegister
#define HID_DESC_VID                0x0416                      // wVendorID
#define HID_DESC_PID                0xC300                      // wProductID
#define HID_DESC_VERID              0x0200                      // wVersionID
#define HID_DESC_RSVD               0                           // RESERVED



#define HIDI2C_STATE_INIT               0
#define HIDI2C_STATE_READY              1
#define HIDI2C_STATE_SEND_HIDDESC       2
#define HIDI2C_STATE_SEND_REPORTDESC    3
#define HIDI2C_STATE_SEND_INPUTREPORT   4
#define HIDI2C_STATE_SEND_GETREPORT     5
#define HIDI2C_STATE_SEND_RESET         6
#define HIDI2C_STATE_OFF                0xFF

/* Feature Report for RID1 */
typedef struct __attribute__((packed))
{
    BYTE    reportId;               // Must match with HID descriptor reportID for each sensor
    BYTE    connectionType;         // Sensor Connection Type : Integrated, Attached or External
    BYTE    reportingState;         // Reporting State of the sensor
    BYTE    powerState;             // Power State of the sensor
    BYTE    sensorState;            // Sensor State
    WORD    reportInterval;         // Report Interval
    WORD    sensorAccuracy;         // Sensor Accuracy
    WORD    sensorResolution;       // Sensor Resolution
    WORD    changeSensitivityAbs;   // Change Sensitivity Absolute
    SSMALL  rangeMaximum;           // Range Maximum
    SSMALL  rangeMinimum;           // Range Minimum
    WORD    minimumReportInterval;  // Specifies the minimum report interval supported by the sensor
} HID_REPORT_FEATURE_RID1;

#define HID_FEATURE_REPORT_SIZE_RID1    sizeof(HID_REPORT_FEATURE_RID1)

#define RID1_FEATURE_DEFAULT_INTERVAL       (100)   // ms
#define RID1_FEATURE_DEFAULT_ACCURACY       (4)     // 0.4 mg
#define RID1_FEATURE_DEFAULT_RESOLUTION     (4)     // 0.4 mg
#define RID1_FEATURE_DEFAULT_SENSITIVITY    (20)    // mg
#define RID1_FEATURE_DEFAULT_MAX            (8000)  // mg
#define RID1_FEATURE_DEFAULT_MIN            (-8000) // mg
#define RID1_FEATURE_DEFAULT_MIN_INTERVAL   (10)    // ms

/* Feature Report for RID2 */
#define RID2_SENSOR_DESC_LENGTH     12

typedef struct __attribute__((packed))
{
    BYTE    reportId;               // Must match with HID descriptor reportID for each sensor
    BYTE    connectionType;         // Sensor Connection Type : Integrated, Attached or External
    BYTE    reportingState;         // Reporting State of the sensor
    BYTE    powerState;             // Power State of the sensor
    BYTE    sensorState;            // Sensor State
    WORD    reportInterval;         // Report Interval
    WORD    sensorAccuracy;         // Sensor Accuracy
    WORD    sensorResolution;       // Sensor Resolution
    WORD    changeSensitivityAbs;   // Change Sensitivity Absolute
    SSMALL  rangeMaximum;           // Range Maximum
    SSMALL  rangeMinimum;           // Range Minimum
    WORD    minimumReportInterval;  // Specifies the minimum report interval supported by the sensor
    WORD    sensorDesc[RID2_SENSOR_DESC_LENGTH] __attribute__((aligned(1)));          // Sensor Description
} HID_REPORT_FEATURE_RID2;

#define HID_FEATURE_REPORT_SIZE_RID2    sizeof(HID_REPORT_FEATURE_RID2)

#define RID2_FEATURE_DEFAULT_INTERVAL       (100)   // ms
#define RID2_FEATURE_DEFAULT_ACCURACY       (4)     // 0.4 mg
#define RID2_FEATURE_DEFAULT_RESOLUTION     (4)     // 0.4 mg
#define RID2_FEATURE_DEFAULT_SENSITIVITY    (20)    // mg
#define RID2_FEATURE_DEFAULT_MAX            0x7FFF  //(8000)  // mg
#define RID2_FEATURE_DEFAULT_MIN            0       //(-8000) // mg
#define RID2_FEATURE_DEFAULT_MIN_INTERVAL   (10)    // ms

/* Input Report for RID1 */
typedef struct __attribute__((packed))
{
    BYTE reportId;      // Must match with HID descriptor reportID for each sensor
    BYTE sensorState;   // Sensor State - required for Sensor
    BYTE sensorEvent;   // Sensor Event - required for Sensor
    BYTE accMotion;     // Accelerometer motion - required for Accelerometer
    WORD accX;
    WORD accY;
    WORD accZ;
} HID_REPORT_INPUT_RID1;

#define HID_INPUT_REPORT_SIZE_RID1  sizeof(HID_REPORT_INPUT_RID1)
// Edward 2015/3/27 Modified.
//#define HID_INPUT_DATA_IDX_RID1     6
#define HID_INPUT_DATA_IDX_RID1     5
// End of Edward 2015/3/27 Modified.

#define RID1_INPUT_DEFAULT_STATE HID_USAGE_SENSOR_STATE_READY
#define RID1_INPUT_DEFAULT_EVENT HID_USAGE_SENSOR_EVENT_DATA_UPDATED

/* Input Report for RID2 */
typedef struct __attribute__((packed))
{
    BYTE reportId;      // Must match with HID descriptor reportID for each sensor
    BYTE sensorState;   // Sensor State - required for Sensor
    BYTE sensorEvent;   // Sensor Event - required for Sensor
	WORD baseX;
    WORD baseY;
    WORD baseZ;
	WORD angleBaseGnd;
    WORD angleScrGnd;
    WORD angleBaseScr;
} HID_REPORT_INPUT_RID2;

#define HID_INPUT_REPORT_SIZE_RID2  sizeof(HID_REPORT_INPUT_RID2)
#define HID_INPUT_DATA_IDX_RID2     5

#define RID2_INPUT_DEFAULT_STATE    HID_USAGE_SENSOR_STATE_READY
#define RID2_INPUT_DEFAULT_EVENT    HID_USAGE_SENSOR_EVENT_DATA_UPDATED


WORD Hidi2c_Slave_GetReportSize(void);
extern int Hidi2c_Slave_SendInputReport(BYTE rid, BYTE * report, WORD len);


/*-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 * HID Over I2C Spec
 *-----------------------------------------------------------------------------
 *---------------------------------------------------------------------------*/

/* HID Descriptor */
#define HID_I2C_HID_DESC_ADDR   0x0001  /* HID descriptor register address */
#define HID_DESC_REG            HID_I2C_HID_DESC_ADDR

/*-----------------------------------------------------------------------------
 * Requests: Command Register (2 bytes)
 * MSB[7:4] Reserved
 * MSB[3:0] Op Code
 * LSB[7:6] Reserved
 * LSB[5:4] Report Type
 * LSB[3:0] Report ID
 *---------------------------------------------------------------------------*/
#define HID_I2C_REQ_OP          (0x0F << 0)
#define HID_I2C_REQ_REPORT_TYPE (0x03 << 4)
#define HID_I2C_REQ_RID         (0x0F << 0)

/* Requests - Command Register - Op Code */
#define HID_I2C_REQ_OP_RESET            0x01
#define HID_I2C_REQ_OP_GET_REPORT       0x02
#define HID_I2C_REQ_OP_SET_REPORT       0x03
#define HID_I2C_REQ_OP_GET_IDLE         0x04
#define HID_I2C_REQ_OP_SET_IDLE         0x05
#define HID_I2C_REQ_OP_GET_PROTOCOL     0x06
#define HID_I2C_REQ_OP_SET_PROTOCOL     0x07
#define HID_I2C_REQ_OP_SET_POWER        0x08

/* HID_I2C_CMD_OPCODE_GET_PROTOCOL, HID_I2C_CMD_OPCODE_GET_PROTOCOL */
#define HID_I2C_PROTOCOL_BOOT       0x00
#define HID_I2C_PROTOCOL_REPORT     0x01

/* HID_I2C_CMD_OPCODE_SET_POWER - LSB of Command Reg */
#define HID_I2C_POWER_STATE_ON      0x00
#define HID_I2C_POWER_STATE_SLEEP   0x01

/* Requests - Command Register - Report Type */
#define HID_I2C_REQ_REPORT_TYPE_INPUT       0x01
#define HID_I2C_REQ_REPORT_TYPE_OUTPUT      0x02
#define HID_I2C_REQ_REPORT_TYPE_FEATURE     0x03

/* Requests - Command Register - Report ID */
#if USE_ACCE_SENSOR
#define RID_ACCELEROMETER   1
#define RID_CUSTOM_SENSOR   2
#else
#define RID_CUSTOM_SENSOR   1
#endif
#define RID_MAX_ID          RID_CUSTOM_SENSOR

//HidSensorDelta
#define HIDSENSOR_DELTA_BELOW_SENSITIVITY   0
#define HIDSENSOR_DELTA_ABOVE_SENSITIVITY   1
#define HIDSENSOR_DELTA_NEW_ASYNC_EVENT     2

/*-----------------------------------------------------------------------------
 * Defintions and macors for report descriptior
 *---------------------------------------------------------------------------*/
#define HID_USAGE_PAGE_SENSOR                                   0x05,0x20

//sensor category usages
#define HID_USAGE_SENSOR_TYPE_COLLECTION                        0x09,0x01

//sensor category motion
#define HID_USAGE_SENSOR_CATEGORY_MOTION                        0x09,0x70
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_1D           0x09,0x71
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_2D           0x09,0x72
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_3D           0x09,0x73

//sensor category other
#define HID_USAGE_SENSOR_CATEGORY_OTHER                         0x09,0xE0
#define HID_USAGE_SENSOR_TYPE_OTHER_CUSTOM                      0x09,0xE1

//data type usages modifiers
//NOTE: the usage tables actually define these as two bytes, but in order
//to get the define macros to work so these are !¢FDor-ed?these are defined
//here as only one byte.
#define HID_USAGE_SENSOR_DATA_MOD_NONE                          0x00
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_ABS        0x10
#define HID_USAGE_SENSOR_DATA_MOD_MAX                           0x20
#define HID_USAGE_SENSOR_DATA_MOD_MIN                           0x30
#define HID_USAGE_SENSOR_DATA_MOD_ACCURACY                      0x40
#define HID_USAGE_SENSOR_DATA_MOD_RESOLUTION                    0x50
#define HID_USAGE_SENSOR_DATA_MOD_THRESHOLD_HIGH                0x60
#define HID_USAGE_SENSOR_DATA_MOD_THRESHOLD_LOW                 0x70
#define HID_USAGE_SENSOR_DATA_MOD_CALIBRATION_OFFSET            0x80
#define HID_USAGE_SENSOR_DATA_MOD_CALIBRATION_MULTIPLIER        0x90
#define HID_USAGE_SENSOR_DATA_MOD_REPORT_INTERVAL               0xA0
#define HID_USAGE_SENSOR_DATA_MOD_FREQUENCY_MAX                 0xB0
#define HID_USAGE_SENSOR_DATA_MOD_PERIOD_MAX                    0xC0
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_RANGE_PCT  0xD0
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_REL_PCT    0xE0
#define HID_USAGE_SENSOR_DATA_MOD_VENDOR_RESERVED               0xF0

//state usages
#define HID_USAGE_SENSOR_STATE                                  0x0A,0x01,0x02
//state selectors
#define HID_USAGE_SENSOR_STATE_UNKNOWN_SEL                      0x0A,0x00,0x08
#define HID_USAGE_SENSOR_STATE_READY_SEL                        0x0A,0x01,0x08
#define HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL                0x0A,0x02,0x08
#define HID_USAGE_SENSOR_STATE_NO_DATA_SEL                      0x0A,0x03,0x08
#define HID_USAGE_SENSOR_STATE_INITIALIZING_SEL                 0x0A,0x04,0x08
#define HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL                0x0A,0x05,0x08
#define HID_USAGE_SENSOR_STATE_ERROR_SEL                        0x0A,0x06,0x08
// state Enum
#define HID_USAGE_SENSOR_STATE_UNKNOWN                          0x01
#define HID_USAGE_SENSOR_STATE_READY                            0x02
#define HID_USAGE_SENSOR_STATE_NOT_AVAILABLE                    0x03
#define HID_USAGE_SENSOR_STATE_NO_DATA                          0x04
#define HID_USAGE_SENSOR_STATE_INITIALIZING                     0x05
#define HID_USAGE_SENSOR_STATE_ACCESS_DENIED                    0x06
#define HID_USAGE_SENSOR_STATE_ERROR                            0x07

//event usages
#define HID_USAGE_SENSOR_EVENT                                  0x0A,0x02,0x02
//event selectors
#define HID_USAGE_SENSOR_EVENT_UNKNOWN_SEL                      0x0A,0x10,0x08
#define HID_USAGE_SENSOR_EVENT_STATE_CHANGED_SEL                0x0A,0x11,0x08
#define HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED_SEL             0x0A,0x12,0x08
#define HID_USAGE_SENSOR_EVENT_DATA_UPDATED_SEL                 0x0A,0x13,0x08
#define HID_USAGE_SENSOR_EVENT_POLL_RESPONSE_SEL                0x0A,0x14,0x08
#define HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY_SEL           0x0A,0x15,0x08
//event enums (for input report)
#define HID_USAGE_SENSOR_EVENT_UNKNOWN                          0x01
#define HID_USAGE_SENSOR_EVENT_STATE_CHANGED                    0x02
#define HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED                 0x03
#define HID_USAGE_SENSOR_EVENT_DATA_UPDATED                     0x04
#define HID_USAGE_SENSOR_EVENT_POLL_RESPONSE                    0x05
#define HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY               0x06

//property usages (get/set feature report)
#define HID_USAGE_SENSOR_PROPERTY                               0x0A,0x00,0x03
#define HID_USAGE_SENSOR_PROPERTY_FRIENDLY_NAME                 0x0A,0x01,0x03
#define HID_USAGE_SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID          0x0A,0x02,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_STATUS                 0x0A,0x03,0x03
#define HID_USAGE_SENSOR_PROPERTY_MINIMUM_REPORT_INTERVAL       0x0A,0x04,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_MANUFACTURER           0x0A,0x05,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_MODEL                  0x0A,0x06,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_SERIAL_NUMBER          0x0A,0x07,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_DESCRIPTION            0x0A,0x08,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE        0x0A,0x09,0x03

//begin connection type selectors
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED_SEL 0x0A,0x30,0x08
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_ATTACHED_SEL   0x0A,0x31,0x08
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL_SEL   0x0A,0x32,0x08
//end connection type selectors
//begin connection type enums
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED 0x01
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_ATTACHED   0x02
#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL   0x03
//end connection type enums

#define HID_USAGE_SENSOR_PROPERTY_SENSOR_DEVICE_PATH            0x0A,0x0A,0x03
#define HID_USAGE_SENSOR_PROPERTY_HARDWARE_REVISION             0x0A,0x0B,0x03
#define HID_USAGE_SENSOR_PROPERTY_FIRMWARE_VERSION              0x0A,0x0C,0x03
#define HID_USAGE_SENSOR_PROPERTY_RELEASE_DATE                  0x0A,0x0D,0x03
#define HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL               0x0A,0x0E,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_ABS        0x0A,0x0F,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_RANGE_PCT  0x0A,0x10,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_REL_PCT    0x0A,0x11,0x03
#define HID_USAGE_SENSOR_PROPERTY_ACCURACY                      0x0A,0x12,0x03
#define HID_USAGE_SENSOR_PROPERTY_RESOLUTION                    0x0A,0x13,0x03
#define HID_USAGE_SENSOR_PROPERTY_RANGE_MAXIMUM                 0x0A,0x14,0x03
#define HID_USAGE_SENSOR_PROPERTY_RANGE_MINIMUM                 0x0A,0x15,0x03
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE               0x0A,0x16,0x03

//begin reporting state selectors
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_SEL             0x0A,0x40,0x08
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_SEL            0x0A,0x41,0x08
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_SEL      0x0A,0x42,0x08
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE_SEL        0x0A,0x43,0x08
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE_SEL       0x0A,0x44,0x08
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE_SEL 0x0A,0x45,0x08
//end reporting state selectors

//begin reporting state enums
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS             0x01
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS            0x02
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS      0x03
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE        0x04
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE       0x05
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE 0x06
//end reporting state enums

#define HID_USAGE_SENSOR_PROPERTY_SAMPLING_RATE                 0x0A,0x17,0x03
#define HID_USAGE_SENSOR_PROPERTY_RESPONSE_CURVE                0x0A,0x18,0x03
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE                   0x0A,0x19,0x03
//begin power state selectors
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED_SEL             0x0A,0x50,0x08
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER_SEL         0x0A,0x51,0x08
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER_SEL          0x0A,0x52,0x08
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE_SEL  0x0A,0x53,0x08
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE_SEL    0x0A,0x54,0x08
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF_SEL          0x0A,0x55,0x08
//end power state selectors

//begin power state enums
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED             0x01
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER         0x02
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER          0x03
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE  0x04
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE    0x05
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF          0x06
//end power state enums

//property usages (get/set feature report)
#define HID_USAGE_SENSOR_PROPERTY_LOCATION                          0x0A,0x2A,0x04
#define HID_USAGE_SENSOR_PROPERTY_LOCATION_DESIRED_ACCURACY         0x0A,0x2B,0x04
//begin location desired accuracy selectors
#define HID_USAGE_SENSOR_DESIRED_ACCURACY_DEFAULT                   0x0A,0x60,0x08
#define HID_USAGE_SENSOR_DESIRED_ACCURACY_HIGH                      0x0A,0x61,0x08
#define HID_USAGE_SENSOR_DESIRED_ACCURACY_MEDIUM                    0x0A,0x62,0x08
#define HID_USAGE_SENSOR_DESIRED_ACCURACY_LOW                       0x0A,0x63,0x08
//end location desired accuracy selectors

//data type motion
//data field usages (input report)
#define HID_USAGE_SENSOR_DATA_MOTION                                0x0A,0x50,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_STATE                          0x0A,0x51,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION                   0x0A,0x52,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_X_AXIS            0x0A,0x53,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Y_AXIS            0x0A,0x54,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Z_AXIS            0x0A,0x55,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY               0x0A,0x56,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_X_AXIS        0x0A,0x57,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_Y_AXIS        0x0A,0x58,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_Z_AXIS        0x0A,0x59,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION               0x0A,0x5A,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_X_AXIS        0x0A,0x5B,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_Y_AXIS        0x0A,0x5C,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_Z_AXIS        0x0A,0x5D,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_SPEED                          0x0A,0x5E,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_INTENSITY                      0x0A,0x5F,0x04

//data type custom
//data field usages (input report)
#define HID_USAGE_SENSOR_DATA_CUSTOM                                0x0A,0x40,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_USAGE                          0x0A,0x41,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_BOOLEAN_ARRAY                  0x0A,0x42,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE                          0x0A,0x43,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_1                        0x0A,0x44,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_2                        0x0A,0x45,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_3                        0x0A,0x46,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_4                        0x0A,0x47,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_5                        0x0A,0x48,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_6                        0x0A,0x49,0x05
//define vendor-specific (non-spec) custom datafields
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_7                        0x0A,0x4A,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_8                        0x0A,0x4B,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_9                        0x0A,0x4C,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_10                       0x0A,0x4D,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_11                       0x0A,0x4E,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_12                       0x0A,0x4F,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_13                       0x0A,0x50,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_14                       0x0A,0x51,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_15                       0x0A,0x52,0x05

//unit usages
#define HID_USAGE_SENSOR_UNITS_NOT_SPECIFIED                        0x65,0x00
#define HID_USAGE_SENSOR_UNITS_DEGREES                              0x65,0x14
//unit deprecated usages
#define HID_USAGE_SENSOR_UNITS_DEPRECATED_G                         0x1A
#define HID_USAGE_SENSOR_UNITS_G                                    HID_USAGE_SENSOR_UNITS_DEPRECATED_G

/*-----------------------------------------------------------------------------
 * Other HID definitions
 * NOTE: These definitions are designed to permit compiling the HID report
 * descriptors with somewhat self-explanatory information to help readability
 * and reduce errors
 *---------------------------------------------------------------------------*/
//input,output,feature flags
#define Data_Arr_Abs                0x00
#define Const_Arr_Abs               0x01
#define Data_Var_Abs                0x02
#define Const_Var_Abs               0x03
#define Data_Var_Rel                0x06

//collection flags
#define Physical                    0x00
#define Application                 0x01
#define Logical                     0x02

// macros for report descriptors
#define HID_USAGE_PAGE(a)           0x05,a
#define HID_USAGE(a)                0x09,a
#define HID_USAGE16(a,b)            0x0A,a,b
#define HID_USAGE_SENSOR_DATA(a,b)  a|b     //This or-s the mod into usage
#define HID_COLLECTION(a)           0xA1,a
#define HID_REPORT_ID(a)            0x85,a
#define HID_REPORT_SIZE(a)          0x75,a
#define HID_REPORT_COUNT(a)         0x95,a
#define HID_USAGE_MIN_8(a)          0x19,a
#define HID_USAGE_MIN_16(a,b)       0x1A,a,b
#define HID_USAGE_MAX_8(a)          0x29,a
#define HID_USAGE_MAX_16(a,b)       0x2A,a,b
#define HID_LOGICAL_MIN_8(a)        0x15,a
#define HID_LOGICAL_MIN_16(a,b)     0x16,a,b
#define HID_LOGICAL_MIN_32(a,b,c,d) 0x17,a,b,c,d
#define HID_LOGICAL_MAX_8(a)        0x25,a
#define HID_LOGICAL_MAX_16(a,b)     0x26,a,b
#define HID_LOGICAL_MAX_32(a,b,c,d) 0x27,a,b,c,d
#define HID_UNIT_EXPONENT(a)        0x55,a
#define HID_INPUT(a)                0x81,a
#define HID_OUTPUT(a)               0x91,a
#define HID_FEATURE(a)              0xB1,a
#define HID_END_COLLECTION          0xC0

#endif //HID_OVER_I2C_GSENSOR_SUPPORTED



#define GS1_Offset  0
#define GS2_Offset  0
#define PG_Value    0x40
#define BG_Value    0x40

#if HID_OVER_I2C_GSENSOR_SUPPORTED
typedef struct _ACC_AXIS
{
    FLAG    Signed_X;      // Must match with HID descriptor reportID for each sensor
    FLAG    Signed_Y;   // Sensor State
    FLAG    Signed_Z;   // Sensor Event
    #if SH_RAW_TYPE_WORD
    WORD    Axis_GS_Y;
    WORD    Axis_GS_Z;
    WORD    Axis_Out_X;
    WORD    Axis_Out_Y;
    WORD    Axis_Out_Z;
    #else
    BYTE    Axis_GS_Y;
    BYTE    Axis_GS_Z;
    BYTE    Axis_Out_X;
    BYTE    Axis_Out_Y;
    BYTE    Axis_Out_Z;
    #endif
} ACC_AXIS;

typedef struct _ACC_CONFIG
{
    FLAG    Axis_X;
    FLAG    Axis_Y;
    FLAG    Axis_Z;
} ACC_CONFIG;

extern HID_REPORT_FEATURE_RID1  Hidi2c_RID1_FeatureReport;
extern HID_REPORT_FEATURE_RID2  Hidi2c_RID2_FeatureReport;
extern HID_REPORT_INPUT_RID1    Hidi2c_RID1_InputReport;
extern HID_REPORT_INPUT_RID2    Hidi2c_RID2_InputReport;



#if SH_RAW_TYPE_WORD
void Gravity_PreProcess(ACC_AXIS *Axis_pntr);
WORD Angle_Calculate(WORD Raw_Y, WORD Raw_Z);
#else
void Gravity_PreProcess(ACC_AXIS *Axis_pntr);
WORD Angle_Calculate(BYTE Raw_Y, BYTE Raw_Z);
#endif
#endif //HID_OVER_I2C_GSENSOR_SUPPORTED



#endif /* ifndef SFL_HUB_H */
