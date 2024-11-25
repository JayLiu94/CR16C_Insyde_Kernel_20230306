/* ----------------------------------------------------------------------------
 * MODULE PURFUNCT.C
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
 * System Control Functions.
 * Based on PC87570 code version 2.00.07[2667-21]-n
 *
 * Revision History (Started March 15, 1998 by GLP)
 *
 * ------------------------------------------------------------------------- */

#define PURFUNCT_C

#include "types.h"
#include "com_defs.h"
#include "defs.h"
#include "purxlt.h"
#include "purfunct.h"
#include "purhook.h"
#include "proc.h"
#include "purcmd.h"
#include "purext.h"
#include "oem.h"
#include "timers.h"
#include "staticfg.h"
#include "ini.h"
#include "purdat.h"
#include "purscn.h"
#include "purmain.h"
#include "puracpi.h"
#include "host_if.h"

#ifndef ACPI_SUPPORTED
#error ACPI_SUPPORTED switch is not defined.
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void set_smi_cause(BYTE smi_number);
static void inc_millisecond_timer(void);


BYTE smi_timer;
BYTE Pulse_Width;

struct
{
   BYTE timer[NUM_INPUTS];
} input;

MS_TIMER millisecond_timer;

TIMED_FUNCT Timed_Function[5];


/* ---------------------------------------------------------------
   Name: GET_BASE, GET_MULTIPLIER

   Input: Timer setting

            Byte = yyyx,xxxx

            Base = X + 1.

             Y    multiplier
            ---   ----------
             0          1
             1         10
             2          1
             3         10
             4        100
             5        500
             6       1000
             7       2000

   Returns base or multiplier value.
   --------------------------------------------------------------- */
#define GET_BASE(timer_setting) ((timer_setting & 0x1F) + 1)
#define GET_MULTIPLIER(timer_setting) multiplier[((timer_setting >> 5) & 7)]
static const int multiplier[] =
{
      1, /* Y = 0. */
     10, /* Y = 1. */
      1, /* Y = 2. */
     10, /* Y = 3. */
    100, /* Y = 4. */
    500, /* Y = 5. */
   1000, /* Y = 6. */
   2000  /* Y = 7. */
};


/* ---------------------------------------------------------------
   Name: System_Control_Function

   Input: data = control function word
          event = Contact event
                  (MAKE_EVENT, BREAK_EVENT, or REPEAT_EVENT)

         --- data is... ---

      xxxy,yyyy,zzzz,zzzz

      (X) = Trigger Action for hotkey and port pin input events
            000 = Perform function on key Make
            001 = Perform function on key Make and Repeat
            010 = Perform function on key Break
            011 = Perform function on key Make,
                  Complement of function on key Break
            100 ~ 110 reserved.
            111 = Perform function on key Make, Repeat and Break

      (Y) = System Control Function to perform

      (Z) = Data to support function described by (Y)

   The Trigger Action denoted by xx = 11 is a special "hold"
   function.  This can be used, for example, to lower a signal for
   as long as the user is holding down the hotkey and raise it
   when the key is released.  This is accomplished by executing a
   function when the key is pressed (MAKE_EVENT) and executing a
   complement function when the key is released (BREAK_EVENT).
   Some of the Port Pin Control Functions do not have a valid
   complement.  The complements of each function when this feature
   is used are enumerated in the following table.

            Standard Function         Complement Function
            ------------------   ----------------------------
      00    NOP                  NOP
      01    Set High             Set Low
      02    Set Low              Set High
      03    Pulse High           Pulse High
      04    Pulse Low            Pulse Low
      05    Toggle               Toggle
   --------------------------------------------------------------- */

/* FUNCT_PTR_V_B is a pointer to a function that returns nothing
   (V for void) and takes a BYTE for a parameter (B for BYTE). */
typedef void (*FUNCT_PTR_V_B)(BYTE);

void System_Control_Function(WORD data, SMALL event)
{
   /*
   The sys_ctrl_complement table maps y,yyyy (function)
   to its complement.  Functions with an index greater
   than MAX_CPL_INDEX use a NOP for the complement. */

   FLAG do_it;
   BYTE * pntr;
   BYTE index, saved_index;
   WORD temp;

   if (Gen_Hookc_Sys_Ctrl_Entry(data, event))
   {
      /* The hook handled the function. */
      ;
   }
   else
   {
      /* The hook did not handle the function. */

      do_it = FALSE;
      if ( (data & 0xE000) == 0x0000 )
      {
         /* xxx = 000 = Perform function on key Make. */

         if (event == MAKE_EVENT) do_it = TRUE;
      }
      else if ( (data & 0xE000) == 0x2000 )
      {
         /* xxx = 001 = Perform function on key Make and Repeat. */

         if (event == MAKE_EVENT)   do_it = TRUE;
         if (event == REPEAT_EVENT) do_it = TRUE;
      }
      else if ( (data & 0xE000) == 0x4000 )
      {
         /* xxx = 010 = Perform function on key Break. */

         if (event == BREAK_EVENT) do_it = TRUE;
      }
      else if ( (data & 0xE000) == 0x6000 )
      {
         /* ( (data & 0xE000) == 0x6000 )

            xxx = 011 = Perform function on key Make and Break. */

         if (event == MAKE_EVENT) do_it = TRUE;
         if (event == BREAK_EVENT)
         {
            do_it = TRUE;
         }
      }
      // Edward 2013/2/18 Added to support make/repeat/break.
      else if ( (data & 0xE000) == 0xE000 )
      {
         /* xxx = 111 = Perform function on key Make, Repeat and Break. */
         if (event == MAKE_EVENT)   do_it = TRUE;
         if (event == REPEAT_EVENT) do_it = TRUE;
         if (event == BREAK_EVENT) do_it = TRUE;
      }
      // End of Edward 2013/2/18 Added.

      if (do_it)
      {  /*
         data contains 0xxy,yyyy,zzzz,zzzz.

         y,yyyy is the function definition for
                the following switch statement.

         For port pin operation functions, zzzz,zzzz
         defines the port and pin.  This is setup as follows.

            zzzz,zzzz = aaaa,0bbb

               aaaa = Port code.
               bbb  = Port pin number.

         Extended_Write_Port(BYTE data, WORD option)
         is used to modify one port pin to HI or LO.

         The data byte input to Extended_Write_Port
         is configured as follows:

            bit 7 - 4 : Port code
            bit     3 : 0 = modify to LO, 1 = modify to HI
            bit 2 - 0 : Port Pin # (0 - 7)

         The option argument is used to modify the operation
            If 0x0000,          bit 3 of data is used to modify pin.
            If 0x0001 - 0xFFFE, pin is pulsed.
            If 0xFFFF,          bit 3 of data is ignored and pin is toggled.

            For pulse, bit 3 of data is used to modify pin
                       and option is amount of delay.
                       After delay, pin is restored. */

         switch ((data >> 8) & 0x1F)
         {
            case Y_NOP:
               /* NOP. */
               break;

            case Y_SET_HIGH:
               /* Set pin high. */
               data |= MASK(bit3);
               Extended_Write_Port((BYTE) (data & 0xFF), 0);
               break;

            case Y_SET_LOW:
               /* Set pin low. */
               data &= ~(MASK(bit3));
               Extended_Write_Port((BYTE) (data & 0xFF), 0);
               break;

            case Y_PULSE_HIGH:
               /* Pulse pin high. */

               data |= MASK(bit3);
               Extended_Write_Port((BYTE) (data & 0xFF), Pulse_Width);
               break;

            case Y_PULSE_LOW:
               /* Pulse pin low. */

               data &= ~(MASK(bit3));
               Extended_Write_Port((BYTE) (data & 0xFF), Pulse_Width);
               break;

            case Y_TOGGLE:
               /* Toggle pin. */
               Extended_Write_Port((BYTE) (data & 0xFF), 0xFFFF);
               break;
            case Y_GEN_SCAN_CODE:
               /* Generate scan code as if from keyboard.

                  zzzz,zzzz is the Insyde Software key number. */

               Send_Key((BYTE) (data & 0xFF), MAKE_EVENT);
               Send_Key((BYTE) (data & 0xFF), BREAK_EVENT);

               Disable_Irq();
               if (Timer_B.bit.SEND_ENABLE == 0)   /* 1 = Device send request.
                                                          (Request already set.) */
               {
                  if (Check_Scan_Transmission())   /* Is data available? */
                     Start_Scan_Transmission();    /* Yes, start new transmission. */
               }
               Enable_Irq();
               break;
            case Y_OEM_FUNCT:
               /* OEM System Control Function. */
               Gen_Hookc_OEM_Sys_Ctrl_Funct(data, event);
               break;

            default:
               /* data = 0xxy,yyyy,zzzz,zzzz. */

               if ( (data & 0x1800) == 0x1000)
               {  /*
                  y,yyyy set to 1,0tuv is for SMI/SCI generation when tuv != 0.

                  t: 1 = Generate SMI in Legacy mode with query value = ZZh.
                  u: 1 = Generate SMI in ACPI mode   with query value = ZZh.
                  v: 1 = Generate SCI in ACPI mode   with query value = ZZh. */

                  #if ACPI_SUPPORTED
                  if (Ext_Cb3.bit.ACPI_ENABLED)
                  {
                     /* ACPI mode. */

                     if (data & 0x0100)
                     {
                        /* v = 1,
                           Generate SCI with query value = ZZh. */

                        /* Set cause flag for this event and start ACPI_Timer. */
                        Set_SCI_Cause((BYTE) (data & 0xFF));
                     }

                     if (data & 0x0200)
                     {
                        /* u = 1,
                           Generate SMI with query value = ZZh.
                           Set cause flag for this event and start SMI Timer. */
                        set_smi_cause((BYTE) (data & 0xFF));
                     }
                  }
                  else
                  #endif
                  {
                     /* Legacy mode. */

                     if (data & 0x0400)
                     {
                        /* t = 1,
                           Generate SMI with query value = ZZh.
                           Set cause flag for this event and start SMI Timer. */
                        set_smi_cause((BYTE) (data & 0xFF));
                     }
                  }
               }
               break;
         }
      }
   }
}

/* ---------------------------------------------------------------
   Name: Sys_Funct_Init

   Initialize the System Control Functions.
   --------------------------------------------------------------- */
void Sys_Funct_Init(void)
{
   Pulse_Width = 39;
   /* Initialize the millisecond timer. */
   millisecond_timer.word = 0;

}

/* ---------------------------------------------------------------
   Name: Handle_Sys_Funct

   Handle the periodic System Control Functions.
   This is called at a 1 mS interval.
   --------------------------------------------------------------- */
void Handle_Sys_Funct(void)
{
   #define PINS_ON  (1)
   #define PINS_OFF (0)

   const BYTE * pntr;

   FLAG Changed;
   BYTE * pntr2;
   BYTE b_temp;
   BYTE event_temp;
   WORD w_temp;
   SMALL index;

   event_temp = 0;
   Changed = FALSE;

   /* Update the millisecond timer. */
   inc_millisecond_timer();

   /* Handle SMI. */
   if (smi_timer)
   {
      /* When the SMI Timer is non-zero, it is running.
         When it counts down to zero, the SMI service routine
         is run.

         The SMI Timer is, actually, only one bit wide.
         Service is handled within 1 millisecond.  */

      smi_timer = 0;

      /* Generate an SMI.

         For a pulsed SMI (edge triggered), the SMI signal is pulsed.

         For a level triggered SMI, the SMI signal is set or cleared.
         The Host will send a command to read the cause of the SMI.
         The signal will be deactivated when no more causes exist. */

      /* Set the SMI_EVT bit. */
      Write_Host_Status_Reg2(Read_Host_Status_Reg2() |
                             maskSTATUS_PORT2_SMIEVT);

      /* Activate the signal. */
        #if SXI_SUPPORTED
		CLEAR_BIT(HIPM1IE, IESCIE);	/* Disable SCI output enbable. */
		SET_BIT(HIPM1IE, IESMIE);		/* Enable SMI output enable. */
		#endif
      switch ((Cfg0E >> shiftSMI_OP_SELECT) & maskSMI_OP_SELECT)
      {
         case SXI_SET_HI:
            /* Set the SMI pin high. */
            b_temp = SMI_Pin | MASK(bit3);
            Extended_Write_Port(b_temp, 0);
            break;

         case SXI_SET_LO:
            /* Set the SMI pin low. */
            #if SXI_SUPPORTED
            if ((SMI_Pin & 0xF0) == (HW_CODE << 4))
               b_temp = SMI_Pin | MASK(bit3);
            else
            b_temp = SMI_Pin & ~(MASK(bit3));
            #else
            b_temp = SMI_Pin & ~(MASK(bit3));
            #endif
            Extended_Write_Port(b_temp, 0);
            break;

         case SXI_PULSE_HI:
            /* Pulse the SMI pin high. */
            b_temp = SMI_Pin | MASK(bit3);
            Extended_Write_Port(b_temp, SMI_Pulse << 2);
            break;

         case SXI_PULSE_LO:
            /* Pulse the SMI pin low. */
            #if SXI_SUPPORTED
            if ((SMI_Pin & 0xF0) == (HW_CODE << 4))
               b_temp = SMI_Pin | MASK(bit3);
            else
            b_temp = SMI_Pin & ~(MASK(bit3));
            #else
            b_temp = SMI_Pin & ~(MASK(bit3));
            #endif
            Extended_Write_Port(b_temp, SMI_Pulse << 2);
            break;
      }
   }

   index = 0;
   do
   {
      if (Timed_Function[index].timer)
      {
         /* This timer is not zero so it is running. */
         if ((Timed_Function[index].timer >> 5) <= millisecond_timer.field.y)
         {
            if ((Timed_Function[index].timer & 0x1F) == 0)
            {
               Timed_Function[index].timer = 0;
               System_Control_Function(Timed_Function[index].operation, MAKE_EVENT);
            }
            else
               Timed_Function[index].timer--;
         }
      }
      index++;
   }
   while(index < 5);


}

/* ---------------------------------------------------------------
   Name: set_smi_cause

   Setup the cause flag for an SMI and start the SMI Timer.

   smi_number = FIRST_SMI_CAUSE through 255.
      The Host uses this number to determine the cause of the SMI.
   --------------------------------------------------------------- */
static void set_smi_cause(BYTE smi_number)
{
   BYTE index;

   if ( (smi_number >= FIRST_SMI_CAUSE) && (smi_number <= LAST_SMI_CAUSE) )
   {
      /* The SMI number has a flag in the SMI flags array. */

      /* Change cause number FIRST_SMI_CAUSE through 255 to 0 through xxx. */
      smi_number -= FIRST_SMI_CAUSE;

      /* Change SMI cause number to skip SCI causes in array. */
      smi_number += (NUM_SCI_FLAGS_BYTES * 8);

      Cause_Flags[smi_number >> 3] |= Byte_Mask((SMALL) (smi_number & 7));
   }
   else
   {
      /* The SMI number does not have a flag in the SMI flags array.
         Buffer the SMI number in SMI_Event_Buffer if there is room.

         Put the SMI number in the buffer and increment the input
         index.  If the input index equals the output index, the
         buffer is full and the SMI number just placed in the buffer
         will not be used. */

      /* Get the input index. */
      index = SMI_Event_In_Index;

      /* Put the SMI number in the buffer. */
      SMI_Event_Buffer[index] = smi_number;

      /* Increment the index. */
      index++;
      if (index >= EVENT_BUFFER_SIZE) index = 0;

      /* If the buffer is not full, update the input index. */
      if (index != SMI_Event_Out_Index) SMI_Event_In_Index = index;
   }

   /* Pending SMI events are sent when the ACPI Timer
      goes to zero.  This timer counts at a 1 millisecond
      rate.  Setting the timer to 1 will cause the SMI
      event to be generated within 1 mS. */

   smi_timer = 1;
}

/* ---------------------------------------------------------------
   Name: inc_millisecond_timer

   Increments millisecond timer and generates a value for Y.

   This is used by millisecond timers that use the following
   timer setting:

      Byte = yyyx,xxxx

       Y         formula                 value
      ---   -----------------   --------------------------
       0    (X+1) *    1 +  0      1 to    32 microseconds
       1    (X+1) *    3 + 30     33 to   126 microseconds
       2    (X+1) *    1 +  0      1 to    32 milliseconds
       3    (X+1) *   10 +  0     10 to   320 milliseconds
       4    (X+1) *  100 +  0    100 to  3200 milliseconds
       5    (X+1) *  500 +  0    500 to 16000 milliseconds
       6    (X+1) * 1000 +  0   1000 to 32000 milliseconds
       7    (X+1) * 2000 +  0   2000 to 64000 milliseconds

      This routine is meant for timer settings with Y >= 2
      Timer settings with Y set to 0 or 1 will use the following
      formulae.

          Y         formula                 value
         ---   -----------------   --------------------------
          0    (X+1) *    1 +  0      1 to    32 milliseconds
          1    (X+1) *   10 +  0     10 to   320 milliseconds

   Every       millisecond, timers with Y = 0 or 2 are to be updated.
   Every   10 milliseconds, timers with Y = 0 through 3 are to be updated.
   Every  100 milliseconds, timers with Y = 0 through 4 are to be updated.
   Every  500 milliseconds, timers with Y = 0 through 5 are to be updated.
   Every 1000 milliseconds, timers with Y = 0 through 6 are to be updated.
   Every 2000 milliseconds, timers with Y = 0 through 7 are to be updated.

   The millisecond_timer is a WORD configured as follows.

      Digit   Count    Bits
      -----   -----   ------
         1s   0 - 9   0 -  3
        10s   0 - 9   4 -  7
       100s   0 - 4   8 - 10
       500s   0 - 1     11
      1000s   0 - 1     12

      Y is stored in Bits 13 through 15.

   Every millisecond, a new value for Y is generated.  This
   routine will generate a value for Y between 2 and 7, inclusive.
   This value for Y may be used by the timers to decide whether to
   update the timer or not.  If the Y value stored in the timer <= Y,
   the timer is updated.  Note that this procedure will not correctly
   update timers with Y set to 1.
   --------------------------------------------------------------- */
static void inc_millisecond_timer(void)
{
   BYTE y;
   MS_TIMER ms_timer;

   ms_timer.word = millisecond_timer.word;

   y = 2;                                 /* Y = 2. */
   ms_timer.word++;                       /* ms_timer.field.ones++; */
   #if TICKER_1MS
   if (ms_timer.field.ones == 10)
   #else // TICKER_1MS
   if (ms_timer.field.ones == 2)
   #endif // TICKER_1MS
   {
      y++;                                /* Y = 3. */
      ms_timer.field.ones = 0;
      ms_timer.word += 0x0010;            /* ms_timer.field.tens++; */
      if (ms_timer.field.tens == 10)
      {
         y++;                             /* Y = 4. */
         ms_timer.field.tens = 0;
         ms_timer.word += 0x0100;         /* ms_timer.field.hundreds++; */
         if (ms_timer.field.hundreds == 5)
         {
            y++;                          /* Y = 5. */
            ms_timer.field.hundreds = 0;
            ms_timer.word ^= 0x0800;      /* ms_timer.field.fivehundreds++; */
            if (ms_timer.field.fivehundreds == 0)
            {
               y++;                       /* Y = 6. */
               ms_timer.word ^= 0x1000;   /* ms_timer.field.thousands++; */
               if (ms_timer.field.thousands == 0)
               {
                  y++;                    /* Y = 7. */
               }
            }
         }
      }
   }

   ms_timer.field.y = y;
   millisecond_timer.word = ms_timer.word;
}
