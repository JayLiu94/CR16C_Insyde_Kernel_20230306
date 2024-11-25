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
# Make file for MOD core level.  This file format is
# for use with Microsoft NMAKER utility version 1.20.
#
# Revision History (Started March 17, 2000 by GLP)
#
#******************************************************************************

.SUFFIXES: .obj .s .c

{oem\$(OEMDIR)\mod\core}.c{oem\$(OEMDIR)\mod\core\obj}.$(OBJTYP):
!  if ("$(SWITCH)"=="CPP")
   echo crcc $(CFLAGS) oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).i
   crcc $(CFLAGS) oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).i
!  else	if ("$(CR_VER)"=="1.1")
   echo crcc $(CFLAGS)
   echo      oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP)
!  else
   echo crcc $(CFLAGS) -zntemp\output.fil
   echo      oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) oem\$(OEMDIR)\mod\core\$(@B).c -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP)
#   cmd /c copy temp\output.fil temp\warn.fil>NUL
   Echo OK>temp\ok.fil
   Echo ERR>temp\output.fil
   if not exist temp\warn.fil cmd /c copy temp\ok.fil temp\output.fil>NUL
   del temp\ok.fil
   if exist temp\warn.fil del temp\warn.fil
   find "OK" temp\OUTPUT.FIL>NUL
!  endif

{oem\$(OEMDIR)\mod\core}.s{oem\$(OEMDIR)\mod\core\obj}.$(OBJTYP):
   echo crasm $(AFLAGS) -Loem\$(OEMDIR)\mod\core\obj\$(@B).lst -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\mod\core\$(@B).s
   crasm $(AFLAGS) -Loem\$(OEMDIR)\mod\core\obj\$(@B).lst -o oem\$(OEMDIR)\mod\core\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\mod\core\$(@B).s

all: \
#     oem\$(OEMDIR)\mod\core\obj\PURMAIN.$(OBJTYP)
      
#oem\$(OEMDIR)\mod\core\obj\PURMAIN.$(OBJTYP): oem\$(OEMDIR)\mod\core\PURMAIN.C \



