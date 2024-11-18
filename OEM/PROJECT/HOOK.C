/* ----------------------------------------------------------------------------
 * MODULE HOOK.C
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * This file contains OEM stub "hooks". To replace a
 * stub with OEM code (or supplement the core code),
 * add the code to the hooks in this file or comment
 * out the stub and put the new hook in another file.
 *
 * Revision History (Started July 23, 1996 by GLP)
 *
 * GLP
 * 29 Jun 98   Updated for use with CORE 1.01.20.
 * 05 Oct 98   Updated for new Keyboard Controller functionality.
 * 19 Oct 98   Updated Hookc_Read_EC and Hookc_Write_EC because
 *             of addition of overlapping Config EC space.
 * 17 Aug 99   Added Hookc_Chk_Flashing_Safe.  Updated copyright.
 *             Moved Hookc_Read_EC and Hookc_Write_EC because they
 *             are in PURACPI module.
 * 24 Aug 99   Added Hookc_OEM_Sys_Ctrl_Funct and
 *             Hookc_Get_Hotkey_Pntr.  Changed Hookc_Key_Enable to
 *             take a parameter and return a flag.  Removed
 *             Hookc_Ext_To_Pc and Hookc_Send_To_Pc.
 * 07 Sep 99   Added Hookc_Get_Custom_Table_Size.
 * EDH
 * 15 Feb 05  Added to process command/data write from Third Host Interface.
 * ------------------------------------------------------------------------- */

#define HOOK_C

#include "swtchs.h"
#include "types.h"
#include "purhook.h"
#include "chiphook.h"
#include "regs.h"
#include "ini.h"
#include "proc.h"
#include "host_if.h"
#include "timers.h"
#include "oem.h"
#include "scan.h"
#include "purdat.h"
#include "purmain.h"
#include "purfunct.h"
#include "purxlt.h"
#include "a_to_d.h"
#include "i2c.h"
#if SMB_FLUPDATE_SUPPORTED
#include "fu_oem.h"
#endif
#if HID_OV_I2C_SUPPORTED
#include "sensorhub.h"
#include "hid_i2c.h"
#endif // HID_OV_I2C_SUPPORTED

void Enable_SIB(void);

/* --------- Enabled in PURHOOK.H. --------- */

/* ---------------------------------------------------------------
   Name: Hookc_Low_Power_Enter

   Hook just before entering low power mode.  Interrupts
   are disabled here, but will be enabled after this hook.
   --------------------------------------------------------------- */
void Hookc_Low_Power_Enter(void)
{

   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Low_Power_Exit

   Hook after exiting low power mode with a valid wake up event.

   The interrupts are disabled.

   Return with interrupts enabled.
   --------------------------------------------------------------- */
void Hookc_Low_Power_Exit(void)
{
   Enable_Irq();



}

/* ---------------------------------------------------------------
   Name: Hookc_Exit_Idle


   --------------------------------------------------------------- */
void Hookc_Exit_Idle(void)
{
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Enter_Service


   --------------------------------------------------------------- */
void Hookc_Enter_Service(void)
{
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Get_Multibyte

   Return a byte of data in the lower byte of the returned
   WORD.  Return a 0 in the higher byte of the returned WORD.
   --------------------------------------------------------------- */
WORD Hookc_Get_Multibyte(void)
{
   return 0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Key_Enable

   Input: data - Translated data to send to host.

   Returns:
      TRUE  if data may be sent to the host.
      FALSE to prevent data from being sent to the host.

   This hook is called before sending translated data from the
   keyboard to the host.  If the translate bit in the Controller
   Command Byte is not set, the hook is not called.
   --------------------------------------------------------------- */
FLAG Hookc_Key_Enable(WORD data)
{
   return(TRUE);
}

/* ---------------------------------------------------------------
   Name: Hookc_Unlock


   --------------------------------------------------------------- */
void Hookc_Unlock(void)
{
   ;
}

/* ---------------------------------------------------------------
   Name: Hookc_Service_Scan


   --------------------------------------------------------------- */
FLAG Hookc_Service_Scan(void)
{
   return (TRUE);
}



/*********************** Hooks in PURACPI.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Read_EC

   This function is called before data is
   read from the Embedded Controller (EC) space.

   This is called for both
      Command 80h, Read Embedded Controller
   and
      Command 90h, Legacy Read EC Space.

   Input: index is the offset into the EC space.

          index = 0 through LAST_VALID_CONFIG_EC_ADDRESS is
                  in FIRST_EC_CONFIG_VARIABLE and up.

          index = (LAST_VALID_CONFIG_EC_ADDRESS + 1) through LAST_CONFIG_EC_ADDRESS
                  are invalid overlapping ACPI EC space addresses.

          index = (LAST_CONFIG_EC_ADDRESS + 1) through
                  (EC_ADDR_MAX + LAST_CONFIG_EC_ADDRESS + 1) are in
                  EC_Space[index - (LAST_CONFIG_EC_ADDRESS + 1)].

   Return: a value with all bits set to allow the CORE to handle
           reading the EC space.
           otherwise, return the data (byte in lower part of WORD)
           to be sent to the Host.
   --------------------------------------------------------------- */
WORD Hookc_Read_EC(BYTE index)
{
   return (WORD) ~0;
}

/* ---------------------------------------------------------------
   Name: Hookc_Write_EC

   This function is called before data is
   written to the Embedded Controller (EC) space.

   This is called for both
      Command 81h, Write Embedded Controller
   and
      Command 91h, Legacy Write EC Space.

   Input: EC_Addr is the offset into the EC_Space.
          data is the byte to write into the EC_Space array.

          EC_Addr = 0 through LAST_VALID_CONFIG_EC_ADDRESS is
                    in FIRST_EC_CONFIG_VARIABLE and up.

          EC_Addr = (LAST_VALID_CONFIG_EC_ADDRESS + 1) through LAST_CONFIG_EC_ADDRESS
                    are invalid overlapping ACPI EC space addresses.

          EC_Addr = (LAST_CONFIG_EC_ADDRESS + 1) through
                    (EC_ADDR_MAX + LAST_CONFIG_EC_ADDRESS + 1) are in
                    EC_Space[EC_Addr - (LAST_CONFIG_EC_ADDRESS + 1)].

   Return: 0 to allow the CORE to handle writing the EC space.
           otherwise, return 1.
   --------------------------------------------------------------- */
FLAG Hookc_Write_EC(BYTE index, BYTE data)
{
   return(0);
}

/*********************** Hooks in PUREXT.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Chk_Flashing_Safe

   This function is called by Command 06h.

   Return: 0 if it is NOT safe to flash the PROM at this time.
           1 is if IS safe to flash the PROM now.
   --------------------------------------------------------------- */
FLAG Hookc_Chk_Flashing_Safe(void)
{
   return (FLAG) 1;  /* Assume it is safe. */
}

/*********************** Hooks in PURFUNCT.C *********************/

/* ---------------------------------------------------------------
   Name: Hookc_Sys_Ctrl_Entry

   Input: data = control function word
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)

         --- data is... ---

      0xxy,yyyy,zzzz,zzzz

      (X) = Trigger Action for hotkey and port pin input events
            00 = Perform function on key Make
            01 = Perform function on key Make and Repeat
            10 = Perform function on key Break
            11 = Perform function on key Make,
                 Complement of function on key Break

      (Y) = System Control Function to perform

      (Z) = Data to support function described by (Y)

   This function is called when a System Control Function is to be
   performed.  It is called before any System Control Function
   processing is done to allow the System Control Function to be
   replaced.

   Return: 0 to allow the CORE to handle control function.
           otherwise, return 1.
   --------------------------------------------------------------- */
FLAG Hookc_Sys_Ctrl_Entry(WORD data, SMALL event)
{
   return(0);
}

/* ---------------------------------------------------------------
   Name: Hookc_OEM_Sys_Ctrl_Funct

   Input: data = control function word
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)

         --- data is... ---

      0xx1,0000,zzzz,zzzz

      (X) = Trigger Action for hotkey and port pin input events
            00 = Perform function on key Make
            01 = Perform function on key Make and Repeat
            10 = Perform function on key Break
            11 = Perform function on key Make,
                 Complement of function on key Break

      (Z) = Data to support function

   This function is called when the OEM System Control Function is to
   be performed.  It is only called when the contact event matches
   X in the control function word as in the following table:

       X       Called when event is
      ---   --------------------------
      00    MAKE_EVENT
      01    MAKE_EVENT or REPEAT_EVENT
      10    BREAK_EVENT
      11    MAKE_EVENT or BREAK_EVENT

   Return: nothing.
   --------------------------------------------------------------- */
void Hookc_OEM_Sys_Ctrl_Funct(WORD data, SMALL event)
{
   ;
}

/*********************** Hooks in PURSCN.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Scan_Keys

   --------------------------------------------------------------- */
void Hookc_Scan_Keys(void)
{
   ;
}

/*********************** Hooks in PURXLT.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Key_Proc

   --------------------------------------------------------------- */
void Hookc_Key_Proc(void)
{
   ;
}

int Hookc_enable_FnSticky(void)
{
    return (int) 0;
}


/* --------- Enabled in CHIPHOOK.H. --------- */

/*********************** Hooks in I2C.C ***********************/
#if I2C_SLAVE_SUPPORTED
BYTE Hookc_GetReadPrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE rval;
    #if HID_OV_I2C_SUPPORTED
    if ((Channel == HID_CHANNEL) && (I2c_Var_Pntr->Addr == I2C_HID_ADDR))
    {
        if (I2c_Var_Pntr->i2c_slave_rcount != 0)
        {
            Hidi2c_Slave_Rx(I2c_Var_Pntr->i2c_slave_rbuf, I2c_Var_Pntr->i2c_slave_rcount);
        }
        Hidi2c_Slave_Tx((const BYTE **)&I2c_Var_Pntr->i2c_slave_wbuf, (WORD *) &I2c_Var_Pntr->count);

        I2c_Var_Pntr->count++;
        rval = I2c_Var_Pntr->i2c_slave_prtl = SMB_READ_BLOCK;
    }
    else
    #endif // HID_OV_I2C_SUPPORTED
    if (I2c_Var_Pntr->i2c_slave_cmd < 0x20)
    {
        rval = I2C_READ_WORD;
    }
    else
    {
        rval = I2C_READ_BLOCK;
    }
    return(rval);
}

BYTE Hookc_GetWritePrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    BYTE rval;

    #if HID_OV_I2C_SUPPORTED
    if ((Channel == HID_CHANNEL) && (I2c_Var_Pntr->Addr == I2C_HID_ADDR))
    {
        I2c_Var_Pntr->i2c_slave_prtl = I2C_WRITE;
        I2c_Var_Pntr->count = HID_I2C_BUF_SIZE;
        return(I2C_WRITE);
    }
    else
    #endif // HID_OV_I2C_SUPPORTED
    if (I2c_Var_Pntr->i2c_slave_cmd < 0x20)
    {
        rval = I2C_WRITE_WORD;
    }
    else
    {
        rval = I2C_WRITE_BLOCK;
    }
    return(rval);
}

void Hookc_MasterWrite(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    #if HID_OV_I2C_SUPPORTED
    if ((Channel == HID_CHANNEL) && (I2c_Var_Pntr->Addr == I2C_HID_ADDR))
    {
        Hidi2c_Slave_Rx(I2c_Var_Pntr->i2c_slave_rbuf, I2c_Var_Pntr->i2c_slave_rcount);
    }
    #endif //HID_OV_I2C_SUPPORTED
}

void Hookc_SlaveDone(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    #if HID_OV_I2C_SUPPORTED
    return;
    #endif // HID_OV_I2C_SUPPORTED
}

#if HID_OV_I2C_SUPPORTED
BYTE Hookc_ReceivePrtl(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    if ((Channel == HID_CHANNEL) && (I2c_Var_Pntr->Addr == I2C_HID_ADDR))
    {
        Hidi2c_Slave_Tx((const BYTE **)&I2c_Var_Pntr->i2c_slave_wbuf, (WORD *) &I2c_Var_Pntr->count);

        I2c_Var_Pntr->count++;
        I2c_Var_Pntr->i2c_slave_prtl = SMB_READ_BLOCK;

        return(1);
    }

    return(0);
}


void Hookc_Cfg_Slave_BufPntr(I2C_VAR *I2c_Var_Pntr, BYTE Channel)
{
    if ((Channel == HID_CHANNEL) && (I2c_Var_Pntr->Addr == I2C_HID_ADDR))
    {
        //Hidi2c_Slave_Cfg_BufPntr(I2c_Var_Pntr->i2c_slave_rbuf, I2c_Var_Pntr->i2c_slave_wbuf, Channel);
        Hidi2c_Slave_Cfg_BufPntr(I2c_Var_Pntr, Channel);
    }
}

#endif // HID_OV_I2C_SUPPORTED
#endif // I2C_SLAVE_SUPPORTED
/*********************** Hooks in IRQ.C ***********************/

/* ---------------------------------------------------------------
   Name: Hookc_Irq_Timer_A

   Called in Irq_Timer_A interrupt routine.
   --------------------------------------------------------------- */
void Hookc_Irq_Timer_A(void)
{
   ;
}

/************************ Hooks in SCAN.C ************************/

/* ---------------------------------------------------------------
   Name: Hookc_Get_Rc_Pntr

   Return: a NULL pointer to allow the CORE code to handle pointer.
           otherwise, return a pointer to the row/column table.
   --------------------------------------------------------------- */
/* <<< 2001/4/27 Modified for upper 64k. */
BYTE *Hookc_Get_Rc_Pntr(void)
{
   return((void *)0);
}
/* ---------------------------------------------------------------
   Name: Hookc_Get_Kbd_Type

   Return: 0 for a U.S. keyboard, 1 for a Japanese keyboard,
           anything else to allow CORE to determine keyboard type.
   --------------------------------------------------------------- */
BYTE Hookc_Get_Kbd_Type(void)
{
   return(0xFF);
}

/* ---------------------------------------------------------------
   Name: Hookc_Get_Custom_Table_Size

   Return: FFh to allow CORE to determine the size of the Custom
           Key Table,
           anything else is the size of the Custom Key Table.
   --------------------------------------------------------------- */
SMALL Hookc_Get_Custom_Table_Size(void)
{
   return(0xFF);
}

/* ---------------------------------------------------------------
   Name: Hookc_Get_Ext_Hotkey_Pntr

   Return: a NULL pointer to allow the CORE code to handle pointer.
           otherwise, return a pointer to the end of the table
           containing the external keyboard hotkey definitions.

   The table must look like:

      .
      .
      .

      WORD holding the hotkey function for hotkey #4.
      BYTE holding the hotkey scan code for hotkey #4.

      BYTE holding the hotkey scan code for hotkey #3.
      WORD holding the hotkey function for hotkey #3.

      WORD holding the hotkey function for hotkey #2.
      BYTE holding the hotkey scan code for hotkey #2.

      BYTE holding the hotkey scan code for hotkey #1.
      WORD holding the hotkey function for hotkey #1.

      BYTE with bits 0 - 3 holding the number of hotkeys.

   The first hotkey definition is at the end (the higher addresses)
   of the table.  The other definitions follow in the lower value
   addresses.  The order of the hotkey scan code and hotkey function
   alternate.

   The pointer must point to the byte holding the number of
   hotkeys.
   --------------------------------------------------------------- */
BYTE *Hookc_Get_Ext_Hotkey_Pntr(void)
{
   return((void *)0);
}


#if PORT80_SUPPORTED

/* ---------------------------------------------------------------
   Name: OEM_Get_Port80_Val

   Get Value from port 80
   --------------------------------------------------------------- */
void OEM_Get_Port80_Val(BYTE port80_val)
{

}

#endif // PORT80_SUPPORTED



#if HW_KB_SCN_SUPPORTED

void OEM_Chage_Default_HW_KBS_CFG()
{
    // KBS_CFG_INDX = KBS_DLY1;
    // KBS_CFG_DATA = KBS_DLY1_val;

    // KBS_CFG_INDX = KBS_DLY2;
    // KBS_CFG_DATA = KBS_DLY2_val;

    // KBS_CFG_INDX = KBS_RTYTO;
    // KBS_CFG_DATA = KBS_RTYTO_val;

     KBS_CFG_INDX = KBS_CNUM;
     KBS_CFG_DATA = MAX_SCAN_LINES;

    // KBS_CFG_INDX = KBS_CDIV;
    // KBS_CFG_DATA = KBS_CDIV_val;
}

#endif

FLAG Hookc_Timer_A_Check(void)
{
    return (TRUE);
}

