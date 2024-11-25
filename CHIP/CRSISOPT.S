# -----------------------------------------------------------------------------
# MODULE CRSISOPT.S
#
# Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
# -----------------------------------------------------------------------------
# Crisis module enabled option.  When the crisis module is used, the Keyboard 
# Controller Header in the IREOPT.S file is used for the crisis module header 
# (if the Boot ROM code that is masked into the microprocessor is used) and 
# this file is used to define the Keyboard Controller Header for the main 
# Keyboard Controller code.
#
# Revision History (Started August 10, 1999 by GLP)
#
# -----------------------------------------------------------------------------
   .include ..\oem\project\inc\swtchs.h

	.text
	.align	2

   .code_label start

   .double _ROM_SIZE
   .double start              # 2-byte pointer (code_label)
                            # to start of code.
   .byte _CHECKSUM_DATA     # Byte to force checksum to 0.
   .byte 0                  # Reserved.
