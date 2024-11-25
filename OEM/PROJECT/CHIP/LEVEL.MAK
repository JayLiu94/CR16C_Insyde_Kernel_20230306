#******************************************************************************
# Module Name: LEVEL.MAK
#
# Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
#
# You may not reproduce, distribute, publish, display, perform, modify, adapt,
# transmit, broadcast, present, recite, release, license or otherwise exploit
# any part of this publication in any form, by any means, without the prior
# written permission of Insyde Software Corporation.
#
#******************************************************************************
#
# Make file for oem level.  This file format is
# for use with Microsoft NMAKER utility version 1.20.
#
# Revision History (Started May 10, 1996 by GLP)
#
# GLP
# 22 Nov 96   Added support for I2C bus (ACCESS.bus).
# 09 May 97   Added SYSCFG file.
# 29 Jun 98   Updated for use with CORE 1.01.20.
# 30 Sep 98   Updated for new Keyboard Controller functionality.
# 05 Aug 99   Added copyright.  Added OEMCRSIS and CRSISICU modules.
# 19 Aug 99   Replaced $(TMP) with temp.
#******************************************************************************

PS2_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
A_TO_D_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\a_to_d.h
OEM_H=$(OEM_H1) oem\project\inc\oem.h
REGINIT_H=chip\inc\types.h oem\$(OEMDIR)\chip\inc\$(REGINIT).h


.SUFFIXES: .obj .s .c

{oem\$(OEMDIR)\chip}.c{oem\$(OEMDIR)\obj}.$(OBJTYP):
   echo crcc $(CFLAGS) -zntemp\output.fil
   echo      oem\$(OEMDIR)\chip\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) oem\$(OEMDIR)\chip\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
   Echo OK>temp\ok.fil
   Echo ERR>temp\output.fil
   if not exist temp\warn.fil cmd /c copy temp\ok.fil temp\output.fil>NUL
   del temp\ok.fil
   if exist temp\warn.fil del temp\warn.fil
   find "OK" temp\OUTPUT.FIL>NUL

{oem\$(OEMDIR)\chip}.s{oem\$(OEMDIR)\obj}.$(OBJTYP):
   echo crasm $(AFLAGS) -Loem\$(OEMDIR)\obj\$(@B).lst -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\chip\$(@B).s
   crasm $(AFLAGS) -Loem\$(OEMDIR)\obj\$(@B).lst -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\chip\$(@B).s

all: oem\$(OEMDIR)\obj\$(REGINIT).$(OBJTYP)

oem\$(OEMDIR)\obj\$(REGINIT).$(OBJTYP): oem\$(OEMDIR)\CHIP\$(REGINIT).C \
      $(REGINIT_H) $(A_TO_D_H) $(OEM_H) $(PS2_H)


