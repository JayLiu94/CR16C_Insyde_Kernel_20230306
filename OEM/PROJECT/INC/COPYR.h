# -----------------------------------------------------------------------------
# MODULE COPYR.H
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
# This file contains the macro for the copyright notice.
#
# -----------------------------------------------------------------------------

	.macro INLINE_COPYRIGHT
	.byte	"Copyright 1996-2010, all rights reserved"
	.byte	10,13
	.byte	"Insyde Software Corp."
	.endm
