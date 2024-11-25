/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   SENSORHUB.C                                                                                           */
/*            This file contains Sensor Hub Lite for Angle calculaton implementation.                      */
/* Project:                                                                                                */
/*            Simple Sensor Hub Lite Firmware for uRider Notebook Embedded Controller Peripherals.         */
/*---------------------------------------------------------------------------------------------------------*/

#define SENSORHUB_C

#if HW_FLOATING_SUPPORTED
#include "sfa.h"
#endif // HW_FLOATING_SUPPORTED
#include "sensorhub.h"
#include "purdat.h"
//#include <math.h>
float atanf(float);

#if SHL_SUPPORTED

HID_REPORT_FEATURE_RID1  Hidi2c_RID1_FeatureReport;
HID_REPORT_FEATURE_RID2  Hidi2c_RID2_FeatureReport;
HID_REPORT_INPUT_RID1    Hidi2c_RID1_InputReport;
HID_REPORT_INPUT_RID2    Hidi2c_RID2_InputReport;

/*-----------------------------------------------------------------------------
 * HID Descriptor - 2 sensors: (1) 3D Accelerometer and (2) Customer Sensor
 *---------------------------------------------------------------------------*/
const BYTE Hidi2c_ReportDescriptor[] =
{
    HID_USAGE_PAGE_SENSOR,
    HID_USAGE_SENSOR_TYPE_COLLECTION,
    HID_COLLECTION(Physical),

    /*===== Start 3D ACCELEROMETER DESCRIPTION ==============================*/
	HID_REPORT_ID(RID_ACCELEROMETER),
    HID_USAGE_PAGE_SENSOR,
    HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_3D,
    HID_COLLECTION(Physical),
        /*===== Start 3D Accelerometer Feature Description ==================*/
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(2),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED_SEL,
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_ATTACHED_SEL,
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(5),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_POWER_STATE,
            HID_LOGICAL_MIN_8(0x00),
            HID_LOGICAL_MAX_8(0x05),
            HID_REPORT_SIZE(0x08),
            HID_REPORT_COUNT(0x01),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(6),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
                HID_USAGE_SENSOR_STATE_READY_SEL,
                HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
                HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
                HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
                HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
                HID_USAGE_SENSOR_STATE_ERROR_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION,HID_USAGE_SENSOR_DATA_MOD_ACCURACY),
            HID_LOGICAL_MIN_8(0x00),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0C),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION,HID_USAGE_SENSOR_DATA_MOD_RESOLUTION),
            HID_LOGICAL_MIN_8(0x00),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0C),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION,HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_ABS),
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION,HID_USAGE_SENSOR_DATA_MOD_MAX),
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION,HID_USAGE_SENSOR_DATA_MOD_MIN),
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_PROPERTY_MINIMUM_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

        /*===== Start 3D Accelerometer Input Description ====================*/
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(6),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
                HID_USAGE_SENSOR_STATE_READY_SEL,
                HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
                HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
                HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
                HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
                HID_USAGE_SENSOR_STATE_ERROR_SEL,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_EVENT,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(5),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_EVENT_UNKNOWN_SEL,
                HID_USAGE_SENSOR_EVENT_STATE_CHANGED_SEL,
                HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED_SEL,
                HID_USAGE_SENSOR_EVENT_DATA_UPDATED_SEL,
                HID_USAGE_SENSOR_EVENT_POLL_RESPONSE_SEL,
                HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY_SEL,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,

        // Edward 2015/3/26 modified.
        //#if HID_I2C_TEST_INPUT_ORDER
        #if !HID_I2C_TEST_INPUT_ORDER
        // End of Edward 2015/3/26 modified.
        HID_USAGE_SENSOR_DATA_MOTION_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(1),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_INPUT(Data_Var_Abs),
        #endif //HID_I2C_TEST_INPUT_ORDER

        HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_X_AXIS,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80), /* -32767 */
            HID_LOGICAL_MAX_16(0xFF, 0x7F), /* +32767 */
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),        /* -127 */
            HID_LOGICAL_MAX_8(0x7F),        /* +127 */
            HID_REPORT_SIZE(8),
            #endif // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Y_AXIS,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),
            HID_LOGICAL_MAX_8(0x7F),
            HID_REPORT_SIZE(8),
            #endif // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Z_AXIS,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),
            HID_LOGICAL_MAX_8(0x7F),
            HID_REPORT_SIZE(8),
            #endif // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0D),
            HID_INPUT(Data_Var_Abs),

        #if !HID_I2C_TEST_INPUT_ORDER
        HID_USAGE_SENSOR_DATA_MOTION_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(1),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_INPUT(Data_Var_Abs),
        #endif //HID_I2C_TEST_INPUT_ORDER
    HID_END_COLLECTION,
    /*===== End 3D ACCELEROMETER DESCRIPTION ================================*/

    /*===== Start Customer Sensor DESCRIPTION ===============================*/
	HID_REPORT_ID(RID_CUSTOM_SENSOR),
    HID_USAGE_PAGE_SENSOR,
    HID_USAGE_SENSOR_TYPE_OTHER_CUSTOM,
    HID_COLLECTION(Physical),
        /*===== Start Customer Feature Description ==========================*/
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(2),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED_SEL,
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED_SEL,
                HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(5),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_POWER_STATE,
            HID_LOGICAL_MIN_8(0x00),
            HID_LOGICAL_MAX_8(0x05),
            HID_REPORT_SIZE(0x08),
            HID_REPORT_COUNT(0x01),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE_SEL,
                HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(6),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
                HID_USAGE_SENSOR_STATE_READY_SEL,
                HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
                HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
                HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
                HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
                HID_USAGE_SENSOR_STATE_ERROR_SEL,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_CUSTOM_VALUE,HID_USAGE_SENSOR_DATA_MOD_ACCURACY),
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0x03, 0x00),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_CUSTOM_VALUE,HID_USAGE_SENSOR_DATA_MOD_RESOLUTION),
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0x03, 0x00),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_CUSTOM_VALUE,HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_ABS),
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0xFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_10,HID_USAGE_SENSOR_DATA_MOD_MAX),
            HID_LOGICAL_MIN_16(0x10, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_10,HID_USAGE_SENSOR_DATA_MOD_MIN),
            HID_LOGICAL_MIN_16(0x10, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_PROPERTY_MINIMUM_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_PROPERTY_SENSOR_DESCRIPTION,
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(RID2_SENSOR_DESC_LENGTH),
            HID_FEATURE(Const_Var_Abs),

        /*===== Start Customer Input Description ============================*/
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(6),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
                HID_USAGE_SENSOR_STATE_READY_SEL,
                HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
                HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
                HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
                HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
                HID_USAGE_SENSOR_STATE_ERROR_SEL,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_EVENT,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(5),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION(Logical),
                HID_USAGE_SENSOR_EVENT_UNKNOWN_SEL,
                HID_USAGE_SENSOR_EVENT_STATE_CHANGED_SEL,
                HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED_SEL,
                HID_USAGE_SENSOR_EVENT_DATA_UPDATED_SEL,
                HID_USAGE_SENSOR_EVENT_POLL_RESPONSE_SEL,
                HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY_SEL,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_1,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),
            HID_LOGICAL_MAX_8(0x7F),
            HID_REPORT_SIZE(8),
            #endif  // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0E),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_2,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),
            HID_LOGICAL_MAX_8(0x7F),
            HID_REPORT_SIZE(8),
            #endif // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0E),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_3,
            #if SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_16(0x01, 0x80),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            #else // SH_RAW_TYPE_WORD
            HID_LOGICAL_MIN_8(0x80),
            HID_LOGICAL_MAX_8(0x7F),
            HID_REPORT_SIZE(8),
            #endif // SH_RAW_TYPE_WORD
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x0E),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_4,
            HID_LOGICAL_MIN_16(0x00, 0x00),
            HID_LOGICAL_MAX_16(0x68, 0x01),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0x00),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_5,
            HID_LOGICAL_MIN_16(0x00, 0x00),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_INPUT(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_6,
            HID_LOGICAL_MIN_16(0x00, 0x00),
            HID_LOGICAL_MAX_16(0xFF, 0x7F),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_INPUT(Data_Var_Abs),
    HID_END_COLLECTION,
    /*===== End Customer Sensor DESCRIPTION =================================*/

    HID_END_COLLECTION  /* End of Report Descriptor */
};

#define HID_REPORT_DESC_LENGTH    sizeof(Hidi2c_ReportDescriptor)

const WORD HID_Descriptor[] =
{
    HID_DESC_HIDDESC_LENGTH,        //  0 wHIDDescLength
    HID_DESC_BCD_VER,               //  2 bcdVersion
    HID_DESC_REPORT_DESC_LENGTH,    //  4 wReportDescLength
    HID_DESC_REPORT_DESC_REG,       //  6 wReportDescRegister
    HID_DESC_INPUT_REG,             //  8 wInputRegister
    HID_DESC_MAX_INPUT_LENGTH,      // 10 wMaxInputLength
    HID_DESC_OUTPUT_REG,            // 12 wOutputRegister
    HID_DESC_MAX_OUTPUT_LENGTH,     // 14 wMaxOutputLength
    HID_DESC_CMD_REG,               // 16 wCommandRegister
    HID_DESC_DATA_REG,              // 18 wDataRegister
    HID_DESC_VID,                   // 20 wVendorID
    HID_DESC_PID,                   // 22 wProductID
    HID_DESC_VERID,                 // 24 wVersionID
    HID_DESC_RSVD,                  // 26 Reserved
    HID_DESC_RSVD                   // 28 Reserved
};




WORD Hidi2c_Slave_GetReportSize(void)
{
    return  (sizeof(Hidi2c_ReportDescriptor));
}
/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_InitReportFeature - init feature report
 *---------------------------------------------------------------------------*/
void Hidi2c_Slave_InitReportFeature(void)
{
    #if USE_ACCE_SENSOR
    Hidi2c_RID1_FeatureReport.reportId                = RID_ACCELEROMETER;
    Hidi2c_RID1_FeatureReport.connectionType          = HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED;
    Hidi2c_RID1_FeatureReport.reportingState          = HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS;
    Hidi2c_RID1_FeatureReport.powerState              = HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER;
    Hidi2c_RID1_FeatureReport.sensorState             = HID_USAGE_SENSOR_STATE_READY;
    Hidi2c_RID1_FeatureReport.reportInterval          = RID1_FEATURE_DEFAULT_INTERVAL;
    Hidi2c_RID1_FeatureReport.sensorAccuracy          = RID1_FEATURE_DEFAULT_ACCURACY;
    Hidi2c_RID1_FeatureReport.sensorResolution        = RID1_FEATURE_DEFAULT_RESOLUTION;
    Hidi2c_RID1_FeatureReport.changeSensitivityAbs    = RID1_FEATURE_DEFAULT_SENSITIVITY;
    Hidi2c_RID1_FeatureReport.rangeMaximum            = RID1_FEATURE_DEFAULT_MAX;
    Hidi2c_RID1_FeatureReport.rangeMinimum            = RID1_FEATURE_DEFAULT_MIN;
    Hidi2c_RID1_FeatureReport.minimumReportInterval   = RID1_FEATURE_DEFAULT_MIN_INTERVAL;
    #endif //USE_ACCE_SENSOR

    #if USE_CUSTOM_SENSOR
    Hidi2c_RID2_FeatureReport.reportId                = RID_CUSTOM_SENSOR;
    Hidi2c_RID2_FeatureReport.connectionType          = HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED;
    Hidi2c_RID2_FeatureReport.reportingState          = HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS;
    Hidi2c_RID2_FeatureReport.powerState              = HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER;
    Hidi2c_RID2_FeatureReport.sensorState             = HID_USAGE_SENSOR_STATE_READY;
    Hidi2c_RID2_FeatureReport.reportInterval          = RID2_FEATURE_DEFAULT_INTERVAL;
    Hidi2c_RID2_FeatureReport.sensorAccuracy          = RID2_FEATURE_DEFAULT_ACCURACY;
    Hidi2c_RID2_FeatureReport.sensorResolution        = RID2_FEATURE_DEFAULT_RESOLUTION;
    Hidi2c_RID2_FeatureReport.changeSensitivityAbs    = RID2_FEATURE_DEFAULT_SENSITIVITY;
    Hidi2c_RID2_FeatureReport.rangeMaximum            = RID2_FEATURE_DEFAULT_MAX;
    Hidi2c_RID2_FeatureReport.rangeMinimum            = RID2_FEATURE_DEFAULT_MIN;
    Hidi2c_RID2_FeatureReport.minimumReportInterval   = RID2_FEATURE_DEFAULT_MIN_INTERVAL;
    Hidi2c_RID2_FeatureReport.sensorDesc[0]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[1]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[2]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[3]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[4]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[5]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[6]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[7]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[8]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[9]           = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[10]          = ' ';
    Hidi2c_RID2_FeatureReport.sensorDesc[11]          = 0;
    #endif //USE_CUSTOM_SENSOR
}

/*-----------------------------------------------------------------------------
 * Hidi2c_Slave_InitReportInput - init feature report
 *---------------------------------------------------------------------------*/
void Hidi2c_Slave_InitReportInput(void)
{
    #if USE_ACCE_SENSOR
    Hidi2c_RID1_InputReport.reportId     = RID_ACCELEROMETER;
    Hidi2c_RID1_InputReport.sensorState  = RID1_INPUT_DEFAULT_STATE;
    Hidi2c_RID1_InputReport.sensorEvent  = RID1_INPUT_DEFAULT_EVENT;
    Hidi2c_RID1_InputReport.accX = 0x00;
    Hidi2c_RID1_InputReport.accY = 0x00;
    Hidi2c_RID1_InputReport.accZ = 0x00;
    Hidi2c_RID1_InputReport.accMotion = HIDSENSOR_DELTA_BELOW_SENSITIVITY;
    #endif //USE_ACCE_SENSOR

    #if USE_CUSTOM_SENSOR
    Hidi2c_RID2_InputReport.reportId     = RID_CUSTOM_SENSOR;
    Hidi2c_RID2_InputReport.sensorState  = RID2_INPUT_DEFAULT_STATE;
    Hidi2c_RID2_InputReport.sensorEvent  = RID2_INPUT_DEFAULT_EVENT;
	Hidi2c_RID2_InputReport.baseX = 0;
    Hidi2c_RID2_InputReport.baseY = 0;
    Hidi2c_RID2_InputReport.baseZ = 0;
	Hidi2c_RID2_InputReport.angleBaseGnd = 0;
    Hidi2c_RID2_InputReport.angleScrGnd  = 0;
    Hidi2c_RID2_InputReport.angleBaseScr = 0;
    #endif //USE_CUSTOM_SENSOR
}


#define GS_RBLOCK_SIZE  12
BYTE GS_rData[GS_RBLOCK_SIZE], Sensor_num;
WORD Angle_B, Angle_P, Angle_PB;

const ACC_CONFIG Axis_Matrix[2] =
{
    {1, 0, 1},
    {0, 0, 0}
};

#if SH_RAW_TYPE_WORD
//void Gsensor_report(WORD Base_X, WORD Base_Y, WORD Base_Z, WORD Tablet_X, WORD Tablet_Y, WORD Tablet_Z)
void Gsensor_report(WORD Base_X, WORD Base_Y, WORD Base_Z, WORD Panel_X, WORD Panel_Y, WORD Panel_Z)
#else
void Gsensor_report(BYTE Base_X, BYTE Base_Y, BYTE Base_Z, BYTE Panel_X, BYTE Panel_Y, BYTE Panel_Z)
#endif
{
    ACC_AXIS Panel;
    ACC_AXIS Base;
    WORD Angle;

    Panel.Signed_X = Axis_Matrix[0].Axis_X;
    Panel.Signed_Y = Axis_Matrix[0].Axis_Y;
    Panel.Signed_Z = Axis_Matrix[0].Axis_Z;
    Panel.Axis_Out_X = Panel_X;
    Panel.Axis_Out_Y = Panel_Y;
    Panel.Axis_Out_Z = Panel_Z;

    Base.Signed_X = Axis_Matrix[1].Axis_X;
    Base.Signed_Y = Axis_Matrix[1].Axis_Y;
    Base.Signed_Z = Axis_Matrix[1].Axis_Z;
    Base.Axis_Out_X = Base_X;
    Base.Axis_Out_Y = Base_Y;
    Base.Axis_Out_Z = Base_Z;

    // Panel G-Sensor
    Gravity_PreProcess(&Panel);
    // Base G-Sensor
    Gravity_PreProcess(&Base);

    if (Hidi2c_Slave_State != HIDI2C_STATE_OFF) //if (Hidi2c_Slave_State == HIDI2C_STATE_INIT)
    {
        if (Sensor_num)
        {
            // Report Panel G-Sensor (Sensor_num = 1)
            Sensor_num = 0;
            #if SH_RAW_TYPE_WORD
            GS_rData[0]  = (BYTE) ~Panel.Axis_Out_X & 0xFF;
            GS_rData[1]  = (BYTE) (~Panel.Axis_Out_X >> 8) & 0xFF;
            GS_rData[2]  = (BYTE) ~Panel.Axis_Out_Y & 0xFF;
            GS_rData[3]  = (BYTE) (~Panel.Axis_Out_Y >> 8) & 0xFF;
            GS_rData[4]  = (BYTE) ~Panel.Axis_Out_Z & 0xFF;
            GS_rData[5]  = (BYTE) (~Panel.Axis_Out_Z >> 8) & 0xFF;
            Hidi2c_Slave_SendInputReport(1, (BYTE *)&GS_rData, 6);
            #else // SH_RAW_TYPE_WORD
            GS_rData[0]  = ~Panel.Axis_Out_X;
            GS_rData[1]  = ~Panel.Axis_Out_Y;
            GS_rData[2]  = ~Panel.Axis_Out_Z;
            Hidi2c_Slave_SendInputReport(1, (BYTE *)&GS_rData, 3);
            #endif //SH_RAW_TYPE_WORD
        }
        else
        {
            // Report Base G-Sensor and Angle (Sensor_num = 0)
            Sensor_num = 1;
            // Calculate Panel Angle
            Angle = Angle_Calculate(Panel.Axis_GS_Y, Panel.Axis_GS_Z);

            if (Panel.Signed_Y || Panel.Signed_Z)
            {
                if (Panel.Signed_Y && Panel.Signed_Z)
                {
                    Angle = 180 + Angle;
                }
                else if (Panel.Signed_Z)
                {
                    Angle = 180 - Angle;
                }
                else
                {
                    Angle = 360 - Angle;
                }
            }
            Angle_P = Angle;

            // Calculate Base Angle
            Angle = Angle_Calculate(Base.Axis_GS_Y, Base.Axis_GS_Z);

            if (Base.Signed_Y || Base.Signed_Z)
            {
                if (Base.Signed_Y && Base.Signed_Z)
                {
                    Angle = 180 - Angle;
                }
                else if (Base.Signed_Z)
                {
                    Angle = 180 + Angle;
                }
            }
            else
            {
                Angle = 360 - Angle;
            }

            Angle_B = Angle;

            if (Angle_B > Angle_P)
            {
                Angle_PB = 360 - Angle_B + Angle_P;
            }
            else
            {
                Angle_PB = (Angle_P - Angle_B);
            }

            //Angle_PB = 360 - Angle_PB;

        }
    }
}

void Gravity_PreProcess(ACC_AXIS *Axis_pntr)
{
    #if SH_RAW_TYPE_WORD

    if (Axis_pntr->Axis_Out_X >= 0x200)
    {
        Axis_pntr->Axis_Out_X = ((~Axis_pntr->Axis_Out_X) & 0x3FF) + 0x001;
        Axis_pntr->Signed_X ^= 1;
    }

    if (Axis_pntr->Axis_Out_Y >= 0x200)
    {
        Axis_pntr->Axis_Out_Y = ((~Axis_pntr->Axis_Out_Y) & 0x3FF) + 0x001;
        Axis_pntr->Signed_Y ^= 1;
    }

    if (Axis_pntr->Axis_Out_Z >= 0x200)
    {
        Axis_pntr->Axis_Out_Z = ((~Axis_pntr->Axis_Out_Z) & 0x3FF) + 0x001;
        Axis_pntr->Signed_Z ^= 1;
    }
    #else // SH_RAW_TYPE_WORD
    if (Axis_pntr->Axis_Out_X >= 0x80)
    {
        Axis_pntr->Axis_Out_X = ~Axis_pntr->Axis_Out_X;
        Axis_pntr->Signed_X ^= 1;
    }

    if (Axis_pntr->Axis_Out_Y >= 0x80)
    {
        Axis_pntr->Axis_Out_Y = ~Axis_pntr->Axis_Out_Y;
        Axis_pntr->Signed_Y ^= 1;
    }

    if (Axis_pntr->Axis_Out_Z >= 0x80)
    {
        Axis_pntr->Axis_Out_Z = ~Axis_pntr->Axis_Out_Z;
        Axis_pntr->Signed_Z ^= 1;
    }
    #endif // SH_RAW_TYPE_WORD

    Axis_pntr->Axis_GS_Y = Axis_pntr->Axis_Out_Y;
    Axis_pntr->Axis_GS_Z = Axis_pntr->Axis_Out_Z;

    #if SH_RAW_TYPE_WORD
    Axis_pntr->Axis_Out_X = (WORD) ((DWORD)Axis_pntr->Axis_Out_X)*1000/256;
    Axis_pntr->Axis_Out_Y = (WORD) ((DWORD)Axis_pntr->Axis_Out_Y)*1000/256;
    Axis_pntr->Axis_Out_Z = (WORD) ((DWORD)Axis_pntr->Axis_Out_Z)*1000/256;
    #else
    Axis_pntr->Axis_Out_X = (BYTE) ((WORD)Axis_pntr->Axis_Out_X)*1000/256;
    Axis_pntr->Axis_Out_Y = (BYTE) ((WORD)Axis_pntr->Axis_Out_Y)*1000/256;
    Axis_pntr->Axis_Out_Z = (BYTE) ((WORD)Axis_pntr->Axis_Out_Z)*1000/256;
    #endif

    if (Axis_pntr->Signed_X)
    {
        Axis_pntr->Axis_Out_X = ~Axis_pntr->Axis_Out_X;
    }
    if (Axis_pntr->Signed_Y)
    {
        Axis_pntr->Axis_Out_Y = ~Axis_pntr->Axis_Out_Y;
    }
    if (Axis_pntr->Signed_Z)
    {
        Axis_pntr->Axis_Out_Z = ~Axis_pntr->Axis_Out_Z;
    }
}

#if SH_RAW_TYPE_WORD
WORD Angle_Calculate(WORD Raw_Y, WORD Raw_Z)
#else
WORD Angle_Calculate(BYTE Raw_Y, BYTE Raw_Z)
#endif
{
    float Angle;
    if (Raw_Z == 0)
    {
        Raw_Z = 1;
    }
    Angle = ((float)Raw_Y / (float)Raw_Z);
    Angle = (atanf(Angle)) * 180 / 3.14159;
    return ((WORD) Angle);
}
#endif // SHL_SUPPORTED

