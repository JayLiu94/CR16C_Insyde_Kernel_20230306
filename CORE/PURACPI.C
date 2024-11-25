/* ----------------------------------------------------------------------------
 * MODULE PURACPI.C
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
 * This file handles ACPI routines.
 * SCI event generation.
 * ACPI SMBus transfer initiated by ACPI command.
 *
 * Revision History (Started March 19, 1998 by GLP)
 *
 * 01 Apr 98   Changed acpi_chk_protocol to setup the count of bytes in
 *             SMB_ACPI[SMB_BCNT_INDX] for a PRTCL_READ_BLOCK instead of a
 *             PRTCL_WRITE_BLOCK. Added code to clear the OBF flag in
 *             SMB_Status at the end of a transfer.
 * 14 Apr 98   Command 81h in ACPI_Dat was setting up for an SMBus transaction
 *             before receiving the data. Added Flag.ACPI_IN_PROGRESS handling.
 *             Removed Disable_Irq() and Enable_Irq() when updating
 *             SMB_ACPI[SMB_STS_INDX] because it is not modified by interrupt.
 *             Added alarm handling to acpi_idle.
 * 12 May 98   Changed Command 84h (Query Embedded Controller) to return values
 *             for PPK SCIs.
 * 15 May 98   Added initialization of SMB_ACPI[SMB_PRTCL_INDX] and
 *             SMB_ACPI[SMB_STS_INDX] in ACPI_Init.  Added handling for
 *             EC_Space[] indices SMB_DNE_CAUSE_INDX, SMB_ALRM_CAUSE_INDX,
 *             ACPI_TMR_CAUSE_INDX, ACPI_TMR_LO_INDX, and ACPI_TMR_HI_INDX.
 * 04 Jun 98   Added code to handle SMB_DEV_DENIED and SMB_CMD_DENIED
 *             in SMB_Status[].
 * 15 Jun 98   Changed ACPI to use SMBus level 0 and to wait until SMBus level
 *             0 buffer is not in use. Added call to Free_Smb_Level_0() at the
 *             end of a transfer.  Shadowed selector.  Changed acpi_idle to
 *             allow a call to acpi_start_protocol when in Legacy mode.
 *             Moved code that checks if protocol register was written to from
 *             ACPI_Dat case0x81 to Write_EC_Space.  Corrected read/write bit
 *             handling for PRTCL_READ_BYTE, PRTCL_READ_WORD, and PRTCL_READ_BLOCK.
 * 30 Sep 98   Updated for new Keyboard Controller functionality.
 * 19 Oct 98   Changed Read_EC_Space and Write_EC_Space because
 *             of addition of ACPI EC space overlapping Config
 *             and to allow access to registers.
 * 18 Aug 99   Updated copyright.
 *
 *             Changed acpi_start_protocol and acpi_chk_busy.  The
 *             acpi_start_protocol routine was loading the SMB_Buffer
 *             for level 0 before checking if level 0 was free.  The
 *             acpi_chk_busy routine was being called after filling the
 *             SMB_Buffer but another process may have been using it so
 *             the check was being done too late.
 *
 *             Changed PRTCL_WRITE_BLOCK case in acpi_chk_busy
 *             because the source_pntr was being used before
 *             initialization for storing command byte.
 * 26 Aug 99   Corrected error in PRTCL_READ_BYTE.  The protocol
 *             being used was SMB_WRITE_BYTE it should have been
 *             SMB_READ_BYTE.
 * EDH
 * 29 Sep 00   Corrected error in ACPI read/write from index 0x00-0x5F.
 * 13 May 04   Implement SMBus 1.1 and 2.0 for PEC and Write Block process call.
 * ------------------------------------------------------------------------- */

#define PURACPI_C

#include "swtchs.h"

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

#if !ACPI_SUPPORTED
/* If this file is not being used, there is no need to take
   up code or RAM space.  But, ANSI C does not allow an empty
   source file.  The following is an attempt at keeping code
   size small while allowing the ACPI code to be added easily. */
static void nothing(void);
static void nothing(void)
{
   ;
}
#else

#include "defs.h"
#include "types.h"
#include "puracpi.h"
#include "purdat.h"
#include "com_defs.h"
#include "host_if.h"
#include "oem.h"
#include "proc.h"
#include "purmain.h"
#include "purhook.h"
#include "i2c.h"
#include "staticfg.h"
#include "timers.h"
#include "dev.h"
#include "smb_tran.h"

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void acpi_idle(void);
static void acpi_start_protocol(void);
static void acpi_chk_busy(void);
static void acpi_chk_protocol(void);

/* First EC space address used for hardware register access.
   This accesses Config table offset 0x200. */
#define FIRST_REG_EC_ADDRESS 0xC0

BYTE ACPI_Timer;
BYTE acpi_selector;

FUNCT_PTR_V_V acpi_service_pntr;

/* The ACPI commands use SMBus level 0. */

/* ---------------------------------------------------------------
   Name: ACPI_Init

   Initialization called on reset.
   --------------------------------------------------------------- */
void ACPI_Init(void)
{
   acpi_service_pntr = acpi_idle;
   ACPI_Timer = 1;

   SMB_ACPI[SMB_PRTCL_INDX] = 0; /* ACPI SMBus protocol register. */
   SMB_ACPI[SMB_STS_INDX] = 0;   /* ACPI SMBus status register. */

   /* Initialize cause value to return for SMBus transfer done. */
   EC_Space[SMB_DNE_CAUSE_INDX] = SMB_PROTOCOL_DONE;

   /* Initialize cause value to return for SMBus alarm received. */
   EC_Space[SMB_ALRM_CAUSE_INDX] = SMB_PROTOCOL_DONE;

   /* Initialize cause value to return for EC_Space Timer timed out. */
   EC_Space[ACPI_TMR_CAUSE_INDX] = ACPI_TMR_DONE;

   /* Stop the EC_Space Timer. */
   EC_Space[ACPI_TMR_HI_INDX] = 0x80;

   acpi_selector = 0xFF;
}

/* ---------------------------------------------------------------
   Name: Handle_ACPI

   Service ACPI request.  This is called at a 1 mS interval.
   --------------------------------------------------------------- */
void Handle_ACPI(void)
{
   if ((EC_Space[ACPI_TMR_HI_INDX] & 0x80) == 0)
   {
      /* The EC_Space timer is enabled. */

      EC_Space[ACPI_TMR_LO_INDX]--;

      if (EC_Space[ACPI_TMR_LO_INDX] == 0xFF)
         EC_Space[ACPI_TMR_HI_INDX]--;

      if ( (EC_Space[ACPI_TMR_HI_INDX] == 0) &&
           (EC_Space[ACPI_TMR_LO_INDX] == 0) )
      {
         /* Set cause flag for this event and start ACPI_Timer. */
         Set_SCI_Cause(ACPI_TMR_DONE);

         /* Stop EC_Space timer. */
         EC_Space[ACPI_TMR_HI_INDX] = 0x80;
      }
   }

   if (ACPI_Timer)
   {
      /* When the ACPI Timer is non-zero, it is running.
         When it counts down to zero, an ACPI service routine
         is run.  The service routine is the routine that
         is pointed to by the acpi_service_pntr variable.

         If an ACPI routine needs service quickly, it sets the
         ACPI Timer to 1.  Service will be handled within 1
         millisecond.  If an ACPI routine needs service at a
         later timer, the ACPI Timer can be set to a larger value.  */

      ACPI_Timer--;
      if (ACPI_Timer == 0) (acpi_service_pntr)();
   }
}

/* ---------------------------------------------------------------
   Name: ACPI_Cmd

   At this point, Hif_Var[HIF_PM1].Cmd_Byte == 0, Hif_Var[HIF_PM1].Tmp_Load == 0.

   Handle ACPI commands.
   --------------------------------------------------------------- */
WORD ACPI_Cmd(BYTE command_num)
{
   WORD rval;
   SMALL index;
   FLAG done;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0x80:                 /* Read Embedded Controller. */

         Flag.ACPI_IN_PROGRESS = 1;

         /* Generate interrupt to signal the Host that the
            command has been taken and more data can be sent. */
         ACPI_Gen_Int();

         Hif_Var[HIF_PM1].Cmd_Byte = command_num; /* Signal to wait for data from Host. */
         break;

      case 0x81:                 /* Write Embedded Controller. */

         Flag.ACPI_IN_PROGRESS = 1;

         /* Generate interrupt to signal the Host that the
            command has been taken and more data can be sent. */
         ACPI_Gen_Int();

                                 /* Tmp_Load will be used as a byte counter. */
         Hif_Var[HIF_PM1].Cmd_Byte = command_num; /* Signal to wait for data from Host. */
         break;

      case 0x82:                 /* Burst Enable Embedded Controller. */

         /* Set Burst bit in secondary Host interface status register. */
         Write_Host_Status_Reg2(Read_Host_Status_Reg2() |
                                maskSTATUS_PORT2_BURST);

         rval = 0x90;   /* Get Burst Acknowledge byte to send to Host. */

         /*
         The controller is in burst mode.
         It should be waiting for instructions from the Host.
         It may exit burst mode when the Burst Disable command
         is received, to process a critical event, or when a
         timeout is detected.  The timeout settings are:
         First Access         - 400 microseconds.
         Subsequent Accesses  - 50 microseconds each.
         Total Burst Time     - 1 millisecond.
         If the burst mode is exited for any reason other
         than the Burst Disable command, the controller
         should generate an SCI. */

         break;

      case 0x83:                 /* Burst Disable Embedded Controller. */

         /* Generate interrupt to signal the Host
            that the command has been taken. */
         ACPI_Gen_Int();

         /* Clear Burst bit in secondary Host interface status register. */
         Write_Host_Status_Reg2(Read_Host_Status_Reg2() &
                                ~maskSTATUS_PORT2_BURST);
         break;

      case 0x84:                 /* Query Embedded Controller. */

         /* The Host has issued this command because the SCI_EVT bit
            in the secondary Host interface status register is set. */

         /* Test the cause array.
            Bit 0 of first  byte is cause value 1,
            Bit 1 of first  byte is cause value 2,
                .
                .
                .
            Bit 7 of first  byte is cause value 8,
            Bit 0 of second byte is cause value 9,
            etc.

            SCIs can be generated with certain commands and
            downloaded hotkeys.  The cause values for these
            are held in SCI_Event_Buffer.

            The SMB_PROTOCOL_DONE and SMB_ALRM_RECEIVED cause
            values are taken from EC_Space[SMB_DNE_CAUSE_INDX] and
            EC_Space[SMB_ALRM_CAUSE_INDX], respectively.  Both of
            these variables are initialized to SMB_PROTOCOL_DONE. */

         /* Initialize cause number to zero.  If there are
            no events pending, return a zero to the Host. */
         rval = 0;

         index = 0;
         done = FALSE;
         do
         {
            if (Cause_Flags[index] != 0)
            {
               /* There is at least one event pending. */

               Hif_Var[HIF_PM1].Tmp_Load = Cause_Flags[index];

               while ( (Hif_Var[HIF_PM1].Tmp_Load & 1) == 0 )
               {
                  Hif_Var[HIF_PM1].Tmp_Load = Hif_Var[HIF_PM1].Tmp_Load >> 1;
                  rval++;
               }

               /* rval has the bit number of the event. */

               /* Clear the cause bit. */
               Cause_Flags[index] &= Cpl_Byte_Mask((SMALL) rval);

               /* Convert bit number to cause number. */
               rval += (index * 8) + 1;

               if (rval == SMB_PROTOCOL_DONE)
               {
                  /* For SMBus transaction done, return
                     the cause value stored in EC_Space. */
                  rval = EC_Space[SMB_DNE_CAUSE_INDX];
               }
               else if (rval == SMB_ALRM_RECEIVED)
               {
                  /* For SMBus alarm received, return
                     the cause value stored in EC_Space. */
                  rval = EC_Space[SMB_ALRM_CAUSE_INDX];
               }
               else if (rval == ACPI_TMR_DONE)
               {
                  /* For EC_Space Timer timed out, return
                     the cause value stored in EC_Space. */
                  rval = EC_Space[ACPI_TMR_CAUSE_INDX];
               }

               done = TRUE;
            }
            else
            {
               index++;
               if (index == NUM_SCI_FLAGS_BYTES) done = TRUE;
            }
         }
         while(!done);

         if (rval == 0)
         {
            /* Check buffered SCIs. */

            if (SCI_Event_Out_Index != SCI_Event_In_Index)
            {
               /* There is at least one event pending. */

               /* Get the SCI cause number. */
               rval = SCI_Event_Buffer[SCI_Event_Out_Index];

               /* Update the index. */
               SCI_Event_Out_Index++;
               if (SCI_Event_Out_Index >= EVENT_BUFFER_SIZE)
                  SCI_Event_Out_Index = 0;
            }
         }

         /* Clear the SCI_EVT bit. */
         Write_Host_Status_Reg2(Read_Host_Status_Reg2() &
                                ~maskSTATUS_PORT2_SCIEVT);

         switch ((Cfg0E >> shiftSCI_OP_SELECT) & maskSCI_OP_SELECT)
         {
            /* If the SCI signal was set high or low,
               deactivate the SCI signal now. */
            case SXI_SET_HI:
            case SXI_SET_LO:
               /* Toggle the SCI pin. */
               Extended_Write_Port(SCI_Pin, 0xFFFF);
               break;
         }

         /* Setup the ACPI Timer in order to transfer control
            to check if another event is pending.  If an SCI
            event is pending, another SCI will be generated. */
         ACPI_Timer = 1;
         break;
   }

   return (rval);
}

/* ---------------------------------------------------------------
   Name: ACPI_Dat

   Input: command == command number.
          data == data byte.

   At this point, Hif_Var[HIF_PM1].Cmd_Byte == 0.

   Handle the data part of ACPI commands.
   --------------------------------------------------------------- */

   /* ACPI response to writes to SMBus Protocol register. */

   #define PRTCL_WRITE_QUICK     0x2   /* Write Quick Command. */
   #define PRTCL_READ_QUICK      0x3   /* Read Quick Command. */
   #define PRTCL_SEND_BYTE       0x4   /* Send Byte. */
   #define PRTCL_RECEIVE_BYTE    0x5   /* Receive Byte. */
   #define PRTCL_WRITE_BYTE      0x6   /* Write Byte. */
   #define PRTCL_READ_BYTE       0x7   /* Read Byte. */
   #define PRTCL_WRITE_WORD      0x8   /* Write Word. */
   #define PRTCL_READ_WORD       0x9   /* Read Word. */
   #define PRTCL_WRITE_BLOCK     0xA   /* Write Block. */
   #define PRTCL_READ_BLOCK      0xB   /* Read Block. */
   #define PRTCL_PROC_CALL       0xC   /* Process Call. */
   #define PRTCL_PROC_BLOCK      0xD   /* Process Block Call. */
   #define PRTCL_I2C_WR_BLOCK 0x0E  /* I2C Write Block */

WORD ACPI_Dat(BYTE command_num, BYTE data)
{
   WORD rval;
   rval = (WORD) ~0; /* Default return value: "nothing to send". */

   switch(command_num)
   {
      case 0x80:                 /* Read Embedded Controller. */
         rval = Read_EC_Space(data);
         Flag.ACPI_IN_PROGRESS = 0;
         break;

      case 0x81:                 /* Write Embedded Controller. */

         Write_EC_Space(command_num, data, HIF_PM1);

         /* Generate interrupt to signal the Host that the
            data has been taken and more data can be sent or
            the command is being worked on. */
         ACPI_Gen_Int();

         if (Hif_Var[HIF_PM1].Tmp_Load > 1) Flag.ACPI_IN_PROGRESS = 0;
         break;
   }

   return (rval);
}

/* ---------------------------------------------------------------
   Name: Read_EC_Space

   Read Embedded Controller (EC) space array.

   Used for Read Embedded Controller and Legacy Read EC Space.
   --------------------------------------------------------------- */
WORD Read_EC_Space(BYTE data)
{
   WORD rval;
   BYTE *pntr;

   /* Allow hook to read the data. */
   rval = Gen_Hookc_Read_EC(data);
   if (rval == (WORD) ~0)
   {
      /* Hook has signaled that the CORE should read the data. */

      /* Setup to send data from memory. */
      if ((data >= ACPI_SMB_BASE) && (data <= (ACPI_SMB_BASE + SMB_ALRM_DATA_INDX)))
      {
         data = data - ACPI_SMB_BASE;
      rval = (WORD) EC_Space[data];
   }
      else
      {
         rval = 0;
      }
   }
   return(rval);
}

/* ---------------------------------------------------------------
   Name: Write_EC_Space

   Write Embedded Controller (EC) space array.

   Used for Write Embedded Controller and Legacy Write EC Space.

   Return: 1 when this routine has handled the command.
           0 to allow the calling routine to continue processing.
           Hif_Var[HIF_PM1].Tmp_Load = 1 when address received.
           Hif_Var[HIF_PM1].Tmp_Load = 2 when data received.
   --------------------------------------------------------------- */
FLAG Write_EC_Space(BYTE command_num, BYTE data, BYTE hif)
{
   FLAG ret_flag;
   BYTE *pntr;
   SMALL index;

   if (Hif_Var[hif].Tmp_Load == 0)
   {
      /* Just received address byte from Host. */
      Hif_Var[hif].Tmp_Load++;             /* Increment data byte counter. */
      EC_Addr = data;         /* Store address byte. */
      Hif_Var[hif].Cmd_Byte = command_num; /* Signal to wait for data from Host. */

      ret_flag = 1;           /* Processing is complete for now. */
   }
   else
   {
      /* Just received data byte from Host. */
      Hif_Var[hif].Tmp_Load++;             /* Increment data byte counter. */
//      ret_flag = Gen_Hookc_Write_EC(data);
      ret_flag = Gen_Hookc_Write_EC(EC_Addr, data);
      if (ret_flag)
      {
         /* The hook handled the command.
            Processing is complete.
            ret_flag != 0. */
         ;
      }
      else if ((EC_Addr >= ACPI_SMB_BASE) && (EC_Addr <= (ACPI_SMB_BASE + SMB_ALRM_DATA_INDX)))
      {
         index = EC_Addr - ACPI_SMB_BASE;
         /* Store data byte from Host. */
         EC_Space[index] = data;

         if (index == SMB_PRTCL_INDX)
         {
            /* This was a write to the protocol register.
               Initiate the transaction on the SMBus. */

            /* Clear all status bits but bit 6 (ALRM bit). */
            SMB_ACPI[SMB_STS_INDX] &= 0x40;

            if (data)
            {  /*
               For non-zero data written to the protocol register,
               setup the ACPI Timer in order to transfer
               control to handle the ACPI SMBus protocol. */
               ACPI_Timer = 1;
            }
         }
         /* ret_flag = 0.  Allow calling routine
            to continue processing this command. */
      }
   }

   return(ret_flag);
}

/* ---------------------------------------------------------------
   Name: Set_SCI_Cause

   Setup the cause flag for an SCI and start the ACPI_Timer.

   sci_number = 1 through 255,
      The Host uses this number to determine the cause of the SCI.
   --------------------------------------------------------------- */
void Set_SCI_Cause(BYTE sci_number)
{
    BYTE index;

    if (sci_number <= LAST_SCI_CAUSE)
    {
        /* The SCI number has a flag in the SCI flags array. */

        /* Change cause number 1 through 255 to 0 to 254. */
        sci_number -= 1;

        /* Set the flag. */
        Cause_Flags[sci_number >> 3] |= Byte_Mask((SMALL) (sci_number & 7));
    }
    else
    {
        /* The SCI number does not have a flag in the SCI flags array.
           Buffer the SCI number in SCI_Event_Buffer if there is room.

           Put the SCI number in the buffer and increment the input
           index.  If the input index equals the output index, the
           buffer is full and the SCI number just placed in the buffer
           will not be used. */

        /* Get the input index. */
        index = SCI_Event_In_Index;

        /* Put the SCI number in the buffer. */
        SCI_Event_Buffer[index] = sci_number;

        /* Increment the index. */
        index++;
        if (index >= EVENT_BUFFER_SIZE) index = 0;

        /* If the buffer is not full, update the input index. */
        if (index != SCI_Event_Out_Index) SCI_Event_In_Index = index;
    }

    /* Pending SCI events are checked when the ACPI Timer
      goes to zero.  This timer counts at a 1 millisecond
      rate.  Setting the timer to 1 will cause the SCI
      events to be checked within 1 mS.  An SCI will be
      generated if a pending SCI event exists. */

    ACPI_Timer = 1;
    Enable_Timer_A();

}

/* ---------------------------------------------------------------
   Name: ACPI_Gen_Int

   Generate an SCI interrupt.

   For a pulsed SCI (edge triggered), the SCI signal is pulsed.

   For a level triggered SCI, the SCI signal is set or cleared.
   The Host will send a command to read the cause of the SCI.
   The signal will be deactivated when no more causes exist.
   --------------------------------------------------------------- */
void ACPI_Gen_Int(void)
{
   BYTE pin;
   WORD time;

   if (Ext_Cb3.bit.ACPI_ENABLED)
   {
      #if SXI_SUPPORTED
      CLEAR_BIT(HIPM1IE, IESMIE);  /* Disable SMI output enable. */
      SET_BIT(HIPM1IE, IESCIE);      /* Enable SCI output enable. */
      #endif
      switch ((Cfg0E >> shiftSCI_OP_SELECT) & maskSCI_OP_SELECT)
      {
         case SXI_SET_HI:
            /* Set the SCI pin high. */
            pin = SCI_Pin | MASK(bit3);
            Extended_Write_Port(pin, 0);
            break;

         case SXI_SET_LO:
            /* Set the SCI pin low. */
            #if SXI_SUPPORTED
            if ((SCI_Pin & 0xF0) == (HW_CODE << 4))
               pin = SCI_Pin | MASK(bit3);
            else
            pin = SCI_Pin & ~(MASK(bit3));
            #else
            pin = SCI_Pin & ~(MASK(bit3));
            #endif
            Extended_Write_Port(pin, 0);
            break;

         case SXI_PULSE_HI:
            /* Pulse the SCI pin high. */
            pin = SCI_Pin | (MASK(bit3));
            Extended_Write_Port(pin, SCI_Pulse << 2);
            break;

         case SXI_PULSE_LO:
            /* Pulse the SCI pin low. */
            #if SXI_SUPPORTED
            if ((SCI_Pin & 0xF0) == (HW_CODE << 4))
               pin = SCI_Pin | MASK(bit3);
            else
            pin = SCI_Pin & ~(MASK(bit3));
            #else
            pin = SCI_Pin & ~(MASK(bit3));
            #endif
            Extended_Write_Port(pin, SCI_Pulse << 2);
            break;
      }
   }
}

/* ---------------------------------------------------------------
   --------------------------------------------------------------- */
static void acpi_idle(void)
{
   #define DONE_CAUSE_INDEX ((SMB_PROTOCOL_DONE-1) >> 3)
   #define DONE_CAUSE_MASK  (1 << ((SMB_PROTOCOL_DONE-1) & 7))
   #define ALRM_CAUSE_INDEX ((SMB_ALRM_RECEIVED-1) >> 3)
   #define ALRM_CAUSE_MASK  (1 << ((SMB_ALRM_RECEIVED-1) & 7))

   BYTE *pntr;
   SMALL index;

   if (EC_Space[SMB_PRTCL_INDX])
   {
      /* This was a non-zero write to the protocol register.
         Initiate the transaction on the SMBus. */
      acpi_start_protocol();
   }
   else if (Ext_Cb3.bit.ACPI_ENABLED)
   {
      /* In ACPI mode, generate SCI events if necessary. */

      /* Setup to skip sending an SCI. */
      index = NUM_SCI_FLAGS_BYTES;

      if ( (Read_Host_Status_Reg2() &
           (maskSTATUS_PORT2_SCIEVT + maskSTATUS_PORT_OBF)) != 0)
      {
         /* If the SCIEVT flag is set, an SCI has been
            generated and the Host has not sent the Query
            EC command yet.  Do not send another event SCI. */

         /* If the OBF flag is set, the Host has not
            taken data that is in the secondary Host
            interface.  This may be the response to
            an ACPI command. Do not send an event SCI. */
         ;
      }
      else if (Flag.ACPI_IN_PROGRESS)
      {
         /* A read or write EC space command is
            in progress.  Do not send an event SCI. */
         ;
      }
      else
      {
         /* Setup to generate an SCI if needed. */
         index = 0;

         /* Check if alarm SCI event should be sent. */
         if ( (Flag.ACPI_SMB_ALARM == 1) &&
              ((SMB_ACPI[SMB_STS_INDX] & 0x40) == 0) &&
              ((Cause_Flags[DONE_CAUSE_INDEX] & DONE_CAUSE_MASK) == 0) )
         {
            /* An alarm is in the I2C alarm buffer.
               There is no alarm data waiting for the Host to take it.
               An SMB_PROTOCOL_DONE event is not pending. */

            /* Get pointer to alarm data. */
            pntr = I2c_Get_Alarm(0);    // Check ???

            /* Put alarm in ACPI buffer. */
            SMB_ACPI[SMB_ALRM_ADDR_INDX]   = *pntr++;
            SMB_ACPI[SMB_ALRM_DATA_INDX+0] = *pntr++;
            SMB_ACPI[SMB_ALRM_DATA_INDX+1] = *pntr;

            /* Clear alarm flag. */
            Flag.ACPI_SMB_ALARM = 0;

            /* Clear all status bits and set bit 6 (ALRM bit). */
            SMB_ACPI[SMB_STS_INDX] = 0x40;

            /* Set cause flag for this event and start ACPI_Timer. */
            Set_SCI_Cause(SMB_ALRM_RECEIVED);
         }
      }

      while (index < NUM_SCI_FLAGS_BYTES)
      {
         if (Cause_Flags[index] != 0)
         {
            /* There is at least one event pending. */

            /* Set the SCI_EVT flag. */
            Write_Host_Status_Reg2(Read_Host_Status_Reg2() |
                                   maskSTATUS_PORT2_SCIEVT);

            /* Generate the signal. */
            ACPI_Gen_Int();

            /* Because the SCI_EVT flag is set and an SCI has
               been generated, the Host will send a Query EC
               command.  The ACPI Timer will be setup in that
               command in order to transfer control back to
               here to check if another event is pending. */

            /* Stop the loop. */
            index = NUM_SCI_FLAGS_BYTES;
         }
         else
         {
            index++;
         }
      }
   }
}

static void acpi_start_protocol(void)
{
   /* Setup the ACPI_Timer in order to transfer control
      to check when SMBus level 0 is available. */
   ACPI_Timer = 1;
   acpi_service_pntr = acpi_chk_busy;
}

static void acpi_chk_busy(void)
{
   FLAG prtcl_done;
   BYTE *source_pntr;
   BYTE *smb_pntr;
   SMALL count;

   if (Smb_Level_0_Free(ACPI_SMB_CHANNEL) == 0)    // Check ???
   {
      ACPI_Timer = 1;
   }
   else
   {
      /*
      ACPI response to writes to SMBus Protocol register.

      To initiate a transfer on the SMBus,
      the SMB_Buffer must be setup:

      SMB_Buffer[level][0] holds the SMBus protocol type.
      SMB_Buffer[level][1] holds the write byte count.
      SMB_Buffer[level][2] holds the slave address.
      SMB_Buffer[level][3] holds the command byte.
      SMB_Buffer[level][4] holds the
               .           data
               .           to be
               .           sent to
      SMB_Buffer[level][x] the device.

      In EC space, the SMB_ADDR_INDX (Address register),
                   the SMB_CMD_INDX  (Command register),
               and the SMB_DATA_INDX (Data register 0 through 31)
               are in contiguous locations. */

      #define PRTCL_WRITE_QUICK  0x2   /* Write Quick Command. */
      #define PRTCL_READ_QUICK   0x3   /* Read Quick Command. */
      #define PRTCL_SEND_BYTE    0x4   /* Send Byte. */
      #define PRTCL_RECEIVE_BYTE 0x5   /* Receive Byte. */
      #define PRTCL_WRITE_BYTE   0x6   /* Write Byte. */
      #define PRTCL_READ_BYTE    0x7   /* Read Byte. */
      #define PRTCL_WRITE_WORD   0x8   /* Write Word. */
      #define PRTCL_READ_WORD    0x9   /* Read Word. */
      #define PRTCL_WRITE_BLOCK  0xA   /* Write Block. */
      #define PRTCL_READ_BLOCK   0xB   /* Read Block. */
      #define PRTCL_PROC_CALL    0xC   /* Process Call. */
      #define PRTCL_PROC_BLOCK   0xD   /* Process Block Call. */
      #define PRTCL_I2C_WR_BLOCK 0x0E  /* I2C Write Block */

      prtcl_done = FALSE;

      smb_pntr = &SMB_Buffer[ACPI_SMB_CHANNEL][0];

      /* Most protocols use the slave address
         with read/write bit = 0 (for write). */
      *(smb_pntr + 2) = SMB_ACPI[SMB_ADDR_INDX] & ~(0x01);

      if (EC_Space[SMB_PRTCL_INDX] & 0x80)
      {
         EC_Space[SMB_PRTCL_INDX] &= ~0x80;
         I2c_Var[ACPI_SMB_CHANNEL].i2c_state2.bit.PEC_SUPPORTED = 1;
      }
      else
         I2c_Var[ACPI_SMB_CHANNEL].i2c_state2.bit.PEC_SUPPORTED = 0;    // Check ???

      switch (EC_Space[SMB_PRTCL_INDX])
      {
         case PRTCL_WRITE_QUICK:
            /* ACPI SMBus Protocol 2h, Write Quick Command. */

            /* Send the address byte with read/write
               bit = 0 (for write) to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_QUICK_COMMAND;

            /* Write byte count. */
            *smb_pntr++ = 1;
            break;

         case PRTCL_READ_QUICK:
            /* ACPI SMBus Protocol 3h, Read Quick Command. */

            /* Send the address byte with read/write
               bit = 1 (for read) to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_QUICK_COMMAND;

            /* Write byte count. */
            *smb_pntr++ = 1;

            /* Slave address with read/write bit = 1 (for read). */
            *smb_pntr = SMB_ACPI[SMB_ADDR_INDX] | 0x01;
            break;

         case PRTCL_SEND_BYTE:
            /* ACPI SMBus Protocol 4h, Send Byte. */

            /* Send the address and command bytes to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_SEND_BYTE;

            /* Write byte count. */
            *smb_pntr++ = 2;

            /* Slave address already setup. */
            smb_pntr++;

            /* Command byte. */
            *smb_pntr = SMB_ACPI[SMB_CMD_INDX];
            break;

         case PRTCL_RECEIVE_BYTE:
            /* ACPI SMBus Protocol 5h, Receive Byte. */

            /* Send the address to the I2C bus
               and get the byte from the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_RECEIVE_BYTE;

            /* Write byte count. */
            *smb_pntr++ = 1;

            /* Slave address with read/write bit = 1 (for read). */
            *smb_pntr = SMB_ACPI[SMB_ADDR_INDX] | 0x01;
            break;

         case PRTCL_WRITE_BYTE:
            /* ACPI SMBus Protocol 6h, Write Byte. */

            /* Send the address, command, and a data byte to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_WRITE_BYTE;

            /* Write byte count. */
            *smb_pntr++ = 3;

            /* Slave address already setup. */
            smb_pntr++;

            /* Command byte. */
            *smb_pntr++ = SMB_ACPI[SMB_CMD_INDX];

            /* Data byte. */
            *smb_pntr = SMB_ACPI[SMB_DATA_INDX];
            break;

         case PRTCL_READ_BYTE:
            /* ACPI SMBus Protocol 7h, Read Byte. */

            /* Send the address and command to the I2C
               bus and get the byte from the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_READ_BYTE;

            /* Write byte count. */
            *smb_pntr++ = 2;

            /* Slave address with read/write bit = 1 (for read). */
            *smb_pntr++ = SMB_ACPI[SMB_ADDR_INDX] | 0x01;

            /* Command byte. */
            *smb_pntr = SMB_ACPI[SMB_CMD_INDX];
            break;

         case PRTCL_WRITE_WORD:
            /* ACPI SMBus Protocol 8h, Write Word. */

            /* Send the address, command, and 2 bytes to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_WRITE_WORD;

            /* Write byte count. */
            *smb_pntr++ = 4;

            /* Slave address already setup. */
            smb_pntr++;

            /* Command and two data bytes. */
            source_pntr = &SMB_ACPI[SMB_CMD_INDX];
            count = 3;
            while (count)
            {
               *smb_pntr++ = *source_pntr++;
               count--;
            }
            break;

         case PRTCL_READ_WORD:
            /* ACPI SMBus Protocol 9h, Read Word. */

            /* Send the address and command bytes to the
               I2C bus and get two bytes from the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_READ_WORD;

            /* Write byte count. */
            *smb_pntr++ = 2;

            /* Slave address with read/write bit = 1 (for read). */
            *smb_pntr++ = SMB_ACPI[SMB_ADDR_INDX] | 0x01;

            /* Command byte. */
            *smb_pntr = SMB_ACPI[SMB_CMD_INDX];
            break;

         case PRTCL_WRITE_BLOCK:
         case PRTCL_I2C_WR_BLOCK:
            /* ACPI SMBus Protocol Ah, Write Block. */

            /* Send the address, command, byte
               count, and data bytes to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_WRITE_BLOCK;

            /* Write byte count. */
            //*smb_pntr++ = SMB_ACPI[SMB_BCNT_INDX] + 3;
            if (EC_Space[SMB_PRTCL_INDX]== PRTCL_I2C_WR_BLOCK)
               *smb_pntr++ = SMB_ACPI[SMB_BCNT_INDX] + 2;
            else
               *smb_pntr++ = SMB_ACPI[SMB_BCNT_INDX] + 3;

            /* Slave address already setup. */
            smb_pntr++;

            source_pntr = &SMB_ACPI[SMB_CMD_INDX];

            /* Command byte. */
            *smb_pntr++ = *source_pntr++;

            /* Get number of data bytes to send. */
            count = (SMALL) SMB_ACPI[SMB_BCNT_INDX];

            /* Byte count. */
            //*smb_pntr++ = count;
            if (EC_Space[SMB_PRTCL_INDX]== PRTCL_WRITE_BLOCK)
               *smb_pntr++ = count;
            /* Data bytes. */
            while (count)
            {
               *smb_pntr++ = *source_pntr++;
               count--;
            }
            break;

         case PRTCL_READ_BLOCK:
            /* ACPI SMBus Protocol Bh, Read Block. */

            /* Send the address and command bytes to the I2C
               bus and get the data bytes from the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_READ_BLOCK;

            /* Write byte count. */
            *smb_pntr++ = 2;

            /* Slave address with read/write bit = 1 (for read). */
            *smb_pntr++ = SMB_ACPI[SMB_ADDR_INDX] | 0x01;

            /* Command byte. */
            *smb_pntr = SMB_ACPI[SMB_CMD_INDX];
            break;

         case PRTCL_PROC_CALL:
            /* ACPI SMBus Protocol Ch, Process Call. */

            /* Send the address byte, command byte and 2 data bytes
               to the I2C bus and get 2 bytes from the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_PROCESS_CALL;

            /* Write byte count. */
            *smb_pntr++ = 4;

            /* Slave address already setup. */
            smb_pntr++;

            /* Command and two data bytes. */
            source_pntr = &SMB_ACPI[SMB_CMD_INDX];
            count = 3;
            while (count)
            {
               *smb_pntr++ = *source_pntr++;
               count--;
            }
            break;

         case PRTCL_PROC_BLOCK:
            /* ACPI SMBus Protocol Ah, Write Block. */

            /* Send the address, command, byte
               count, and data bytes to the I2C bus. */

            /* SMBus protocol type. */
            *smb_pntr++ = SMB_PROCESS_BLOCK;

            /* Write byte count. */
            *smb_pntr++ = SMB_ACPI[SMB_BCNT_INDX] + 3;

            /* Slave address already setup. */
            smb_pntr++;

            source_pntr = &SMB_ACPI[SMB_CMD_INDX];

            /* Command byte. */
            *smb_pntr++ = *source_pntr++;

            /* Get number of data bytes to send. */
            count = (SMALL) SMB_ACPI[SMB_BCNT_INDX];

            /* Byte count. */
            *smb_pntr++ = count;

            /* Data bytes. */
            while (count)
            {
               *smb_pntr++ = *source_pntr++;
               count--;
            }
            break;


         default:
            /* (This routine does not run when the protocol register is
                set to 0.  So, SMBus Protocol 0 will not be acted on.) */

            /* ACPI SMBus Protocol = 0h, Controller Not In Use. */
            /* ACPI SMBus Protocol = 1h, Reserved. */
            /* ACPI SMBus Protocol > Ch, Protocol not supported. */

            /* Set status to "SMBus Host Unsupported Protocol". */
            SMB_ACPI[SMB_STS_INDX] |= 0x19;

            prtcl_done = TRUE;
            break;
      }

      if (prtcl_done)
      {
         SMB_ACPI[SMB_PRTCL_INDX] = 0;

         /* Set cause flag for this event and start ACPI_Timer. */
         Set_SCI_Cause(SMB_PROTOCOL_DONE);

         /* The SCI event to signal the Host that the SMBus
            transaction is completed will be handled by acpi_idle. */

         acpi_service_pntr = acpi_idle;
      }
      else
      {
         //Smb_Write_Level_0(ACPI_SMB_CHANNEL, acpi_selector);
         SMB_Status[ACPI_SMB_CHANNEL].byte = MASK(SMB_IBF);
         I2c_Execute_Protocol(SMB_Buffer[ACPI_SMB_CHANNEL], ACPI_SMB_CHANNEL, 1);

         /* The ACPI_Timer will be started
            at the end of the SMBus transfer. */
         acpi_service_pntr = acpi_chk_protocol;
      }
   }
}

static void acpi_chk_protocol(void)
{
   BYTE *smb_pntr;
   BYTE *acpi_pntr;
   SMALL count;

   if ( (SMB_Status[ACPI_SMB_CHANNEL].byte & (MASK(SMB_IBF))) == 0 )
   {
      /* The transfer was completed.

      Get data (if any).  The SMB_Buffer looks like:

      SMB_Buffer[level][0] holds the number of bytes read.
      SMB_Buffer[level][1] holds the
               .           data
               .           read
               .           from
      SMB_Buffer[level][x] the device.

      Put data in ACPI SMBus data locations. */

      acpi_pntr = &SMB_ACPI[SMB_DATA_INDX];
      smb_pntr = &SMB_Buffer[ACPI_SMB_CHANNEL][0];
      count = *smb_pntr++;

      //if (SMB_ACPI[SMB_PRTCL_INDX] == PRTCL_READ_BLOCK)
      if ((SMB_ACPI[SMB_PRTCL_INDX] == PRTCL_READ_BLOCK)
          || (SMB_ACPI[SMB_PRTCL_INDX] == PRTCL_PROC_BLOCK))
      {
         /* For Block Read protocol, SMB_Buffer[level][1]
            holds the number of bytes following in the
            SMB_Buffer.  This count should be one less
            than the value in SMB_Buffer[level][0].

            The count of SMB_Buffer[level][0] - 1 will be used
            to ensure that the buffer will not overflow as a
            result of using a bad value from SMB_Buffer[level][1]. */

         if (count != 0) count--;
         SMB_ACPI[SMB_BCNT_INDX] = count;
         smb_pntr++;
      }

      while (count)
      {
         *acpi_pntr++ = *smb_pntr++;
         count--;
      }

      if (SMB_Status[ACPI_SMB_CHANNEL].byte & (MASK(SMB_CMD_DENIED)))
      {
         /* Set status to "SMBus Device Command Access Denied". */
         SMB_ACPI[SMB_STS_INDX] |= 0x12;
      }
      else if (SMB_Status[ACPI_SMB_CHANNEL].byte & (MASK(SMB_DEV_DENIED)))
      {
         /* Set status to "SMBus Device Access Denied". */
         SMB_ACPI[SMB_STS_INDX] |= 0x17;
      }
      else if (SMB_Status[ACPI_SMB_CHANNEL].byte & (MASK(SMB_ERROR)))
      {
         /* Set status to "SMBus Unknown Failure". */
         SMB_ACPI[SMB_STS_INDX] |= 0x07;
      }
      else if (SMB_Status[ACPI_SMB_CHANNEL].byte &(MASK(SMB_PEC_ERROR)))
      {
         /* Set status to "SMBus Device Packet Error Checking error". */
         SMB_ACPI[SMB_STS_INDX] |= 0x1F;
      }
      else
      {
         /* Set SMBus done flag. */
         SMB_ACPI[SMB_STS_INDX] |= 0x80;
      }

      /* Free SMBus level 0 and shadow the SMB_X nibble of the selector. */
      Free_Smb_Level_0(ACPI_SMB_CHANNEL);

      /* Clear protocol byte. */
      SMB_ACPI[SMB_PRTCL_INDX] = 0;

      /* Set cause flag for this event and start ACPI_Timer. */
      Set_SCI_Cause(SMB_PROTOCOL_DONE);

      /* The SCI event to signal the Host that the SMBus
         transaction is completed will be handled by acpi_idle. */

      acpi_service_pntr = acpi_idle;
   }
}

#endif /* if ACPI_SUPPORTED */
