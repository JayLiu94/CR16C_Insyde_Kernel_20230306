/*-----------------------------------------------------------------------------
 * MODULE PROC.H
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
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 21, 2000 by GLP)
 *
 * ------------------------------------------------------------------------- */

#ifndef PROC_H
#define PROC_H

#include <asm.h>

#include "types.h"
#include "icu.h"

void Chip_Low_Power_Enter(void);
void Chip_Low_Power_Exit(void);
FLAG Get_Odd_Parity(SMALL data);
FLAG Get_Irq_Enable_Flag(void);
BYTE Read_Input_Port_1(void);
BYTE Read_Output_Port_2(void);
void Write_Output_Port_2(BYTE data);
BYTE Extended_Read_Port(BYTE data, FLAG output);
void Extended_Write_Port(BYTE data, WORD option);

/* ---------------------------------------------------------------
   Definitions for port_write_table.

   When any of these are changed, check
   the port code definitions below.

   PWT_x_SHADOW_RD is the read shadow location.
   PWT_x_PORT      is the register location.
   PWT_x_SHADOW_WR is the write shadow location

   Up to 16 ports of 8 bits can be defined.  Entries for ports
   with invalid numbers or that should not be written to are
   filled with 0's.  If the port is a read/write port, the read
   shadow location is the port itself and the write shadow
   location is set to 0.  If the port is a write only port, the
   read shadow location is a RAM location to be read and the
   write shadow location is the same RAM location to keep a copy
   of the port data.  If the port can be read at a different
   location than the port itself, the read shadow location will
   contain this port address and the write shadow location will
   be set to 0.
   --------------------------------------------------------------- */

/* Port 0 is setup for port code 0h. */
#define PWT_0_SHADOW_RD P0DOUT_ADDR
#define PWT_0_PORT      P0DOUT_ADDR
#define PWT_0_SHADOW_WR 0

/* Port 1 is setup for port code 1h. */
#define PWT_1_SHADOW_RD P1DOUT_ADDR
#define PWT_1_PORT      P1DOUT_ADDR
#define PWT_1_SHADOW_WR 0

/* Port 2 is setup for port code 2h. */
#define PWT_2_SHADOW_RD P2DOUT_ADDR
#define PWT_2_PORT      P2DOUT_ADDR
#define PWT_2_SHADOW_WR 0

/* Port 3 is setup for port code 3h. */
#define PWT_3_SHADOW_RD P3DOUT_ADDR
#define PWT_3_PORT      P3DOUT_ADDR
#define PWT_3_SHADOW_WR 0

/* Port 4 for port code 4h. */
#define PWT_4_SHADOW_RD P4DOUT_ADDR
#define PWT_4_PORT      P4DOUT_ADDR
#define PWT_4_SHADOW_WR 0

/* Port 5 is setup for port code 5h. */
#define PWT_5_SHADOW_RD P5DOUT_ADDR
#define PWT_5_PORT      P5DOUT_ADDR
#define PWT_5_SHADOW_WR 0

/* Port 6 is setup for port code 6h. */
#define PWT_6_SHADOW_RD P6DOUT_ADDR
#define PWT_6_PORT      P6DOUT_ADDR
#define PWT_6_SHADOW_WR 0

/* Port 7 is setup for port code 7h. */
#define PWT_7_SHADOW_RD P7DOUT_ADDR
#define PWT_7_PORT      P7DOUT_ADDR
#define PWT_7_SHADOW_WR 0

/* Port 8 is setup for port code 8h. */
#define PWT_8_SHADOW_RD P8DOUT_ADDR
#define PWT_8_PORT      P8DOUT_ADDR
#define PWT_8_SHADOW_WR 0

/* Port 9 is setup for port code 9h. */
#define PWT_9_SHADOW_RD P9DOUT_ADDR
#define PWT_9_PORT      P9DOUT_ADDR
#define PWT_9_SHADOW_WR 0

/* Port A is setup for port code Ah. */
#define PWT_A_SHADOW_RD PADOUT_ADDR
#define PWT_A_PORT      PADOUT_ADDR
#define PWT_A_SHADOW_WR 0

/* Port B is setup for port code Bh. */
#define PWT_B_SHADOW_RD PBDOUT_ADDR
#define PWT_B_PORT      PBDOUT_ADDR
#define PWT_B_SHADOW_WR 0

/* Port C is setup for port code Ch. */
#define PWT_C_SHADOW_RD PCDOUT_ADDR
#define PWT_C_PORT      PCDOUT_ADDR
#define PWT_C_SHADOW_WR 0

/* Port D is setup for port code Dh. */
#define PWT_D_SHADOW_RD PDDOUT_ADDR
#define PWT_D_PORT      PDDOUT_ADDR
#define PWT_D_SHADOW_WR 0

/* Port E is setup for port code Eh. */
#define PWT_E_SHADOW_RD PEDOUT_ADDR
#define PWT_E_PORT      PEDOUT_ADDR
#define PWT_E_SHADOW_WR 0

/* Port F is setup for port code Fh. */
#define PWT_F_SHADOW_RD HIIRQC_ADDR
#define PWT_F_PORT      HIIRQC_ADDR
#define PWT_F_SHADOW_WR 0


/* ---------------------------------------------------------------
   Port code definitions.

   To allow a code to be used when calling
   the Extended_Write_Port routine.
   --------------------------------------------------------------- */
#define P0_CODE  0   /* Port code  0 used for port 0. */
#define P1_CODE  1   /* Port code  1 used for port 1. */
#define P2_CODE  2   /* Port code  2 used for port 2. */
#define P3_CODE  3   /* Port code  3 used for port 3. */
#define P4_CODE  4   /* Port code  4 used for port 4. */
#define P5_CODE  5   /* Port code  5 used for port 5. */
#define P6_CODE  6   /* Port code  6 used for port 6. */
#define P7_CODE  7   /* Port code  7 used for port 7. */
#define P8_CODE  8   /* Port code  8 used for port 8. */
#define P9_CODE  9   /* Port code  9 used for port 9. */
#define PA_CODE  10  /* Port code  A used for port A. */
#define PB_CODE  11  /* Port code  B used for port B. */
#define PC_CODE  12  /* Port code  C used for port C. */
#define PD_CODE  13  /* Port code  D used for port D. */
#define PE_CODE  14  /* Port code  E used for port E. */

#define HW_CODE 15   /* Port code 15 used for control SMI,SCI. */

/* ---------------------------------------------------------------
   UNUSED_PIN is used to define a port and pin that is not used.
   This will be used in the flasher code to enable a flasher to be
   disconnected.  It will also be used in other places to
   initialize a pin definition that is not used.  For instance,
   one of the possible monitored inputs that is unused.
   --------------------------------------------------------------- */
#define UNUSED_PIN ((0x0F << 4) | 7)

/* ---------------------------------------------------------------
   EXT_IRQ_HI and EXT_IRQ_LO are used to define an input port code
   that is used for external interrupt definitions.
   --------------------------------------------------------------- */
#define EXT_IRQ_HI (0x0C)
#define EXT_IRQ_LO (0x0D)

/* ---------------------------------------------------------------
   PSR bit.
   --------------------------------------------------------------- */
#define PSR_E_BIT  9

/* ---------------------------------------------------------------
   Name: Wait

   Used to put the processor in wait mode.

   The idle state is the light-sleep state.  After flipping the
   required switches, a WAIT instruction of some sort is used
   to put the processor in this state.  It can exit this state,
   returning to the ACTIVE state, quickly enough to respond to
   any interrupt event: a timer flag, a local keystroke, a
   byte from a PS2 device, a host write to a command register.

   C prototype:
      void Wait(void);
   --------------------------------------------------------------- */
#define Wait() _eiwait_()



/* ---------------------------------------------------------------
   Name: Idle

   Used to put the processor in idle mode.

   The idle state is the middle-sleep state.  After flipping the
   required switches, a WAIT instruction with PMSR.IDLE set is used
   to put the processor in this state.  It can exit this state,
   returning to the ACTIVE state, quickly enough to respond to
   external interrupt events which defined in MIWU.

   C prototype:
      void Idle(void);
   --------------------------------------------------------------- */
#define Idle() SET_BIT(PMCSR, PMCSR_IDLE); _eiwait_()

/* ---------------------------------------------------------------
   Name: Low_Power

   This routine is used to:
      enable interrupts,
      put the processor in low power mode, and
      on exit from low power mode, disable interrupts.

   On entry, interrupts are disabled.

   The low power state is the deep-sleep state.  This is used
   as a suspend of the Keyboard Controller.  The command may
   shut off the processor's oscillator and most of its resources,
   and the processor will enter a state in which only its RAM is
   presumed preserved.  It can be awakened by some action, but
   some subsystems may have to be reinitialized.  It may take a
   longer time to wake up from the low power state than it does
   to wake up from the idle state.

   The wake up may be an interrupt or a reset.  If the processor
   has the capability of telling when a reset comes after a low
   power instruction or after a power on, then this routine
   is the opportunity to set that status.

   The Low_Power_Mode flag is used to check if the processor has
   just come from a low power mode.  (It does not check if the
   processor is in a low power mode, it checks if it just returned
   from that mode.)  This is used as an indication of a valid wake
   up from a low power mode.  The interrupt service routines check
   if the processor was in a low power mode.  If it was, the wake
   up source is checked to see if this is a valid wake up
   interrupt.  If it is, The Low_Power_Mode flag will be cleared
   to indicate that this is a valid wake up.  In other
   words, this is a signal that the processor should not
   be placed back into the low power mode.

   The Low_Power_Mode flag was set prior to calling this routine.

   C prototype:
      void Low_Power(void);
   --------------------------------------------------------------- */
/* First, flag that processor is in (or about to be in) low power mode.
   Then, go into low power mode. */

#define Low_Power()  SET_BIT(PMCSR, PMCSR_IDLE | PMCSR_DHF); _eiwait_(); _di_()

/* ---------------------------------------------------------------
   Name: Disable_Irq, Enable_Irq, Get_Irq_Enable_Flag

   Used to disable, enable, or read the state of the interrupt mechanism.

   C prototypes:
      void Disable_Irq(void);
      void Enable_Irq(void);
      FLAG Get_Irq_Enable_Flag(void);
   --------------------------------------------------------------- */
#define Disable_Irq()   _di_()
#define Enable_Irq()    _ei_()
FLAG Get_Irq_Enable_Flag(void);

/* ---------------------------------------------------------------
   Name: Byte_Mask, Word_Mask, Cpl_Byte_Mask, Cpl_Word_Mask

   Used to create a one-bit mask from a bit number.

   C prototypes:
      BYTE Byte_Mask(SMALL bit_number);
      WORD Word_Mask(SMALL bit_number);
      BYTE Cpl_Byte_Mask(SMALL bit_number);
      WORD Cpl_Word_Mask(SMALL bit_number);
   --------------------------------------------------------------- */
#define Byte_Mask(x)       (1 << (x))
#define Word_Mask(x)       (1 << (x))
#define Cpl_Byte_Mask(x) (~(1 << (x)))
#define Cpl_Word_Mask(x) (~(1 << (x)))

/* ---------------------------------------------------------------
   Name: Read_Byte_Port, Write_Byte_Port

   Used to read or write an 8-bit port.

   For a memory mapped microprocessor, these functions are
   simple memory access C instructions.  But for an I/O mapped
   microprocessor these functions provide a place for the
   appropriate I/O instructions to allow access to the ports.

   C prototypes:
      BYTE Read_Byte_Port(PORT_BYTE_PNTR address);
      void Write_Byte_Port(PORT_BYTE_PNTR address, BYTE data);

   Input: address is the location of the port.
          data is the byte of data to write for Write_Port.

   Returns: a byte of data for Read_Port.
   --------------------------------------------------------------- */
#define Read_Byte_Port(address) *address
#define Write_Byte_Port(address, data) *address = data



#endif /* ifndef PROC_H */

