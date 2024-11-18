/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     OEMTYPE.H - OEM types definition.
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/
#ifndef OEMTYPE_H
#define OEMTYPE_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

typedef union _FAN_CB
{
   BYTE byte;
   struct
   {
      FIELD FanLevels:   3;   /* Bit 3 - 0, Fan Level. */
      FIELD Controlled:  1;   /* Fan Controlled by EC or Host. */
      FIELD MaxFanLevel: 3;
      FIELD :  1;             /* Fan Controlled by EC or Host. */
   } field;
} FAN_CB;

typedef union _BRIGHT_CB
{
   BYTE byte;
   struct
   {
      FIELD BrightLevel:    4;   /* Bit 3 - 0, Fan Level. */
      FIELD MaxLevel:       4;
   } field;
} BRIGHT_CB;

typedef union _CHARGER_CB
{
   BYTE byte;
   struct
   {
      FIELD AutoLearn:      1;  /* Bit 0, Battery in Autolearning. */
      FIELD :               3;
      FIELD LearnLvl:       4;  /* Bit4 ~ 7, Battery laerning step. */
   } field;
} CHARGER_CB;




typedef volatile struct _OEMSMB_DESCRIPTOR
{
    BYTE Chnl;              /* SMBus Channel. */
    BYTE ADDR;              /* Slave Address. */
    BYTE PRTCL;             /* protocol. */
    BYTE DevCmd;            /* Device Command. */
    BYTE *pntr;             /* Pointer to save info. */
} OEMSMB_DESCRIPTOR;

typedef volatile struct _THML_DESCRIPTOR
{
    BYTE Chnl;              /* SMBus Channel. */
    BYTE ADDR;
    BYTE PRTCL;             /* Bit 7 ~ 6 for channel select, bit 5 ~ 0 for protocol. */
    BYTE DevCmd;            /* Port Pin defined following kernel code. */
    BYTE data;              /* data byte to be written to thermal.. */
    BYTE next;              /* Next process need. */
} THML_DESCRIPTOR;

typedef union _DEV_SMBCTRL
{
    BYTE byte;
    struct
    {
        FIELD PollIndex:    4;
        FIELD :             2;
        FIELD FirstLoop:    1;
        FIELD DevTable:     1;
    } field;
} DEV_SMBCTRL;

typedef volatile struct _THMLTBL_DESCRIPTOR
{
    BYTE Trip;
    BYTE Hyst;
} THMLTBL_DESCRIPTOR;

typedef volatile struct _BL_DESCRIPTOR
{
    BYTE BATBL;
    BYTE ADBL;
} BL_DESCRIPTOR;

typedef volatile struct _BAT_INFO
{
    WORD Voltage;           
    WORD Temperature;           
    WORD BAT_RSOC;          
    WORD BAT_RC;            
    WORD BAT_RTTE;          
    WORD BAT_Status;        
    WORD BAT_FCC;           
    WORD BAT_DC;            
    WORD BAT_DV; 
    BYTE DataValid;
} BAT_INFO;

#endif /* ifndef OEMTYPE_H */
