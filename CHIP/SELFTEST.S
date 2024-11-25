# -----------------------------------------------------------------------------
# MODULE SELFTEST.S
#
# Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
# -----------------------------------------------------------------------------
#
# This file contains code used to perform the self test.
#
# Revision History (Started December 11, 1996 by GLP)
#
# GLP
# 19 Aug 99	Updated copyright.
# -----------------------------------------------------------------------------

   .text
   .align	2

# -----------------------------------------------------------------------------
# MACRO Name: TEST_REG
#
# Tests a register by writing "checkerboard" data patterns
# (AA and 55), and then reading back the data (for each pattern)
# to check data integrity.
#
# Returns: Zero flag clear if bad register.
# -----------------------------------------------------------------------------

        .macro TEST_REG regname
        movw    $0xAAAA,{regname}
        cmpw    $0xAAAA,{regname}
        bne     1f

        movw    $0x5555,{regname}
        cmpw    $0x5555,{regname}
1:
        .endm

# -----------------------------------------------------------------------------
# Name: Self_Test
#
# Test registers, flags, and RAM.
#
# Input: Enter with interrupts disabled.
#
# Returns: 0x55 if test passed, 0 if test failed.
#
# C prototype:
#    WORD Self_Test(void);
# -----------------------------------------------------------------------------

# This is a C routine coded in assembly language.

   .globl      _Self_Test
_Self_Test:
         addd    $0xfffffffc,(sp)
         stord   (ra),0:s(sp)

# The flags are checked first to allow them to be used in the test.
# At this point, it is assumed that R0 is OK.

   spr	psr,r0          # Get the Processor Status Register.
   orw	$0xE5,r0        # Set Carry, Low, Flag,
   lpr	r0,psr          # Zero, and Negative bits.

        bcc     bad_test        # Carry    (bit 0)
        bls     bad_test        # Low      (bit 2)
        bfc     bad_test        # Flag     (bit 5)
        bne     bad_test        # Zero     (bit 6)
        ble     bad_test        # Negative (bit 7)

   andw	$0xFF1A,r0      # Clear Carry, Low, Flag,
   lpr	r0,psr          # Zero, and Negative bits.

        bcs     bad_test        # Carry    (bit 0)
        bhi     bad_test        # Low      (bit 2)
        bfs     bad_test        # Flag     (bit 5)
        beq     bad_test        # Zero     (bit 6)
        bgt     bad_test        # Negative (bit 7)

# Do not test the stack pointers.

# R0 through R6 are scratch registers and may be modified.

        TEST_REG r0
        bne     bad_test

        TEST_REG r1
        bne     bad_test

        TEST_REG r2
        bne     bad_test

        TEST_REG r3
        bne     bad_test

        TEST_REG r4
        bne     bad_test

        TEST_REG r5
        bne     bad_test

        TEST_REG r6
        bne     bad_test

# R7 through R13 must be saved.

        movw    r7,r0
        TEST_REG r7
        movw    r0,r7
        bne     bad_test

        movw    r8,r0
        TEST_REG r8
        movw    r0,r8
        bne     bad_test

        movw    r9,r0
        TEST_REG r9
        movw    r0,r9
        bne     bad_test

        movw    r10,r0
        TEST_REG r10
        movw    r0,r10
        bne     bad_test

        movw    r11,r0
        TEST_REG r11
        movw    r0,r11
        bne     bad_test

        movw    r12,r0
        TEST_REG r12
        movw    r0,r12
        bne     bad_test

        movw    r13,r0
        TEST_REG r13
        movw    r0,r13
        bne     bad_test

        movd    $_RAM_TEST_START,(r12)
        movd    $_RAM_TEST_END,(r13)
        bal     (ra),test_ram
        cmpw    $0,r0
        bne     bad_test

        # Tests passed.

        movw    $0x55,r0        # R0 holds return value.
        br      exit

bad_test:                       # Test failed.
        movw    $0,r0           # R0 holds return value.

exit:   loadd   0:s(sp),(ra)
        subd    $0xfffffffc,(sp)
        jump    (ra)              # Return to caller.

# -----------------------------------------------------------------------------
# Name: test_ram
#
# Do a non-destructive memory test of RAM locations.
#
# NOTE: Interrupts are disabled on entry so RAM will not be
#       modified by any other code during the RAM tests.
#       MAKE SURE NMI HANDLER (if NMI's are used for some purpose
#       by a particular OEM) DOES NOT MODIFY RAM WITHIN THE RAM
#       SPACE THAT IS BEING TESTED!  NMI's cannot be masked off
#       so they could occur during the RAM memory tests.
#
# Input: R2 - Address of RAM start location.
#        r3 - Address of RAM end location.
#
# Returns: 1 if bad RAM, 0 if OK.
#
# C prototype:
#    FLAG test_ram(BYTE * start_pntr, BYTE * end_pntr)
# -----------------------------------------------------------------------------
test_ram:
        movw    $0:s,r0         # Clear error flag.

        movw    $0x55:m,r6
        movw    $0xAA:m,r5
        br      test

loop:   loadb   0(r12),r4      # Save original data.

        storb   r6,0(r12)
        loadb   0(r12),r1
        cmpb    $0x55,r1
        beq     ok1
        movw    $1,r0           # Set error flag.

ok1:    storb   r5,0(r12)
        loadb   0(r12),r1
        cmpb    $0xAA,r1
        beq     ok2
        movw    $1,r0           # Set error flag.

ok2:    storb   r4,0(r12)      # Restore original data.
        addw    $1:s,r12         # Bump start pointer.

test:   cmpd    (r12), (r13)
        bhi     leave           # Exit if start pointer <= end pointer.
        cmpw    $0,r0
        beq     loop            # Loop if no error occurred.

leave:  jump    (ra)

