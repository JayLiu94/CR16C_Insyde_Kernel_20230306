/*-----------------------------------------------------------------------------
 * Copyright (c) 2006-2011 by Nuvoton Technology Corporation
 * All rights reserved.
 *<<<--------------------------------------------------------------------------
 * File Contents:
 *     HID.c - HID over I2C module
 *
 * Project:
 *     Driver Set for WPCE78nx Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------>>>*/

#include "swtchs.h"
#include "types.h"
#include "i2c.h"
#include "regs.h"
#include "ps2.h"
#include "purdat.h"
#include "purxlt.h"
#include "hid.h"
#include "icu.h"
#include "pindef.h"
//#include "fu_oem.h"

BYTE Report_ID;     /* Device Report ID. */
BYTE Comb_State;

//HID_KBD kBuffer;    /* Keyboard Input Report Buffer. */
BYTE kBuffer[12];         /* Keyboard Input Report Buffer. */

BYTE KBD_Retry;
FLAG bFnKeyHold;
// Johnny 5/7/2015 added for JP KBD feature
extern BITS_8 temp_scanner_state;
// End of 5/7/2015 added
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
#define Media_Sys_Crisis        0x0230

// oem hot key
#define HotKey_BrgDwn           0x1000
#define HotKey_BrgUp            0x1001
#define Hotkey_DisplaySw        0x1002
#define HotKey_TCL_LDAngle      0x1003  // Europe
#define HotKey_TCL_RDAngle      0x1004  // Europe
#define HotKey_TCL_Period       0x1005  // Europe
#define HotKey_Question         0x1006  // Question
#define HotKey_Search           0x1007  // Search

const BYTE MediaKey_table[][3] =
{
    0x0F, (Media_NextTrack     & 0xFF), (Media_NextTrack     >> 8),
    0x10, (Media_PreviousTrack & 0xFF), (Media_PreviousTrack >> 8),
    0x11, (Media_Stop          & 0xFF), (Media_Stop          >> 8),
    0x12, (Media_PlayPause     & 0xFF), (Media_PlayPause     >> 8),
    0x17, (Media_Mute          & 0xFF), (Media_Mute          >> 8),
    0x18, (Media_VolumeUp      & 0xFF), (Media_VolumeUp      >> 8),
    0x19, (Media_VolumeDown    & 0xFF), (Media_VolumeDown    >> 8),
    0x37, (Media_Mail          & 0xFF), (Media_Mail          >> 8),
    0x38, (Media_WWW_Search    & 0xFF), (Media_WWW_Search    >> 8),
    0x39, (Media_WWW_Home      & 0xFF), (Media_WWW_Home      >> 8),
    0x56, (Media_WWW_Back      & 0xFF), (Media_WWW_Back      >> 8),
    0x57, (Media_WWW_Forward   & 0xFF), (Media_WWW_Forward   >> 8),
    0x5E, (Media_WWW_Stop      & 0xFF), (Media_WWW_Stop      >> 8),
    0x5F, (Media_WWW_Refresh   & 0xFF), (Media_WWW_Refresh   >> 8),
    0x60, (Media_WWW_Favorites & 0xFF), (Media_WWW_Favorites >> 8),
    0x6D, (Media_Calculator    & 0xFF), (Media_Calculator    >> 8),
    0x6E, (Media_MyComputer    & 0xFF), (Media_MyComputer    >> 8),
    0x6F, (Media_MediaSelect   & 0xFF), (Media_MediaSelect   >> 8)
};

#define MediaKey_Convert_Size (sizeof(MediaKey_table)/(sizeof(BYTE) * 3))

void HID_Init(void)
{
    kBuffer[0] = 0x00;    /* LSB of Size */
    kBuffer[1] = 0x00;    /* MSB of Size */
    kBuffer[2] = 0x00;    //Report_ID;
    kBuffer[3] = 0x00;
    kBuffer[4] = 0x00;
    kBuffer[5] = 0x00;
    kBuffer[6] = 0x00;
    kBuffer[7] = 0x00;
    kBuffer[8] = 0x00;
    kBuffer[9] = 0x00;
    kBuffer[10] = 0x00;
    kBuffer[11] = 0x00;
    //kBuffer[2] = 0x00;
    Comb_State = 0;
    Report_ID = 0;
    KBD_Retry = 0;
}

/*-----------------------------------------------------------------------------
 * KBD_to_HID - send KBD HID data to SOC
 *---------------------------------------------------------------------------*/
int Hidi2c_Slave_SendInputReport(BYTE * report, WORD len);

void KBD_to_HID(BYTE data, SMALL event)
{
    BYTE comb = 0;
    BYTE i;
    BYTE usageH;
    BYTE usageL;
    FLAG find = 0;

    static const BYTE HID_table[] =
    {/*  0     1     2     3     4     5     6     7
         8     9     A     B     C     D     E     F */
        0x00, 0x42, 0x00, 0x3E, 0x3C, 0x3A, 0x3B, 0x45,   /* 0x00 ~ 0x07 */
        0x00, 0x43, 0x41, 0x3F, 0x3D, 0x2B, 0x35, 0x00,   /* 0x08 ~ 0x0F */
        0x00, 0x00, 0x00, 0x88, 0x00, 0x14, 0x1E, 0x00,   /* 0x10 ~ 0x17 */
        0x00, 0x00, 0x1D, 0x16, 0x04, 0x1A, 0x1F, 0x00,   /* 0x18 ~ 0x1F */
        0x00, 0x06, 0x1B, 0x07, 0x08, 0x21, 0x20, 0x00,   /* 0x20 ~ 0x27 */
        0x00, 0x2C, 0x19, 0x09, 0x17, 0x15, 0x22, 0x00,   /* 0x28 ~ 0x2F */
        0x00, 0x11, 0x05, 0x0B, 0x0A, 0x1C, 0x23, 0x00,   /* 0x30 ~ 0x37 */
        0x00, 0x00, 0x10, 0x0D, 0x18, 0x24, 0x25, 0x00,   /* 0x38 ~ 0x3F */
        0x00, 0x36, 0x0E, 0x0C, 0x12, 0x27, 0x26, 0x00,   /* 0x40 ~ 0x47 */
        0x00, 0x37, 0x38, 0x0F, 0x33, 0x13, 0x2D, 0x00,   /* 0x48 ~ 0x4F */
        0x00, 0x87, 0x34, 0x00, 0x2F, 0x2E, 0x00, 0x00,   /* 0x50 ~ 0x57 */
        0x39, 0x00, 0x28, 0x30, 0x00, 0x32, 0x00, 0x00,   /* 0x58 ~ 0x5F */
        0x00, 0x64, 0x00, 0x92, 0x8A, 0x00, 0x2A, 0x8B,   /* 0x60 ~ 0x67 */
        0x00, 0x59, 0x89, 0x5C, 0x5F, 0x00, 0x00, 0x00,   /* 0x68 ~ 0x6F */
        0x62, 0x63, 0x5A, 0x5D, 0x5E, 0x60, 0x29, 0x53,   /* 0x70 ~ 0x77 */
        0x44, 0x57, 0x5B, 0x56, 0x55, 0x61, 0x47, 0x9A,   /* 0x78 ~ 0x7F */
        0x40, 0x58, 0xE3, 0xE7, 0x65, 0x81, 0x82, 0x83,   /* 0x80 ~ 0x87 */
        0xE1, 0xE5, 0xE2, 0xE6, 0xE0, 0xE4, 0x00, 0x00,   /* 0x88 ~ 0x8F */
        0x00, 0x48, 0x00, 0x9A, 0x00, 0x00, 0x00, 0x00,   /* 0x90 ~ 0x97 */
        0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0x48, 0x54,   /* 0x98 ~ 0x9F */
        0x49, 0x4C, 0x4A, 0x4D, 0x4B, 0x4E, 0x50, 0x52,   /* 0xA0 ~ 0xA7 */
        0x51, 0x4F                                        /* 0xA8 ~ 0xA9 */
    };

    static const BYTE Comb_Table[] =
    /* L-Win, R-Win, L-Shift, R-Shift, L-ALT, R-ALT, L-CTL, R-CTL */
    {0x08, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10};

    if (event == REPEAT_EVENT)
    {
        return;
    }
    else if ((data == 0x82) || (data == 0x83))
    {
        comb = data - 0x81;
    }
    else if ((data >= 0x88) && (data <= 0x8D))
    {
        comb = data - 0x85;
        switch(comb)
        {
            case 0x05:
                temp_scanner_state.bit.ALT = 1; // Alt
                break;
        }
    }
    else if (temp_scanner_state.bit.ALT)
    {
        switch(data)
        {
            // ALT+ESC
            case 0x76:
                data = 0x0E;
                break;
        }
    }

    if (comb)
    {
        Report_ID = 0x08;   /* [MCHT 2012/04/24] */
        if (event == MAKE_EVENT)
        {
            Comb_State |= Comb_Table[comb - 1];
        }
        else
        {
            Comb_State &= ~Comb_Table[comb - 1];
            temp_scanner_state.bit.ALT = 0;
        }
        data = 0;
    }
    else
    {
        find = 0;
        // enhance keyboard -- media key
        for(i = 0; ((i < MediaKey_Convert_Size) && (find == 0)); i++)
        {
            if (data == MediaKey_table[i][0])
            {
                find = 1;
                Report_ID = 0x09;
                usageL = MediaKey_table[i][1];
                usageH = MediaKey_table[i][2];
            }
        }

        if (find == 0)
        {
            Report_ID = 0x08;
            data = HID_table[data];
        }
    }

    kBuffer[0] = 0x0B; /* LSB of Size */
    kBuffer[1] = 0; /* MSB of Size */
    kBuffer[2] = Report_ID;

    kBuffer[3] = Comb_State;
    kBuffer[4] = 0;

    if (event == MAKE_EVENT)
    {   /* Key make */
        if (Report_ID == 0x08)
        {
            for (i = 0; i < 6; i++)
            {   /* Searching key buffer. */
                if (kBuffer[i + 5] == 0)
                {   /* Available for update. */
                    kBuffer[i + 5] = data;
                    break;
                }
            }
        }
        else if (Report_ID == 0x09)
        {
            kBuffer[3] = usageL;
            kBuffer[4] = usageH;
            for (i = 0; i < 6; i++)
            {   /* Searching key buffer. */
                kBuffer[i + 5] = 0;
            }
        }
    }
    else
    {   /* Key released. */
        for (i = 1; i < 8; i++)
        {
            kBuffer[i+3] = 0;
        }
    }

    //Hidi2c_Slave_SendInputReport((BYTE *) kBuffer, 0x0B);
    if (Hidi2c_Slave_SendInputReport((BYTE *) kBuffer, 0x0B) != 0)
    {
        KBD_Retry = 1;
    }
}

FLAG HotKey_to_HID(WORD data, SMALL event)
{
    BYTE i;
    BYTE usageH;
    BYTE usageL;

    // check if not hotkey then exit
    if((data > HotKey_Search) || (data < HotKey_BrgDwn))
    {
        return (0);
    }

    if (event == REPEAT_EVENT)
    {
        return (1);
    }

    kBuffer[0] = 0x0B; /* LSB of Size */
    kBuffer[1] = 0; /* MSB of Size */
    kBuffer[2] = Report_ID;

    if (event == MAKE_EVENT)
    {   /* Key make */
        if (Report_ID == 0x08)
        {
            for (i = 0; i < 6; i++)
            {   /* Searching key buffer. */
                kBuffer[i + 7] = 0;
            }
        }
        if (Report_ID == 0x0A)
        {
            for (i = 0; i < 6; i++)
            {   /* Searching key buffer. */
                kBuffer[i + 5] = 0;
            }
        }
    }
    else
    {   /* Key released. */
        for (i = 0; i < 8; i++)
        {
            if(bFnKeyHold)
            {
                i += 2;
                kBuffer[i+3] = 0;
            }
            else
            {
                kBuffer[i+3] = 0;
            }
        }
    }
    if (Hidi2c_Slave_SendInputReport((BYTE *) kBuffer, 0x0B) != 0)
    {
        KBD_Retry = 1;
    }
    return (1);
}

void Clear_emulate_Keys(void)
{
    BYTE i;


    for (i = 0; i < 9; i++)
    {
        kBuffer[i+3] = 0;
    }
    bFnKeyHold = 0;
    //Hidi2c_Slave_SendInputReport((BYTE *) kBuffer, 0x0B);
    if (Hidi2c_Slave_SendInputReport((BYTE *) kBuffer, 0x0B) != 0)
    {
        KBD_Retry = 1;
    }
}
