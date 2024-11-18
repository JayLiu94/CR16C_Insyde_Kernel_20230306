/* ----------------------------------------------------------------------------
 * MODULE PURDEV.C
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Contains auxiliary keyboard and auxiliary device (mouse)
 * serial send/receive and lock/unlock code.
 *
 * Revision History (Started April 24, 1995 by GLP)
 *
 * GLP
 * 23 May 98   Disabled interrupts before writing to Int_Var.xxx
 *             variable.
 * 17 Apr 98   Removed the Aux_Portx.inh flag.
 * 12 May 98   Removed Ssft_Byte.
 * 14 May 98   Updated because EN_AUX_PORT3 flag was moved.
 * 19 Aug 99   Updated copyright.
 * 07 Sep 99   Removed CMD_BC switch.
 *
 * EDH
 * 11 Dec 00   Support the fourth PS/2 port as port 4. It will be
 *             recognized as port 0 with Active PS/2 Multiplexing
 *             driver.
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * ------------------------------------------------------------------------- */

#define PURDEV_C

#include "swtchs.h"
#include "defs.h"
#include "purdev.h"
#include "purscn.h"
#include "purdat.h"
#include "purini.h"
#include "types.h"
#include "ps2.h"
#include "proc.h"
#include "timers.h"
#include "regs.h"

/* ---------------------------------------------------------------
   Name: Lock_Line

   Device locking is required to block multiple transmissions
   of devices through the port 60/64 PC interface. The inhibit
   timer is started in order to provide a delay so that the
   auxiliary device is given time to recognize it has been
   "locked" (clock line pulled low) before continuing.

   Input: port - Port number
                 Devices Supported:
                     AUX_PORT1_CODE
                     AUX_PORT2_CODE
                     AUX_PORT3_CODE
   --------------------------------------------------------------- */
void Lock_Line(SMALL port)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();

   switch (port)
   {
      case AUX_PORT1_CODE:
         Int_Lock_Aux_Port1();
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         Int_Lock_Aux_Port2();
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         Int_Lock_Aux_Port3();
         break;
      #endif
   }

   /* Start inhibit delay timer. */
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Lock_Aux_Devs

   Force auxiliary keyboard and auxiliary device (mouse) clock
   lines low so they cannot transmit data while the code is busy
   handling command/data sent from the Host.  Timer B is also
   started with Timer B event "INH_CLEAR" loaded.
   --------------------------------------------------------------- */
void Lock_Aux_Devs(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();

   Int_Lock_Aux_Port1();

   #if AUX_PORT2_SUPPORTED
   Int_Lock_Aux_Port2();
   #endif
   #if AUX_PORT3_SUPPORTED
   Int_Lock_Aux_Port3();
   #endif

   Disable_Gentmo(); /* Clear any pending tmo. */

   /* Start inhibit delay timer. */
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Aux_Cmd_Setup

   This routine is used before calling Send_To_Aux to setup
   sequence for primary device.

   The primary device is the device that responds to commands
   and sends the response to the Host.  A secondary device's
   responses are not sent to the Host.

   Input: aux_pntr - Address of device variable "block"
                     (Aux_Port4, Aux_Port1, Aux_Port2, or Aux_Port3)
          data - Byte that is to be sent.
   --------------------------------------------------------------- */
void Aux_Cmd_Setup(AUX_PORT *aux_pntr, BYTE data)
{
   if ((data == 0xFF) || (data == 0xF6))
   {
      /* Set sequence for "reset" or "set default" command. */
      aux_pntr->sequence = RESET1;
   }
   else if (aux_pntr->sequence != RESET1)
   {
      /* If the sequence has not previously been set
         to RESET1 and has not been handled yet,
         set sequence for other commands. */
      aux_pntr->sequence = CMD1;
   }
}

/* ---------------------------------------------------------------
   Name: Send_To_Aux

   Transmit data byte to auxiliary keyboard or auxiliary device
   (mouse).  This function starts the transmit and then waits
   for the transmission to be completed.  The function does not
   return until the transmission has been completed (or until an
   error occurs during transmission).

   Input: aux_pntr - Address of device variable "block"
                     (Aux_Port1, Aux_Port2, or Aux_Port3)
          port - Port number
                 AUX_PORT1_CODE, AUX_PORT2_CODE, AUX_PORT3_CODE
          data - Byte to send

   Returns: 1 if error, 0 if no error.
   --------------------------------------------------------------- */
FLAG Send_To_Aux(AUX_PORT *aux_pntr, SMALL port, BYTE data)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif
   #ifndef AUX_PORT1_MSGS
   #error AUX_PORT1_MSGS switch is not defined.
   #endif
   #ifndef AUX_PORT2_MSGS
   #error AUX_PORT2_MSGS switch is not defined.
   #endif
   #ifndef AUX_PORT3_MSGS
   #error AUX_PORT3_MSGS switch is not defined.
   #endif
   FLAG done;

   /* Initialize state. */
   aux_pntr->lock = 1;        /* Flag that aux is locked out. */
   Aux_Status.Send_Flag = 1;  /* Flag that data is being sent to aux. */
   aux_pntr->ack = 0;         /* Flag that acknowledge is not reguired. */

   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   if (port == AUX_PORT1_CODE)
   {
      Clr_Aux_Port1_Clk(); /* Inhibit transmission from device. */
      Clr_Aux_Port1_Dat(); /* Transmit request (takes effect later
                              when clock is allowed to go high. */
   }
   #else
    Clr_Aux_Port1_Clk(); /* Inhibit transmission from device. */
    Clr_Aux_Port1_Dat(); /* Transmit request (takes effect later
                            when clock is allowed to go high. */
   #endif

   #if AUX_PORT3_SUPPORTED
    else if (port == AUX_PORT3_CODE)
    {
       Clr_Aux_Port3_Clk(); /* Inhibit transmission from device. */
       Clr_Aux_Port3_Dat(); /* Transmit request (takes effect later
                               when clock is allowed to go high. */
    }
   #endif
/* 2006/10/14 Mark add AUX_PORT2_SUPPORTED Switch */
   #if AUX_PORT2_SUPPORTED
   else
   {
      Clr_Aux_Port2_Clk(); /* Inhibit transmission from device. */
      Clr_Aux_Port2_Dat(); /* Transmit request (takes effect later
                              when clock is allowed to go high. */
   }
   #endif
/* 2006/10/14 Mark add enden */
   /* Start inhibit delay timer. */
   Disable_Irq();
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   Enable_Irq();

   /* Wait for inhibit delay timer to expire.  This allows device
      to recognize that it has been inhibited from transmitting. */
   while(Timer_B.bit.INH_CLEAR) ;

   aux_pntr->lock = 0;  /* Clear lock state. */

   /* Mark the active port number and start
      timeout for device to start clocking data. */
   Disable_Irq();
   Int_Var.Active_Aux_Port = port;
   Enable_Irq();
   Enable_Gentmo(SEND_TMO);

   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   if(port == AUX_PORT1_CODE)
   {
      Start_Aux_Port1_Transmit(data);  /* Start data transmission. */
   }
   #else
   Start_Aux_Port1_Transmit(data);  /* Start data transmission. */
   #endif

   #if AUX_PORT3_SUPPORTED
   else if(port == AUX_PORT3_CODE)
   {
      Start_Aux_Port3_Transmit(data);  /* Start data transmission. */
   }
   #endif

/* 2006/10/14 Mark add AUX_PORT2_SUPPORTED Switch */
   #if AUX_PORT2_SUPPORTED
   else
   {
      Start_Aux_Port2_Transmit(data);  /* Start data transmission. */
   }
   #endif
/* 2006/10/14 Mark add enden */

   done = FALSE;
   do
   {
      /* Check timeout error.  (Bit is set by Timer A
         interrupt handler if general timeout timer expires.) */
      if(Service.bit.AUX_PORT_SND) done = TRUE;

      /* Check if send is complete. */
      if(Aux_Status.Send_Flag == 0) done = TRUE;
   }
   while(!done);

   return (Service.bit.AUX_PORT_SND);
}

/* ---------------------------------------------------------------
   Name: Check_Transmit

   Check to see if data transmission should be allowed.
   If the device is enabled and it is in a valid state for
   starting a transmission (not currently transmitting data),
   then it is OK to transmit.

   Input: port number
   Returns: TRUE if OK to transmit, otherwise returns FALSE.
   --------------------------------------------------------------- */
FLAG Check_Transmit(SMALL port)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   #ifndef AUX_PORT2_SUPPORTED
   #error AUX_PORT2_SUPPORTED switch is not defined.
   #endif

   FLAG bit;

   switch (port)
   {
      case AUX_PORT1_CODE:
         if(Aux_Port1.kbd == 1)
         {
            /* Keyboard compatible device is on Auxiliary Port 1. */
            bit = ((Ext_Cb0.bit.EN_AUX_PORT1 & Aux_Port1.valid) |
                   !Ext_Cb0.bit.AUTO_SWITCH);
         }
         else
         {
            /* Mouse compatible device is on Auxiliary Port 1. */
            bit = (Ext_Cb0.bit.EN_AUX_PORT1 & Aux_Port1.valid);
         }
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         if(Aux_Port2.kbd == 1)
         {
            /* Keyboard compatible device is on Auxiliary Port 2. */
            bit = ((Ext_Cb0.bit.EN_AUX_PORT2 & Aux_Port2.valid) |
                   !Ext_Cb0.bit.AUTO_SWITCH);
         }
         else
         {
            /* Mouse compatible device is on Auxiliary Port 2. */
            bit = (Ext_Cb0.bit.EN_AUX_PORT2 & Aux_Port2.valid);
         }
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         if(Aux_Port3.kbd == 1)
         {
            /* Keyboard compatible device is on Auxiliary Port 3. */
            bit = ((Ext_Cb0.bit.EN_AUX_PORT3 & Aux_Port3.valid) |
                   !Ext_Cb0.bit.AUTO_SWITCH);
         }
         else
         {
            /* Mouse compatible device is on Auxiliary Port 3. */
            bit = (Ext_Cb0.bit.EN_AUX_PORT3 & Aux_Port3.valid);
         }
         break;
      #endif
   }

   if(bit) return(TRUE);
   else return(FALSE);
}

/* ---------------------------------------------------------------
   Name: Enable_Gentmo

   Enables timer for auxiliary keyboard/device send/receive
   timeout error detect.  Uses Timer A (1 ms base time) and
   a software counter (Gen_Timer) to implement timer.

   Input: time - Time out length (x 1 mS)
          Valid Options:
            SEND_TMO
            ACK_TMO
            RECEIVE_TMO
            TRANSMIT_TMO
   --------------------------------------------------------------- */
void Enable_Gentmo(WORD time)
{
   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();
   #if TICKER_1MS
   Gen_Timer = time & 0xFF;
   Load_Timer_A();
   Timer_A.bit.TMR_GENTMO = 1;
   #else // TICKER_1MS
   Load_Timer_PS2(time);
   #endif // TICKER_1MS
   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Disable_Gentmo

   Disables timer for auxiliary keyboard/device send/receive
   timeout error detect.
   --------------------------------------------------------------- */
void Disable_Gentmo(void)
{
   FLAG interrupt_enable;

   interrupt_enable = Get_Irq_Enable_Flag();
   Disable_Irq();

   #if TICKER_1MS
   Timer_A.bit.TMR_GENTMO = 0;
   Gen_Timer = 0;       /* Clear timeout counter. */
   #else // TICKER_1MS
   Stop_Timer_PS2();
   #endif // TICKER_1MS

   if (interrupt_enable) Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: Inhibit_Delay

   IRQ's are enabled on exit.
   --------------------------------------------------------------- */
void Inhibit_Delay(void)
{
   /* Start inhibit delay timer. */
   Disable_Irq();
   Load_Timer_B();
   Timer_B.bit.INH_CLEAR = 1;
   Enable_Irq();

   /* Wait for inhibit delay timer to expire.  This allows device
      to recognize that it has been inhibited from transmitting. */
   while(Timer_B.bit.INH_CLEAR) ;
}

/* ---------------------------------------------------------------
   Name: Save_Mouse_State

   Intercept mouse commands and maintain the mouse state.  The state
   can later be retrieved using the BCh command.  See description of
   this command in PUREXT.C for definitions of state bytes.

   Input: data for mouse.
   --------------------------------------------------------------- */
#if MOUSE_EMULATION
const BYTE AUX_FF[] = {0xAA, 0x00};
const BYTE AUX_EB[] = {0x08, 0x00, 0x00};
extern BYTE Aux_Array[];
WORD Save_Mouse_State(BYTE data)
#else // MOUSE_EMULATION
void Save_Mouse_State(BYTE data)
#endif // MOUSE_EMULATION
{
   BYTE cmd_byte;

   #if MOUSE_EMULATION
   WORD rval;

   rval = (WORD) 0xFA;
   if (Emu_Aux.bit.bit0)
       rval = (WORD) data;
   #endif // MOUSE_EMULATION

   if (Mouse_Cmd == 0)
   {
      /* This is the first byte of a command.
         Put the command in cmd_byte.
         The next byte may be the data for the command. */
      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      /* Reset the mouse byte count if receive mouse command from Host. */
      Int_Mouse.field.Byte_Count = 0;
      if ((data != 0xF3) && (data != 0xF2))
         /* If this is not F3 command that want to send to mouse. */
         Int_Mouse.field.Cmd_Count = 0; /* Yes, clear the F3 command count. */
      #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED,
      cmd_byte = data;
   }
   else
   {
      /* This is the second byte of a 2-byte command.
         Mouse_Cmd holds command.  data holds data byte. */
      cmd_byte = Mouse_Cmd;
   }

   /* cmd_byte holds command. */

   switch (cmd_byte)
   {
      case 0xE6:  /* Reset scaling.  1-byte command. */
         /* Set scaling to 1:1. */
         Mouse_State[0] &= ~SCALING_2_1;  /* Clear bit. */
         break;

      case 0xE7:  /* Set scaling.  1-byte command. */
         /* Set scaling to 2:1. */
         Mouse_State[0] |= SCALING_2_1;   /* Set bit. */
         break;

      case 0xE8:  /* Set resolution.  2-byte command. */
         if (Mouse_Cmd == 0)
         {
            /* This is the first byte (command byte) of the 2-byte
               command.  Save the command (E8h) in Mouse_Cmd.  The
               next byte received from the Host will be the data byte. */
            Mouse_Cmd = cmd_byte;
         }
         else
         {
            /* This is the second byte (data byte) of the 2-byte command. */

            Mouse_State[1] = data;  /* Update resolution setting. */
            Mouse_Cmd = 0;          /* Clear command storage for last byte. */
         }
         break;
      #if MOUSE_EMULATION
      case 0xE9:
         Aux_Response = respCMD_AUX;
         Aux_Tmp_Pntr = Mouse_State;      /* Pointer to array. */
         Aux_Tmp_Load1 = 3;               /* Number of bytes in array. */
         break;
      case 0xEB:
         Aux_Response = respCMD_AUX;
         Aux_Tmp_Pntr = (BYTE *) &AUX_EB; /* Pointer to array. */
         Aux_Tmp_Load1 = 3;               /* Number of bytes in array. */
         break;
      #endif // MOUSE_EMULATION

      case 0xEA:  /* Set to stream mode.  1-byte command. */
         Mouse_State[0] &= ~REMOTE_MODE;  /* Clear bit. */
         break;

      case 0xF0:  /* Set to remote mode.  1-byte command. */
         Mouse_State[0] |= REMOTE_MODE;   /* Set bit. */
         break;
      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      case 0xF2:    /* F2 command that will be send to mouse. */
         if (Int_Mouse.field.INT_DRV)
         /* If three F3 command had be send to mouse? */
         Mouse_Cmd = cmd_byte;         // Save F2 command.
         #if MOUSE_EMULATION
         /* Added for PS/2 mouse emulation. */
         Aux_Response = respCMD_AUX;
         Aux_Tmp_Pntr = (BYTE *) &AUX_FF + 1; /* Pointer to array. */
         Aux_Tmp_Load1 = 1;           /* Number of bytes in array. */
          /* End of Added. */
         #endif // MOUSE_EMULATION

         break;
      #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      case 0xF3:  /* Set sampling rate.  2-byte command. */
         if (Mouse_Cmd == 0)
         {
            /* This is the first byte (command byte) of the 2-byte
               command.  Save the command (F3h) in Mouse_Cmd.  The
               next byte received from the Host will be the data byte. */
            Mouse_Cmd = cmd_byte;
         }
         else
         {
            /* This is the second byte (data byte) of the 2-byte command. */
            #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            switch (Int_Mouse.field.Cmd_Count)
            {
               case 0:
               case 3:
               case 4:
                  if (data == 0xC8)
                     Int_Mouse.field.Cmd_Count++;
                  break;
               case 1:
                  if (data == 0x64)
                     Int_Mouse.field.Cmd_Count++;
                  break;
               case 2:
               case 5:
                  if (data == 0x50)
                  { /* Yes, set Intelligent mouse driver had be installed. */
                     Int_Mouse.field.INT_DRV = 1;
                     if (Int_Mouse.field.Cmd_Count == 5)
                     {
                        Int_Mouse.field.INT_DRV5 = 1;
                        Int_Mouse.field.Cmd_Count = 0;
                     }
                     else
                        Int_Mouse.field.Cmd_Count++;
                  }
                  break;
            }
            #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            Mouse_State[2] = data;  /* Update sampling rate. */
            Mouse_Cmd = 0;          /* Clear command storage for last byte. */
         }
         break;

      case 0xF4:  /* Enable.  1-byte command. */
         Mouse_State[0] |= ENABLED;    /* Set bit. */
         break;

      case 0xF5:  /* Disable.  1-byte command. */
         Mouse_State[0] &= ~ENABLED;   /* Clear bit. */
         break;

      case 0xFF:  /* Reset.  1-byte command. */
         #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         Reset_Int_Mouse();
         #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         #if MOUSE_EMULATION
         /* Added for PS/2 mouse emulation. */
         Emu_Aux.bit.bit0 = 0;
         rval = (WORD) 0xFA;
         Aux_Response = respCMD_AUX;
         Aux_Tmp_Pntr = (BYTE *) &AUX_FF; /* Pointer to array. */
         Aux_Tmp_Load1 = 2;           /* Number of bytes in array. */
         Aux_Send_Timer = 400;
          /* End of Added. */
         #endif // MOUSE_EMULATION

      case 0xF6:  /* Set defaults.  1-byte command. */

         /* Set to defaults. */
         Mouse_State[0] = 0;     /* Scaling=1:1, disable, stream mode. */
         Mouse_State[1] = 2;     /* Resolution = 4 counts per mm. */
         Mouse_State[2] = 100;   /* Sampling rate = 100 Hz. */
         break;
      #if MOUSE_EMULATION
      case 0xFE:
         rval = (WORD) ~0;
      case 0xEE:
         Emu_Aux.bit.bit0 = 1;
         break;
      case 0xEC:
         Emu_Aux.bit.bit0 = 0;
         rval = (WORD) 0xFA;
         break;
      #endif // MOUSE_EMULATION

      default:
         /* Don't process these commands.

            Commands below E6 are invalid.
            Command E9 is Status request.
            Command EB is Read data.
            Command EC is Reset wrap mode.
            Command ED is invalid.
            Command EE is Set wrap mode.
            Command EF is invalid.
            Command F1 is invalid.
            Command F2 is Read device type.
            Commands F7 - FE are invalid. */
         ;
   }
   #if MOUSE_EMULATION
   return(rval);
   #endif // MOUSE_EMULATION
}


