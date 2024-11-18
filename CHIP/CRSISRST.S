# -----------------------------------------------------------------------------
# MODULE CRSISRST.S
#
# Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
# -----------------------------------------------------------------------------
#
# This file contains code used to perform initialization after a reset for the
# crisis module.
#
# Revision History (Started August 6, 1999 by GLP)
#
# GLP
# 08/17/99  Added vector table with pointer to
#     Do_Crisis_Recovery.
# 09/02/99  Added conditional assembly to allow code to be
#     removed when CRISIS_CMD_HANDLER is FALSE.
# -----------------------------------------------------------------------------

   .include ..\oem\project\inc\swtchs.h

   .text
   .align   2

   # Control is passed here from a CPU reset (location 0).

   .globl   start
start:
        .if {CRISIS_CMD_HANDLER}
   br over  # 4 bytes are allowed for this instruction.
   nop      # This allows the crisis vectors to be at a
            # fixed location from the start label.

   # The following are vector(s) into the crisis module.

   .code_label _Do_Crisis_Recovery
   .double _Do_Crisis_Recovery

over:
        .endif

        # The non-maskable interrupt may be recognized
        # at any time.  Setup the INTBASE register
        # and stack pointers as soon as possible.

        # Initialize the INTBASE register to
        # point to the interrupt dispatch table.
   movd  $__dispatch_table,(r1,r0)
   lprd   (r1,r0),intbase

        # Setup the program stack pointer and interrupt
        # stack pointer.  The interrupt stack must reside
        # at a different address than the program stack.

   movd  $__STACK_START, (sp) # Initialize the
                                        # Stack Pointer register.

   movd  $__ISTACK_START,(r1,r0)   # Initialize the
   lprd  (r1,r0),isp         # Interrupt Stack Pointer register.

        # Initialize the Processor Status Register (PSR).
        # Ensure that the interrupt enable bits are off.
   movw  $0,r0
   lpr   r0,psr

   br _OEM_Crisis_Reset # Allow the OEM to do
                        # some initialization.
