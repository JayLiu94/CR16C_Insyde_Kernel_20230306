# -----------------------------------------------------------------------------
# MODULE CR_FOOTR.S
#
# Copyright(c) 1999-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
# -----------------------------------------------------------------------------
# Crisis module footer.
#
# Revision History (Started August 9, 1999 by GLP)
#
# -----------------------------------------------------------------------------

   .text
   .align	2

   .globl _KBC_PAGE
   .code_label _KBC_OFFSET
   .globl _Footer

_Footer:

   .byte 0x49               # Signature byte set to 0x49.
   .byte 0x4E               # Signature byte set to 0x4E.
   .word _KBC_PAGE          # Base Address Configuration (BACFG) or
                            # PAGE Register setting.
   .double _KBC_OFFSET        # 2-byte pointer (code_label)
                            # to start of KBC code.
   .byte 0                  # Reserved.
   .byte 0                  # Reserved.
