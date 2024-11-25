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
# Make file for core level.  This file format is
# for use with Microsoft NMAKER utility version 1.20.
#
# Revision History (Started May 10, 1996 by GLP)
#
# GLP
# 22 Nov 96   Added support for I2C bus.
# 11 Feb 98   Added D_TO_A_H dependency to PUREXT.
# 27 Feb 98   Added OEMRC_H dependency to PURSCN.  Updated PURMAIN_H
#	      and PURCONST_H.  Added OEM_H and PURHOOK_H dependencies
#	      to PUREXT.
# 04 Feb 98   Changed to be for use with Microsoft NMAKER utility
#	      version 1.20.  Was for use with Microsoft MAKE utility
#	      version 4.07 and compatible with Microsoft NMAKE utility
#	      version 1.13.
# 06 Mar 98   Updated SLI2C_H.
# 17 Mar 98   Updated dependencies.  Removed SLI2C.
# 19 Mar 98   Updated dependencies.  Added PURACPI module.
# 15 Apr 98   Added I2C_H dependency to PURACPI.
# 30 Apr 98   Added A_TO_D_H dependency to PUREXT.
# 06 May 98   Added PURMAIN_H dependency to PURINI.
# 12 May 98   Updated dependencies.
# 14 May 98   Updated OEM_H.
# 27 May 98   Used -z option with crcc to stop build when a warning
#             is generated.
# 08 Jun 98   Added test to stop build when a warning is generated.
# 23 Jun 98   Added PURALARM module.
# 10 Jul 98   Added TIMERS_H dependency to PURCMD.
# 17 Jul 98   Added PURFUNCT module.
# 30 Sep 98   Updated for new Keyboard Controller functionality.
# 12 Oct 98   Added PUREXT_H dependency to PURINI.
# 19 Oct 98   Added DEV_H dependency to PURACPI.
# 06 Aug 99   Added copyright and PURCRSIS module.
# 19 Aug 99   Updated dependencies.  Replaced $(TMP) with temp.
# 07 Sep 99   Updated STATICFG_H.
#******************************************************************************

A_TO_D_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\a_to_d.h
CRISIS_H=chip\inc\crisis.h
DEFS_H=core\inc\defs.h
D_TO_A_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\d_to_a.h
HOST_IF_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\host_if.h
ID_H=chip\inc\id.h
DEV_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\dev.h
I2C_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\i2c.h
INI_H=core\inc\defs.h oem\$(OEMDIR)\inc\swtchs.h oem\$(OEMDIR)\inc\ini.h
OEM_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\regs.h chip\inc\types.h
OEM_H=$(OEM_H1) oem\$(OEMDIR)\inc\oem.h
OVER_H=oem\$(OEMDIR)\inc\over.h
PROC_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h oem\$(OEMDIR)\inc\icu.h chip\inc\proc.h
PS2_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
PURACPI_H=oem\$(OEMDIR)\inc\swtchs.h core\inc\puracpi.h
PURCMD_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purcmd.h
PURCONST_H=oem\$(OEMDIR)\inc\swtchs.h core\inc\purconst.h
PURCRSIS_H=core\inc\purcrsis.h
PURCRVER_H=oem\$(OEMDIR)\inc\over.h core\inc\purcrver.h
PURDAT_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURDAT_H2=oem\$(OEMDIR)\inc\oemcause.h core\inc\defs.h oem\$(OEMDIR)\inc\oem_data.h
PURDAT_H=$(PURDAT_H1) $(PURDAT_H2) oem\$(OEMDIR)\inc\ini.h core\inc\purdat.h
PURDEV_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purdev.h
PUREXT_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PUREXT_H2=oem\$(OEMDIR)\inc\oemcause.h core\inc\defs.h oem\$(OEMDIR)\inc\oem_data.h
PUREXT_H=$(PUREXT_H1) $(PUREXT_H2) core\inc\purdat.h core\inc\purext.h
PURFUNCT_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purfunct.h
PURHOOK_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purhook.h
PURINI_H=core\inc\purini.h
PURMAIN_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purmain.h
PURSCN_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURSCN_H2=oem\$(OEMDIR)\inc\oemcause.h core\inc\defs.h oem\$(OEMDIR)\inc\oem_data.h
PURSCN_H=$(PURSCN_H1) $(PURSCN_H2) core\inc\purdat.h core\inc\purscn.h
PURXLT_H1=core\inc\defs.h oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h
PURXLT_H2=core\inc\purscn.h core\inc\purdat.h chip\inc\proc.h
PURXLT_H3=core\inc\purconst.h oem\$(OEMDIR)\inc\oemcause.h oem\$(OEMDIR)\inc\oem_data.h
PURXLT_H4=oem\$(OEMDIR)\inc\ini.h core\inc\purhook.h
PURXLT_H5=$(PURXLT_H1) $(PURXLT_H2) $(PURXLT_H3) $(PURXLT_H4)
PURXLT_H=$(PURXLT_H5) oem\$(OEMDIR)\inc\icu.h core\inc\purfunct.h core\inc\purxlt.h
RAMINIT_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\raminit.h
SCAN_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
SCAN_H2=oem\$(OEMDIR)\inc\icu.h oem\$(OEMDIR)\inc\oem.h chip\inc\regs.h
SCAN_H=$(SCAN_H1) $(SCAN_H2) chip\inc\scan.h
SELFTEST_H=chip\inc\selftest.h
STATICFG_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purxlt.h
SWTCHS_H=oem\$(OEMDIR)\inc\swtchs.h
TIMERS_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\timers.h
TYPES_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h

.SUFFIXES: .obj .s .c

{core}.c{core\obj}.$(OBJTYP):
!  if ("$(SWITCH)"=="CPP")
   echo crcc $(CFLAGS) core\$(@B).c -o core\obj\$(@B).i
   crcc $(CFLAGS) core\$(@B).c -o core\obj\$(@B).i
!  else	if ("$(CR_VER)"=="1.1")
   echo crcc $(CFLAGS)
   echo      core\$(@B).c -o core\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) core\$(@B).c -o core\obj\$(@B).$(OBJTYP)
!  else
   echo crcc $(CFLAGS) -zntemp\output.fil
   echo      core\$(@B).c -o core\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) core\$(@B).c -o core\obj\$(@B).$(OBJTYP)
   Echo OK>temp\ok.fil
   Echo ERR>temp\output.fil
   if not exist temp\warn.fil cmd /c copy temp\ok.fil temp\output.fil>NUL
   del temp\ok.fil
   if exist temp\warn.fil del temp\warn.fil
   find "OK" temp\OUTPUT.FIL>NUL
!  endif

{core}.s{core\obj}.$(OBJTYP):
   echo crasm $(AFLAGS) -Lcore\obj\$(@B).lst -o core\obj\$(@B).$(OBJTYP) core\$(@B).s
   crasm $(AFLAGS) -Lcore\obj\$(@B).lst -o core\obj\$(@B).$(OBJTYP) core\$(@B).s

all: core\obj\PURACPI.$(OBJTYP)	 core\obj\PURSCN.$(OBJTYP)   \
     core\obj\PURCMD.$(OBJTYP)   core\obj\PURCRSIS.$(OBJTYP) \
     core\obj\PURDAT.$(OBJTYP)   core\obj\PURDEV.$(OBJTYP)   \
     core\obj\PUREXT.$(OBJTYP)   core\obj\PURFUNCT.$(OBJTYP) \
     core\obj\PURINI.$(OBJTYP)   core\obj\PURMAIN.$(OBJTYP)  \
     core\obj\PURPM.$(OBJTYP)   core\obj\PURXLT.$(OBJTYP)

core\obj\PURACPI.$(OBJTYP): core\PURACPI.C \
      $(SWTCHS_H) $(DEFS_H) $(TYPES_H) $(PURACPI_H) \
      $(PURDAT_H) $(HOST_IF_H) $(OEM_H) $(PROC_H) $(PURMAIN_H) \
      $(PURHOOK_H) $(I2C_H) $(STATICFG_H) $(TIMERS_H) $(DEV_H)

core\obj\PURCMD.$(OBJTYP): core\PURCMD.C \
      $(SWTCHS_H) $(PURCMD_H) $(PUREXT_H) $(DEFS_H) $(TYPES_H) $(PURDAT_H) \
      $(HOST_IF_H) $(PROC_H) $(PS2_H) $(PURSCN_H) \
      $(PURXLT_H) $(PURHOOK_H) $(PURDEV_H) $(OEM_H) $(INI_H) \
      $(SELFTEST_H) $(DEV_H) $(PURACPI_H) $(TIMERS_H) $(STATICFG_H) \
      $(RAMINIT_H)

core\obj\PURPM.$(OBJTYP): core\PURPM.C \
      $(SWTCHS_H) $(PURCMD_H) $(PUREXT_H) $(DEFS_H) $(TYPES_H) $(PURDAT_H) \
      $(HOST_IF_H) $(PROC_H) $(PS2_H) $(PURSCN_H) \
      $(PURXLT_H) $(PURHOOK_H) $(PURDEV_H) $(OEM_H) $(INI_H) \
      $(SELFTEST_H) $(DEV_H) $(PURACPI_H) $(TIMERS_H) $(STATICFG_H) \
      $(RAMINIT_H)

core\obj\PURCRSIS.$(OBJTYP): core\PURCRSIS.C \
      $(SWTCHS_H) $(TYPES_H) $(DEFS_H) $(PURCRSIS_H) $(CRISIS_H) \
      $(HOST_IF_H) $(PURCRVER_H) $(ID_H)

core\obj\PURDAT.$(OBJTYP): core\PURDAT.C \
      $(PURDAT_H) $(PURXLT_H) $(STATICFG_H)

core\obj\PURDEV.$(OBJTYP): core\PURDEV.C \
      $(SWTCHS_H) $(DEFS_H) $(PURDEV_H) $(PURSCN_H) $(PURDAT_H) $(TYPES_H) \
      $(PS2_H) $(PROC_H) $(TIMERS_H)

core\obj\PUREXT.$(OBJTYP): core\PUREXT.C \
      $(SWTCHS_H) $(DEFS_H) $(PUREXT_H) $(PURDAT_H) $(PURINI_H) \
      $(PURCMD_H) $(PURCONST_H) $(PROC_H) \
      $(PS2_H) $(HOST_IF_H) $(I2C_H) $(D_TO_A_H) $(OEM_H) \
      $(PURMAIN_H) $(TIMERS_H) $(PURACPI_H) $(A_TO_D_H) \
      $(PURXLT_H) $(PURFUNCT_H) $(DEV_H) $(STATICFG_H) $(PURHOOK_H)

core\obj\PURFUNCT.$(OBJTYP): core\PURFUNCT.C \
      $(TYPES_H) $(DEFS_H) $(PURXLT_H) $(PURFUNCT_H) $(PURHOOK_H) \
      $(PROC_H) $(PURCMD_H) $(PUREXT_H) $(OEM_H) $(TIMERS_H) \
      $(STATICFG_H) $(INI_H) $(PURDAT_H) $(PURSCN_H) $(PURMAIN_H) \
      $(PURACPI_H) $(HOST_IF_H)

core\obj\PURINI.$(OBJTYP): core\PURINI.C \
      $(SWTCHS_H) $(PURHOOK_H) $(TYPES_H) $(PURDAT_H) $(INI_H) \
      $(INIRTABL_H) $(PURINI_H) $(PS2_H) $(HOST_IF_H) \
      $(OEM_H) $(DEV_H) $(SCAN_H) \
      $(I2C_H) $(TIMERS_H) $(PURSCN_H) \
      $(PURDEV_H) $(PURACPI_H) $(PURMAIN_H) \
      $(PURFUNCT_H) $(STATICFG_H) $(PURXLT_H) $(PROC_H) $(RAMINIT_H) \
      $(PUREXT_H)

core\obj\PURMAIN.$(OBJTYP): core\PURMAIN.C \
      $(SWTCHS_H) $(DEFS_H) $(TYPES_H) \
      $(PURDAT_H) $(HOST_IF_H) $(PURSCN_H) $(PS2_H) \
      $(PURHOOK_H) $(PROC_H) $(SCAN_H) $(PURCMD_H) $(PURCONST_H) \
      $(PURDEV_H) $(PURINI_H) $(PURMAIN_H) $(INI_H) \
      $(OEM_H) $(TIMERS_H) $(PURACPI_H) \
      $(PURFUNCT_H) $(PURXLT_H) $(STATICFG_H) $(DEV_H) $(RAMINIT_H)

core\obj\PURSCN.$(OBJTYP): core\PURSCN.C \
      $(SWTCHS_H) $(DEFS_H) $(TYPES_H) $(PURSCN_H) $(PURDAT_H) \
      $(PROC_H) $(SCAN_H) $(PURXLT_H) $(OEM_H) $(PURHOOK_H) \
      $(TIMERS_H) $(PURMAIN_H) $(STATICFG_H)

core\obj\PURXLT.$(OBJTYP): core\PURXLT.C \
      $(DEFS_H) $(TYPES_H) $(PURXLT_H) $(PURSCN_H) $(PURDAT_H) \
      $(PROC_H) $(PURHOOK_H) $(PURFUNCT_H) $(STATICFG_H)

