# -----------------------------------------------------------------------------
# MODULE RESET.S
#
# Copyright(c) 1995-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
# -----------------------------------------------------------------------------
#
# This file contains code used to perform initialization after a
# reset.  Based on PC87570 code version 2.00.07[2667-21]-n.  This
# is for use with version 2.0 and up of the CR16 tools.
#
# Revision History (Started March 16, 2000 by GLP)
#
# -----------------------------------------------------------------------------

   .include	..\oem\project\inc\swtchs.h
   .include	..\oem\project\inc\copyr.h

   .text
   .align	2

   # Control is passed here from a CPU reset (location 0).

   .globl	start
   .code_label	start
   .code_label	skip_jbb
start:
   br           skip_jbb

   .globl       jbb_entry_point
   .code_label  jbb_entry_point
jbb_entry_point:
   movw         $0xAA55, r1
   storw        r1, 0x17FBE


# Notes on the Non-Maskable Interrupt (NMI):
#
# After reset, NMI is disabled and must remain disabled until the
# software initializes the interrupt table, interrupt base and the
# interrupt mode.  The ICU processes the PFAIL pin to send to the
# CR16B NMI input.
#
# The external PFAIL interrupt is enabled by setting the ENLCK bit and
# remains enabled until a reset occurs.  This allows the external NMI
# feature to be enabled only after the Interrupt Base Register and the
# interrupt stack pointer have been set up.
#
# Alternatively, the external PFAIL interrupt can be enabled by setting
# the EN bit, which remains enabled until an interrupt event or a reset
# occurs.  The NMISTAT register holds the status of the current pending
# NMI request.  When the bit in NMISTAT is set to 1, an NMI request to
# the core is issued.  NMISTAT is cleared each time its contents are
# read.
#
# Because the Non-Maskable Interrupt is, actually, maskable at reset,
# the setup of the INTBASE register and interrupt stack pointer can be
# delayed.

   # On reset, the PSR (Processor Status Register) is cleared to
   # 0 except for PSR.E, which is set to 1.  Clear PSR.E (disable
   # interrupts).
skip_jbb:
   di

        # Setup the program stack pointer and interrupt
        # stack pointer.  The interrupt stack must reside
        # at a different address than the program stack.

   movd	$__STACK_START, (sp)	# Initialize the
                                        # Stack Pointer register.

   movd	$__ISTACK_START, (r1,r0)	# Initialize the
   lprd	(r1,r0), isp			# Interrupt Stack Pointer register.

 #----------------------------------------------------------------------#
 # disable all interrupts by clearing PSR                               #
 #----------------------------------------------------------------------#

    movw    $0, r0
    lpr     r0, psr

   # In order to debug the program, all the trap handlers must be
   # put into the dispatch table.  This is done by using the TMON
   # SVC trap with 0x101 as a parameter.  TMON will then copy all
   # the addresses of the trap handlers routines into the dispatch
   # table pointed to by the __dispatch_table constant variable.
   # When moving from the development phase to the product phase,
   # the line "excp svc" should be modified to "nop".
 #----------------------------------------------------------------------#
 # Set the Extended Dispatch bit in the CFG register (this is the       #
 # default configuration for CR16C) and enable instruction cache        #
 #----------------------------------------------------------------------#
   spr     cfg, r0	# copy cfg to r0
   orw    $0x100, r0	# set ED, do not enable instruction cache when running from MRAM
.if {RUN_FROM_MRAM}
   andw    $0x100, r0
.else
   orw     $0x010, r0	# set ED, IC
.endif
   lpr     r0, cfg	#load r0 to cfg register
 #----------------------------------------------------------------------#
 # Initialize the intbase (pointer to the dispatch table).              #
 #----------------------------------------------------------------------#
   movd	$__dispatch_table, (r1,r0)
   lprd	(r1,r0), intbase

 #----------------------------------------------------------------------#
 # Add here code for incorrect checksum handling                        #
 #----------------------------------------------------------------------#

 #----------------------------------------------------------------------#
 # jump to the main function                                            #
 #----------------------------------------------------------------------#

   br	_OEM_Reset              # Allow the OEM to do
                                # some initialization.



   .globl	__eop
   .code_label	__eop
__eop::
        br __eop


   INLINE_COPYRIGHT
   .align	2  # Align code to even address.
               # This is done in case the copyright
               # message causes the next address
               # to be an odd address.

