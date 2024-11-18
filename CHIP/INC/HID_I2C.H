/* ---------------------------------------------------------------
 * MODULE HID_I2C.H
* --------------------------------------------------------------- */

#ifndef HID_I2C_H
#define HID_I2C_H

#include "i2c.h"

#ifndef HID_OV_I2C_SUPPORTED
#error HID_OV_I2C_SUPPORTED switch is not defined.
#elif HID_OV_I2C_SUPPORTED

/* EC channel and slave addr and Interrupt pin for HID Over I2C support */
#define HID_I2C_CH          0x02
#define HID_I2C_ADDR        (0x50 << 1)
#define LPC_I2C_ADDR        0x68
#define HID_I2C_BUF_SIZE    64  /* enough or too mcuh ?? */

#define USE_ACCE_SENSOR     1   /* standard Accelerometer sensor */
#define USE_CUSTOM_SENSOR   1   /* customer sensor */
#define SH_RAW_TYPE_WORD    0   /* Sensor raw data type, 0 for BYTE, 1 for WORD */

extern BYTE Hidi2c_Slave_Buf[HID_I2C_BUF_SIZE];

// The HID Button Usage ID for Media Center Keypad
// HID Usage Page: 0x01
#define System_Power_Event      0x81
#define System_Sleep_Event      0x82
#define System_Wake_Event       0x83

// The HID Button Usage ID for Media Center and Enhance keyboard
// HID Usage Page: 0x0C
#define Media_Guide             0x008D
#define Media_ChanPageUp        0x009C
#define Media_ChanPageDown      0x009D
#define Media_Play              0x00B0
#define Media_Pause             0x00B1
#define Media_Record            0x00B2
#define Media_FastForward       0x00B3
#define Media_Rewind            0x00B4
#define Media_SkipForward       0x00B5
#define Media_NextTrack         0x00B5
#define Media_SkipBack          0x00B6
#define Media_PreviousTrack     0x00B6
#define Media_Stop              0x00B7
#define Media_PlayPause         0x00CD
#define Media_Volume            0x00E0
#define Media_Mute              0x00E2
#define Media_Bass              0x00E3
#define Media_Treble            0x00E4
#define Media_BassBoost         0x00E5
#define Media_Loudness          0x00E7
#define Media_VolumeUp          0x00E9
#define Media_VolumeDown        0x00EA
#define Media_BassUp            0x0152
#define Media_BassDown          0x0153
#define Media_TrebleUp          0x0154
#define Media_TrebleDown        0x0155
#define Media_MediaSelect       0x0183
#define Media_Mail              0x018A
#define Media_Calculator        0x0192
#define Media_MyComputer        0x0194
#define Media_Print             0x0208
#define Media_MoreInfo          0x0209
#define Media_WWW_Search        0x0221
#define Media_WWW_Home          0x0223
#define Media_WWW_Back          0x0224
#define Media_WWW_Forward       0x0225
#define Media_WWW_Stop          0x0226
#define Media_WWW_Refresh       0x0227
#define Media_WWW_Favorites     0x022A

// The HID Button Usage ID for Media Center
// HID Usage Page: 0xFFBC
#define CIR_GreenStart          0x0D
#define CIR_ClosedCaptionOnOff  0x2B
#define CIR_TeletextOnOff       0x5A
#define CIR_TeletextRed         0x5B
#define CIR_TeletextGreen       0x5C
#define CIR_TeletextYellow      0x5D
#define CIR_TeletextBlue        0x5E
#define CIR_LiveTV              0x25
#define CIR_Music               0x47
#define CIR_RecordedTV          0x48
#define CIR_Pictures            0x49
#define CIR_Videos              0x4A
#define CIR_FMRdios             0x50
#define CIR_Extras              0x3C
#define CIR_ExtrasApp           0x3D
#define CIR_DVDMenu             0x24
#define CIR_DVDAngle            0x4B
#define CIR_DVDAudio            0x4C
#define CIR_DVDSubtitle         0x4D
#define CIR_Eject               0x28
#define CIR_DVDTopMenu          0x43
#define CIR_Ext0                0x32
#define CIR_Ext1                0x33
#define CIR_Ext2                0x34
#define CIR_Ext3                0x35
#define CIR_Ext4                0x36
#define CIR_Ext5                0x37
#define CIR_Ext6                0x38
#define CIR_Ext7                0x39
#define CIR_Ext8                0x3A
#define CIR_Ext9                0x80
#define CIR_Ext10               0x81
#define CIR_Ext11               0x6F
#define CIR_Zoom                0x27
#define CIR_ChannelInput        0x42
#define CIR_SubAudio            0x2D
#define CIR_10                  0x3E
#define CIR_11                  0x3F
#define CIR_12                  0x40

// The HID Button Usage ID for Media Center Keypad
// HID Usage Page: 0x07
#define KeyPad_Up               0x0052
#define KeyPad_Down             0x0051
#define KeyPad_Left             0x0050
#define KeyPad_Right            0x004F
#define KeyPad_OK               0x0052
#define KeyPad_0                0x0027
#define KeyPad_1                0x001E
#define KeyPad_2                0x001F
#define KeyPad_3                0x0020
#define KeyPad_4                0x0021
#define KeyPad_5                0x0022
#define KeyPad_6                0x0023
#define KeyPad_7                0x0024
#define KeyPad_8                0x0025
#define KeyPad_9                0x0026
#define KeyPad_Clear            0x0029
#define KeyPad_Enter            0x0028
#define KeyPad_Sharp            0xE120
#define KeyPad_Star             0xE125


#define HID_Descriptor_Length   30

#define KBD_HID_I2C_CHANNEL     SMB_3

#define KBD_HID_I2C_ADDR    (0x1D << 1)

#define KBD_HIDDesc_Reg     0x0001
#define KBD_ReportDesc_Reg  0x0002
#define KBD_Input_Reg       0x0003
#define KBD_Output_Reg      0x0004
#define KBD_Command_Reg     0x0005
#define KBD_Data_Reg        0x0006

#define KBD_HID_INT_ACTIVE      0
#define KBD_HID_INT_INACTIVE    1

#define AUX_HID_I2C_CHANNEL     SMB_3

#define AUX_HID_I2C_ADDR    0x50

#define AUX_HIDDesc_Reg     0x0001
#define AUX_ReportDesc_Reg  0x0002
#define AUX_Input_Reg       0x0003
#define AUX_Output_Reg      0x0004
#define AUX_Command_Reg     0x0005
#define AUX_Data_Reg        0x0006

#define AUX_HID_INT_ACTIVE      0
#define AUX_HID_INT_INACTIVE    1

#define HID_Initial     bit0
#define HID_Request     bit1
#define HID_PwrSts      bit2
#define HID_DataReq     bit3

typedef volatile struct _HID_I2C_VAR
{
    BYTE    TxBuf[16];
    BYTE    RxBuf[16];

    WORD    Register;
    WORD    Command;
    WORD    Frequency;
    WORD    Protocol;
    WORD    HostData;

    BITS_8  Sts;
} HID_I2C_VAR;


extern BITS_8 HID_STATE;
#define Keyboard_Required       bit0
#define Mouse_Required          bit1
#define PowerEvent_Required     bit2
#define AudioCtrl_Required      bit3
#define MediaCenter_Required    bit4
#define Keypad_Required         bit5
#define Host_Request            bit6


extern HID_I2C_VAR  Kbd_HID;
extern WORD HID_Descriptor[];
extern BYTE Hidi2c_ReportDescriptor[];

void Kbd_Handle_Request(void);
void Kbd_HID_Reset(void);
void Kbd_HID_Get_Report(void);
void Kbd_HID_Set_Report(void);

void HID_I2C_Init(void);
void HID_Handle(void);
void HID_Get_Register(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_Check_Command_Count(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_Check_Data_Request(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_Receive_Done(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_Transmit_Done(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Cfg_HID_BufPntr(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_ReceivePtrl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_GetReadPrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void HID_GetWritePrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel);

void Hidi2c_Slave_Cfg_BufPntr(I2C_VAR *I2c_Var_Pntr, BYTE Channel);
void Hidi2c_Slave_Rx(const BYTE * rcv_buff, WORD rcv_size);
void Hidi2c_Slave_Tx(const BYTE ** buff, WORD * size);
int Hidi2c_Slave_SendInputReport(BYTE rid, BYTE * report, WORD len);

void KBD_to_HID(BYTE data, SMALL event);
void AUX_to_HID(BYTE data, SMALL port);
void HID_Keyboard_Event(BYTE data, SMALL event);
void HID_Power_Event(BYTE data, SMALL event);
void HID_AudioCtrl_Event(WORD data, SMALL event);
void HID_MediaCenter_Event(BYTE data, SMALL event);
void HID_KeyPad_Event(WORD data, SMALL event);

#endif /* if HID_OVER_I2C_SUPPORTED */



#endif /* ifndef HID_I2C_H */
