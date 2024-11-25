/*-----------------------------------------------------------------------------
 * MODULE CRISIS.C
 *
 * Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 * Revision History (Started August 10, 1999 by GLP)
 *
 * GLP
 * 08/18/99 Added CRISIS_CMD_HANDLER and BOOTABLE_CRISIS switches.
 *          Updated comments.
 * ------------------------------------------------------------------------- */

#define CRISIS_C

/* ----------------------------------------------------------------------------

   This file contains part of the Internal ROM code for crisis recovery.
   Control is passed to Crisis_Reset from OEMCRSIS after intialization
   has been performed.

   Appended to the end of the crisis module is a footer.  This footer
   is used to find the main keyboard controller code.  Since the footer
   is not checksummed it contains a signature word.  If the signature
   is valid, the footer can be used to find the header of the main
   keyboard controller code.  A checksum is performed on the code and
   if the code passes the checksum, a jump is made to the code.

   If the checksum fails, then processing goes to the crisis command
   handler.  This command handler is operated in polled mode using no
   interrupts.  A subset of the standard commands are supported;
   enough to allow the system to boot to DOS with a featureless BIOS.
   Also, the External Memory update commands are supported in the 0xh
   block, as described in the handler.  These may be used by an update
   utility to update the External Memory.

   The main keyboard controller code can also force entry to crisis
   mode via the standard command handler with commands 06h and 07h.
   This is done when the user wants to update the External Memory
   after first doing a normal boot.  Crisis mode is entered via a
   fixed jump vector which is placed after the crisis module header.
   The crisis module header is placed before the crisis module in ROM.
   The system configuration header points to the crisis module header.
   The crisis module header has a pointer to the start of the crisis
   module code.  This actually points to a branch instruction that
   branches to the start of the code.  The branch instruction is 4
   bytes long and after the branch instruction is the pointer to the
   crisis entry location.  This allows the main keyboard controller
   code to access the crisis entry, even if the crisis code evolves
   over time.  Once the main keyboard controller code jumps to the
   crisis entry, the update commands are then supported, so updating
   can begin.  To eliminate duplication, the update commands are NOT
   supported in the standard command handler.

   Once crisis mode is entered, the only return to the main keyboard
   controller code is via an Internal RAM program or system reset.

   Above all, the crisis region must be COMPLETELY independant of
   the External Memory region.  This means there should be no
   INCLUDE files used which could evolve with the main keyboard
   controller code.  Keeping this rule involves some duplication of
   code, but not much.  Similarly, the main keyboard controller code
   should not depend on any of the crisis code.  The only link between
   the two worlds is done through the jump table vector.  This vector
   location must remain fixed!
   --------------------------------------------------------------- */

#include <asm.h>

/* ---------------------------------------------------------------
   BE CAREFUL WHAT YOU INCLUDE HERE !
   --------------------------------------------------------------- */
#include "swtchs.h"
#include "types.h"
#include "regs.h"
#include "defs.h"
#include "purcrsis.h"
#include "crisis.h"
#include "host_if.h"
#include "oem.h"
#include "oemcrsis.h"
#include "timers.h"
#include "purhook.h"
#if RAM_BASED_FLASH_UPDATE
extern void FLASH_UPDATE_init(void);
#else
#include "fu.h"
#endif

#ifndef CRISIS_CMD_HANDLER
#error CRISIS_CMD_HANDLER switch is not defined.
#endif

#ifndef BOOTABLE_CRISIS
#error BOOTABLE_CRISIS switch is not defined.
#endif

#ifndef CRISIS_CMD_0FH
#error CRISIS_CMD_0FH switch is not defined.
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void go_kbc_main(void);
#if CRISIS_CMD_HANDLER
static void do_recovery(void);
 #if CRISIS_CMD_0FH
 static void flash_setup(void);
 #endif
#endif

#define FOOTER_SIGNATURE_WORD 0x4E49

typedef struct _FOOTER_BLK
{
	WORD signature;      /* Byte in lower address  = 0x49,
                           Byte in higher address = 0x4E */

   WORD page;           /* Base Address Configuration (BACFG) or
                           PAGE Register setting. */

   DWORD kbc_header_pntr;/* Pointer to the beginning of the main
                           keyboard controller code header (code label,
                           not absolute address). */
	BYTE reserved1;
	BYTE reserved2;

} FOOTER_BLK;

typedef struct _KBC_BLK
{
    DWORD num_bytes;   /* Number of bytes in code. */

    DWORD start_code;  /* Pointer to the beginning of the main
                        keyboard controller code (code label,
                        not absolute address). */

	BYTE checksum;    /* Byte to force checksum to 0. */

	BYTE reserved;

} KBC_BLK;

/* The interrupt dispatch table is copied into RAM in the .data section. */
#pragma section(".data", _ram_dispatch_table)

/* The variables for the crisis code are located in the .data section. */
#pragma section(".data", bCrisis_Cmd_Num, bCrisis_Cmd_Index)
#pragma section(".data", Crisis_Command_Byte, Crisis_Flags)
#pragma section(".data", wCrisis_Prog_Index, wCrisis_Prog_Start)
#pragma section(".data", wCrisis_Prog_Len)

void (* _ram_dispatch_table[RAM_DISPATCH_SIZE])(void);
BYTE bCrisis_Cmd_Num;
BYTE bCrisis_Cmd_Index;
BITS_8 Crisis_Command_Byte;
BITS_8 Crisis_Flags;
DWORD wCrisis_Prog_Index;
DWORD wCrisis_Prog_Start;
DWORD wCrisis_Prog_Len;
extern void (* OUTSIDE_ADDR[])(void);
extern void Load_Crisis_1ms(void);
extern BYTE CRISIS_END;


/* ---------------------------------------------------------------
   Name: cmdxx - Unused Command

   Use this handler for any commands that are not used.

   --------------------------------------------------------------- */
#define cmdxx() bcrisis_cmd_index = 0;  /* Clear the index. */

/* ---------------------------------------------------------------
   Name: Crisis_Reset

   Get here from OEMCRSIS after some initialization has
   been performed.  All interrupts (except NMI) are disabled.
   --------------------------------------------------------------- */

void Crisis_Reset(void)
{
   register int sp __asm__("sp");

   int i;
   void (* const *source)(void);
   void (* *dest)(void);
   FOOTER_BLK *footer_blk_pntr;

   // init Flash Update functionality
#if RAM_BASED_FLASH_UPDATE
   FLASH_UPDATE_init(); // ram version
#else // RAM_BASED_FLASH_UPDATE
    FLASH_UPDATE_init((unsigned long)&BOOT_BLOCK_ADDRESS, 0);
#endif // RAM_BASED_FLASH_UPDATE


   /* Set so that:
      IRQ 1, 11, and 12 bits will be 0 if IRQ generation is
      turned off, level interrupt is selected, standard ISA
      polarity, and open drain type interrupt signals. */
   HIIRQC = 0;

   /* The interrupt dispatch table is copied into RAM to allow the
      page register to be changed without disturbing the interrupt
      vector mechanism. */

   /* Move interrupt dispatch table to RAM. */
   source = _dispatch_table;
   dest = _ram_dispatch_table;
   for (i = 0; i < RAM_DISPATCH_SIZE; i++) *dest++ = *source++;

   /* Setup the interrupt base register to
      point to the new location of the table. */
   _lprd_("intbase", (long) _ram_dispatch_table);

   if (OEM_crisis_check() == 0)
   {
      /* Check if the footer has a valid signature. */

      /* The footer is at the end of the crisis code.  This is calculated
         from the build and resides at OUTSIDE_ADDR. */
      footer_blk_pntr = (FOOTER_BLK *) &CRISIS_END;
      if ( footer_blk_pntr->signature == FOOTER_SIGNATURE_WORD )
      {
         /*
          The footer signature is valid.  Do a checksum on the code.
         */

          go_kbc_main();
      }
   }

   /* At this point, OEM_crisis_check returned a non-zero value
      (indicating that the OEM wants to run the crisis code) or
      the RAM routine detected a problem. */

   #if CRISIS_CMD_HANDLER
   /* Allow the OEM to run some code. */
   OEM_entering_crisis();

   /* Clear all crisis status flags. */
   Crisis_Flags.byte = 0;
   TWDT0 = OEM_TIMER_A_CNT;
    Load_Crisis_1ms();

   /*
   There is no return from Crisis_Command_Handler() (and also no
   return to the OEMCRSIS.C file).  Since there are no returns, the
   stack pointer is reset to save stack space which would never be
   used and a jump is made. */
   __asm__("movd $__STACK_START,(sp)");
   __asm__("br _Crisis_Command_Handler");

   #else

   /*
   If the crisis command handler is not included, there is no return
   from OEM_entering_crisis.  (and also no return from this routine).
   Since there are no returns, the stack pointer is reset to save
   stack space which would never be used and a jump is made. */
   __asm__("movd $__STACK_START,(sp)");

   /* Allow the OEM to run the rest of the crisis code. */
   __asm__("br _OEM_entering_crisis");

   #endif
}

/* ---------------------------------------------------------------
   Name: go_kbc_main

   Input:   nothing
   Returns: nothing
   --------------------------------------------------------------- */
void go_kbc_main(void)
{
   //register int r6 __asm__("r6");
   register long r12 __asm__("r12");

   BYTE save_page;
   KBC_BLK *kbc_hdr_pntr;
   BYTE *chk_pntr;
   SMALL checksum;
   DWORD length;
   FOOTER_BLK *footer_blk_pntr;

   /* Check if the checksum of the Keyboard Controller Header and code
      is OK. */
     footer_blk_pntr = (FOOTER_BLK *) &CRISIS_END;
   /* Point to the Keyboard Controller Header.  The kbc_header_pntr
      is a code_label pointer (as opposed to an absolute pointer). */
   kbc_hdr_pntr = (KBC_BLK *) (footer_blk_pntr->kbc_header_pntr << 1);

   /* Set the base address to allow access
      to the Keyboard Controller Header. */

   /* At this point, the crisis code can no longer be
      read since the page register may not be correct. */

   length = kbc_hdr_pntr->num_bytes;
   chk_pntr = (BYTE *) kbc_hdr_pntr;
   checksum = 0;
   do
   {
      checksum += *chk_pntr++;
      length--;
   }
   while (length);

   /* If the main keyboard controller code is OK, transfer control to it. */
   if ((checksum & 0xFF) == 0)
   {
      /* Get the start_code value. */
      r12 = (long) kbc_hdr_pntr->start_code;
      /* Use the following test to get the compiler to
         generate code for the previous "r6 = " instruction.
         (The compiler cannot see that r6 is
         being used in the "jump r6" instruction.)
         This will not work if r6 is 0.  Since the Keyboard
         Controller Header is placed before the code, this is OK.
         Jump to start_external_code(). */
      if (r12) __asm__("jump (r12)");
   }

   /* Set the base address to allow a return to the crisis code. */
//   BACFG = save_page;
}


#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Do_Crisis_Recovery

   Here, crisis mode is entered.  Control is passed to here because
   cmd 06h or 07h was invoked.

   Disable all irqs for the duration because the code will only
   operate in polled mode from now on.  Support some standard
   commands and RAM load and execute commands.

   Input:   nothing
   Returns: nothing
   --------------------------------------------------------------- */
void Do_Crisis_Recovery(void)
{
   /* Entered here from Command 6 or 7.  The state of the memory
      (the memory map) is unknown.  The memory map is to be set up.

      Assume the interrupt stack pointer and the interrupt vector
      table are set so that nonmaskable interrupts are handled.

      No local variables are used, so the stack pointer may be set.

      Now, ensure that interrupts are
      disabled and reset the stack pointers. */

   __asm__("di");
   __asm__("movd $__ISTACK_START,(r1, r0)");

   __asm__("movd $__STACK_START,(sp)");
   /* At this point, the stack pointer has been set.  If the interrupt
      stack pointer is set to a location within this new stack and
      the stack pointer is used in the nonmaskable interrupt routine,
      there could be a problem if an interrupt occurs before the next
      instruction. */

   __asm__("lprd (r12),isp");

   __asm__("br _do_recovery");
}
static void do_recovery(void)
{
   int i;
   void (* const *source)(void);
   void (* *dest)(void);

   /* Move interrupt dispatch table to RAM. */
   source = _dispatch_table;
   dest = _ram_dispatch_table;
   for (i = 0; i < RAM_DISPATCH_SIZE; i++) *dest++ = *source++;
   /* Setup the interrupt base register to
      point to the new location of the table. */
   _lprd_("intbase", (long) _ram_dispatch_table);

   /* No return from Crisis_Command_Handler(). */
   __asm__("movd $__STACK_START,(sp)");
   __asm__("br _Crisis_Command_Handler");
}
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Crisis_Init_Cmd_Hndlr

   Performs initialization prior to running the crisis
   command handler.

   Initializes the Host interface status port.  Enable the
   keyboard (set the inhibit switch bit to off, disable the
   password) to allow the Host to run.

   Also, performs other initialization such as turning on
   Fast A20 support.

   --------------------------------------------------------------- */
void Crisis_Init_Cmd_Hndlr(void)
{
    OEM_Hookc_Crisis_Init_Cmd_Hndlr();
    HIKMST = maskSTATUS_PORT_ENABLE;

      /* Disable IRQ1 generation so Host will not be
      sent an IRQ1 with data through the Host interface. */
   Disable_Host_IRQ1();
}
#endif

#if CRISIS_CMD_HANDLER
#if BOOTABLE_CRISIS
/* ---------------------------------------------------------------
   Name: CmdD1 - Write Output Port

   Write to 8042 output port 2.

      byte 1 - data

      Bit 7 = Keyboard data output line (inverted on PS/2).
      Bit 6 = Keyboard clock output line.
      Bit 5 = Input buffer empty (auxiliary interrupt IRQ12 on PS/2).
      Bit 4 = Output buffer full (Generates IRQ1).
      Bit 3 = Reserved (inverted auxiliary clock output line on PS/2).
      Bit 2 = Reserved (inverted auxiliary data output line on PS/2).
      Bit 1 = Gate A20.
      Bit 0 = System reset.

      Just send bit 1 of data to Gate A20 output.

   Input:
      if bCrisis_Cmd_Index = 0, host_byte = command byte
      else host_byte = data byte.

   Returns:
      updated bCrisis_Cmd_Index
   --------------------------------------------------------------- */
void CmdD1(BYTE host_byte)
{
   if (bCrisis_Cmd_Index == 0)
      bCrisis_Cmd_Index++; /* Increment the index when the
                              command byte is received. */
   else
   {
      /* Otherwise, this is the data byte. */

      /* Put bit 1 into Gate A20 bit. */

      if (host_byte & 0x02) OEM_A20_High();  /* Force Gate A20 high. */
      else                  OEM_A20_Low();   /* Force Gate A20 low. */

      bCrisis_Cmd_Index = 0;  /* Clear the index. */
   }
}
#endif
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Cmd08 - Download Program to RAM

   Put the program into RAM.

      byte 1 - Starting Address (high byte)
      byte 2 - Starting Address (low byte)
      byte 3 - Program Length (high byte) (1-based)
      byte 4 - Program Length (low byte)  (1-based)
      byte 5, 6, ... - Actual Program Bytes

   Input:
      if bCrisis_Cmd_Index = 0, host_byte = command byte
      else host_byte = data byte.

   Returns:
      updated bCrisis_Cmd_Index
   --------------------------------------------------------------- */
void Cmd08(BYTE host_byte)
{
   BYTE *mem_ptr;

   switch (bCrisis_Cmd_Index)
   {
      case 0:
         wCrisis_Prog_Index = 0; /* Clear the download byte index */
         bCrisis_Cmd_Index++;    /* and increment the command index */
         break;                  /* when the command byte is received. */

      case 1:
         /* First byte of start address. */
         wCrisis_Prog_Start = host_byte;
         bCrisis_Cmd_Index++;
         break;

      case 2:
         /* Second byte of start address. */
         wCrisis_Prog_Start = (wCrisis_Prog_Start << 8) + host_byte;
         bCrisis_Cmd_Index++;
         break;

      case 3:
         /* Third byte of start address. */
         wCrisis_Prog_Start = (wCrisis_Prog_Start << 8) + host_byte;
         bCrisis_Cmd_Index++;
         break;

      case 4:
         /* High byte of program length. */
         wCrisis_Prog_Len = host_byte;
         bCrisis_Cmd_Index++;
         break;

      case 5:
         /* Low byte of program length. */
         wCrisis_Prog_Len = ((WORD) wCrisis_Prog_Len << 8) + host_byte;
         bCrisis_Cmd_Index++;
         break;

      default:
         /* This is one of the actual program bytes. */

         /* mem_ptr = location to put this byte. */
         mem_ptr = (BYTE *) (wCrisis_Prog_Start + wCrisis_Prog_Index);

         *mem_ptr = host_byte;   /* Store data. */

         wCrisis_Prog_Index++;   /* Increment program byte index (0-based). */

         if (wCrisis_Prog_Index >= wCrisis_Prog_Len)
         {
            /* If program index >= program length (1-based) */

            bCrisis_Cmd_Index = 0;  /* Clear the index. */
         }
         else
         {
            ;  /* Leave bCrisis_Cmd_Index at 5. */
         }
         break;
   }
}
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Cmd09 - Execute RAM Program

   Do a subroutine call to RAM at the specified address.

      byte 1 - Starting Address (high byte)
      byte 2 - Starting Address (low byte)

   Input:
      if bCrisis_Cmd_Index = 0, host_byte = command byte
      else host_byte = data byte.

   Returns:
      updated bCrisis_Cmd_Index
   --------------------------------------------------------------- */
void Cmd09(BYTE host_byte)
{
   FUNCT_PTR_V_V funct_ptr;

   switch (bCrisis_Cmd_Index)
   {
      case 0:
         bCrisis_Cmd_Index++;    /* Increment the command index */
         break;                  /* when the command byte is received. */

      case 1:
         /* First byte of start address. */
         wCrisis_Prog_Start = host_byte;
         bCrisis_Cmd_Index++;
         break;

      case 2:
         /* Second byte of start address. */
         wCrisis_Prog_Start = (wCrisis_Prog_Start << 8) + host_byte;
         bCrisis_Cmd_Index++;
         break;

      default:
         /* Third byte of start address.  Notice that the
            (previously loaded) high bytes is only shifted
            7 bits.  Bit 0 of the low byte is always expected
            to be zero.  (It is a word-aligned address.) */
         wCrisis_Prog_Start =
            (wCrisis_Prog_Start << 7) + (host_byte >> 1);

         /* Call the function. */
         funct_ptr = (FUNCT_PTR_V_V) wCrisis_Prog_Start;
         funct_ptr();

         bCrisis_Cmd_Index = 0;  /* Clear the index. */
         break;
   }
}
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Cmd0A -

   Reserved for Reset command.
   --------------------------------------------------------------- */
void Cmd0A(void)
{
   ;
}
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Cmd0B -

   Reserved for Get First Flashable Address.
   --------------------------------------------------------------- */
void Cmd0B(void)
{
   ;
}
#endif

#if CRISIS_CMD_HANDLER
#if CRISIS_CMD_0FH
/* ---------------------------------------------------------------
   Name: Cmd0F -

   Setup for Host to flash external PROM.
   --------------------------------------------------------------- */
void Cmd0F(void)
{  /*
   This assumes that the code is location independant.  The code in
   ROM at location _ram_flash_setup will be moved to RAM in the heap. */
   /* R3 is the source pointer. */
   __asm__("movd  $_ram_flash_setup,(r13)");

   /* R2 is the destination pointer. */
   __asm__("movd  $_HEAP$_START,(r12)");

   /* R1 is used to stop the transfer. */
   __asm__("movd  $_ram_end_program,(ra)");
   __asm__("lshd  $1:s,(ra)");
   __asm__("lshd  $1:s,(r13)");

   __asm__("cmd0f_loop:");

   /* Get a word from ROM. */
   __asm__("loadw 0(r13),r0");

   /* Store the word in RAM. */
   __asm__("storw r0,0(r12)");

   /* Bump the source and destination pointers. */
   __asm__("addd  $2,(r13)");
   __asm__("addd  $2,(r12)");

   /* Loop if not done. */
   __asm__("cmpd  (ra),(r13)");
   __asm__("bne   cmd0f_loop");

   /* Jump to routine in RAM. */
   __asm__("movd  $_HEAP$_START,(r12)");
   __asm__("lshd  $-1:s,(r12)");
   __asm__("jump  (r12)");
}

void ram_flash_setup(void)
{
   /* The following is the program that is loaded into RAM. */

   /* Clear error and aux device bits from status port. */
   CLEAR_BIT(HIKMST,(maskSTATUS_PORT_PARITY  |  /* Parity error. */
        maskSTATUS_PORT_GENTMO  |  /* General time out. */
        maskSTATUS_PORT_SENDTMO |  /* AT's send time out. */
        maskSTATUS_PORT_AUXOBF));   /* PS/2's aux device bit. */

   /* Disable IRQ1 generation so Host will not be sent an
      IRQ1 with the data.  It is assumed that when the
      interrupt is disabled, the interrupt signal stays low. */

   CLEAR_BIT(HICTRL, HICTRL_OBFKIE);

   /* Put 55h in output buffer. */
   HIKDO = 0x55;

   /* Wait for a byte from the Host (command or data). */
   while ( IS_BIT_CLEAR(HIKMST, maskSTATUS_PORT_IBF)) ;

   /* Setup watchdog timer. */

   TWCP = TWCP_MDIV & 0;/* 32.768 KHz divided by 2**0,
                           causes the prescaler to pass
                           the signal to the timer. */
   TWDT0 = 0x1FFF;      /* Set for 1/4 second output. */
   TWCFG = 0;           /* Select T0OUT as the watchdog clock. */

   WDCNT = HIKMDI;      /* Watchdog will reset in
                           1/4 * (data sent by host) seconds. */

   /* Loop until watchdog times out. */
   while (1)
   {
      ;
   }
}
void ram_end_program(void)
{  /*
   The ram_end_program routine follows the ram_flash_setup
   routine.  The ram_end_program label will be used to
   determine the size of the routine to send to RAM. */
   ;
}
#endif
#endif

#if CRISIS_CMD_HANDLER
/* ---------------------------------------------------------------
   Name: Crisis_Data_To_Host - Send data to the Host

   Clears error bits in the Host Interface status port and
   sends a command response byte or a byte of keyboard data
   to the Host.  A Host IRQ1 is generated if the hardware is
   setup to send it.

   The hardware should have been setup to send a Host IRQ1
   if keyboard interrupts are enabled in controller command
   byte.  Support for IRQ generation is needed to get through
   POST because POST checks for this!  (Normally, generating
   IRQs will not be needed here.)

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
void Crisis_Data_To_Host(BYTE data)
{
    /* Wait until Host has taken last data byte. */
    while (Output_Buffer_Full()) ;

    /* Clear error and aux device bits from status port. */
    CLEAR_BIT(HIKMST, (maskSTATUS_PORT_PARITY  |  /* Parity error. */
                maskSTATUS_PORT_GENTMO  |  /* General time out error. */
                maskSTATUS_PORT_SENDTMO |  /* Send time out error for AT. */
                maskSTATUS_PORT_AUXOBF));   /* Aux device for PS/2. */

    HIKDO = data;  /* Put data in output buffer. */
}
#endif

#if CRISIS_CMD_HANDLER
#if BOOTABLE_CRISIS
/* ---------------------------------------------------------------
   Name: Crisis_Aux_Data_To_Host

   Send auxiliary device (mouse) data to the Host.

   Clears error bits in the Host Interface status port and
   sends a byte of auxiliary device (mouse) data to the
   Host.  A Host IRQ12 is generated if the hardware is setup
   to send it.

   The hardware should have been setup to send a Host IRQ1
   if auxiliary device (mouse) interrupts are enabled in the
   controller command byte.  Although, POST never requires
   support for IRQ generation from the auxiliary device (mouse).

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
void Crisis_Aux_Data_To_Host(BYTE data)
{
   /* Wait until Host has taken last data byte. */
   while (Output_Buffer_Full()) ;

   /* Clear error bits from status port. */
   CLEAR_BIT(HIKMST, (maskSTATUS_PORT_PARITY  |  /* Parity error. */
               maskSTATUS_PORT_GENTMO  |  /* General time out error. */
               maskSTATUS_PORT_SENDTMO));  /* Send time out error for AT. */

   /* Set aux device bit in status port. */
   SET_BIT(HIKMST, maskSTATUS_PORT_AUXOBF);      /* Aux device for PS/2. */

   HIMDO = data;  /* Put data in mouse output buffer. */
}
#endif
#endif
