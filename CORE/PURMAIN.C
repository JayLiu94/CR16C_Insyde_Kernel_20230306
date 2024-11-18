/* ----------------------------------------------------------------------------
 * MODULE PURMAIN.C
 *
 * Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Contains main processing loop (interrupt
 * driven) for the keyboard controller/scanner.
 *
 * Revision History (Started April 25, 1995 by GLP)
 *
 * GLP
 * 27 Nov 96   Added support for I2C bus (ACCESS.bus).
 * 13 Dec 96   Changed main_service so that each service
 *             routine has a chance to run.
 * 17 Jan 97   Removed "Gen_Info.bit.BREAK_AUXK = 0;" statements
 *             from translate_to_pc().  This is handled before
 *             calling the routine.  Added code to update
 *             Ext_Cb3.bit.SWAP_AUX or Ext_Cb3.bit.SWAP_AXK when
 *             a hotplug is detected.  Changed
 *                if (something) function();
 *             to
 *                if (something) {function();}
 *             If the function() is a #define with more than one
 *             statement, the if will fail unless the {} are used.
 * 06 May 97   Added check for ACPI_ENABLED flag to service_pci2.
 * 26 Jan 98   Added special handling for Command ACh in
 *             send_local_to_pc.
 * 02 Mar 98   Added Set_SCI_Cause, Generate_SCI, Set_SMI_SCI_Cause,
 *             Generate_SMI_SCI, Set_SMI_Cause, and Generate_SMI.
 *             Changed reference to Flag.ACPI_ENABLED to use the
 *             OEM_ACPI_Mode routine.
 * 09 Mar 98   Changed service_sbiabt to service_smbus.
 * 17 Mar 98   Added include files.
 * 20 Mar 98   Changed service_10mS routine into service_1mS
 *             routine.  Changed service_smbus into
 *             service_core_10.  SMBus service is handled in
 *             service_1mS.  Added SCI event handling in
 *             service_1mS.  Removed Generate_SCI.  Changed
 *             Generate_SMI_SCI to allow the SCI generation to be
 *             handled by the service_1mS routine.
 *             Removed the STATUS_PORT2_SMI handling.
 * 31 Mar 98   Added Gen_Hookc_Service_1mS.
 * 17 Apr 98   Hotplug did not always work.  For example, a mouse is
 *             on Port 1 and a keyboard is on Port 2.  The mouse is
 *             locked and the keyboard is unlocked.  If the keyboard
 *             is swapped to the mouse port, it will not be detected
 *             as a Hotplug because the port is locked.  This was
 *             corrected by removing the Aux_Portx.inh flag.  The
 *             auxiliary devices are not locked under Host control.
 *             When the Host locks the auxiliary keyboard or mouse
 *             devices, the data is simply ignored in the
 *             service_aux_port routine.
 * 24 Apr 98   Added support for the virtual Host interface.
 * 30 Apr 98   The output buffer was not being restored for the
 *             virtual Host interface.  Added Command A2h in
 *             service_pci2.
 * 06 May 98   The code was was written to simply wait for a timeout
 *             when updating devices to match the primary device.
 *             This has been changed to recognize a valid response
 *             from a device instead of waiting for a timeout.  For
 *             an invalid response, the old method of waiting for
 *             inactivity is used.
 *
 *             A hotplug of the keyboard was resulting in the Host
 *             receiving 7Fh as if from a mouse device.  To stop
 *             incorrect data from being sent to the Host,
 *             send_to_ext was changed to clear the
 *             Service.bit.AUX_PORT_SND flag when an error is
 *             detected on transmission.
 *
 *             Removed Flag.SCAN_LOAD, Flag.PPKT_LOAD, and
 *             Flag.PASS_LOAD.
 *
 *             Changed service_pci2 to allow return from
 *             processing without completing current command.
 * 08 May 98   Changed send_to_ext to return a busy indication
 *             if data is being transferred to/from a device.
 *             Updated service_aux_port and do_sequence to allow
 *             a switch to a moving mouse if all other devices
 *             are not active.  The mouse enable command is sent
 *             to the mouse to ensure that the next byte out of
 *             the mouse is the first byte of a packet.
 * 12 May 98   Changed service_ghost to service_core_7.  Removed
 *             Ssft_Byte.
 * 14 May 98   Updated for changes in Ext_Cb0 and Ext_Cb3 and
 *             Ext_Cb4 being changed to Ps2_Ctrl.
 * 01 Jun 98   Added Version Control Block (VCB) to allow VCB.EXE
 *             utility to display Keyboard Controller's version
 *             from a binary file.
 * 15 Jun 98   Changed the ACTIVE case in service_aux_port to
 *             stop data from being sent to the Host if a command
 *             has been sent to the primary device of the same type.
 *             If a secondary mouse was being moved (ACTIVE) and
 *             the Host sent a command to the primary mouse, the
 *             data from the secondary mouse was allowed to be
 *             sent to the Host.  The Host, however, expected the
 *             acknowledge byte from the command, not data packets.
 *             Changed Set_SCI_Cause to start the ACPI_Timer
 *             whether in ACPI mode or not.
 * 25 Jun 98   Added to the ACTIVE case of service_aux_port.  If
 *             a command has been sent to a primary mouse and
 *             there is an active secondary mouse, the "set default"
 *             command is sent to the secondary mouse to stop it
 *             from sending data.
 *             Updated send_auxd_to_pc to allow the timeout error
 *             byte to be sent to the Host (by setting the valid
 *             flag).
 *             Added call to Handle_SMBus_Alarm in service_1mS.
 *             Changed case AA_RCVD in do_sequence to use the
 *             port's inactivity timer (Aux_Portx.ticker) instead
 *             of the main inactivity timer (Aux_Port_Ticker).
 *             Changed handle_unlock so that if an ACK is expected
 *             from a device, only that device is unlocked.
 * 08 Jul 98   Added code to send_local_to_pc for diagnostic mode
 *             (DIAG_MODE).
 * 10 Jul 98   Changed routine positions in service_table to
 *             change priorities.  Changed main_service to handle
 *             a service request flag and then loop back to get
 *             the flags again.  The code does not follow through
 *             to check the next flag.  Changed send_local_to_pc
 *             to service_send (because all service_send did was
 *             call send_local_to_pc).  Added Disable_Irq() and
 *             Enable_Irq() around Service.bit.xxx.
 *             Added code to Load_Timer_B() and set
 *             Timer_B.bit.INH_CLEAR flag when auxiliary device
 *             is locked.  Changed handle_unlock to check for
 *             flag instead of loading timer and then checking
 *             for flag.  Changed service_pci to call
 *             handle_unlock instead of posting a request.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 12 Oct 98   Changed get_response to allow the response to the
 *             F2h keyboard command from Japanese keyboard
 *             (84h translated to 54h) to be different than from
 *             a U.S. keyboard (83h translated to 41h).
 * 10 Aug 99   Updated do_sequence to allow AUX_PORT3_SUPPORTED
 *             to be defined as OFF.  Updated copyright.
 * 24 Aug 99   Changed send_ext_to_pc and send_to_pc routines to
 *             allow external keyboard hotkeys.  Removed
 *             Hookc_Ext_To_Pc and Hookc_Send_To_Pc.
 * 02 Sep 99   Added handling for Timer_A.bit.HIF2_SAFETY in
 *             service_pci2().
 * 20 Jan 00   Support for Active PS/2 Multiplexing (APM) is
 *             marked as V21.1.
 *             Support to disable internal mouse if external mouse
 *             exists is marked as V21.3.
 *             INACTIVE_TIME changed from 13 to 5 (Marked as V21.4)
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
 * 11 Mar 06   solving mice will be turned into carzy when being slipped at the same time.
 * ------------------------------------------------------------------------- */

#define PURMAIN_C

#include "swtchs.h"
#include "defs.h"
#include "types.h"
#include "purdat.h"
#include "host_if.h"
#include "purscn.h"
#include "ps2.h"
#include "purhook.h"
#include "proc.h"
#include "scan.h"
#include "purcmd.h"
#include "purpm.h"
#include "purconst.h"
#include "purdev.h"
#include "purini.h"
#include "purmain.h"
#include "ini.h"
#include "oem.h"
#include "timers.h"
#include "puracpi.h"
#include "purfunct.h"
#include "purxlt.h"
#include "staticfg.h"
#include "dev.h"
#include "raminit.h"
#include "com_defs.h"
#include "port80.h"
#include "ecdthif.h"
#if DEEP_IDLE_S0
#include "i2c.h"
#include "oemdat.h"
#endif

#if RAM_BASED_FLASH_UPDATE
extern BYTE FLASH_UPDATE_HANDLE_ADDR;
extern void FLASH_UPDATE_handle(void);
#else
#include "fu.h"
#endif


#ifndef PURMAIN_MSGS
#error PURMAIN_MSGS switch is not defined.
#endif

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif


/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void main_service(void);
//static void service_core_10(void);
static void service_flash(void);
static void service_core_11(void);
static void service_core_12(void);
static void service_OEM_0(void);
static void service_OEM_1(void);
static void service_OEM_2(void);
static void service_scan(void);
static void service_pci(void);
static void service_unlock(void);
static void service_aux_port(void);
static void service_send(void);
static void service_1mS(void);
static void service_core_6(void);
static void service_core_7(void);
       void service_pci2(void);
       void service_pci3(void);
static void service_ext_irq(void);
static void check_aux(void);
static FLAG send_to_ext(AUX_PORT *aux_pntr, SMALL port, BYTE data);
static void do_sequence(AUX_PORT *aux_pntr, SMALL port);
static WORD get_response(void);
static WORD get_multibyte(HIF_VAR* hvp, BYTE hif);
static void send_auxk_to_pc(AUX_PORT *aux_pntr, SMALL port);
static void send_auxd_to_pc(AUX_PORT *aux_pntr, SMALL port);
static void init_ext_hotkeys(void);
static FLAG send_ext_to_pc(WORD data, FLAG break_prefix_flag);
static FLAG send_to_pc(WORD data, FLAG break_prefix_flag);
#if EXT_HOTKEY
static FLAG common_send_to_pc(WORD data, FLAG break_prefix_flag, SMALL num_ext_hotkeys);
#else //EXT_HOTKEY
static FLAG common_send_to_pc(WORD data, FLAG break_prefix_flag);
#endif //EXT_HOTKEY
static void handle_unlock(void);
static WORD translate_to_pc(WORD data, FLAG break_prefix_flag);
static void restore_HIF(SMALL level);

/* The IBM reference says that the keyboard Power-On Reset (POR)
   can take up to 9 seconds.  The Basic Assurance Test (BAT) is
   run after the POR and can take up to 900 milliseconds. */

/* 1 second to allow device to power up. */
#define POWER_UP_TIME 100

/* Allow 1 second to complete reset command (BAT). */
#define RESET_CMD_TIME 100

/* 1/4 second to complete other commands. */
#define OTHER_CMD_TIME 25

/* V21.4 2000/01/20 {. */
/* 130 milliseconds of auxiliary device inactivity. */
/* #define INACTIVE_TIME 13 */
/* 50 milliseconds of auxiliary device inactivity. */
#define INACTIVE_TIME 5
/* V21.4 2000/01/20 }. */

/* ---------------------------------------------------------------
   Main service loop.

   Wait in idle state until an IRQ causes an exit from idle. If
   the IRQ handler posted a service request (via bSERVICE) then
   dispatch control to the appropriate service handler.  Otherwise,
   go back to idle state.  After all service requests have been
   handled, return to idle state.
   --------------------------------------------------------------- */
const FUNCT_PTR_V_V service_table[16] =
{
   service_unlock,   /* Unlock device transmission. */
   service_pci,      /* Host command/data service. */
   service_aux_port, /* Send byte from auxiliary port to Host. */
   service_send,     /* Send byte from scanner (internal keyboard). */
   service_scan,     /* Scanner service. */
   service_1mS,      /* 1 millisecond elapsed. */
   service_pci3,     /* Third Host command/data service.. */
   service_core_7,   /* Reserved for CORE. */
   service_pci2,     /* Secondary Host command/data service. */
   service_ext_irq,  /* External IRQ. */
   service_flash,    /* Service Flash updated handler. */
   service_core_11,  /* Reserved for CORE. */
   service_core_12,  /* Reserved for CORE */
   service_OEM_0,    /* OEM Service 0. */
   service_OEM_1,    /* OEM Service 1. */
   service_OEM_2     /* OEM Service 2. */
};
void main(void)
{

   /* Interrupts are disabled. */
   Reset_Initialization();
   #if EXT_HOTKEY
   init_ext_hotkeys();
   #endif   // EXT_HOTKEY

   // if JBB after flash update init Ccb42 with default value
   if (FLASH_jbb == 0xAA55)
   {
        Ccb42.byte = 0x47;

#if !RAM_BASED_FLASH_UPDATE
        FLASH_jbb = 0; // if FU is ROM BASED - clear FLASH_jbb
#endif
   }

   while(1)
   {
      /* Interrupts must be enabled when idle or low power state is
         entered so the processor can be wakened by any interrupt request. */
      #if !DEEP_IDLE_S0
      if (Flag.ENTER_LOW_POWER)
      {  /*
         If the ENTER_LOW_POWER flag is set... */

         Chip_Low_Power_Enter();

         /* Let OEM setup for low power mode. */
         Gen_Hookc_Low_Power_Enter();

         Int_Var.Low_Power_Mode = 1;

         while(Flag.ENTER_LOW_POWER)
         {
            /* As long as the ENTER_LOW_POWER flag is
               set, keep entering the low power mode
               but do not process OEM hook again. */

            /* At this point, interrupts are disabled. */

            Low_Power();   /* Enable interrupts and go into
                              low power mode.  On exit from
                              low power mode, disable interrupts. */

            /* At this point, the low power mode has been
               exited after servicing an interrupt.

               The interrupts are disabled. */

            if (Int_Var.Low_Power_Mode)
            {
               /* If the wake up event was not valid, the
                  Low_Power_Mode flag will still be set. */
               ;
            }
            else
            {
               /* At this point, the wake up event was valid. */

               Flag.ENTER_LOW_POWER = 0;

               /* Lock all devices to allow handle_unlock to work. */
               Lock_Aux_Devs();        /* Lock auxiliary devices. */
               Lock_Scan();            /* Lock internal keyboard (scanner). */
               Service.bit.UNLOCK = 1; /* Post unlock service request. */

               Chip_Low_Power_Exit();
               Gen_Hookc_Low_Power_Exit();

               /* At this point, the interrupts are enabled.
                  Hookc_Low_Power_Exit enables the interrupts. */

               /* Wakeup_Task_H and Wakeup_Task_L
                  contain a Control Function Word.

                  The general format of the Control Function Word is:

                     000y,yyyy,zzzz,zzzz

                  where:

                     y,yyyy      = function to perform.
                     zzzz,zzzz   = data to support individual functions.

                     y,yyyy = 0 = Nop. */

               if ((Wakeup_Task_H & 0x1F) != 0)
               {
                  /* Event is present. */

                  /* Do the Port Pin function. */
                  System_Control_Function(((WORD) Wakeup_Task_H << 8)
                                           | Wakeup_Task_L, MAKE_EVENT);
               }

               /*
               The processor will not reenter low power mode because
               a valid wakeup event occurred.  It will, however, enter
               idle mode.  The previous Lock_xxx routines started
               Timer_B.  This will allow the processor to exit idle mode.
               */
            }
         }
      }
      else
      #endif
      {
         #if DEEP_IDLE_S0
         if ((T2CKC & 0x38) /* || Timer_A.bit.TMR_SCAN */ || (Aux_Port1.sequence != IDLE)
              || (I2c_Var[0].I2c_Irq_State > 1) || (I2c_Var[1].I2c_Irq_State > 1)
              || Input_Buffer2_Full() || Input_Buffer3_Full() || Input_Buffer_Full()
              || (PSTAT & PSTAT_SOT) || PECI_TRANS_BUSY() || DIDLE_Delay || Service.word
              )
         {
            Wait();
         }
         else
         {
            //Chip_Idle_Enter();
            Chip_Low_Power_Enter();

            //Idle();        /* Go into regular idle mode. */
            Low_Power();
            PMCSR = 0;
            SET_BIT(SMB1CTL1, SMBCTL1_INTEN | SMBCTL1_NMINTE);
            SET_BIT(SMB2CTL1, SMBCTL1_INTEN | SMBCTL1_NMINTE);
            SET_BIT(SMB3CTL1, SMBCTL1_INTEN | SMBCTL1_NMINTE);
            SET_BIT(SMB4CTL1, SMBCTL1_INTEN | SMBCTL1_NMINTE);
            //Chip_Idle_Exit();
            Chip_Low_Power_Exit();
            Enable_Irq();
         }

         #else
         Wait();
         #endif
         // End of Edward 2012/2/3 Modified.

         /* At this point, the idle mode has been
            exited after servicing an interrupt. */

         Gen_Hookc_Exit_Idle();  /* Hook for just after leaving the idle state. */

         main_service();
      }

      Disable_Irq();
   }
}

/* ---------------------------------------------------------------
   --------------------------------------------------------------- */
void ConsoleCommandProcess(void);
static void main_service(void)
{
   /* Check for new/more service requests.

      Check for a request flag.  The check is done according to
      priority.  If a request flag is set, handle the request and
      loop back to get the flags again.  Do not follow through to
      check the next flag.  The flags are to be checked in order. */

   WORD service_flags;
   SMALL index;

   service_flags = Service.word;
   while(service_flags != 0)
   {
      Gen_Hookc_Enter_Service();

      index = 0;
      if (Service.bit.CORE_7)
      {
        Service.bit.CORE_7 = 0;
        ConsoleCommandProcess();
      }
      while((service_flags & 1) == 0)
      {
         service_flags >>= 1;
         index++;
      }

      /*
      Since some service flags are set by interrupt routines,
      the interrupts will be disabled.  Clearing the service
      request bit may take more than one instruction.  If
      the clearing is interrupted, incorrect data could be
      put into the Service variable. */

      Disable_Irq();
      Service.word &= Cpl_Word_Mask(index);  /* Clear service request. */
      Enable_Irq();

      (service_table[index])();              /* Dispatch to service handler. */

      service_flags = Service.word;
   }
}

/* ---------------------------------------------------------------
   Name: service_core_6, service_core_7, service_core_10,
         service_core_11, service_core_12

   Reserved for CORE.
   --------------------------------------------------------------- */
static void service_core_6(void)
{
   /* The service flag has already been cleared. */
   ;
}

static void service_core_7(void)
{
   /* The service flag has already been cleared. */
   ;
}

static void service_flash(void)
{
    /* The service flag has already been cleared. */
#if ECDT_SHM_SUPPORTED
    if (ECDT_SHM_HIF_handle() == TRUE)
    {
        // this was an ECDT SHM command - no need to go to FLASH_UPDATE_handle()
        SET_BIT(SMC_CTRL, HSEM2_IE); // HSEM2_IE bit
        return;
    }
#endif

    // check for Flash Update
#if INCLUDE_CRISIS

#if RAM_BASED_FLASH_UPDATE
    // there is CRISIS - call FLASH_UPDATE_handle() in RAM of CRISIS
    void (*FLASH_UPDATE_handle_ptr)(void) = (void (*)(void))&FLASH_UPDATE_HANDLE_ADDR;
    (*FLASH_UPDATE_handle_ptr)();
#else // ROM BASED_FLASH_UPDATE
    // there is CRISIS - call FLASH_UPDATE_handle() in ROM
    FLASH_UPDATE_handle();
#endif // RAM/ROM_BASED_FLASH_UPDATE

#else // not INCLUDE_CRISIS
    CLEAR_BIT(DEVCNT, SPI_TRIS);
    CLEAR_BIT(PWDWN_CTL1, FIU_PD);

    // there is no CRISIS - call FLASH_UPDATE_handle() (ROM or RAM)
    FLASH_UPDATE_handle();

#endif // INCLUDE_CRISIS

    SET_BIT(SMC_CTRL, HSEM2_IE); // HSEM2_IE bit
}

static void service_core_11(void)
{
   /* The service flag has already been cleared. */
   ;
}

static void service_core_12(void)
{
   /* The service flag has already been cleared. */
   ;
}


/* ---------------------------------------------------------------
   Name: service_OEM_0, service_OEM_1, service_OEM_2

   OEM Service 0, OEM Service 1, and OEM Service 2.
   --------------------------------------------------------------- */
static void service_OEM_0(void)
{
   /* The service flag has already been cleared. */
   Gen_Hookc_Service_OEM_0();
}

static void service_OEM_1(void)
{
   /* The service flag has already been cleared. */
   Gen_Hookc_Service_OEM_1();
}

static void service_OEM_2(void)
{
   /* The service flag has already been cleared. */
   Gen_Hookc_Service_OEM_2();
}

/* ---------------------------------------------------------------
   Name: service_scan

   Handles service request from local keyboard key press.
   --------------------------------------------------------------- */
static void service_scan(void)
{
    /* The service flag has already been cleared. */
    if (Gen_Hookc_Service_Scan())
    {
        Handle_Scan();
    }
}

/* ---------------------------------------------------------------
   Name: service_pci

   Handle the command or data sent from the Host that is waiting
   in the primary Host interface input data register.  On entry
   the input data register interrupt is disabled, internal
   keyboard (scannner) is locked, and AUX devices are locked to
   prevent data transmissions during the processing of the input
   buffer command/data (this was all done by the input data buffer
   full IRQ handler).
   --------------------------------------------------------------- */
static void service_pci(void)
{
   WORD data;

   /* The service flag has already been cleared. */

   data = Get_Host_Byte();

   if (data > 255)
   {
      /* The Host byte is a command.  Clear the command/data flag. */
      data &= 0xFF;

      /* In multiplex mode, prefix command 90 is write to port0,
                                           91 is write to port1,
                                           92 is write to port2,
                                           93 is write to port3.   */
      if (((data & 0xFC) == 0x90) && MULPX.bit.Multiplex)
      { /* Command is 90/91/92/93 and in Multiple mode. */
         MULPX.bit.PREFIX_CMD = 1;  /* Assume 90~3h command in multiplex. */
         MULPX.byte &= ~(0x03);
         MULPX.byte |= (data & 0x03);  /* Save AUX port (0~3) */
      }
      else
      {
         if (MULPX.bit.PREFIX_CMD && (data != 0xA7 && data != 0xA8))
         {
            MULPX.bit.PREFIX_CMD = 0;
         }
         Srvc_Pccmd((BYTE) data);   /* Handle the command. */
      }

   }
   else
   {
  /* The Host byte is data.  The input buffer had data from
         a Host write to port 60h, do something with the data. */

      if (MULPX.bit.PREFIX_CMD)
      {
         Hif_Var[HIF_KBC].Cmd_Byte = 0xD4;           /* Data is the AUX command that want to send to AUX. */
      }

      Srvc_Pcdat(data);
   }

   if (Kbd_Response.bit.CMD_RESPONSE)
   {
      /* If multibyte return of data to the Host is required,
         then ensure that the SEND bit is set. */
      Disable_Irq();
      Service.bit.SEND = 1;      /* Post service request. */
      Enable_Irq();
   }

   if (Service.bit.AUX_PORT_SND || Service.bit.SEND)
   {
      /* Data is to be sent to the Host.  Skip unlock. */
      ;
   }
   else
   {
      if (Hif_Var[HIF_KBC].Cmd_Byte == 0)
      {
         /* No more data is required from Host to
            complete Host command processing.  Do unlock. */

         handle_unlock();
      }
      else
      {
         /* More data is required from Host to
            complete Host command processing. */

         /* Skip unlock and re-enable PC interface
            interrupt (input buffer full interrupt)
            and wait for data byte. */

         Enable_Host_IBF_Irq();
      }
   }
}

/* ---------------------------------------------------------------
   Name: service_pci2

   Handle the command or data sent from the Host that is
   waiting in the secondary Host interface input data register.
   On entry the input data register interrupt is disabled.
   --------------------------------------------------------------- */
void service_pci2(void)
{
   WORD data;
   BYTE status;

   /* The service flag has already been cleared. */
   /* Handle the command/data from third Host interface. */

   do
   {
      Disable_Irq();
      Service.bit.PCI2 = 0;   /* Clear the service flag. */
      Enable_Irq();

      /* Get the Host interface status before reading the byte
         from the Host.  The OBF flag may be needed.  Once the
         byte is read, the Host may clear any data in the output
         buffer and this will change the status of the OBF flag. */
      status = Read_Host_Status_Reg2();

      /* Now get the byte. */
      data = Get_Host_Byte2();

      if (data > 255)
      {
         /* The Host byte is a command.  Clear the command/data flag. */
         data &= 0xFF;

         /* Commands A0h (push HIF2 level) and A1h (pop
            HIF2 level) are intercepted here.  All other
            commands are sent to the command interpreter. */


            HIF_Response[HIF_PM1].bit.CMD_RESPONSE = 0;

            Srvc_Pm1cmd((BYTE) data);  /* Handle the command. */
      }
      else
      {
         /* The Host byte is data.  The input buffer
            had data from a Host write to the secondary
            data port, do something with the data. */
         Srvc_Pm1dat(data);
      }

      if (HIF_Response[HIF_PM1].bit.CMD_RESPONSE)
      {
         /* This is a multibyte command response. */

         /* Re-enable secondary PC interface interrupt. */
         Enable_Host_IBF2_Irq();

         if (!Output_Buffer2_Full())
         {
            data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_PM1], HIF_PM1);
            Data_To_Host2(data);
         }

         /* Timer_A.bit.HIF2_SAFETY is used to check the transfer.
            The transfer is aborted if the time between bytes is
            too long.

            Although, Timer_A.bit.HIF2_SAFETY is used, it is actually
            larger than one bit. */

         Disable_Irq();
         Timer_A.bit.HIF2_SAFETY = HIF2_SAFETY_TIME;
         Enable_Irq();

         /* If multibyte return is interrupted
            by a new command, abort sending bytes.

            It may be a command from the same level.
            The output of the bytes will be stopped.

            It may be a command from another level
            (Command A0h).  This would require saving
            the status of this level. */
         while ( (HIF_Response[HIF_PM1].bit.CMD_RESPONSE) &&
                 (Service.bit.PCI2 == 0) )
         {
            if (!Output_Buffer2_Full())
            {
               /* Send new data to Host when it has taken last byte. */
               data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_PM1], HIF_PM1);
               Data_To_Host2(data);

               Disable_Irq();
               Timer_A.bit.HIF2_SAFETY = HIF2_SAFETY_TIME;
               Enable_Irq();
            }

            /* Abort the transfer if the host does not get the
               next byte within HIF2_SAFETY_TIME milliseconds. */
            if (Timer_A.bit.HIF2_SAFETY == 0)
               HIF_Response[HIF_PM1].bit.CMD_RESPONSE = 0;
         }
      }
   }
   while (Service.bit.PCI2);/* If multibyte return was interrupted by
                               a new command, process new command.  OR
                               if command requires more data, get it. */

   /* Re-enable secondary PC interface interrupt. */
   Enable_Host_IBF2_Irq();
}

/* ---------------------------------------------------------------
   Name: service_pci3

   Handle the command or data sent from the Host that is
   waiting in the third Host interface input data register.
   On entry the input data register interrupt is disabled.
   --------------------------------------------------------------- */
void service_pci3(void)
{
   WORD data;
   BYTE status;
   /* The service flag has already been cleared. */


   /* Handle the command/data from third Host interface. */

   do
   {
      Disable_Irq();
      Service.bit.PCI3 = 0;   /* Clear the service flag. */
      Enable_Irq();

      /* Get the Host interface status before reading the byte
         from the Host.  The OBF flag may be needed.  Once the
         byte is read, the Host may clear any data in the output
         buffer and this will change the status of the OBF flag. */
      status = Read_Host_Status_Reg3();

      /* Now get the byte. */
      data = Get_Host_Byte3();

      if (data > 255)
      {
         /* The Host byte is a command.  Clear the command/data flag. */

         HIF_Response[HIF_PM2].bit.CMD_RESPONSE = 0;

         Srvc_Pm2cmd((BYTE) data);  /* Handle the command. */
      }
      else
      {
         /* The Host byte is data.  The input buffer
            had data from a Host write to the third
            data port, do something with the data. */
         Srvc_Pm2dat(data);
      }

      if (HIF_Response[HIF_PM2].bit.CMD_RESPONSE)
      {
         /* This is a multibyte command response. */

         /* Re-enable third PC interface interrupt. */
         Enable_Host_IBF3_Irq();

         if (!Output_Buffer3_Full())
         {
            data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_PM2], HIF_PM2);
            Data_To_Host3(data);
         }

         /* Timer_A3.bit.HIF3_SAFETY is used to check the transfer.
            The transfer is aborted if the time between bytes is
            too long.

            Although, Timer_A.bit.HIF3_SAFETY is used, it is actually
            larger than one bit. */

         Disable_Irq();
         Timer_A3.bit.HIF3_SAFETY = HIF3_SAFETY_TIME;
         Enable_Irq();

         /* If multibyte return is interrupted
            by a new command, abort sending bytes.

            It may be a command from the same level.
            The output of the bytes will be stopped. */

         while ( (HIF_Response[HIF_PM2].bit.CMD_RESPONSE) &&
                 (Service.bit.PCI3 == 0) )
         {
            if (!Output_Buffer3_Full())
            {
               /* Send new data to Host when it has taken last byte. */
               data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_PM2], HIF_PM2);
               Data_To_Host3(data);

               Disable_Irq();
               Timer_A3.bit.HIF3_SAFETY = HIF3_SAFETY_TIME;
               Enable_Irq();
            }

            /* Abort the transfer if the host does not get the
               next byte within HIF2_SAFETY_TIME milliseconds. */
            if (Timer_A3.bit.HIF3_SAFETY == 0)
               HIF_Response[HIF_PM2].bit.CMD_RESPONSE = 0;
         }
      }
   }
   while (Service.bit.PCI3);/* If multibyte return was interrupted by
                               a new command, process new command.  OR
                               if command requires more data, get it. */

   /* Re-enable third PC interface interrupt. */
   Enable_Host_IBF3_Irq();
}

/* ---------------------------------------------------------------
   Name: service_unlock

   Unlock aux devices and re-enable Host interface IRQ if it
   is ok to do so.
   --------------------------------------------------------------- */
static void service_unlock(void)
{
   /* The service flag has already been cleared. */
   handle_unlock();
}

/* ---------------------------------------------------------------
   Name: service_aux_port

   Send data received from auxiliary port to the Host or handle
   an error while attempting to send to device.  The data can
   come from either a mouse compatible device or from a keyboard
   compatible device.

   If the device is a keyboard compatible device, then the
   data is sent using the standard keyboard obf and IRQ1.

   If the device is a mouse compatible device, then the
   data is sent using the auxiliary obf flag and IRQ12.
   --------------------------------------------------------------- */
static void service_aux_port(void)
{
   #ifndef AUX_PORT1_MSGS
   #error AUX_PORT1_MSGS switch is not defined.
   #endif
   #ifndef AUX_PORT2_MSGS
   #error AUX_PORT2_MSGS switch is not defined.
   #endif
   #ifndef AUX_PORT3_MSGS
   #error AUX_PORT3_MSGS switch is not defined.
   #endif

   /* The service flag has already been cleared. */

   FLAG error = FALSE;
   FLAG skip_it = FALSE;
   AUX_PORT *aux_pntr;

   aux_pntr = Make_Aux_Pntr(Int_Var.Active_Aux_Port);

   Aux_Port_Ticker = 0;    /* Reset the main inactivity timer. */
   aux_pntr->ticker = 0;   /* Reset this port's inactivity timer. */

   if ( (Gen_Timer != 0) || (!Parity_Is_Ok()) )
   {
      aux_pntr->valid = 0;
      error = TRUE;
   }

   switch (aux_pntr->sequence)
   {
      case TYPE_S3:
         /*
         Received ACK while attempting to find
         the type of the device.  Expecting AA. */
      case IDLE:
         /*
         There has been no activity from this device for a while.
         Therefore, this is the first byte of a packet from a
         mouse or a byte of keyboard data or the device has just
         been plugged in and is sending the result of the power
         up test.  This cannot be an ACK byte from a command
         sent to the device because the sequence would not be IDLE. */

         #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         if (error)
         {
            /* Timeout error or parity error.

               Handle this as a device that was just plugged in.
               The first data from the device was an error.

               The next valid byte may be the AA signal. */

            aux_pntr->sequence = TYPE_S1;
            skip_it = TRUE;
         }
         else if (Aux_Data.byte == 0xAA)
         {
            if(!Aux_Port1.valid || (Int_Var.Active_Aux_Port != AUX_PORT1_CODE))
            {  /*
                This will be taken as a signal that a
                device was just plugged in and went through
                its power up test.  Ignore this byte. */

                aux_pntr->sequence = AA_RCVD;
                aux_pntr->valid = 1;
                skip_it = TRUE;
            }
         }
         else if (aux_pntr->kbd == 0)
         {
            /* For a mouse device.
               If another mouse is active, ignore the data
               from this mouse until all activity ceases.
               Otherwise, set this mouse as active. */
            if (MULPX.bit.Multiplex)
            {  /* If KBC in Multiplexing mode then don't care which is in Active. */
               aux_pntr->sequence = ACTIVE;
            }
            else
            {
               if ( (Int_Var.Active_Aux_Port != AUX_PORT1_CODE) &&
                       (aux_pntr->kbd == Aux_Port1.kbd) &&
                       (Aux_Port1.sequence == ACTIVE)
                /* solve mice carzy when mouse slide at the same time*/
                    || (Aux_Port1.sequence == RESET1))

               {
                  aux_pntr->sequence = IGNORE;
                  skip_it = TRUE;
               }

               #if AUX_PORT2_SUPPORTED
               else if ( (Int_Var.Active_Aux_Port != AUX_PORT2_CODE) &&
                         (aux_pntr->kbd == Aux_Port2.kbd) &&
                         (Aux_Port2.sequence == ACTIVE)
                   /* solve mice carzy when mouse slide at the same time*/
                      || (Aux_Port2.sequence == RESET1))
               {
                  aux_pntr->sequence = IGNORE;
                  skip_it = TRUE;
               }
               #endif

               #if AUX_PORT3_SUPPORTED
               else if ( (Int_Var.Active_Aux_Port != AUX_PORT3_CODE) &&
                         (aux_pntr->kbd == Aux_Port3.kbd) &&
                         (Aux_Port3.sequence == ACTIVE)
                      /* solve mice carzy when mouse slide at the same time*/
                         || (Aux_Port3.sequence == RESET1) )
               {
               aux_pntr->sequence = IGNORE;
               skip_it = TRUE;
               }
               #endif
               else
               {
                  /* This is the active mouse device.
                     A mouse can be on Auxiliary Port 1, 2, or 3. */

                  if (Ps2_Ctrl.bit.M_AUTO_SWITCH)
                  {  /*
                     If primary mouse autoswitching is
                     enabled, make this the primary mouse. */

                     Ps2_Ctrl.byte &= ~(maskPRI_MOUSE << shiftPRI_MOUSE);
                     Ps2_Ctrl.byte |= (Int_Var.Active_Aux_Port << shiftPRI_MOUSE);
                  }

                  aux_pntr->sequence = ACTIVE;
               }
            }
         }
         else
         {
            /* For a keyboard device.
               A keyboard can be on Auxiliary Port 1 or 2. */

            if (Ps2_Ctrl.bit.K_AUTO_SWITCH)
            {  /*
               If primary keyboard autoswitching is
               enabled, make this the primary keyboard. */

               Ps2_Ctrl.byte &= ~(maskPRI_KBD << shiftPRI_KBD);
               Ps2_Ctrl.byte |= (Int_Var.Active_Aux_Port << shiftPRI_KBD);
            }

            aux_pntr->sequence = ACTIVE;
         }
         #else // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         aux_pntr->sequence = ACTIVE;
         #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         break;

      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      case ACTIVE:
         /*
         This has been an active device.  Data bytes are being
         received from the device.  This may be packet data from
         a mouse or a byte of keyboard data.  This cannot be data
         from a command sent to the device because the sequence
         would not be ACTIVE.

         If a command has been sent to this type of primary device,
         ignore the data from this device until all activity ceases.

         Otherwise, allow data to be sent to the Host. */
         if (!MULPX.bit.Multiplex)
         {
            if ( (Int_Var.Active_Aux_Port != AUX_PORT1_CODE) &&
                 (aux_pntr->kbd == Aux_Port1.kbd) &&
                 ( (Aux_Port1.sequence == RESET1) ||
                   (Aux_Port1.sequence == CMD1) ) )
            {
               aux_pntr->sequence = IGNORE;
               skip_it = TRUE;
            }

            #if AUX_PORT2_SUPPORTED
            else if ( (Int_Var.Active_Aux_Port != AUX_PORT2_CODE) &&
                      (aux_pntr->kbd == Aux_Port2.kbd) &&
                      ( (Aux_Port2.sequence == RESET1) ||
                        (Aux_Port2.sequence == CMD1) ) )
            {
               aux_pntr->sequence = IGNORE;
               skip_it = TRUE;
            }
            #endif

            #if AUX_PORT3_SUPPORTED
            else if ( (Int_Var.Active_Aux_Port != AUX_PORT3_CODE) &&
                      (aux_pntr->kbd == Aux_Port3.kbd) &&
                      ( (Aux_Port3.sequence == RESET1) ||
                        (Aux_Port3.sequence == CMD1) ) )
            {
               aux_pntr->sequence = IGNORE;
               skip_it = TRUE;
            }
            #endif
            if ( (aux_pntr->kbd == 0) && (skip_it) )
            {
               /* This is a secondary mouse and a command has been sent
                  to the primary mouse.  Stop this mouse from sending data
                  by sending the "set default" command to the mouse. */

               /* Send_To_Aux won't return until send complete or error sending. */
               if (Send_To_Aux(aux_pntr, Int_Var.Active_Aux_Port, 0xF6))
               {
                  /* Error, assume no external device. */

                  Disable_Irq();
                  Service.bit.AUX_PORT_SND = 0;
                  Enable_Irq();

                  aux_pntr->valid = 0;
                  aux_pntr->setflag1 = 0;
                  aux_pntr->setflag2 = 0;

                  Gen_Timer = 0;          /* Clear timeout counter
                                             to remove error indication. */
               }
               else
               {
                  /* Initialize this mouse's status bytes. */
                  aux_pntr->state_byte0 = 0;    /* scaling 1:1, disabled,
                                                stream mode. */
                  aux_pntr->state_byte1 = 2;    /* Resolution Setting. */
                  aux_pntr->state_byte2 = 0x64; /* Sampling Rate. */
               }

               aux_pntr->ack = 0;      /* Do not wait for acknowledge. */
            }
         }
          break;

      case TYPE_S1:
         /*
         There was no activity from the devices for a while.
         The first byte of a packet from a mouse or a byte of
         keyboard data resulted in an error.  This is being
         handled as a device that was just plugged in.

         The next valid byte may be the AA signal. */

         skip_it = TRUE;

         if (error)
         {
            /* Timeout error or parity error. */
            ;
         }
         else if (Aux_Data.byte == 0xAA)
         {  /*
            This will be taken as a signal that a device was
            just plugged in and went through its power up test. */

            aux_pntr->valid = 1;
            aux_pntr->sequence = AA_RCVD;
         }
         break;

      case TYPE_S2:
         /*
         In an attempt to find the type of the device on this port,
         a reset command has been sent.  If this byte is the ACK
         from the reset command or an AA, change the sequence.
         Otherwise, ignore any data from the device for a while. */

         skip_it = TRUE;

         if (error)
         {
            /* Timeout error or parity error. */

            aux_pntr->sequence = IGNORE;
         }
         else if (Aux_Data.byte == 0xFA)
         {
            /* Received ACK, expecting AA. */
            aux_pntr->sequence = TYPE_S3;
         }
         else if (Aux_Data.byte == 0xAA)
         {
            /* Received AA. */
            aux_pntr->valid = 1;
            aux_pntr->sequence = AA_RCVD;
         }
         else
         {
            aux_pntr->sequence = IGNORE;
         }
         break;

      case AA_RCVD:
         /*
         An AA was received before and not allowed to
         be sent to the Host.

         If this byte is 00, then the AA 00 string after the
         mouse's power up test or after the reset command (while
         attempting to determine the device type) has been received.

         If this byte is not 00, then a keyboard device sent the
         AA followed by something else.  This is probably a hotplug
         of a keyboard followed by keystrokes.

         In any case, the data will be ignored.  The device may
         be a secondary device or it may be a mouse device.  The
         AA was never sent to the Host and this byte may be
         the second byte of mouse packet data.  The mouse may
         get out of sync if this is allowed to be sent to the Host.
         The data will not be allowed to be sent to the Host until
         all data has stopped for a while. */

         skip_it = TRUE;

         if (!error)
         {  /*
            No timeout error and no parity error. */

            if (Aux_Data.byte == 00)
            {  /*
               Data received is 00, mark this as a
               hotplug of a mouse device on this port. */

               aux_pntr->kbd = 0;
               aux_pntr->sequence = HOTPLGM;

               #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
               aux_pntr->intm = 0;
               aux_pntr->intseq = 0;
               if (!Int_Mouse.field.Add4byte)
               {
                  Ps2_Ctrl.byte &= ~(maskPRI_MOUSE << shiftPRI_MOUSE);
                  Ps2_Ctrl.byte |= (Int_Var.Active_Aux_Port << shiftPRI_MOUSE);
               }
               #else
               Ps2_Ctrl.byte &= ~(maskPRI_MOUSE << shiftPRI_MOUSE);
               Ps2_Ctrl.byte |= (Int_Var.Active_Aux_Port << shiftPRI_MOUSE);
               #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

               if (MULPX.bit.Multiplex)
               { /* Mouse APM mode enable. */
                  MULPX.bit.Mx_hotplug = 1;  /* Set Hotplug sequence. */
                  MULPX.byte &= ~(0x03);                    /* Clear Port number. */
                  MULPX.byte |= Int_Var.Active_Aux_Port; /* Set Port number. */
                  Aux_Port_Ticker = 0;       /* Reset the main inactivity timer. */
                  aux_pntr->ticker = 0;      /* Reset this port's inactivity timer. */
                  if (!Send_To_Aux(aux_pntr, Int_Var.Active_Aux_Port, 0xFF))
                  {  /* Send reset command to mouse in order to get AA, 00. */
                     /* At this point, no error occurred. */
                     // aux_pntr->ack = 1;/* Flag to wait for acknowledge. */
                     aux_pntr->sequence = RESET1;  /* Set Port sequence. */
                  }
               }

            }
            else
            {  /*
               Mark this as a hotplug of a keyboard device on this port. */

               aux_pntr->kbd = 1;
               aux_pntr->sequence = HOTPLGK;
            }
         }
         else
         {
            aux_pntr->sequence = IGNORE;
         }
         break;

      case RESET1:
         /*
         This is a primary device.  Secondary devices cannot have
         this sequence code.  A primary device is the device that
         is to respond to commands.  The secondary device(s) of
         the same type are to be updated to match.

         A reset command was previously sent to this device. */

         if (MULPX.bit.Mx_hotplug && (Aux_Data.byte == 0xFA))
         {  /* Hotplug sequence is setted. */
            skip_it = TRUE;   /* Impede FA be send to Host. */
            MULPX.bit.Mx_hotplug = 0;  /* Clear Hotplug sequence */
         }

      case CMD1:
         /*
         This is a primary device.  Secondary devices cannot have
         this sequence code.  A primary device is the device that
         is to respond to commands.  The secondary device(s) of
         the same type are to be updated to match.

         A command other than a reset was sent to this device. */

         /* We will notify Intelligent mouse driver that mouse is wheel mouse. */
         #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         if (Int_Mouse.field.INT_DRV && !aux_pntr->kbd && (Mouse_Cmd == 0xF2))
         { /* If this is mouse and Intelligent mouse driver had be installed
               and this is F2 command. */
            if (Aux_Data.byte != 0xFA)
            {
               Mouse_Cmd = 0;
               aux_pntr->intseq = 1;
               //if (Aux_Data.byte != 0x03) aux_pntr->intm = 0;
               if ((Aux_Data.byte != 0x03) && (Aux_Data.byte != 0x04)) aux_pntr->intm = 0;
               else
               {
                  if (Aux_Data.byte == 0x04)
                  {
                     aux_pntr->intm5 = 1;
                  }
                  aux_pntr->intm = 1;
                  Int_Mouse.field.Add4byte = 1;
               }
            }
         }
         #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         break;

      case CMDK:
         /*
         This is a secondary keyboard that is being updated to match
         the primary keyboard.  A command was sent to the keyboard
         on an Auxiliary Port and a response was received.  The
         command may have been the one of the commands issued
         at case CMDK of do_sequence.  These result in an
         FAh byte (ACK) being sent from the keyboard.  If this byte
         is an FAh, update the Aux_Port_Ticker so that the next
         command can be sent. */

         if (!error)
         {  /*
            No timeout error and no parity error. */

            if (Aux_Data.byte == 0xFA) Aux_Port_Ticker = OTHER_CMD_TIME;
         }

         skip_it = TRUE;
         break;

      case CMDM:
         /*
         This is a secondary mouse that is being updated to match
         the primary mouse.  A command was sent to the mouse on
         an Auxiliary Port and a response was received.  The
         command may have been the "set default" command issued at
         case RESETM of do_sequence or one of the other commands
         issued at case CMDM of do_sequence.  These result in an
         FAh byte (ACK) being sent from the mouse.  If this byte
         is an FAh, update the Aux_Port_Ticker so that the next
         command can be sent. */

         if (!error)
         {  /*
            No timeout error and no parity error. */
            #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            if (Int_Mouse.field.INT_DRV && (Mouse_Cmd == 0xF2))
            {
               if (Aux_Data.byte != 0xFA)
               {
                  Mouse_Cmd = 0;
                  aux_pntr->intseq = 1;
                  //if (Aux_Data.byte != 0x03)  aux_pntr->intm = 0;
                  if ((Aux_Data.byte != 0x03) && (Aux_Data.byte != 0x04)) aux_pntr->intm = 0;
                  else
                  {
                     if (Aux_Data.byte == 0x04)
                     {
                        if (!Aux_Port1.intm5
                        #if AUX_PORT2_SUPPORTED
                        && !Aux_Port2.intm5
                        #endif
                        #if AUX_PORT3_SUPPORTED
                        && !Aux_Port3.intm5
                        #endif
                        )
                           Int_Mouse.field.Add4byte = 0;
                        aux_pntr->intm5 = 1;
                     }
                     aux_pntr->intm = 1;
                     if (!Int_Mouse.field.Add4byte)
                     {
                        aux_pntr->state_byte0 = 0; /* scaling 1:1, disabled, stream mode. */
                        aux_pntr->state_byte1 = 2;       /* Resolution Setting. */
                        aux_pntr->state_byte2 = 0x64;    /* Sampling Rate. */
                        aux_pntr->intseq = 0;
                        /* Update the sequence. */
                        MULPX.bit.Mx_hotplug  = 1;   /* Set Hotplug sequence. */
                        Aux_Port_Ticker  = 0;    /* Reset the main inactivity timer. */
                        aux_pntr->ticker = 0;    /* Reset this port's inactivity timer. */
                        if (! Send_To_Aux(aux_pntr, Int_Var.Active_Aux_Port, 0xFF))
                        { /* Send reset command to mouse in order to get AA, 00.
                            At this point, no error occurred. */
                           aux_pntr->sequence = RESET1;   /* Set Port sequence. */
                        }
                        Ps2_Ctrl.byte &= ~(maskPRI_MOUSE << shiftPRI_MOUSE);
                        Ps2_Ctrl.byte |= (Int_Var.Active_Aux_Port << shiftPRI_MOUSE);
                     }
                     Int_Mouse.field.Add4byte = 1;
                  }
                  Aux_Port_Ticker = OTHER_CMD_TIME;
               }
            }
            else
            #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            {
               if (Aux_Data.byte == 0xFA) Aux_Port_Ticker = (OTHER_CMD_TIME - 5);
            }
            Int_Var.Ticker_10 = 1;
         }
         else
         {
            Mouse_Cmd = 0;
            #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            aux_pntr->intseq = 1;
            Int_Mouse.field.Seq = 0;
            #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         }

         skip_it = TRUE;
         break;
         /* <<< V5.1 2001/2/8 Moved the Hotplug mouse sequence to here. */
      case HOTPLGM:
         /* --- HOTPLGM ---
         An AA 00 string was received.  This is taken to
         be a hot plug of a mouse.  The new mouse is to
         be brought up to date.
         /* >>> V5.1 2001/2/8 Moved */
         /* --- RESETM ---
         These sequence numbers and up are used to reset and update a
         device.  All data from this device is being ignored until
         it has timed out. */
      case RESETM:
         if (Aux_Data.byte == 0)
         {
            Aux_Port_Ticker = OTHER_CMD_TIME;
            aux_pntr->sequence = CMDM;
         }
         skip_it = TRUE;
         break;
      default:
         /*
         All other sequence numbers will ignore data from the device.

         --- IGNORE ---
         All data from this device is being ignored until it has
         timed out.  It may be that an AA was received followed
         by an error.  Or some data is being ignored.  This is
         done so that the Host does not get out of sync with a
         mouse.  If the second byte of a packet is allowed to
         be sent to the Host after the first byte has been
         ignored, the mouse will be out of sync.  After a period
         of inactivity, the device will be allowed to send data
         again.

         --- HOTPLGK ---
         An AA byte was received followed by a period of
         inactivity.  This is taken to be a hot plug of a
         keyboard.  The new keyboard is to be brought up to date.

			<<< V5.1 2001/2/8 Removed from here to RESETM.
         --- HOTPLGM ---
         An AA 00 string was received.  This is taken to
         be a hot plug of a mouse.  The new mouse is to
         be brought up to date.
         >>> V5.1 2001/2/8 Removed.

         --- RESETK, ---
         These sequence numbers and up are used to reset and update a
         device.  All data from this device is being ignored until
         it has timed out. */
         if (Int_Var.Active_Aux_Port != AUX_PORT3_CODE)
         {
            if (Flag.Aux_Not_RESP)
            {
               Flag.Aux_Not_RESP = 0;
               aux_pntr->sequence = IDLE;
            }
         }
         #if AUX_PORT3_SUPPORTED
         else if (Aux_Port3.disable)
         {
            aux_pntr->sequence = IDLE;
            Ext_Cb0.bit.EN_AUX_PORT3 = 0; /* Disable TB. */
            Clr_Aux_Port3_Clk();
         }
         #endif
         skip_it = TRUE;
         break;
      #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   }

   if (aux_pntr->disable) skip_it = TRUE;

   if (skip_it)
   {
      ;
   }
   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   else if (aux_pntr->kbd)
   {
      /* Keyboard compatible device is on Auxiliary Port. */

      if (Ccb42.bit.DISAB_KEY == 1)
      {
         /* The auxiliary keyboards are
            disabled.  Ignore the data. */
         ;
      }
      else
      {
         /* Send keyboard data received from Auxiliary Port to Host. */

         send_auxk_to_pc(aux_pntr, Int_Var.Active_Aux_Port);
      }
   }
   #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   else
   {
      /* Mouse compatible device is on Auxiliary Port. */

      if (Ccb42.bit.DISAB_AUX == 1)
      {
         /* The auxiliary devices (mice)
            are disabled.  Ignore the data. */
         ;
      }
      else
      {
         /* Send mouse data received from Auxiliary Port to Host. */
         #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         if (Int_Mouse.field.Add4byte && !aux_pntr->intm && (aux_pntr->sequence == ACTIVE))
         {
            Int_Mouse.field.Byte_Count++;    /* increment the byte count. */
            if ((Int_Mouse.field.Byte_Count == 0) && Int_Mouse.field.INT_DRV5 && !aux_pntr->intm5)
               Aux_Data.byte &= 0x0F;
         }
         #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

         if (Expect_Count != 0)
         {
            Expect_Count -= 1;
         }
         send_auxd_to_pc(aux_pntr, Int_Var.Active_Aux_Port);
      }
   }

   if (Output_Buffer_Full())
   {
      Enable_Host_OBE1_Irq();
      Enable_Host_IBF_Irq();
   }
   else
   {
      handle_unlock();
   }
}

/* ---------------------------------------------------------------
   Name: service_send

   Send data from the local keyboard or from multibyte command
   responses to the Host.

   Send scan codes from local keyboard (scanned keyboard) to the
   Host.  Also handle multiple byte transmissions for standard
   commands and extended commands that return more than one byte
   to the Host.

   When sending multiple bytes, the first byte is sent
   immediately, but the remaining bytes are sent by generating
   another send request via the function "handle_unlock" which
   will call "Start_Scan_Transmission".  If more bytes are to
   be sent, "Start_Scan_Transmission" will start Timer A, and
   the Timer A interrupt handler will generate the send request
   when the response timer has expired!
   --------------------------------------------------------------- */
static void service_send(void)
{
   /* The service flag has already been cleared. */

   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   FLAG temp_flag, send;
   WORD data;

   send = FALSE;

   if (
         #if AUX_PORT3_SUPPORTED
         ((Aux_Port3.kbd == 0) && (Aux_Port3.ack)) ||
         #endif
         #if AUX_PORT2_SUPPORTED
         ((Aux_Port2.kbd == 0) && (Aux_Port2.ack)) ||
         #endif
         ((Aux_Port1.kbd == 0) && (Aux_Port1.ack))
      )
   {
      /* Skip if there is a mouse waiting to send an ACK. */
      ;
   }
   else
   {
      Disable_Irq();

      if (Output_Buffer_Full())
      {
         /* Host has not taken last byte yet
            Setup to send byte later. */

         /* SEND_DELAY is, actually, more than one bit. */
         Timer_B.bit.SEND_DELAY = RESPONSE_TIME;
         Load_Timer_B();
         Timer_B.bit.SEND_ENABLE = 1;

         Enable_Host_IBF_Irq();  /* Re-enable PC interface interrupt. */
         Enable_Irq();
      }
      else
      {
         /* Lock auxiliary devices. */
         Int_Lock_Aux_Port1();

         #if AUX_PORT2_SUPPORTED
         Int_Lock_Aux_Port2();
         #endif

         #if AUX_PORT3_SUPPORTED
         Int_Lock_Aux_Port3();
         #endif

         /* Start inhibit delay timer. */
         Load_Timer_B();
         Timer_B.bit.INH_CLEAR = 1;

         Enable_Irq();
         #if MOUSE_EMULATION
         if((Aux_Response == respCMD_AUX) && (Aux_Send_Timer == 0) && Aux_Tmp_Load1)
         {
            data = *Aux_Tmp_Pntr++;
            Aux_Tmp_Load1--;

            if (Aux_Tmp_Load1 == 0) { /* Keep going if not end of array. */
                                      /* Otherwise, stop by clearing. */
               Aux_Response = 0;
            }

            if(!Ccb42.bit.DISAB_AUX) {
               Aux_Data_To_Host(data);
            }
            handle_unlock();
         }
         else if (Kbd_Response.byte == 0)  // jacob 0419: modify for fix mouse emulation issue,--
         #else // MOUSE_EMULATION
         if (Kbd_Response.byte == 0)
         #endif // MOUSE_EMULATION
         {
            data = Get_Buffer();
            if (data == (WORD) ~0)
            {
               /* Buffer is empty. */
               handle_unlock();
            }
            else
            {
               /* data = byte from buffer. */

               if (Ext_Cb3.bit.DIAG_MODE == 0)
               {
                  send = TRUE;
               }
               else
               {  /*
                  Diagnostic mode.
                  data = RC (row/column) byte from buffer.  Set
                  SENDING_SCAN_RC flag to indicate to Data_To_Host
                  routine to send to Host with no interrupt. */

                  Gen_Info.bit.BREAK_SCAN = 0;
                  Flag.SENDING_SCAN_RC = 1;
                  Data_To_Host(data);
                  handle_unlock();
               }
            }
         }
         else if (Kbd_Response.bit.CMD_RESPONSE == 0)
         {
            /* This is not a multibyte command response. */
            data = get_response();
            send = TRUE;
         }
         else
         {
            /* This is a multibyte command response. */

            if ( (Kbd_Response.byte & maskKBD_RESPONSE_CODE) ==
                 (respCMD_AC & maskKBD_RESPONSE_CODE) )
            {
               /* Special handling for Command ACh. */

               if (Ccb42.bit.DISAB_KEY == 1)
               {
                  /* Do not send anything if keyboard is disabled. */
                  ;
               }
               //else if ( (Hif_Var[HIF_KBC].Tmp_Byte[0] & 1) == 0 )
               //{
               //   /* Get code and bump Tmp_Byte[0]. */
               //   data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_KBC], HIF_KBC);
               //   /* Send code. */
               //   Data_To_Host(data);
               //}
               //else
               //{
               //   /* This time, do not send code.  This causes an
               //      extra delay in sending the codes which appears
               //      to help the Host.  Just bump Tmp_Byte[0]. */
               //   Hif_Var[HIF_KBC].Tmp_Byte[0]++;
               //}
               else
               {
                  data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_KBC], HIF_KBC);
                  /* Send code. */
                  Data_To_Host(data);
               }
            }
            else
            {
               data = get_multibyte((HIF_VAR*)&Hif_Var[HIF_KBC], HIF_KBC);
               Data_To_Host(data);
            }

            handle_unlock();
         }
      }
   }

   if (send)
   {
      temp_flag = Gen_Info.bit.BREAK_SCAN;
      Gen_Info.bit.BREAK_SCAN = 0;

      /* Send it. */
      if (send_to_pc(data, temp_flag))
      {
         Gen_Info.bit.BREAK_SCAN = 1;  /* Break prefix code. */
      }
      handle_unlock();
   }
}

/* ---------------------------------------------------------------
   Name: service_1mS

   Service performed at a 1 millisecond rate.
   --------------------------------------------------------------- */
static void service_1mS(void)
{
   /* The service flag has already been cleared. */

   SMALL index;

#if PORT80_SUPPORTED
   if (READ_BIT(DP80STS, FNE))
   {
       SDP_ENABLE();
       while(READ_BIT(DP80STS, FNE))
       {
           SDP_WRITE(0, DP80BUF);
       }
   }
#endif


   Disable_Irq();
   #if MOUSE_EMULATION
   if (Aux_Send_Timer) {
      Aux_Send_Timer--;
      if(Aux_Send_Timer == 0) Service.bit.SEND = 1;      /* Post service request. */
   }
   #endif // MOUSE_EMULATION
   Int_Var.Ticker_10--;
   Enable_Irq();
   if (Int_Var.Ticker_10 == 0)
   {
      Disable_Irq();
      /* Ensure that KBSOUT keep in pull low. */
//       KBSOUT0 &= ~(((WORD) Msk_Strobe_H << 8) | Msk_Strobe_L);
	   Int_Var.Ticker_10 = 10;
	   Int_Var.Ticker_100--;
	   Enable_Irq();

       //#if AUX_PORT1_SUPPORTED || AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
       check_aux();
       //#endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

     // check for Flash Update
     //(*FLASH_UPDATE_handle_ptr)();
   }
    #if ECDT_SHM_SUPPORTED
    if (ECDT_SHM_HIF_handle() == FALSE)
    #endif // ECDT_SHM_SUPPORTED
    {
        FLASH_UPDATE_handle();
    }

	Handle_Sys_Funct();

   #if ACPI_SUPPORTED
	Handle_ACPI();
	#endif

//   Handle_SMBus_Alarm();
   Gen_Hookc_Service_1mS();

#if PORT80_SUPPORTED
   SDP_DISABLE();
#endif

}


void Check_ITP(void)
{
    if (Ext_Cb3.bit.DISABLE_TB)  //TP is disable
    {
        if(Ext_Cb0.bit.EN_AUX_PORT1)
        {
            Lock_Aux_Devs();  /* Prevent auxiliary device transmissions. */
            Ext_Cb0.bit.ITP_DISABLED = 1;   //TP_0226-08
            Ext_Cb0.bit.EN_AUX_PORT1 = 0;
            Aux_Port1.sequence = IDLE;
            Load_Timer_B();
            Timer_B.bit.UNLOCK_TMO = 1;
        }
    }
    else if (!Ext_Cb0.bit.EN_AUX_PORT1)
    {
        Aux_Port1.valid = 1;
        Ext_Cb0.bit.ITP_DISABLED = 0;
        Ext_Cb0.bit.EN_AUX_PORT1 = 1;
        Aux_Port1.lock = 0;
        Aux_Port1.sequence = IDLE;
        Load_Timer_B();
        Timer_B.bit.UNLOCK_TMO = 1;
    }
}

/* ---------------------------------------------------------------
   Name: check_aux

   Periodic check for auxiliary (PS/2) devices.
   --------------------------------------------------------------- */
static void check_aux(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   #ifndef AUX_PORT2_SUPPORTED
   #error AUX_PORT2_SUPPORTED switch is not defined.
   #endif

   if (
         #if AUX_PORT3_SUPPORTED
         Aux_Port3.lock &&
         #endif

         #if AUX_PORT2_SUPPORTED
         Aux_Port2.lock &&
         #endif

         Aux_Port1.lock ||
        (Int_Var.Active_Aux_Port) )
   {  /*
      If all the devices are locked, the ticker is cleared.
      The Host may be communicating to the Keyboard Controller
      and keeping the devices locked out.  If the ticker was
      updated in this case, any device that is waiting to send
      data will not be able to and the code may incorrectly
      conclude that the devices are inactive.

      If Int_Var.Active_Aux_Port is not zero, the ticker is cleared.
      This signals that data is to be sent to a device, that data
      is expected from a device (ACK), or that a start bit has been
      received from a device.  This routine may decide to send
      data to a device.  If the device is near the end of sending
      the data, it cannot be stopped.  Clearing the Aux_Port_Ticker
      here guards against the ticker timing out and attempting
      to send data to the device. */

      Aux_Port_Ticker = 0; /* Reset the main inactivity timer. */
   }
   else
   {
      /* Increment the main inactivity timer. */
      Aux_Port_Ticker++;
      if (Aux_Port_Ticker == 0) Aux_Port_Ticker = 0xFF;
   }

   if (
       #if AUX_PORT3_SUPPORTED
       (Aux_Port3.sequence == IDLE) &&
       #endif

       #if AUX_PORT2_SUPPORTED
       (Aux_Port2.sequence == IDLE) &&
       #endif

       (Aux_Port1.sequence == IDLE))
   {
      /* All ports are idle. */

      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      if (Flag.NEW_PRI_K)
      {  /*
         A request to find a new primary keyboard has been
         posted.  There are only two auxiliary keyboards possible. */

         if ( (Ps2_Ctrl.byte & (maskPRI_KBD << shiftPRI_KBD))
              == (AUX_PORT2_CODE << shiftPRI_KBD) )
         {  /*
            The primary keyboard is set to be on Auxiliary Port 2.
            If there is a keyboard on port 1 or 3, make sure that it
            is up to date and then make it the primary keyboard. */

            #if AUX_PORT3_SUPPORTED
            if ((Aux_Port3.kbd) && (Aux_Port3.valid))
            {
               if ((Aux_Port3.kbd) && (Aux_Port3.valid))
                  Aux_Port3.sequence = CMDK;
            }
            else
            #endif

               Flag.NEW_PRI_K = 0;
         }
         else
         {  /*
            The primary keyboard is set to be on Auxiliary Port 2.
            If there is a keyboard on port 1 or 4, make sure that it
            is up to date and then make it the primary keyboard. */
            #if AUX_PORT2_SUPPORTED
            if ((Aux_Port2.kbd) && (Aux_Port2.valid))
            {
               if ((Aux_Port2.kbd) && (Aux_Port2.valid))
                  Aux_Port2.sequence = CMDK;
            }
            else
            #endif
               Flag.NEW_PRI_K = 0;
         }
      }

      if (Flag.NEW_PRI_M)
      {  /*
         A request to find a new primary mouse has been
         posted.  A mouse can be on Auxilary Port 1, 2, or 3. */

         switch (Ps2_Ctrl.byte & (maskPRI_MOUSE << shiftPRI_MOUSE))
         {
            case (AUX_PORT1_CODE << shiftPRI_MOUSE):
               /* The primary mouse is set to be on Auxiliary Port 1. */

               #if AUX_PORT2_SUPPORTED
               if ((Aux_Port2.kbd == 0) && (Aux_Port2.valid))
               {  /*
                  There is a mouse on port 2, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port2.sequence = CMDM;
               }
               #endif

               #if AUX_PORT3_SUPPORTED
               #if AUX_PORT2_SUPPORTED
               else if ((Aux_Port3.kbd == 0) && (Aux_Port3.valid))
               #else
               if ((Aux_Port3.kbd == 0) && (Aux_Port3.valid))
               #endif
               {  /*
                  There is a mouse on port 3, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port3.sequence = CMDM;
               }
               #endif
               #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
               else
               #endif
                  Flag.NEW_PRI_M = 0;
               break;

            #if AUX_PORT2_SUPPORTED
            case (AUX_PORT2_CODE << shiftPRI_MOUSE):
               /* The primary mouse is set to be on Auxiliary Port 2. */

               #if AUX_PORT3_SUPPORTED
               if ((Aux_Port3.kbd == 0) && (Aux_Port3.valid))
               {  /*
                  There is a mouse on port 3, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port3.sequence = CMDM;
               }
               else if ((Aux_Port1.kbd == 0) && (Aux_Port1.valid))
               #else
               if ((Aux_Port1.kbd == 0) && (Aux_Port1.valid))
               #endif
               {  /*
                  There is a mouse on port 1, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port1.sequence = CMDM;
               }
               else Flag.NEW_PRI_M = 0;
               break;
            #endif

            #if AUX_PORT3_SUPPORTED
            case (AUX_PORT3_CODE << shiftPRI_MOUSE):
               /* The primary mouse is set to be on Auxiliary Port 3. */
               if ((Aux_Port1.kbd == 0) && (Aux_Port1.valid))
               {  /*
                  There is a mouse on port 1, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port1.sequence = CMDM;
               }

               #if AUX_PORT2_SUPPORTED
               else if ((Aux_Port2.kbd == 0) && (Aux_Port2.valid))
               {  /*
                  There is a mouse on port 2, make sure that it
                  is up to date and then make it the primary mouse. */

                  Aux_Port2.sequence = CMDM;
               }
               #endif

               else Flag.NEW_PRI_M = 0;
               break;
            #endif
         }
      }
      #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   }

   do_sequence(&Aux_Port1, AUX_PORT1_CODE);

   #if AUX_PORT2_SUPPORTED
   do_sequence(&Aux_Port2, AUX_PORT2_CODE);
   #endif
   #if AUX_PORT3_SUPPORTED
   do_sequence(&Aux_Port3, AUX_PORT3_CODE);
   #endif
}

void do_sequence(AUX_PORT *aux_pntr, SMALL port)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   SMALL test;
   FLAG enable_all_devices = FALSE;
   FLAG reset_external_mouse = FALSE;
   FLAG update_external_mouse = FALSE;

   if (Int_Var.Active_Aux_Port == port)
   {  /*
      The Int_Var.Active_Aux_Port variable is not zero.  It is set
      to this port.  This signals that data is to be sent to this
      port, that data is expected from this port (ACK), or that a
      start bit has been received from this port.  This routine may
      decide to send data to a device.  If the device is near the
      end of sending the data, it cannot be stopped.  Clearing the
      inactivity timer here guards against the timer timing out and
      attempting to send data to the device. */
      aux_pntr->ticker = 0;
   }
   else if (aux_pntr->lock)
   {  /*
      If this device is locked, this port's inactivity timer is
      not updated.  The Host may be communicating to the Keyboard
      Controller and keeping the devices locked out.  If the timer
      was updated in this case, any device that is waiting to send
      data will not be able to and the code may incorrectly
      conclude that the device is inactive. */
      ;
   }
   else
   {
      /* Increment this port's inactivity timer. */
      aux_pntr->ticker++;
      if (aux_pntr->ticker == 0) aux_pntr->ticker = 0xFF;
   }

   switch (aux_pntr->sequence)
   {
      case IDLE:
         /* There has been no activity from this device for a while. */
         break;

      case ACTIVE:
         /* This has been an active device.  Stay at this
            sequence number until the device is inactive. */
         if (aux_pntr->ticker >= INACTIVE_TIME)
         {
            /* There has been INACTIVE_TIME of inactivity
               at this port.  There has been no activity
               on this Auxiliary Port for INACTIVE_TIME. */
            aux_pntr->sequence = IDLE;
         }
         break;

      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      case TYPE_S1:
         /*
         There was no activity from the devices for a while.
         The first byte of a packet from a mouse or a byte of
         keyboard data resulted in an error.  This is being
         handled as a device that was just plugged in.  Wait
         until the devices are inactive and then attempt to
         find the type of this device. */

         if (Aux_Port_Ticker >= POWER_UP_TIME)
         {
            /* Send reset command to device and go to next step. */
            aux_pntr->valid = 1;
            if (send_to_ext(aux_pntr, port, 0xFF) == 0)
               aux_pntr->sequence = TYPE_S2;
         }
         break;

      case TYPE_S2:
         /* A reset command has been sent to the device on this port.
            When an ACK or AA is received, the sequence is changed.
            If neither is received, just forget it and go to idle. */
         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {
            /* A sufficient period of device inactivity
               has occurred since sending a reset command
               to this device and the sequence has not changed. */

            enable_all_devices = TRUE;
            aux_pntr->sequence = IDLE;
         }
         break;

      case TYPE_S3:
         /*
         Received ACK while attempting to find
         the type of the device.  Expecting AA. */
         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {
            /* There has been no activity on any
               Auxiliary Port for RESET_CMD_TIME. */
            enable_all_devices = TRUE;
            aux_pntr->sequence = IDLE;
         }
         break;
      #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

      case AA_RCVD:
         /*
         An AA was received from the device.  This may be
         the AA after the device's power up test or after
         a reset command that was sent to the device
         (while attempting to determine the device type). */

         if (aux_pntr->ticker >= RESET_CMD_TIME)
         {  /*
            A sufficient period of device inactivity has occurred
            since receiving the AA.  This must be a keyboard. */

            aux_pntr->kbd = 1;
            aux_pntr->sequence = HOTPLGK;
         }
         break;

      case RESET1:
         /*
         This is a primary device.  Secondary devices cannot have
         this sequence code.  A primary device is the device that
         is to respond to commands.  The secondary device(s) of
         the same type are to be updated to match.

         A reset command was sent to this device. */

         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {  /*
            A sufficient period of device inactivity has
            occurred since issuing the reset command to
            the device on this Auxiliary Port. */

            #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            /* Use the Aux_Port_Ticker temporarily. */
            if (aux_pntr->kbd) Aux_Port_Ticker = RESETK;
            else  Aux_Port_Ticker = RESETM;
            if (!MULPX.bit.Multiplex || aux_pntr->kbd)
            {  /* If in Multiplex mode and this port is kbd
                  else this port is kbd/aux. */

               if ( (port != AUX_PORT1_CODE) && Aux_Port1.valid &&
                                 (aux_pntr->kbd == Aux_Port1.kbd) )
                     Aux_Port1.sequence = Aux_Port_Ticker;

               #if AUX_PORT2_SUPPORTED
               if ( (port != AUX_PORT2_CODE) && Aux_Port2.valid &&
                           (aux_pntr->kbd == Aux_Port2.kbd) )
                  Aux_Port2.sequence = Aux_Port_Ticker;
               #endif

               #if AUX_PORT3_SUPPORTED
               if ( (port != AUX_PORT3_CODE) && Aux_Port3.valid &&
                     (aux_pntr->kbd == Aux_Port3.kbd) &&
                     (Ext_Cb0.bit.EN_AUX_PORT3))

                  Aux_Port3.sequence = Aux_Port_Ticker;
               #endif
            }
            #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

            aux_pntr->sequence = ACTIVE;

            /* Restore the Aux_Port_Ticker. */
            Aux_Port_Ticker = RESET_CMD_TIME;
         }
         break;

      case CMD1:
         /*
         This is a primary device.  Secondary devices cannot have
         this sequence code.  A primary device is the device that
         is to respond to commands.  The secondary device(s) of
         the same type are to be updated to match.

         A command other than a reset was sent to this device. */

         if (Aux_Port_Ticker >= OTHER_CMD_TIME)
         {
            /* A sufficient period of device inactivity has occurred
               since issuing a command other than a reset to the
               device on this Auxiliary Port. */

            #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            /* Use the Aux_Port_Ticker temporarily. */
            if (aux_pntr->kbd) Aux_Port_Ticker = CMDK;
            else  Aux_Port_Ticker = CMDM;

            if (!MULPX.bit.Multiplex || aux_pntr->kbd)
            {  /* If in Multiplex mode and this port is kbd
                  else this port is kbd/aux. */

               if ( (port != AUX_PORT1_CODE) && Aux_Port1.valid &&
                    (aux_pntr->kbd == Aux_Port1.kbd) )
                  Aux_Port1.sequence = Aux_Port_Ticker;

               #if AUX_PORT2_SUPPORTED
               if ( (port != AUX_PORT2_CODE) && Aux_Port2.valid &&
                    (aux_pntr->kbd == Aux_Port2.kbd) )
                  Aux_Port2.sequence = Aux_Port_Ticker;
               #endif

               #if AUX_PORT3_SUPPORTED
               if ( (port != AUX_PORT3_CODE) && Aux_Port3.valid &&
                    (aux_pntr->kbd == Aux_Port3.kbd) &&
                    (Ext_Cb0.bit.EN_AUX_PORT3))
                  Aux_Port3.sequence = Aux_Port_Ticker;
               #endif

            }
            #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
            aux_pntr->sequence = ACTIVE;

            /* Restore the Aux_Port_Ticker. */
            Aux_Port_Ticker = OTHER_CMD_TIME;
         }
         break;

      #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      case IGNORE:
         /*
         All data from this device is being ignored until it has timed
         out.  It may be that an AA was received followed by an error.
         Or some data is being ignored.  This is done so that the Host
         does not get out of synchronization with a mouse.  If the
         second byte of a packet is allowed to be sent to the Host
         after the first byte has been ignored, the mouse will be out
         of synchronization with the Host.  After a period of
         inactivity, the device will be allowed to send data again. */

         if (aux_pntr->ticker >= INACTIVE_TIME)
         {
            /* There has been no activity
               on this port for INACTIVE_TIME. */
            aux_pntr->sequence = IDLE;
         }
         else if (aux_pntr->kbd == 0)
         {
            /* For a mouse device.

               If this is the only active device, cause an enable
               mouse command to be sent to it.  Sending an enable
               command will cause the next byte from the mouse
               to be the first byte of a packet. */

            switch (port)
            {
               case AUX_PORT1_CODE:
                  /* Check ports 2 and 3. */

                  #if AUX_PORT2_SUPPORTED
                  if ( (Aux_Port2.sequence == IDLE)
                     #if AUX_PORT3_SUPPORTED
                     && (Aux_Port3.sequence == IDLE)
                     #endif
                     )
                  #else
                     #if AUX_PORT3_SUPPORTED
                     if (Aux_Port3.sequence == IDLE)
                     #endif
                  #endif
                  {
                     if (send_to_ext(aux_pntr, port, 0xF4) == 0)
                     {
                        aux_pntr->sequence = CMDM;
                        Aux_Port_Ticker = OTHER_CMD_TIME;
                     }
                  }
                  break;

               #if AUX_PORT2_SUPPORTED
               case AUX_PORT2_CODE:
                  /* Check ports 1, 3 and 4. */

                  if ( (Aux_Port1.sequence == IDLE)
                     #if AUX_PORT3_SUPPORTED
                     && (Aux_Port3.sequence == IDLE)
                     #endif
                     )
                  {
                     if (send_to_ext(aux_pntr, port, 0xF4) == 0)
                     {
                        aux_pntr->sequence = CMDM;
                        Aux_Port_Ticker = OTHER_CMD_TIME;
                     }
                  }
                  break;
               #endif

               #if AUX_PORT3_SUPPORTED
               case AUX_PORT3_CODE:
                  /* Check ports 1, 2 and 4. */

                  if ( (Aux_Port1.sequence == IDLE)
                     #if AUX_PORT2_SUPPORTED
                     && (Aux_Port2.sequence == IDLE)
                     #endif
                     )
                  {
                     if (send_to_ext(aux_pntr, port, 0xF4) == 0)
                     {
                        aux_pntr->sequence = CMDM;
                        Aux_Port_Ticker = OTHER_CMD_TIME;
                     }
                  }
                  break;
               #endif
            }
         }
         break;

      case RESETK:
         /*
         This is a secondary keyboard.  A reset command was sent
         to the primary keyboard and a period of inactivity has
         elapsed.  A reset command must be sent to this keyboard.

         There is a possibility that there is another secondary
         keyboard on the other Auxiliary Port and a command was
         sent to it.  Therefore, wait until there is no activity
         on any Auxiliary Port before sending the command. */

         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {  /*
            A sufficient period of device inactivity has
            occurred since issuing a command to a device. */

            /* Send reset command to keyboard. */
            if (send_to_ext(aux_pntr, port, 0xFF) == 0)
            {
               /* Initialize this keyboard's status bytes. */
               aux_pntr->state_byte0 = SAVE_KBD_STATE_INIT;
               aux_pntr->state_byte1 = Save_Typematic;

               /* Update the sequence. */
               aux_pntr->sequence = WAITRK;
            }
         }
         break;

      case WAITRK:
         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {  /*
            A sufficient period of device inactivity has
            occurred since issuing a command to a device. */

            aux_pntr->sequence = CMDK;
         }
         break;

      case HOTPLGK:
         /*
         An AA byte was received followed by a period of
         inactivity.  This is taken to be a hot plug of a
         keyboard.  The new keyboard is to be brought up to date.

         Initialize this keyboard's status bytes. */

         aux_pntr->state_byte0 = SAVE_KBD_STATE_INIT;
         aux_pntr->state_byte1 = Save_Typematic;

         /* Change the sequence number for the next time through. */
         aux_pntr->sequence = CMDK;

         /* Fall through to CMDK. */
      case CMDK:
         /*
         This is a secondary keyboard that must be updated to
         match the primary keyboard.  A command was sent to a
         keyboard on an Auxiliary Port.  Wait until there is no
         activity on any Auxiliary Port before sending the command. */

         if (Aux_Port_Ticker >= OTHER_CMD_TIME)
         {  /*
            A sufficient period of device inactivity
            has occurred since issuing a command other
            than a reset to a device on an Auxiliary Port.

            See if any further updates are necessary. */

            if ( (aux_pntr->setflag1) && (aux_pntr->setflag2) )
            {  /*
               Send data byte of Scan Code Set command. */
               if (send_to_ext(aux_pntr, port,
                   (Save_Kbd_State >> shiftCODESET) & maskCODESET) == 0)
               {
                  /* Save Scan Code Set number of this keyboard. */
                  aux_pntr->state_byte0 &= ~(maskCODESET << shiftCODESET);
                  aux_pntr->state_byte0 |= (Save_Kbd_State & (maskCODESET << shiftCODESET));

                  aux_pntr->setflag1 = 0;
                  aux_pntr->setflag2 = 0;
               }
            }
            else if (aux_pntr->setflag1)
            {  /*
               Send data byte of update typematic rate command. */
               if (send_to_ext(aux_pntr, port, Save_Typematic) == 0)
               {
                  /* Save typematic rate of this keyboard. */
                  aux_pntr->state_byte1 = Save_Typematic;

                  aux_pntr->setflag1 = 0;
               }
            }
            else if (aux_pntr->setflag2)
            {  /*
               Send data byte of update LEDs command. */
               if (send_to_ext(aux_pntr, port, Save_Kbd_State & maskLEDS) == 0)
               {
                  /* Save LED state of this keyboard. */
                  aux_pntr->state_byte0 &= ~maskLEDS;
                  aux_pntr->state_byte0 |= (Save_Kbd_State & maskLEDS);

                  aux_pntr->setflag2 = 0;
               }
            }
            else
            {
               /* Other updates. */

               /* Set bits of state_byte0 and Save_Kbd_State
                  that are different from each other. */
               test = aux_pntr->state_byte0 ^ Save_Kbd_State;

               if (test & (maskCODESET << shiftCODESET))
               {
                  /* If Scan Code Set number of both keyboards is not
                     the same, update the number of secondary keyboard. */
                  if (send_to_ext(aux_pntr, port, 0xF0) == 0)
                  {
                     /* Set 2 flags for this 2 byte command. */
                     aux_pntr->setflag1 = 1;
                     aux_pntr->setflag2 = 1;
                  }
               }
               else if (aux_pntr->state_byte1 != Save_Typematic)
               {
                  /* If typematic rate of both keyboards is not the same,
                     update the typematic rate of this keyboard. */
                  if (send_to_ext(aux_pntr, port, 0xF3) == 0)
                  {
                     /* Set flag1 for this 2 byte command. */
                     aux_pntr->setflag1 = 1;
                  }
               }
               else if (test & maskLEDS)
               {
                  /* If LED state of both keyboards is not the same,
                     update the LED state of secondary keyboard. */
                  if (send_to_ext(aux_pntr, port, 0xED) == 0)
                  {
                     /* Set flag2 for this 2 byte command. */
                     aux_pntr->setflag2 = 1;
                  }
               }
               else if (test & maskAUXENAB)
               {
                  /* If enabled state of both keyboards is not the same,
                     enable or disable this keyboard. */

                  if (Save_Kbd_State & maskAUXENAB)
                  {
                     if (send_to_ext(aux_pntr, port, 0xF4) == 0)
                        aux_pntr->state_byte0 |= maskAUXENAB;
                  }
                  else
                  {
                     if (send_to_ext(aux_pntr, port, 0xF5) == 0)
                        aux_pntr->state_byte0 &= ~maskAUXENAB;
                  }
               }
               else
               {
                  /* Keyboard on Auxiliary Port 1 or 2 is updated. */

                  aux_pntr->sequence = ACTIVE;
                  enable_all_devices = TRUE;

                  if (Flag.NEW_PRI_K)
                  {
                     /* A request to find a new primary keyboard
                        has been posted.  Since this is a secondary
                        keyboard, make this the primary keyboard. */

                     Flag.NEW_PRI_K = 0;
                     Ps2_Ctrl.byte &= ~(maskPRI_KBD << shiftPRI_KBD);
                     Ps2_Ctrl.byte |= (port << shiftPRI_KBD);
                  }
               }
            }
         }
         break;

      case HOTPLGM:
         /*
         An AA 00 string was received.  This is taken to
         be a hot plug of a mouse.  The new mouse is to be
         brought up to date.
         Fall through to RESETM. */
      case RESETM:
         /*
         This is a secondary mouse.  A reset command was sent
         to the primary mouse and a period of inactivity has
         elapsed.  A reset command (or simply a "set default"
         command) must be sent to this mouse.

         There is a possibility that there is another secondary
         mouse on the other Auxiliary Port and a command was
         sent to it.  Therefore, wait until there is no activity
         on any Auxiliary Port before sending the command.

         OTHER_CMD_TIME will be used instead of RESET_CMD_TIME
         because the "set default" command is used instead of
         the "reset" command. */

         aux_pntr->intseq = 0;
         if (Aux_Port_Ticker >= RESET_CMD_TIME)
         {  /*
            A sufficient period of device inactivity has
            occurred since issuing a command to a device.

            /* Send "set default" command to mouse. */
            if (send_to_ext(aux_pntr, port, 0xFF) == 0)
            {
               /* Initialize this mouse's status bytes. */
               aux_pntr->state_byte0 = 0;    /* scaling 1:1, disabled,
                                                stream mode. */
               aux_pntr->state_byte1 = 2;    /* Resolution Setting. */
               aux_pntr->state_byte2 = 0x64; /* Sampling Rate. */

            }
         }
         break;

      case CMDM:
         /*
         This is a secondary mouse that must be updated to
         match the primary mouse.  A command was sent to a
         mouse on an Auxiliary Port.  Wait until there is no
         activity on any Auxiliary Port before sending the command. */

         if (Aux_Port_Ticker >= OTHER_CMD_TIME)
         {  /*
            A sufficient period of device inactivity
            has occurred since issuing a command other
            than a reset to a device on an Auxiliary Port.

            See if any further updates are necessary. */

            if (aux_pntr->setflag1)
            {  /*
               Send data byte of update sampling rate command. */
               if (send_to_ext(aux_pntr, port, Mouse_State[2]) == 0)
               {
                  /* Save sampling rate of this mouse. */
                  aux_pntr->state_byte2 = Mouse_State[2];

                  aux_pntr->setflag1 = 0;
               }
            }
            else if (aux_pntr->setflag2)
            {  /*
               Send data byte of update resolution command. */
               if (send_to_ext(aux_pntr, port, Mouse_State[1]) == 0)
               {
                  /* Save resolution of this mouse. */
                  aux_pntr->state_byte1 = Mouse_State[1];

                  aux_pntr->setflag2 = 0;
               }
            }
            else
            {
               /* Other updates. */

               /* Set bits of state_byte0 and Mouse_State[0]
                  that are different from each other. */
               test = aux_pntr->state_byte0 ^ Mouse_State[0];

               if (test & SCALING_2_1)
               {
                  /* If scaling state of primary mouse and
                     this mouse is not the same, update
                     the scaling state of this mouse. */

                  if (Mouse_State[0] & SCALING_2_1)
                  {
                     if (send_to_ext(aux_pntr, port, 0xE7) == 0)
                        aux_pntr->state_byte0 |= SCALING_2_1;
                  }
                  else
                  {
                     if (send_to_ext(aux_pntr, port, 0xE6) == 0)
                        aux_pntr->state_byte0 &= ~SCALING_2_1;
                  }
               }
               else if (aux_pntr->state_byte1 != Mouse_State[1])
               {
                  /* If resolution of primary mouse and
                     this mouse is not the same, update
                     the resolution of this mouse. */
                  if (send_to_ext(aux_pntr, port, 0xE8)== 0)
                  {
                     /* Set flag for this 2 byte command. */
                     aux_pntr->setflag2 = 1;
                  }
               }
               else if (aux_pntr->state_byte2 != Mouse_State[2])
               {
                  /* If sampling rate of primary mouse and
                     this mouse is not the same, update
                     the sampling rate of this mouse. */
                  if (send_to_ext(aux_pntr, port, 0xF3) == 0)
                  {
                     /* Set flag for this 2 byte command. */
                     aux_pntr->setflag1 = 1;
                  }
               }
               else if (test & ENABLED)
               {
                  /* If enabled state of primary mouse
                     and this mouse is not the same,
                     enable or disable this mouse. */

                  if (Mouse_State[0] & ENABLED)
                  {
                     if (send_to_ext(aux_pntr, port, 0xF4) == 0)
                        aux_pntr->state_byte0 |= ENABLED;
                  }
                  else
                  {
                     if (send_to_ext(aux_pntr, port, 0xF5) == 0)
                        aux_pntr->state_byte0 &= ~ENABLED;
                  }
               }
               #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
               else if (!aux_pntr->intseq && Int_Mouse.field.INT_DRV && aux_pntr->valid)
               {
                  switch (Int_Mouse.field.Seq)
                  {
                     case 0x00:
                     case 0x03:
                     case 0x04:
                        Mouse_State[2] = 0xC8;  /* C8 64 50 C8 C8 50 */
                        Int_Mouse.field.Seq++;
                        break;
                     case 0x01:
                        Mouse_State[2] = 0x64;
                        Int_Mouse.field.Seq++;
                        break;
                     case 0x02:
                     case 0x05:
                        Mouse_State[2] = 0x50;
                        if (Int_Mouse.field.INT_DRV5)
                           Int_Mouse.field.Seq++;
                        else
                           Int_Mouse.field.Seq = 0x06;
                        break;
                     //case 0x03:
                     case 0x06:
                        send_to_ext(aux_pntr, port, 0xF2);
                        Mouse_Cmd = 0xF2;
                        Int_Mouse.field.Seq = 0;
                        break;
                  }
               }
               #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
               else
               {
                  /* This mouse is updated. */

                  aux_pntr->sequence = ACTIVE;
                  enable_all_devices = TRUE;

                  if (Flag.NEW_PRI_M)
                  {
                     /* A request to find a new primary mouse
                        has been posted.  Since this is a secondary
                        mouse, make this the primary mouse. */

                     Flag.NEW_PRI_M = 0;
                     Ps2_Ctrl.byte &= ~(maskPRI_MOUSE << shiftPRI_MOUSE);
                     Ps2_Ctrl.byte |= (port << shiftPRI_MOUSE);
                  }
               }
            }
         }
         break;
         #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   }

   if (enable_all_devices)
   {
      /* Enable all auxiliary devices (if not locked).
         The devices have been inactive for a while
         or the secondary device(s) has been updated. */

      Aux_Port_Ticker = 0;    /* Reset the main inactivity timer. */
      aux_pntr->ticker = 0;   /* Reset this port's inactivity timer. */
   }
}

#if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
/* ---------------------------------------------------------------
   Name: send_to_ext

   Send byte to secondary device on Auxiliary Port.

   A primary device is the device that is to respond to commands.
   The secondary device(s) of the same type are to be updated to match.

   Input: aux_pntr = pointer to Aux_Port1, 2 or 3.
          data = byte to be sent to device.

   Returns: 1 if transmission to/from a device is in progress.
            0 otherwise.
   --------------------------------------------------------------- */
static FLAG send_to_ext(AUX_PORT *aux_pntr, SMALL port, BYTE data)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   FLAG busy = 0;
   FLAG enabled = FALSE;

   port &= 0x03;
   switch (port)
   {
      case AUX_PORT1_CODE:
         enabled = Ext_Cb0.bit.EN_AUX_PORT1;
         break;

      #if AUX_PORT2_SUPPORTED
      case AUX_PORT2_CODE:
         enabled = Ext_Cb0.bit.EN_AUX_PORT2;
         break;
      #endif

      #if AUX_PORT3_SUPPORTED
      case AUX_PORT3_CODE:
         enabled = Ext_Cb0.bit.EN_AUX_PORT3;
         break;
      #endif
   }

   if (enabled & aux_pntr->valid)
   {  /*
      Auxiliary device is enabled for transmission.

      Sending to a device relies on all devices being locked.
      Control is passed here because a timer has timed-out.
      Normal communications to a device is from a Host command.
      When a Host command is received, the PC interface interrupt
      is disabled and the devices are locked.  Because control
      is passed to here from a timer, there is no guarantee
      that the devices are locked.  Disable the Host interface
      and lock the devices as in the Host interrupt service routine. */

      Disable_Irq();

      if (Int_Var.Active_Aux_Port != 0)
      {  /*
         Int_Var.Active_Aux_Port is not zero.  This indicates that data
         is to be sent to a device, that data is expected from a device
         (ACK), or that a start bit has been received from a device. */

         Enable_Irq();
         busy = 1;
      }
      else
      {
         Disable_Host_IBF_Irq();    /* Disable PC interface interrupt. */
         Lock_Aux_Devs();           /* Prevent auxiliary device transmissions. */
         Lock_Scan();
         Disable_Gentmo();
         Timer_B.bit.UNLOCK_TMO = 0;/* Clear pending unlock requests. */
         Service.bit.UNLOCK = 0;
         Enable_Irq();

         Aux_Port_Ticker = 0;       /* Reset the main inactivity timer. */
         aux_pntr->ticker = 0;      /* Reset this port's inactivity timer. */

         if (aux_pntr->kbd) {Pcxae();}/* Enable Keyboard Interface. */
         else               {Pcxa8();}/* Enable Auxiliary Device Interface. */

         /* Send_To_Aux won't return until send complete or error sending. */
         if (Send_To_Aux(aux_pntr, port, data))
         {
            /* Error, assume no external device. */

            Disable_Irq();
            Service.bit.AUX_PORT_SND = 0;
            Enable_Irq();

            aux_pntr->sequence = IGNORE;
            aux_pntr->valid = 0;
            aux_pntr->setflag1 = 0;
            aux_pntr->setflag2 = 0;

            Gen_Timer = 0;       /* Clear timeout counter
                                    to remove error indication. */
         }

         aux_pntr->ack = 0;      /* Do not wait for acknowledge. */
         Disable_Irq();
         Service.bit.UNLOCK = 1; /* Post unlock service request. */
         Enable_Irq();
      }
   }

   return(busy);
}
#endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

/* ---------------------------------------------------------------
   Name: service_ext_irq

   An external interrupt has occurred, the interrupt was
   disabled, the service flag was set, and control was
   passed to here to generate a System Control Function.
   --------------------------------------------------------------- */
static void service_ext_irq(void)
{
   /* The service flag has already been cleared. */

   Gen_Hookc_Service_Ext_Irq();
}

/* ---------------------------------------------------------------
   Name: get_response

   Gets data required for internal keyboard (scanner)
   responses to keyboard commands sent from Host.

   Input: Kbd_Response.byte has response code.
          This is called when Kbd_Response.bit.CMD_RESPONSE == 0.

   Returns: data to send.
   --------------------------------------------------------------- */
   /* This table has commands that the keyboard may
      send to the Host in response to transmissions, etc. */
   const BYTE response_table[] =
   {
      0x00, /* 0 Undefined. */
      0xFA, /* 1 Manufacurer ID. */
      0xAB, /* 2 Byte 1 of keyboard ID. */
      0xAA, /* 3 BAT completion. */
      0xFC, /* 4 BAT failure. */
      0xEE, /* 5 Echo. */
      0xFA, /* 6 Acknowledge. */
      0xFE, /* 7 Resend. */
      0xFA, /* 8 undefined. */
      0x83, /* 9 Byte 2 of U.S. keyboard ID. */
      0xFA, /* A reset ack. */
      0xEE, /* B Echo command. */
      0xAA, /* C First response. */
      0xFA, /* D Return scan code 2. */
      0x02, /* E. */
      0x84  /* F Byte 2 of Japanese keyboard ID. */
   };

static WORD get_response(void)
{
   WORD result, code;

   code = 0;

   if (Kbd_Response.bit.SECOND_ACK)
   {
      /* Another ACK is waiting. */
      Kbd_Response.bit.SECOND_ACK = 0;
      code = 6;         /* Indicate that ACK should be sent. */
   }

   switch (Kbd_Response.byte & maskKBD_RESPONSE_CODE)
   {
      case 1:
         code = 2;      /* Set up second byte. */
         break;

      case 2:
         if (Get_Kbd_Type() == 0)
         {
            code = 9;   /* U.S. keyboard. */
         }
         else
         {
            code = 0xF; /* Japanese keyboard. */
         }
         break;
      case 0x08:
         code = 0x0D;
         break;

      case 0x0A:
         code = 3;      /* Also send BAT complete. */
         break;

      case 0x0D:
         code = 0x0E;   /* Also send BAT complete. */
         break;
   }

   result = response_table[Kbd_Response.byte & maskKBD_RESPONSE_CODE];

   if (((Kbd_Response.byte & maskKBD_RESPONSE_CODE) == 0x0E) && (result == 0x02))
   {
      if (((Save_Kbd_State >> shiftCODESET) & maskCODESET) == 1)
         result = 0x01;
      else if (((Save_Kbd_State >> shiftCODESET) & maskCODESET) == 3)
         result = 0x03;
   }

   Kbd_Response.byte =
      (Kbd_Response.byte & ~maskKBD_RESPONSE_CODE) | code;

   return(result);
}

/* ---------------------------------------------------------------
   Name: get_multibyte

   Get multiple bytes in response to a command that requires multiple
   bytes to be returned to Host.  These commands include the "standard"
   "AC" command and the extended command "85".  Only 1 byte at a time
   is returned from this function, it keeps track of the last byte it
   sent by maintaining a pointer to the "data packet" that contains
   the multiple bytes it is supposed to retrieve.  The next time it's
   called it will return the next byte in the packet until all bytes
   are sent.

   Input: HIF_Response[hif].byte has multibyte response code.
          This is called when HIF_Response[Channel].bit.CMD_RESPONSE == 1.
          hvp - host interface variables
          hif - Host channel.

   Returns: data to send.
   --------------------------------------------------------------- */

extern const BYTE Version[];

static WORD get_multibyte(HIF_VAR* hvp, BYTE hif)
{
   WORD data;


    switch (HIF_Response[hif].byte & maskKBD_RESPONSE_CODE)
    {
        case (respCMD_AC & maskKBD_RESPONSE_CODE):
            /* Process command "AC". */

            /* Bits 1 through 7 of Tmp_Byte[0] have address
               offset of data "packet" (initially 0). */
         //data = Version[hvp->Tmp_Byte[0]>>1];
         data = Version[hvp->Tmp_Byte[0]];
            hvp->Tmp_Byte[0]++;
            if (data == 0xAA) /* Keep going if not end of table. */
            {
                /* Otherwise, stop by clearing. */

                HIF_Response[hif].byte =
                    HIF_Response[hif].byte & ~maskKBD_RESPONSE_CODE;
                HIF_Response[hif].bit.CMD_RESPONSE = 0;
            }
            break;

        case (respARRAY & maskKBD_RESPONSE_CODE):
            /* Send data from an array.
               Tmp_Pntr has address of byte to send.
               Tmp_Load has number of bytes to send. */

            data = *hvp->Tmp_Pntr++;
            hvp->Tmp_Load--;
            if (hvp->Tmp_Load == 0)   /* Keep going if not end of array. */
            {
                /* Otherwise, stop by clearing. */

                HIF_Response[hif].byte =
                    HIF_Response[hif].byte & ~maskKBD_RESPONSE_CODE;
                HIF_Response[hif].bit.CMD_RESPONSE = 0;
            }
            break;

        default:
            /* Look for an OEM response code if not CORE. */

            data = Gen_Hookc_Get_Multibyte();

            break;
    }

    return(data);
}

/* ---------------------------------------------------------------
   Name: send_auxk_to_pc

   Send data that was received from the auxiliary keyboard to the
   Host.  The keyboard data could have come from the auxiliary
   device port or from the auxilary keyboard port.  The values
   passed allow the function to process keyboard data from
   either port.

   This function checks for errors in the reception as well, so it
   does not "blindly" send the data to the Host.  It will send a
   resend to the keyboard if necessary, and only after the data
   has been received with no errors will the data be sent to the
   Host (or an error code is sent to Host if unable to receive the
   data correctly from auxiliary keyboard).  On entry, the
   auxiliary keyboard is locked out so the data that was just
   received from it can be sent to the Host before there are
   any new data transmissions from the auxiliary keyboard.  After
   this function sends the data, the auxiliary keyboard is
   unlocked again.

   Input: aux_pntr - Address of device variable "block"
                    (Aux_Port1 or Aux_Port2).
          port - port that device is connected to:
                 AUX_PORT1_CODE or AUX_PORT2_CODE
   --------------------------------------------------------------- */
static void send_auxk_to_pc(AUX_PORT *aux_pntr, SMALL port)
{
   FLAG clear_state, clear_valid, resend, error;
   FLAG temp1_flag;
   WORD data, error_bits;

   clear_state = FALSE;
   clear_valid = FALSE;
   resend = FALSE;
   error = FALSE;

   if (Gen_Timer != 0)
   {
      /* Timeout error occurred. */

      if (Aux_Status.Send_Flag)
      {
         /* Timeout during send. */

         /* Byte to send to Host to indicate send error. */
         data = AUX_SEND_ERROR;

         /* Indicate send timeout in status register. */
         error_bits = maskSTATUS_PORT_SENDTMO;

         clear_valid = TRUE;
      }
      else if (aux_pntr->ack)
      {  /*
         Waiting for acknowledge.  Don't send resend request to
         the keyboard for timeout error while waiting for the
         acknowledge byte from the keyboard, just send error to Host. */

         /* Byte to send to Host to indicate send error. */
         data = AUX_SEND_ERROR;

         /* Indicate send timeout and general time out in status register. */
         error_bits = maskSTATUS_PORT_GENTMO+maskSTATUS_PORT_SENDTMO;

         clear_valid = TRUE;
      }
      else
      {  /*
         Timeout error occurred while receiving from keyboard.
         Send a request to the keyboard to resend the data. */

         aux_pntr->retry = 3; /* Set to retry 3 times. */
         resend = TRUE;
      }
   }
   else
   {
      /* No timeout error occured.  At this point, data has been
         received from the keyboard (as opposed to being sent to
         the keyboard).  Check parity of data received. */

      if (Parity_Is_Ok())
      {  /*
         No parity error.  No errors in data received
         from the keyboard.  Send data to Host.  Status
         register error bits will be cleared by "Data_To_Host". */

         aux_pntr->valid = 1;

         if (port == AUX_PORT2_CODE) // assuming port2 is for ext keyboard
         {
            temp1_flag = Gen_Info.bit.BREAK_AUXK;
            Gen_Info.bit.BREAK_AUXK = 0;

            /* Send data from auxiliary keyboard to Host. */
            if (send_ext_to_pc(Aux_Data.byte, temp1_flag))
            {
               Gen_Info.bit.BREAK_AUXK = 1;  /* Break prefix code. */
            }
         }
         else
         {
            temp1_flag = Gen_Info.bit.BREAK_AUXD;
            Gen_Info.bit.BREAK_AUXD = 0;

            /* Send data from auxiliary keyboard to Host. */
            if (send_ext_to_pc(Aux_Data.byte, temp1_flag))
            {
               Gen_Info.bit.BREAK_AUXD = 1;  /* Break prefix code. */
            }
         }

         temp1_flag = aux_pntr->delay; /* Get delayed data flag. */

         /* Clear transmit/receive state. */
         aux_pntr->retry = 0;
         aux_pntr->delay = 0;
         aux_pntr->ack = 0;

         Kbd_Response.byte = 0;  /* Clear response. */

         if (temp1_flag)
         {
            /* The delayed data flag was set.  So "Tmp_Load" has
               data to send to auxiliary keyboard.  Send it now. */

            Aux_Cmd_Setup(aux_pntr, Hif_Var[HIF_KBC].Tmp_Load);
            Send_To_Aux(aux_pntr, port, Hif_Var[HIF_KBC].Tmp_Load);
            /* Won't return until send complete or error sending. */

            aux_pntr->ack = 1;     /* Send_To_Aux clears bit.ack. */

            aux_pntr->retry = 0;
            aux_pntr->delay = 0;
            Aux_Status.Send_Flag = 0;
         }
         else
         {
            clear_state = TRUE;
         }
      }
      else
      {
         /* Parity error in data received from the keyboard. */

         if (aux_pntr->ack)
         {  /*
            Was expecting an acknowledge from the keyboard.
            Don't send resend request to the keyboard for parity
            error while waiting for the acknowledge byte from the
            keyboard, just send error to Host. */

            /* Byte to send to Host to indicate receive error. */
            data = AUX_RECEIVE_ERROR;

            /* Indicate parity error, send timeout, and
               general time out in status register. */
            error_bits = maskSTATUS_PORT_PARITY  +
                         maskSTATUS_PORT_SENDTMO +
                         maskSTATUS_PORT_GENTMO;

            error = TRUE;
         }
         else
         {  /*
            Parity error occurred while receiving from keyboard.
            Send a request to the keyboard to resend the data. */

            aux_pntr->retry = 3; /* Set to retry 3 times. */
            resend = TRUE;
         }
      }
   }

   while(resend)
   {
      /* Send the resend command to keyboard
         if error receiving data from keyboard. */

      if (aux_pntr->retry != 0)
      {
         aux_pntr->retry--;   /* Decrement retry count. */

         /* Send resend command to aux keyboard. */
         if (!Send_To_Aux(aux_pntr, port, KBD_CMD_RESEND))
         {
            /* Transmit is ok, wait for response. */
            resend = FALSE;
            clear_state = TRUE;
         }
      }
      else
      {
         /* Sent the resend command enough times. */

         /* Byte to send to Host to indicate receive error. */
         data = AUX_RECEIVE_ERROR;

         /* Indicate general time out in status register. */
         error_bits = maskSTATUS_PORT_GENTMO;

         clear_valid = TRUE;
         resend = FALSE;
      }
   }

   if (clear_valid)
   {
      /* data       = byte to send to Host to indicate error.
         error_bits = bits for status register to indicate error. */

      aux_pntr->valid = 0;

      if (Check_Transmit(port))
      {  /*
         At this point, the Ext_Cb0.bit.AUTO_SWITCH flag
         is not set.  The autoswitch feature is not selected.
         Send the error to the Host. */

         error = TRUE;
      }
      else
      {  /*
         Otherwise, let the internal keyboard
         handle communications to the Host. */

         clear_state = TRUE;
      }
   }

   if (error)
   {
      Kbd_Response.byte = 0;     /* Clear old response. */
      Error_To_Host(data, error_bits);
      clear_state = TRUE;
   }

   if (clear_state)
   {
      aux_pntr->ack = 0;
      aux_pntr->retry = 0;
      aux_pntr->delay = 0;
      Aux_Status.Send_Flag = 0;
   }

   Aux_Status.Bit_Count = 15;    /* Indicate ready to receive. */
}

/* ---------------------------------------------------------------
   Name: send_auxd_to_pc

   Send data that was received from the mouse to the Host.

   Input: aux_pntr - Address of device variable "block".
                    (Aux_Port1, Aux_Port2, or Aux_Port3).
          port - port that device is connected to:
                 AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE
   --------------------------------------------------------------- */
static void send_auxd_to_pc(AUX_PORT *aux_pntr, SMALL port)
{
   BYTE data, error_bits;
   FLAG clear_valid = FALSE;
   FLAG error = FALSE;

   MULPX.byte &= ~(0x03);
   MULPX.byte |= port;

   if (Gen_Timer != 0)
   {
      /* Timeout error occurred. */

      if (Aux_Status.Send_Flag)
      {
         /* Timeout during send. */

         /* Byte to send to Host to indicate send error. */
         data = AUX_SEND_ERROR;

         /* Indicate send timeout in status register. */
         error_bits = maskSTATUS_PORT_SENDTMO;

         aux_pntr->valid = 1;
         clear_valid = TRUE;
      }
      else if (aux_pntr->ack)
      {
         /* Timeout while waiting for acknowledge byte from mouse
            after sending it something.  Don't send resend command
            to mouse for this error, just send error to Host. */

         /* Byte to send to Host to indicate send error. */
         data = AUX_SEND_ERROR;

         /* Indicate send timeout and general time out in status register. */
         error_bits = maskSTATUS_PORT_GENTMO+maskSTATUS_PORT_SENDTMO;

         aux_pntr->valid = 1;
         clear_valid = TRUE;
      }
      else
      {  /*
         Timeout during receive.  For mouse device, never send a
         resend command to mouse, because the mouse resends the
         entire packet back which gets the mouse driver out of synch. */

         /* Byte to send to Host to indicate receive error. */
         data = AUX_RECEIVE_ERROR;

         /* Indicate general time out in status register. */
         error_bits = maskSTATUS_PORT_GENTMO;

         clear_valid = TRUE;
      }
   }
   else
   {
      /* No timeout errors occured, check parity of data received. */

      if (Parity_Is_Ok())
      {
         /* No parity error.  No errors in data
            transmission.  Send data to Host.  Status
            register error bits will be cleared by "Data_To_Host". */

         aux_pntr->valid = 1;
         Aux_Data_To_Host(Aux_Data.byte);
      }
      else
      {  /*
         Parity error.  For mouse device, never send a resend
         command to mouse, because the mouse resends the entire
         packet back which gets the mouse driver out of synch. */

         /* Indicate parity error and general
            time out in status register. */
         error_bits = maskSTATUS_PORT_PARITY  +
                      maskSTATUS_PORT_GENTMO;

         if (aux_pntr->ack)
         {
            /* Waiting for acknowledge.
               Byte to send to Host to indicate receive error. */
            data = AUX_RECEIVE_ERROR;
         }
         else
         {
            /* Not waiting for acknowledge, send null byte on a
               parity error with both parity error and timeout
               error bits set in the status register. */
            data = 0;
         }

         error = TRUE;
      }
   }

   if (clear_valid)
   {
      if (Check_Transmit(port)) error = TRUE;
      aux_pntr->valid = 0;
   }

   if (error)
   {
      #if MOUSE_EMULATION
      Aux_Response = 0;
      Aux_Tmp_Load1 = 0;
      #endif // MOUSE_EMULATION
      Aux_Error_To_Host(data, error_bits);
   }

   aux_pntr->ack = 0;
   aux_pntr->retry = 0;
   aux_pntr->delay = 0;
   Aux_Status.Send_Flag = 0;

   Aux_Status.Bit_Count = 15; /* Indicate ready to receive. */
}

#if EXT_HOTKEY
/* ---------------------------------------------------------------

   Variable for external keyboard hotkey support.

   --------------------------------------------------------------- */

BYTE save_hotkey;

/* ---------------------------------------------------------------
   Name: init_ext_hotkeys

   Initialization for external keyboard hotkeys.
   --------------------------------------------------------------- */
static void init_ext_hotkeys(void)
{
   save_hotkey = 0;
   Flag.ALT_PRESSED = 0;
   Flag.CTRL_PRESSED = 0;
}
#endif // EXT_HOTKEY
/* ---------------------------------------------------------------
   Name: send_ext_to_pc, send_to_pc

   send_ext_to_pc is just a 'pre-entry' to the original send_to_pc
   routine.  It's sole purpose is to check auxiliary keyboard entries
   for external hotkey functions.  This allows it to set flags for
   CTRL and ALT states, then if BOTH are set, hotkeys can be checked.

   Send data to Host.  If password is enabled, the data
   will not be sent until the password is entered correctly.

   Input: data - Data to send to PC.
          break_prefix_flag - TRUE if last scan code was a break prefix.

   Returns: TRUE if translation mode is enabled and
            scan code was a break prefix.
   --------------------------------------------------------------- */

static FLAG send_ext_to_pc(WORD data, FLAG break_prefix_flag)
{
   #if EXT_HOTKEY
   SMALL num_hotkeys;
   num_hotkeys = *Ext_Hotkey_Pntr & 0x0F;
   return(common_send_to_pc(data, break_prefix_flag, num_hotkeys));
   #else // EXT_HOTKEY
   return(common_send_to_pc(data, break_prefix_flag));
   #endif // EXT_HOTKEY
}

static FLAG send_to_pc(WORD data, FLAG break_prefix_flag)
{
   #if EXT_HOTKEY
   return(common_send_to_pc(data, break_prefix_flag, 0));
   #else //EXT_HOTKEY
   return(common_send_to_pc(data, break_prefix_flag));
   #endif   // EXT_HOTKEY
}

#if EXT_HOTKEY
static FLAG common_send_to_pc(WORD data, FLAG break_prefix_flag, SMALL num_ext_hotkeys)
#else //EXT_HOTKEY
static FLAG common_send_to_pc(WORD data, FLAG break_prefix_flag)
#endif // EXT_HOTKEY
{
   /* The num_ext_hotkeys is non-zero if the data is from an external
      keyboard and there are hotkeys defined.  This value is cleared
      in order to stop a check for the external keyboard hotkeys.  For
      example, if the password is enabled, the num_ext_hotkeys variable
      will be cleared regardless of its state on entry to this routine.

      The hotkey check is made if the data is from the external
      keyboard, the system is in translate mode, the data is not the
      break code prefix, and the password is not enabled. */

   SMALL event;
   FLAG odd;
   BYTE data_byte;
   BYTE const *pntr;
   WORD cntrl_funct;
   FLAG send_it = FALSE;

   if (Ccb42.bit.XLATE_PC == 0)
   {
      /* Send data as is. */
      send_it = TRUE;
      break_prefix_flag = FALSE;
      #if EXT_HOTKEY
      num_ext_hotkeys = 0;
      #endif   // EXT_HOTKEY
   }
   else
   {
      /* Translation mode is enabled. */

      data = translate_to_pc(data, break_prefix_flag);
      if (data == (WORD) ~0)
      {
         /* Don't send break code prefix. */
         break_prefix_flag = TRUE;
         #if EXT_HOTKEY
         num_ext_hotkeys = 0;
         #endif // EXT_HOTKEY
      }
      else
      {
         break_prefix_flag = FALSE;

         #if EXT_HOTKEY
         if (num_ext_hotkeys != 0)
         {
            /* Get state of external keyboard's CTRL and ALT keys. */

            data_byte = (BYTE) (data & 0x00FF);

            if (data_byte == 0x1D)
            {
               /* CTRL key, MAKE or REPEAT, key pressed or repeated. */
               Flag.CTRL_PRESSED = 1;
            }
            else if (data_byte == (0x80 | 0x1D))
            {
               /* CTRL key, BREAK, key released. */
               Flag.CTRL_PRESSED = 0;
               save_hotkey = 0;
            }
            else if (data_byte == 0x38)
            {
               /* ALT key, MAKE or REPEAT, key pressed or repeated. */
               Flag.ALT_PRESSED = 1;
            }
            else if (data_byte == (0x80 | 0x38))
            {
               /* ALT key, BREAK, key released. */
               Flag.ALT_PRESSED = 0;
               save_hotkey = 0;
            }

            if (Flag.CTRL_PRESSED && Flag.ALT_PRESSED)
            {
               /* CTRL and ALT keys are pressed. */
               ;
            }
            else
            {
               num_ext_hotkeys = 0;
            }
         }
         #endif // EXT_HOTKEY
         if (Gen_Hookc_Key_Enable(data) == 0)
         {
            /* Don't send this data. */
            #if EXT_HOTKEY
            num_ext_hotkeys = 0;
            #else //EXT_HOTKEY
            ;
            #endif  // EXT_HOTKEY
         }
         else if (Read_Inhibit_Switch_Bit())
         {
            /* Keyboard is not inhibited. */
            send_it = TRUE;
         }
         else if (Gen_Info.bit.PASS_ENABLE == 0)
         {
            /* Password is not enabled. */
            send_it = TRUE;
         }
         else
         {
            /* Check Password stuff. */

            #if EXT_HOTKEY
            num_ext_hotkeys = 0;
            #endif  // EXT_HOTKEY

            if ((data & 0x80) != 0)
            {
               /* This is break scan code. */
               ;
            }
            else if (data == Pass_Make1)
            {
               /* Reject make code 1. */
               ;
            }
            else if (data == Pass_Make2)
            {
               /* Reject make code 2. */
               ;
            }
            else
            {
               if ( (Pass_Buff_Idx > 0) && (data != Pass_Buff[Pass_Buff_Idx]) )
               {  /*
                  If the current keypress does not match the
                  current password byte, maybe the keypress is the
                  start of the password.  (This could happen if the
                  password is 123 and the user presses 1123.) Set
                  the index to the beginning to allow another check. */
                  Pass_Buff_Idx = 0;
               }

               if (data == Pass_Buff[Pass_Buff_Idx])
               {
                  Pass_Buff_Idx++;

                  if ( (Pass_Buff_Idx == PASS_SIZE) ||
                       (Pass_Buff[Pass_Buff_Idx] == 0) )
                  {
                     /* If the index is equal to the password buffer size,
                        or if the next byte in the password buffer is a null,
                        all the password bytes have been checked. */

                     Pass_Buff_Idx = 0;            /* Reset index to zero. */
                     Gen_Info.bit.PASS_ENABLE = 0; /* De-activate password. */
                     Write_Inhibit_Switch_Bit(0);  /* Re-activate system. */

                     if (Pass_Off != 0)
                     {
                        /* Password disable send code. */
                        data = Pass_Off;
                        send_it = TRUE;
                     }
                  }
               }
               else
               {
                  /* Password failure.  Reset index to zero. */
                  Pass_Buff_Idx = 0;
               }
            }
         }
      }
   }

   #if EXT_HOTKEY
   if (num_ext_hotkeys != 0)
   {
      /* The CTRL and ALT keys on an external keyboard are pressed.
         Check for an external keyboard hotkey. */

      if (data_byte & 0x80)
      {
         /* BREAK, key released. */
         event = BREAK_EVENT;
      }
      else if (data_byte == save_hotkey)
      {
         /* REPEAT, key repeated. */
         event = REPEAT_EVENT;
      }
      else
      {
         /* MAKE, key pressed. */
         event = MAKE_EVENT;
      }

      save_hotkey = data_byte;

      /*
      The external keyboard hotkey table looks like:

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

      The WORDs hold the System Control Function with the high byte
      of the function in the lower address and the low byte of the
      function in the higher address.  The Ext_Hotkey_Pntr points
      to the byte holding the number of hotkeys. */

      odd = 1;
      pntr = Ext_Hotkey_Pntr;
      pntr -= 3;

      data_byte = data_byte & 0x7F;

      do
      {
         if (*pntr == data_byte)
         {
            /* Hotkey was found in the external keyboard hotkey table.
               The hotkey function may or may not be performed.  Whether
               or not the function will be performed, it is a hotkey and
               the code should not be sent to the host. */
            send_it = FALSE;

            if (odd)
            {
               /* For the odd numbered hotkeys. */
               cntrl_funct = ((WORD) *(pntr+1) << 8) | *(pntr+2);
            }
            else
            {
               /* For the even numbered hotkeys. */
               cntrl_funct = ((WORD) *(pntr-2) << 8) | *(pntr-1);
            }

            System_Control_Function(cntrl_funct, event);

            num_ext_hotkeys = 0; /* Stop the loop. */
         }
         else
         {
            if (odd)
            {
               /* Point to even hotkey number. */
               pntr--;
            }
            else
            {
               /* Point to odd hotkey number. */
               pntr -= 5;
            }

            odd = !odd;

            num_ext_hotkeys--;
            if (num_ext_hotkeys == 0)
            {
               if (data_byte == 0x53)
               {
                  /* DEL key was pressed.  System will be reset. */
                  init_ext_hotkeys();
               }
            }
         }
      }
      while(num_ext_hotkeys);
   }
   #endif   // EXT_HOTKEY

   if (send_it) {Data_To_Host(data);}

   return(break_prefix_flag);
}

/* ---------------------------------------------------------------
   Name: handle_unlock

   Unlock devices to allow new aux keyboard/device transmissions,
   and re-enable Host interrupts so data/commands can be received
   from Host.  On entry ALL devices must be locked.  Priority will
   be given to scanner transmissions.

   Also, start scan transmission for local keyboard responses
   (or multibyte command responses) if "Kbd_Response.byte"
   is non-zero.

   Input: On entry all devices must be locked.
   --------------------------------------------------------------- */
static void handle_unlock(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif
   FLAG unl_aux_1, unl_aux_2, unl_aux_3;
   FLAG unl_aux, unl_scan;
   SMALL port;
   AUX_PORT *aux_pntr;

   Disable_Irq();                /* No interrupts! */
   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   aux_pntr = Make_Aux_Pntr(Int_Var.Active_Aux_Port);
   #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED

   if (Output_Buffer_Full())
   {  /*
      Output buffer is full.  The Host has not taken the last
      byte yet.  Delay unlocking auxiliary devices until Host
      has taken the last byte.  Set a timer to check again later. */

      Load_Timer_B();
      Timer_B.bit.UNLOCK_TMO = 1;
   }
   #if MOUSE_EMULATION
   else if (Aux_Tmp_Load1 && Aux_Response) {
      Disable_Irq();                /* No interrupts! */
      Timer_B.bit.SEND_DELAY = RESPONSE_TIME;
      Load_Timer_B();
      Timer_B.bit.SEND_ENABLE = 1;
      Enable_Irq();
   }
   #endif // MOUSE_EMULATION
   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   else if ((Int_Mouse.field.Byte_Count == 0x03) && !aux_pntr->intm)
   {
      if (Mouse_State[0] & ENABLED)
      {
         #if AUX_PORT3_SUPPORTED
         if (!Aux_Port3.kbd && !Aux_Port3.intm)
            Int_Var.Active_Aux_Port = AUX_PORT3_CODE;
         #if AUX_PORT2_SUPPORTED
         else if (!Aux_Port2.kbd && !Aux_Port2.intm)
            Int_Var.Active_Aux_Port = AUX_PORT2_CODE;
         #endif
         #elif AUX_PORT2_SUPPORTED
         if (!Aux_Port2.kbd && !Aux_Port2.intm)
            Int_Var.Active_Aux_Port = AUX_PORT2_CODE;
         #endif
         if (!Aux_Port1.kbd && !Aux_Port1.intm)
            Int_Var.Active_Aux_Port = AUX_PORT1_CODE;
         Aux_Status.Parity_Flag = 0;
         Gen_Timer = 0;
         Aux_Data.byte = 0;
         Disable_Irq();
         Service.bit.AUX_PORT_SND = 1;   // Post service request.
         Enable_Irq();
      }
      else
         Int_Mouse.field.Byte_Count = 0; // Reset the byte count.
   }
   #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   else                          /* Unlock devices. */
   {

      Gen_Hookc_Unlock();

      unl_aux = FALSE;
      unl_scan = FALSE;

      /* Signal that no Auxiliary Ports are active.
         (This may be changed further on.) */
      /* <<< V1.0 2001/5/31 Modified don't clear if AUX_PORT_SND is set. */
      if (!Service.bit.AUX_PORT_SND)
         Int_Var.Active_Aux_Port = 0;
      /* >>> V1.0 2001/5/31 Modified.*/

      if (!Ext_Cb0.bit.ITP_DISABLED || Expect_Count)
         unl_aux_1 = TRUE;
      else
         unl_aux_1 = FALSE;

      unl_aux_2 = TRUE;
      unl_aux_3 = TRUE;

      if (Aux_Port1.ack)
      {  /*
         Waiting for an ACK from auxiliary keyboard or
         auxiliary device (mouse) on Auxiliary Port 1. */

         unl_aux_2 = FALSE;
         unl_aux_3 = FALSE;

         Int_Var.Active_Aux_Port = AUX_PORT1_CODE;
         Enable_Gentmo(ACK_TMO);
         unl_aux = TRUE;
      }

      #if AUX_PORT2_SUPPORTED
      else if (Aux_Port2.ack)
      {  /*
         Waiting for an ACK from auxiliary keyboard or
         auxiliary device (mouse) on Auxiliary Port 2. */

         unl_aux_1 = FALSE;
         unl_aux_3 = FALSE;

         Int_Var.Active_Aux_Port = AUX_PORT2_CODE;
         Enable_Gentmo(ACK_TMO);
         unl_aux = TRUE;
      }
      #endif

      #if AUX_PORT3_SUPPORTED
      else if (Aux_Port3.ack)
      {  /*
         Waiting for an ACK from auxiliary
         device (mouse) on Auxiliary Port 3. */

         unl_aux_1 = FALSE;
         unl_aux_2 = FALSE;
         Int_Var.Active_Aux_Port = AUX_PORT3_CODE;
         Enable_Gentmo(ACK_TMO);
         unl_aux = TRUE;
      }
      #endif
      else if (
                #if AUX_PORT2_SUPPORTED
                #if AUX_PORT3_SUPPORTED
                Aux_Port3.delay ||
                #endif
                (Aux_Port2.delay)
                #elif AUX_PORT3_SUPPORTED
                (Aux_Port3.delay)
                #else
                0 //Aux_Port1.delay
                #endif
                )
      {  /*
         (Aux_Port3 is not checked.  The delay bit is used by the
          keyboard.  Aux_Port3 can only be connected to a mouse.)

         There is data that is waiting to be sent to auxiliary
         keyboard.  Don't unlock the internal keyboard (scanner). */

         unl_aux = TRUE;
      }
      else if (Kbd_Response.bit.CMD_RESPONSE)
      {  /*
         There is a pending multibyte controller command
         response.  (These are handled through scanner logic. */

         unl_scan = TRUE;
      }
      else if (Kbd_Response.byte)
      {
         #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         /* There is a response stored in Kbd_Response byte.
            Check if the keyboard is handling it. */

         /* Get the port number of the primary keyboard. */
         port = (Ps2_Ctrl.byte >> shiftPRI_KBD) & maskPRI_KBD;

         aux_pntr = Make_Aux_Pntr(port);

         if ( (aux_pntr->kbd) && Check_Transmit(port) )
         {
            unl_aux = TRUE;
         }
         else
         {  /*
            There is no pending multibyte controller command response.
            There is a response stored in Kbd_Response byte.
            There is no keyboard on the primary keyboard port or
            an error occurred on the primary keyboard port. */

            unl_scan = TRUE;
         }
         #else // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
         /*
         There is no pending multibyte controller command response.
         There is a response stored in Kbd_Response byte.
         There is no PS/2 keyboard supported. */
         unl_scan = TRUE;
         #endif // AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
      }
      else
      {
         /* There is no data that is waiting
            to be sent to auxiliary keyboard and
            there is no response stored in Kbd_Response byte. */

         Unlock_Scan();

         if (Check_Scan_Transmission())
            unl_scan = TRUE; /* (Leave aux devices locked out.) */
         else
            unl_aux = TRUE;
      }

      if (unl_scan)
      {
         /* Leave auxiliary devices locked out. */
         Unlock_Scan();
         Start_Scan_Transmission();
      }

      if (unl_aux)
      {
         /* Wait for inhibit delay timer to expire.  This allows device
         to recognize that it has been inhibited from transmitting. */
         Enable_Irq();
         while(Timer_B.bit.INH_CLEAR) ;
         Disable_Irq();

         /*
         Release auxiliary device clock lines so the auxiliary
         devices can transmit data.  Re-enable auxiliary device
         IRQs.  Also initialize auxiliary variables. */

         Aux_Status.Bit_Count = 15;
         Aux_Status.Send_Flag = 0;  /* Setup to receive data from device. */

         /*
         If an ACK is expected from an auxiliary  device, only
         that device is setup to allow the device to transmit
         so that the ACK can be received from the device.

         Otherwise, all auxiliary devices are setup to allow
         the device to transmit (so that data can be received
         from the device).

         Enabling all devices allows Hotplug and Hotswap to work.
         For example, a mouse is on Port 1 and a keyboard is on
         Port 2.  The mouse is locked and the keyboard is unlocked.
         If the keyboard is swapped to the mouse port, it will not
         be detected as a Hotplug because the port is locked. */

         if (unl_aux_1 && (!Aux_Port1.disable || Aux_Port1.ack))
         {
            Aux_Port1.lock = 0;
            if (!Ccb42.bit.DISAB_AUX)
            {
               Setup_Aux_Port1_Receive();
            }
         }

         #if AUX_PORT2_SUPPORTED
         if (unl_aux_2 && (!Aux_Port2.disable || Aux_Port2.ack))
         {
            Aux_Port2.lock = 0;
            Setup_Aux_Port2_Receive();
         }
         #endif

         #if AUX_PORT3_SUPPORTED
         if (unl_aux_3 && (!Aux_Port3.disable || Aux_Port3.ack))
         {
            Aux_Port3.lock = 0;
            Setup_Aux_Port3_Receive();
         }
         #endif
      }
   }

   /* Enable Host interface now. */

   Enable_Host_IBF_Irq();  /* Re-enable PC interface interrupt. */
   Enable_Irq();
}

/* ---------------------------------------------------------------
   Name: translate_to_pc

   Translate scan code from "set 2 scan code" to "set 1 scan code".

   Input: data - scan code received from aux keyboard or local keyboard.
          break_prefix_flag - TRUE if last scan code was a break prefix.

   Returns: a value with all bits set if data is break prefix (0xF0);
            otherwise, returns translated key.
   --------------------------------------------------------------- */
static WORD translate_to_pc(WORD data, FLAG break_prefix_flag)
{
   /* Scan code set 2 to scan code set 1 translation table.  First
      byte is a dummy entry because scan code "0" is not translated. */

   static const BYTE scan2_table[] =
   {
      0x00, 0x43, 0x41, 0x3F, 0x3D, 0x3B, 0x3C, 0x58,
      0x64, 0x44, 0x42, 0x40, 0x3E, 0x0F, 0x29, 0x59,
      0x65, 0x38, 0x2A, 0x70, 0x1D, 0x10, 0x02, 0x5A,
      0x66, 0x71, 0x2C, 0x1F, 0x1E, 0x11, 0x03, 0x5B,
      0x67, 0x2E, 0x2D, 0x20, 0x12, 0x05, 0x04, 0x5C,
      0x68, 0x39, 0x2F, 0x21, 0x14, 0x13, 0x06, 0x5D,
      0x69, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x5E,
      0x6A, 0x72, 0x32, 0x24, 0x16, 0x08, 0x09, 0x5F,
      0x6B, 0x33, 0x25, 0x17, 0x18, 0x0B, 0x0A, 0x60,
      0x6C, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0C, 0x61,
      0x6D, 0x73, 0x28, 0x74, 0x1A, 0x0D, 0x62, 0x6E,
      0x3A, 0x36, 0x1C, 0x1B, 0x75, 0x2B, 0x63, 0x76,
      0x55, 0x56, 0x77, 0x78, 0x79, 0x7A, 0x0E, 0x7B,
      0x7C, 0x4F, 0x7D, 0x4B, 0x47, 0x7E, 0x7F, 0x6F,
      0x52, 0x53, 0x50, 0x4C, 0x4D, 0x48, 0x01, 0x45,
      0x57, 0x4E, 0x51, 0x4A, 0x37, 0x49, 0x46, 0x54
   };

   FLAG check_break_bit = FALSE;

   if (data == 0xF0)
   {
      /* Signify that break code prefix was encountered. */
      data = (WORD) ~0;
   }
   else if (data == 0)
   {
      data = 0xFF;      /* Key detection error/overrun. */
   }
   else if ((data & 0x80) == 0)
   {
      /* Translate codes 01 thru 7F. */

      /* The variable "data" has scan code (set 2) to translate.
         Set "data" to the translated (to set 1) scan code. */

      data = scan2_table[data];
      check_break_bit = TRUE;
   }
   else if (data == 0x83)     /* ID code for 101/102 keys. */
   {
      data = 0x41;            /* Translate ID code. */
      check_break_bit = TRUE;
   }
   else if (data == 0x84)     /* ID code for 84 keys. */
   {
      data = 0x54;            /* Translate ID code. */
      check_break_bit = TRUE;
   }

   if (check_break_bit && break_prefix_flag)
   {
      /* Last code received by this routine was the
         break prefix.  This must be a break code.  Set
         high bit to indicate that this is a break code. */

      data |= 0x80;
   }

   return(data);
}


