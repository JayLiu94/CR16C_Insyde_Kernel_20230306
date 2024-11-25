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

CRISIS_H=chip\inc\crisis.h
EXTMEM_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\extmem.h
HOST_IF_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\host_if.h
ICU_H=oem\project\inc\swtchs.h chip\inc\types.h oem\project\inc\icu.h
ID_H=chip\inc\id.h
D_TO_A_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\d_to_a.h
INI_H=core\inc\defs.h oem\project\inc\swtchs.h oem\project\inc\ini.h
I2C_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\i2c.h
IRQ_H=chip\inc\irq.h
OEM_H1=oem\project\inc\swtchs.h chip\inc\regs.h chip\inc\types.h
OEM_H=$(OEM_H1) oem\project\inc\oem.h
REGS_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\regs.h
OEMCRSIS_H=oem\project\inc\swtchs.h oem\project\inc\oemcrsis.h
PS2_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
PURCRVER_H=oem\project\inc\over.h core\inc\purcrver.h
PURCVER_H=oem\project\inc\over.h core\inc\purcver.h
PURDAT_H1=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURDAT_H2=oem\project\inc\oemcause.h core\inc\defs.h oem\project\inc\oem_data.h
PURDAT_H=$(PURDAT_H1) $(PURDAT_H2) oem\project\inc\ini.h core\inc\purdat.h
SCAN_H1=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
SCAN_H2=oem\project\inc\icu.h oem\project\inc\oem.h chip\inc\regs.h
SCAN_H=$(SCAN_H1) $(SCAN_H2) chip\inc\scan.h
SWTCHS_H=oem\project\inc\swtchs.h
TIMERS_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\timers.h
TYPES_H=oem\project\inc\swtchs.h chip\inc\types.h
A_TO_D_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\a_to_d.h
CHIPHOOK_H=chip\inc\chiphook.h
DEFS_H=core\inc\defs.h
D_TO_A_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\d_to_a.h
HOST_IF_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\host_if.h
ICU_H=oem\project\inc\swtchs.h chip\inc\types.h oem\project\inc\icu.h
I2C_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\i2c.h
INI_H=core\inc\defs.h oem\project\inc\swtchs.h oem\project\inc\ini.h
OEM_H1=oem\project\inc\swtchs.h chip\inc\regs.h chip\inc\types.h
OEM_H=$(OEM_H1) oem\project\inc\oem.h
PROC_H=oem\project\inc\swtchs.h chip\inc\types.h oem\project\inc\icu.h chip\inc\proc.h
REGS_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\regs.h
OEM_DATA_H=oem\project\inc\swtchs.h core\inc\purconst.h core\inc\defs.h oem\project\inc\oem_data.h
OEMCAUSE_H=oem\project\inc\swtchs.h core\inc\purconst.h oem\project\inc\oemcause.h
PURFUNCT_H=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purfunct.h
PURDAT_H1=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURDAT_H2=oem\project\inc\oemcause.h core\inc\defs.h oem\project\inc\oem_data.h
PURDAT_H=$(PURDAT_H1) $(PURDAT_H2) oem\project\inc\ini.h core\inc\purdat.h
PUREXT_H1=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PUREXT_H2=oem\project\inc\oemcause.h core\inc\defs.h oem\project\inc\oem_data.h
PUREXT_H=$(PUREXT_H1) $(PUREXT_H2) core\inc\purdat.h core\inc\purext.h
PURHOOK_H=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purhook.h
PURMAIN_H=oem\project\inc\swtchs.h chip\inc\types.h core\inc\purmain.h
PURXLT_H1=core\inc\defs.h oem\project\inc\swtchs.h chip\inc\types.h
PURXLT_H2=core\inc\purscn.h core\inc\purdat.h chip\inc\proc.h
PURXLT_H3=core\inc\purconst.h oem\project\inc\oemcause.h oem\project\inc\oem_data.h
PURXLT_H4=oem\project\inc\ini.h core\inc\purhook.h
PURXLT_H5=$(PURXLT_H1) $(PURXLT_H2) $(PURXLT_H3) $(PURXLT_H4)
PURXLT_H=$(PURXLT_H5) oem\project\inc\icu.h core\inc\purfunct.h core\inc\purxlt.h
RAMINIT_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\raminit.h
REGINIT_H=chip\inc\types.h oem\$(OEMDIR)\chip\inc\$(REGINIT).h
SCAN_H1=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
SCAN_H2=oem\project\inc\icu.h oem\project\inc\oem.h chip\inc\regs.h
SCAN_H=$(SCAN_H1) $(SCAN_H2) chip\inc\scan.h
SWTCHS_H=oem\project\inc\swtchs.h
TIMERS_H=oem\project\inc\swtchs.h chip\inc\types.h chip\inc\timers.h
TYPES_H=oem\project\inc\swtchs.h chip\inc\types.h
SMB_TRAN_H=chip\inc\types.h oem\project\inc\smb_tran.h oem\project\inc\oemdef.h
BTN_CTRL_H=chip\inc\types.h oem\project\inc\BTN_ctrl.h oem\project\inc\oemdef.h
MP3_CTRL_H=chip\inc\types.h oem\project\inc\mp3_ctrl.h
COMM_F_H=chip\inc\types.h oem\project\inc\comm_f.h oem\project\inc\oemdef.h oem\project\inc\oem_type.h
OEMDAT_H=oem\project\inc\oemdat.h oem\project\inc\oem_type.h oem\project\inc\oemdef.h chip\inc\types.h
OEMCMD_H=oem\project\inc\oemcmd.h oem\project\inc\oemdat.h oem\project\inc\oemdef.h
OEM_IRQ_H=oem\project\inc\oem_irq.h oem\project\inc\oemdat.h oem\project\inc\oemdef.h
OEMCHGR_H=oem\project\inc\oemchgr.h


.SUFFIXES: .obj .s .c

{oem\$(OEMDIR)}.c{oem\$(OEMDIR)\obj}.$(OBJTYP):
!  if ("$(SWITCH)"=="CPP")
   echo crcc $(CFLAGS) oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).i
   crcc $(CFLAGS) oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).i
!  else	if ("$(CR_VER)"=="1.1")
   echo crcc $(CFLAGS)
   echo      oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
!  else
   echo crcc $(CFLAGS) -zntemp\output.fil
   echo      oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) oem\$(OEMDIR)\$(@B).c -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP)
   Echo OK>temp\ok.fil
   Echo ERR>temp\output.fil
   if not exist temp\warn.fil cmd /c copy temp\ok.fil temp\output.fil>NUL
   del temp\ok.fil
   if exist temp\warn.fil del temp\warn.fil
   find "OK" temp\OUTPUT.FIL>NUL
!  endif

{oem\$(OEMDIR)}.s{oem\$(OEMDIR)\obj}.$(OBJTYP):
   echo crasm $(AFLAGS) -Loem\$(OEMDIR)\obj\$(@B).lst -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\$(@B).s
   crasm $(AFLAGS) -Loem\$(OEMDIR)\obj\$(@B).lst -o oem\$(OEMDIR)\obj\$(@B).$(OBJTYP) oem\$(OEMDIR)\$(@B).s

all: oem\$(OEMDIR)\obj\CRSISICU.$(OBJTYP) \
     oem\$(OEMDIR)\obj\OEMCRSIS.$(OBJTYP) \
     oem\$(OEMDIR)\obj\ICU.$(OBJTYP)	  \
     oem\$(OEMDIR)\obj\OEM.$(OBJTYP)	  \
     oem\$(OEMDIR)\obj\OEMDAT.$(OBJTYP)   \
     oem\$(OEMDIR)\obj\OEMMAIN.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\OEMCMDS.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\HCONFIG.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\OEMINI.$(OBJTYP)   \
     oem\$(OEMDIR)\obj\SMB_TRAN.$(OBJTYP) \
     oem\$(OEMDIR)\obj\OEMPECI.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\HOOK.$(OBJTYP)     \
     oem\$(OEMDIR)\obj\SCANTABS.$(OBJTYP) \
#     oem\$(OEMDIR)\obj\RAMINIT.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\PWRSEQ.$(OBJTYP)   \
     oem\$(OEMDIR)\obj\EVENTS.$(OBJTYP)   \
     oem\$(OEMDIR)\obj\ECDTHIF.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\FU_OEM.$(OBJTYP)  \
     oem\$(OEMDIR)\obj\SENSRHUB.$(OBJTYP) \
     oem\$(OEMDIR)\obj\HID.$(OBJTYP)




oem\$(OEMDIR)\obj\CRSISICU.$(OBJTYP): oem\$(OEMDIR)\CRSISICU.C \
      $(ICU_H) $(HOST_IF_H)

oem\$(OEMDIR)\obj\OEMCRSIS.$(OBJTYP): oem\$(OEMDIR)\OEMCRSIS.C \
      $(SWTCHS_H) $(REGS_H) $(INI_H) $(TYPES_H) \
      $(OEMCRSIS_H) $(OEM_H) $(CRISIS_H) $(PURCRVER_H)

oem\$(OEMDIR)\obj\ICU.$(OBJTYP): oem\$(OEMDIR)\ICU.C \
      $(SWTCHS_H) $(ICU_H) $(IRQ_H) $(PS2_H) $(TIMERS_H) $(I2C_H) \
      $(OEM_H) $(PURDAT_H) $(PROC_H)

oem\$(OEMDIR)\obj\OEM.$(OBJTYP): oem\$(OEMDIR)\OEM.C \
      $(SWTCHS_H) $(PURDAT_H) $(OEM_H)

oem\$(OEMDIR)\obj\OEMMAIN.$(OBJTYP): oem\$(OEMDIR)\OEMMAIN.C \

oem\$(OEMDIR)\obj\OEMCMDS.$(OBJTYP): oem\$(OEMDIR)\OEMCMDS.C \

oem\$(OEMDIR)\obj\OEMDAT.$(OBJTYP): oem\$(OEMDIR)\OEMDAT.C \

oem\$(OEMDIR)\obj\HCONFIG.$(OBJTYP): oem\$(OEMDIR)\HCONFIG.C \

oem\$(OEMDIR)\obj\OEMINI.$(OBJTYP): oem\$(OEMDIR)\OEMINI.C \

oem\$(OEMDIR)\obj\SMB_TRAN.$(OBJTYP): oem\$(OEMDIR)\SMB_TRAN.C \

oem\$(OEMDIR)\obj\OEMPECI.$(OBJTYP): oem\$(OEMDIR)\OEMPECI.C \

oem\$(OEMDIR)\obj\HOOK.$(OBJTYP): oem\$(OEMDIR)\HOOK.C \
      $(SWTCHS_H) $(TYPES_H) $(PURHOOK_H) $(CHIPHOOK_H) $(REGS_H) \
      $(INI_H) $(PROC_H) $(HOST_IF_H) $(TIMERS_H) $(OEM_H) \
      $(SCAN_H) $(PURDAT_H) $(PURSMB_H) $(PURMAIN_H) \
      $(PURFUNCT_H) $(PURXLT_H)

oem\$(OEMDIR)\obj\STATICFG.$(OBJTYP): oem\$(OEMDIR)\STATICFG.C \
      $(SWTCHS_H) $(PROC_H)

oem\$(OEMDIR)\obj\SCANTABS.$(OBJTYP): oem\$(OEMDIR)\SCANTABS.C \
      $(SWTCHS_H) $(TYPES_H)

oem\$(OEMDIR)\obj\RAMINIT.$(OBJTYP): oem\$(OEMDIR)\RAMINIT.C \
      $(DEFS_H) $(PURDAT_H) $(RAMINIT_H)

oem\$(OEMDIR)\obj\PWRSEQ.$(OBJTYP): oem\$(OEMDIR)\PWRSEQ.C \
        $(PWRSEQ_H) $(PINDEF_H)

oem\$(OEMDIR)\obj\THERMAL.$(OBJTYP): oem\$(OEMDIR)\THERMAL.C \
        $(THERMAL_H) $(PINDEF_H)

oem\$(OEMDIR)\obj\ECDTHIF.$(OBJTYP): oem\$(OEMDIR)\ECDTHIF.C \

oem\$(OEMDIR)\obj\HID.$(OBJTYP): oem\$(OEMDIR)\HID.C \

oem\$(OEMDIR)\obj\FU_OEM.$(OBJTYP): oem\$(OEMDIR)\FU_OEM.C \

