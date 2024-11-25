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
# Make file for chip level.  This file format is
# for use with Microsoft NMAKER utility version 1.20.
#
# Revision History (Started March 17, 2000 by GLP)
#
#******************************************************************************

A_TO_D_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\a_to_d.h
CHIPHOOK_H=chip\inc\chiphook.h
COM_DEFS_H=chip\inc\com_defs.h
COPYR_H=oem\$(OEMDIR)\inc\copyr.h
CRISIS_H=chip\inc\crisis.h
DEBUG_H=chip\inc\debug.h
DEFS_H=core\inc\defs.h
D_TO_A_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\d_to_a.h
FU_H=chip\inc\fu.h
FU_UTILS_H=chip\inc\fu_utils.h
HFCG_H=chip\inc\hfcg.h
HOST_IF_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\host_if.h
ICU_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h oem\$(OEMDIR)\inc\icu.h
ID_H=chip\inc\id.h
DEV_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\dev.h
I2C_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\i2c.h
IRQ_H=chip\inc\irq.h
OEM_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\regs.h chip\inc\types.h
OEM_H=$(OEM_H1) oem\$(OEMDIR)\inc\oem.h
PROC_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h oem\$(OEMDIR)\inc\icu.h chip\inc\proc.h
REGS_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\regs.h
OEMCRSIS_H=oem\$(OEMDIR)\inc\swtchs.h oem\$(OEMDIR)\inc\oemcrsis.h
PS2_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
REGINIT_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\reginit.h
STATICFG_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purxlt.h
PURCRSIS_H=core\inc\purcrsis.h
PURDAT_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURDAT_H2=oem\$(OEMDIR)\inc\oemcause.h core\inc\defs.h oem\$(OEMDIR)\inc\oem_data.h
PURDAT_H=$(PURDAT_H1) $(PURDAT_H2) oem\$(OEMDIR)\inc\ini.h core\inc\purdat.h
PURDEV_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purdev.h
PURHOOK_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purhook.h
PURSCN_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h core\inc\purconst.h
PURSCN_H2=oem\$(OEMDIR)\inc\oemcause.h core\inc\defs.h oem\$(OEMDIR)\inc\oem_data.h
PURSCN_H=$(PURSCN_H1) $(PURSCN_H2) core\inc\purdat.h core\inc\purscn.h
PURXLT_H1=core\inc\defs.h oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h
PURXLT_H2=core\inc\purscn.h core\inc\purdat.h chip\inc\proc.h
PURXLT_H3=core\inc\purconst.h oem\$(OEMDIR)\inc\oemcause.h oem\$(OEMDIR)\inc\oem_data.h
PURXLT_H4=oem\$(OEMDIR)\inc\ini.h core\inc\purhook.h
PURXLT_H5=$(PURXLT_H1) $(PURXLT_H2) $(PURXLT_H3) $(PURXLT_H4)
PURXLT_H=$(PURXLT_H5) oem\$(OEMDIR)\inc\icu.h core\inc\purfunct.h core\inc\purxlt.h
SCAN_H1=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\ps2.h
SCAN_H2=oem\$(OEMDIR)\inc\icu.h oem\$(OEMDIR)\inc\oem.h chip\inc\regs.h
SCAN_H=$(SCAN_H1) $(SCAN_H2) chip\inc\scan.h
SWTCHS_H=oem\$(OEMDIR)\inc\swtchs.h
TIMERS_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h chip\inc\timers.h
TYPES_H=oem\$(OEMDIR)\inc\swtchs.h chip\inc\types.h
SHM_H=$(COM_DEFS) chip\inc\shm.h
SIB_H=chip\inc\sib.h
.SUFFIXES: .obj .s .c

{chip}.c{chip\obj}.$(OBJTYP):
!  if ("$(SWITCH)"=="CPP")
   echo crcc $(CFLAGS) chip\$(@B).c -o chip\obj\$(@B).i
   crcc $(CFLAGS) chip\$(@B).c -o chip\obj\$(@B).i
!  else	if ("$(CR_VER)"=="1.1")
   echo crcc $(CFLAGS)
   echo      chip\$(@B).c -o chip\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) chip\$(@B).c -o chip\obj\$(@B).$(OBJTYP)
!  else
   echo crcc $(CFLAGS) -zntemp\output.fil
   echo      chip\$(@B).c -o chip\obj\$(@B).$(OBJTYP)
   crcc $(CFLAGS) chip\$(@B).c -o chip\obj\$(@B).$(OBJTYP)
   Echo OK>temp\ok.fil
   Echo ERR>temp\output.fil
   if not exist temp\warn.fil cmd /c copy temp\ok.fil temp\output.fil>NUL
   del temp\ok.fil
   if exist temp\warn.fil del temp\warn.fil
   find "OK" temp\OUTPUT.FIL>NUL
!  endif

{chip}.s{chip\obj}.$(OBJTYP):
   echo crasm $(AFLAGS) -Lchip\obj\$(@B).lst -o chip\obj\$(@B).$(OBJTYP) chip\$(@B).s
   crasm $(AFLAGS) -Lchip\obj\$(@B).lst -o chip\obj\$(@B).$(OBJTYP) chip\$(@B).s

all: chip\obj\A_TO_D.$(OBJTYP)	 chip\obj\CR_FOOTR.$(OBJTYP) \
     chip\obj\CRISIS.$(OBJTYP)   chip\obj\CRSISOPT.$(OBJTYP) \
     chip\obj\CRSISRST.$(OBJTYP) chip\obj\D_TO_A.$(OBJTYP)   \
     chip\obj\DEBUG.$(OBJTYP)    chip\obj\HOST_IF.$(OBJTYP)  \
     chip\obj\DEV.$(OBJTYP)      chip\obj\I2C.$(OBJTYP)      \
     chip\obj\IRQ.$(OBJTYP)      chip\obj\PORT80.$(OBJTYP)   \
     chip\obj\PROC.$(OBJTYP)     chip\obj\PS2.$(OBJTYP)      \
     chip\obj\RESET.$(OBJTYP)    chip\obj\SCAN.$(OBJTYP)     \
     chip\obj\SELFTEST.$(OBJTYP) chip\obj\TIMERS.$(OBJTYP)   \
     chip\obj\fu_init.$(OBJTYP)  chip\obj\PECI.$(OBJTYP)     \
     chip\obj\fu_ram.$(OBJTYP)   chip\obj\fu_utils.$(OBJTYP) \
     chip\obj\shm.$(OBJTYP)      chip\obj\fu_hndl.$(OBJTYP)  \
     chip\obj\header.$(OBJTYP)   chip\obj\sib.$(OBJTYP)      \
!  if ($(HID_OV_I2C_SUPPORTED))
     chip\obj\hid_i2c.$(OBJTYP) \
!  endif
     chip\obj\eSPI.$(OBJTYP)     \
     chip\obj\CR_UART.$(OBJTYP) chip\obj\clock.$(OBJTYP)    \
     chip\obj\PWM.$(OBJTYP)      chip\obj\PORT80.$(OBJTYP)



chip\obj\A_TO_D.$(OBJTYP): chip\A_TO_D.C \
      $(A_TO_D_H) $(OEM_H) $(PROC_H) $(TIMERS_H) $(STATICFG_H)

chip\obj\CR_FOOTR.$(OBJTYP): chip\CR_FOOTR.S

chip\obj\CRISIS.$(OBJTYP): chip\CRISIS.C \
      $(SWTCHS_H) $(TYPES_H) $(REGS_H) $(DEFS_H) $(PURCRSIS_H) \
      $(CRISIS_H) $(HOST_IF_H) $(OEM_H) $(OEMCRSIS_H) \
      $(TIMERS_H)

chip\obj\CRSISOPT.$(OBJTYP): chip\CRSISOPT.S

chip\obj\CRSISRST.$(OBJTYP): chip\CRSISRST.S \
      $(SWTCHS_H)

chip\obj\D_TO_A.$(OBJTYP): chip\D_TO_A.C \
      $(D_TO_A_H) $(PURDAT_H)

chip\obj\DEBUG.$(OBJTYP): chip\DEBUG.C \
      $(SWTCHS_H) $(DEBUG_H)

chip\obj\HOST_IF.$(OBJTYP): chip\HOST_IF.C \
      $(SWTCHS_H) $(TYPES_H) $(HOST_IF_H) $(PURDAT_H)

chip\obj\DEV.$(OBJTYP): chip\DEV.C \
      $(DEV_H) $(PS2_H) $(PROC_H) $(PURDAT_H) $(PURDEV_H) \
      $(STATICFG_H) $(PURSCN_H) $(PUREXT_H) $(REGS_H) $(SCAN_H) \
      $(A_TO_D_H) $(D_TO_A_H) $(I2C_H) $(PURXLT_H) $(ID_H)

chip\obj\I2C.$(OBJTYP): chip\I2C.C \
      $(SWTCHS_H) $(TYPES_H) $(I2C_H) $(ICU_H) $(DEFS_H) $(PROC_H) \
      $(PURDAT_H) $(HOST_IF_H) $(OEM_H)

chip\obj\IRQ.$(OBJTYP): chip\IRQ.C \
      $(SWTCHS_H) $(HOST_IF_H) $(PURSCN_H) $(PROC_H) $(IRQ_H) \
      $(PS2_H) $(CHIPHOOK_H) $(TIMERS_H) $(PURDAT_H) $(SCAN_H) \
      $(PURDEV_H) $(STATICFG_H)

chip\obj\PROC.$(OBJTYP): chip\PROC.C \
      $(SWTCHS_H) $(TYPES_H) $(PROC_H) $(REGS_H) $(PS2_H) \
      $(PURDAT_H) $(OEM_H) $(HOST_IF_H) $(PURDEV_H) $(SCAN_H)

chip\obj\PS2.$(OBJTYP): chip\PS2.C \
      $(TYPES_H) $(PS2_H) $(ICU_H) $(PURDAT_H) $(PROC_H)

chip\obj\RESET.$(OBJTYP): chip\RESET.S \
      $(SWTCHS_H) $(COPYR_H)

chip\obj\SCAN.$(OBJTYP): chip\SCAN.C \
      $(SWTCHS_H) $(PURDAT_H) $(SCAN_H) $(OEM_H) $(STATICFG_H) \
      $(PURSCN_H) $(PURHOOK_H) $(PURXLT_H) $(CHIPHOOK_H)

chip\obj\SELFTEST.$(OBJTYP): chip\SELFTEST.S

chip\obj\TIMERS.$(OBJTYP): chip\TIMERS.C \
      $(SWTCHS_H) $(TIMERS_H) $(ICU_H) $(PROC_H) $(OEM_H)

chip\obj\fu_init.$(OBJTYP): chip\fu_init.c \
      $(CHIP_H) $(COM_DEFS_H) $(FU_H) $(SHM_H) $(FU_UTILS_H)

chip\obj\fu_ram.$(OBJTYP): chip\fu_ram.c \
      $(CHIP_H) $(COM_DEFS_H) $(FU_H) $(FU_UTILS_H) $(SHM_H)

chip\obj\fu_utils.$(OBJTYP): chip\fu_utils.c \
      $(CHIP_H) $(COM_DEFS_H) $(FU_UTILS_H) $(FU_H) $(SHM_H)

chip\obj\shm.$(OBJTYP): chip\shm.c \
      $(SHM_H)

chip\obj\fu_hndl.$(OBJTYP): chip\fu_hndl.c \
      $(CHIP_H) $(COM_DEFS_H) $(FU_H)

chip\obj\header.$(OBJTYP): chip\header.c \
      $(HFCG_H)

chip\obj\sib.$(OBJTYPE): chip\sib.c \
      $(CHIP_H) $(COM_DEFS_H) $(SIB_H)

chip\obj\LFCG.$(OBJTYPE): chip\LFCG.c \
    $(TYPES_H)  $(REGS_H)

chip\obj\PWM.$(OBJTYPE): chip\PWM.c \
    $(TYPES_H)  $(REGS_H)

chip\obj\PORT80.$(OBJTYPE): chip\PORT80.c \
    $(TYPES_H)  $(REGS_H)

chip\obj\PECI.$(OBJTYPE): chip\PECI.c \
    $(TYPES_H)  $(REGS_H)

chip\obj\CR_UART.$(OBJTYPE): chip\CR_UART.c \
    $(TYPES_H)  $(REGS_H)

chip\obj\CLOCK.$(OBJTYPE): chip\CLOCK.c \
    $(TYPES_H)  $(REGS_H) $(HFCG_H)

chip\obj\eSPI.$(OBJTYPE): chip\eSPI.c \

