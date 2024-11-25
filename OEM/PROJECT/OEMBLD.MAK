#**************************************************************************
# Module Name: OEMBLD.MAK
#
#          Copyright 1995-2010, all rights reserved
#          Insyde Software Corp.
#
# Make file for National Semiconductor PC87591 keyboard
# controller/scanner firmware.  Based on PC87570 code
# version 2.00.07[2667-21]-n
#
# This file format is for use with
# Microsoft NMAKER utility version 1.20.
#
# Revision History (Started March 7, 2000 by GLP)
#
# EDH
# 18 Jan 01    Allow OEM enable/disable PS/2 port4 support.
# 25 Jan 05    Removed switch for PS/2 port4 support.
#              Added a switch for OEM to enable/disable PS/2 Port2 supported.
# Mark
## 2005/12/1    revise OEM directory for WPC8768/9L
##              change RAM_SIZE 2000(8k) ==> 800(2k)
#
#**************************************************************************

ON=1
OFF=0

.SILENT:

OEMDIR=PROJECT        # OEM directory name.

CHIP_ID = 68Ax

CMD_BC=$(OFF)               # Command BCh (Read Saved Mouse State) ON or OFF.
EXT_GPIO44_WAKEUP=$(OFF)   # External Interrupt EXGPIO44 wakeup support ON or OFF.
EXT_GPIO1_WAKEUP=$(OFF)     # External Interrupt EXGPIO1 wakeup support ON or OFF.
EXT_GPIO46_WAKEUP=$(OFF)   # External Interrupt EXGPIO46 wakeup support ON or OFF.
EXT_GPIO3_WAKEUP=$(OFF)     # External Interrupt EXGPIO3 wakeup support ON or OFF.
EXT_GPIO4_WAKEUP=$(OFF)     # External Interrupt EXGPIO4 wakeup support ON or OFF.
EXT_GPIO5_WAKEUP=$(OFF)     # External Interrupt EXGPIO5 wakeup support ON or OFF.
EXT_IRQ40_WAKEUP=$(OFF)     # External Interrupt EXWINT40 wakeup support ON or OFF.
AUX_PORT3_SUPPORTED=$(OFF)  # ON or OFF for third auxiliary port.
AUX_PORT2_SUPPORTED=$(OFF)  # ON or OFF for third auxiliary port.
I2C_SUPPORTED=$(ON)        # ON or OFF for I2C bus support.
ACPI_SUPPORTED=$(ON)       # ON or OFF for ACPI version.  If set to ON, must also have I2C_SUPPORTED set to ON.
SXI_SUPPORTED=$(ON)
MATRIX_18=$(OFF)           # ON or OFF for Matrix 8*18 support.
EXT_HOTKEY=$(OFF)
SMB_FLUPDATE_SUPPORTED=$(OFF)    #
KEY_SCANNER_SUPPORTED=$(OFF)
SHL_SUPPORTED=$(ON)
HW_FLOATING_SUPPORTED=$(OFF)

!if $(SHL_SUPPORTED)
HID_OV_I2C_SUPPORTED=$(ON)
HID_KBD_SUPPORTED=$(OFF)
!else
HID_OV_I2C_SUPPORTED=$(OFF)
HID_KBD_SUPPORTED=$(OFF)
!endif

!if $(HID_OV_I2C_SUPPORTED)
I2C_SLAVE_SUPPORTED=$(ON)
!else
I2C_SLAVE_SUPPORTED=$(OFF)
!endif
EXTEND_KBC_SUPPORTED=$(ON)    # ON or OFF for Indyde Extended command support in Port 60h/64h.
EXTEND_PM1_SUPPORTED=$(ON)    # ON or OFF for Indyde Extended command support in PM1.
EXTEND_PM2_SUPPORTED=$(ON)    # ON or OFF for Indyde Extended command support in PM2.
MATRIX_SUPPORTED=$(ON)
MOUSE_EMULATION=$(OFF)
KERNEL_TMRB_MFT16=$(OFF)
I2C_GPIO_RECOVERY=$(ON)
ESPI_SUPPORTED=$(ON)

            # Internal keyboard (scanner) tables.
RAM_SCAN_TABLE=$(OFF)       # ON  for RAM allocated for scan table.
            # OFF for no RAM available for scan table.

RUN_FROM_MRAM = $(OFF)	         # If ON, FW runs from MRAM, If OFF, FW runs from FLASH

# Some switches for SWTCHS.H file.
SW0=MESSAGES $(OFF)              # Messages to debugging terminal ON or OFF.
SW1=HOST_IF_MSGS $(OFF)          # If ON, send HOST_IF.C messages.
SW2=PURMAIN_MSGS $(OFF)          # If ON, send PURMAIN.C messages.
SW3=AUX_PORT1_MSGS $(OFF)        # If ON, send Auxiliary Port 1 messages.
SW4=AUX_PORT2_MSGS $(OFF)        # If ON, send Auxiliary Port 2 messages.
SW5=AUX_PORT3_MSGS $(OFF)        # If ON, send Auxiliary Port 3 messages.
SW6=KERNEL_TICKER_T0 $(ON)       # If ON, Kernel uses T0 as time ticker.
SW8=RAM_BASED_FLASH_UPDATE $(ON) # If ON, specifies that flash update code is in RAM (otherwise in ROM).
SW9=TICKER_1MS $(ON)             # If ON, kernel time ticker is 1mS, otherwise time ticker is 5mS.
SW10=DEEP_IDLE_S0 $(OFF)         # If ON, PECI functionality is ON
SW11=PWM_SUPPORTED $(ON)         # If ON, PWM functionality is ON
SW12=PORT80_SUPPORTED $(OFF)     # If ON, PORT80 functionality is ON
SW13=PECI_SUPPORTED $(OFF)       # If ON, PECI functionality is ON
SW14=ECDT_SHM_SUPPORTED $(OFF)   # If ON, ECDT SHM functionality is ON
SW15=HW_KB_SCN_SUPPORTED $(ON)   # If ON, Automatic HW Keyboard scan is supported
SW16=CR_UART_SUPPORTED $(ON)     # If ON, CR_UART functionality is ON

# Crisis module settings.
#
# The CRISIS_PAGE variable is the page number for the PC87570 page
# register.  Set this to NONE for no crisis module or set it to a value
# from 0 to 7.
#
# To put the crisis module in the lower addresses of the 16 K byte top
# boot block of a 256 K byte ROM, set the CRISIS_ variables like:
#
#    CRISIS_PAGE=3
#    CRISIS_OFFSET=0xC000
#
# To put the crisis module just after the System Configuration Header,
# set the CRISIS_ variables like:
#
#    CRISIS_PAGE=0
#    CRISIS_OFFSET=0x20
#
#CRISIS_PAGE=0       # Set to NONE or a value from 0 to 7.
#CRISIS_OFFSET=0x20  # Set to a hexadecimal value.


CRISIS_CMD_HANDLER=$(ON)# ON or OFF to include or exclude support for
         # commands when in crisis mode.
BOOTABLE_CRISIS=$(ON)   # ON or OFF to include or exclude support for
         # commands that allow the host to boot when in
         # crisis mode.  If set to ON, must also have
         # CRISIS_CMD_HANDLER set to ON.
CRISIS_CMD_0FH=$(ON)    # ON or OFF to include or exclude support for
         # crisis command 0Fh (Flash Setup).

# Main keyboard controller code settings.
#
# The KBC_PAGE variable is the page number for the PC87570 page
# register.  If KBC_OFFSET is set to AUTO, this value will be
# overridden.
#
# The KBC_OFFSET variable is the offset within the page.  This is the
# start of the main keyboard controller code or, if ENVIRON is set to
# IRE, the Header.  Set this to AUTO to allow the code to be placed
# after the crisis module or set it to a location in the form 0xYYYY,
# where YYYY is a hexadecimal number.
#
# To put the main keyboard controller code just after the crisis
# module, set the KBC_ variables like:
#
#    KBC_PAGE=0
#    KBC_OFFSET=AUTO
#
# To put the main keyboard controller code just after the System
# Configuration Header, set the KBC_ variables like:
#
#    KBC_PAGE=0
#    KBC_OFFSET=0x20
#
#KBC_PAGE=0    # Set a value from 0 to 7.
#KBC_OFFSET=0x100    # Set to AUTO or to a hexadecimal value.

# The ROM generated is $(ROMNAME).  It is made up of
# different modules that depend on the following settings:
#
#  CRISIS_PAGE
#  KBC_OFFSET
#  ROM_SCAN_TABLE
#
# With CRISIS_PAGE   set to a value from 0 to 7,
#      KBC_OFFSET set to AUTO,
#      ROM_SCAN_TABLE   set to OUTSIDE, the ROM is made up of:
#
#  Binary module     Linker map file
#  --------------    ---------------
#  SYSCFG.BIN         SYSCFG.MAP
#  $(CRISIS_NAME).BIN $(CRISIS_NAME).MAP
#  CR_FOOTR.BIN       CR_FOOTR.MAP
#  $(EC_NAME).BIN     $(EC_NAME).MAP
#  RAMINIT.BIN        RAMINIT.MAP
#  REGINIT.BIN        REGINIT.MAP
#  SCANTABS.BIN       SCANTABS.MAP
#
# When ROM_SCAN_TABLE is set to OUTSIDE, the $(DEBUG_NAME) file does not
# contain the table(s).  To use the debugger, either set the tables to
# INSIDE or manually load the tables into the debugger.
#
# When KBC_OFFSET is NOT set to AUTO, the crisis module is not placed
# in the $(ROMNAME) file.  In this case, if the crisis module is
# selected, the $(CRISIS_ROM) file is generated.  This is made up of:
#
#  Binary module     Linker map file
#  --------------    ---------------
#  $(CRISIS_NAME).BIN $(CRISIS_NAME).MAP
#  CR_FOOTR.BIN       CR_FOOTR.MAP
#
# This $(CRISIS_ROM) file must be located in the correct place in the
# ROM by other means.
#
# Another option is for the keyboard controller utility to create the
# configuration and scanner table files.  These files are then taken
# from the ROMSRC directory instead of the files output by this build.
# Using the ROMSRC directory instead of the OBJ directory guards against
# the files being deleted when building CLEAN.  The following files
# can be replaced:
#
#  RAMINIT.BIN
#  REGINIT.BIN
#  SCANTABS.BIN
#
# In order to use this option, the ROM_SCAN_TABLE build options must be set to
# OUTSIDE.
#
# The utility generates a file to replace the RAMINIT.BIN
# and REGINIT.BIN files.  It generates separate files to replace the
# SCANTABS.BIN file to allow up to four ROM scanner tables to be used.
# To use this option, put the files in the OEM\$(OEMDIR)\ROMSRC
# directory and change the CONFIG, SCANTAB1, SCANTAB2, SCANTAB3, and
# SCANTAB4 variable definitions from a blank to the names of the binary
# files.
#
# The fourth ROM table is only available when RAM_SCAN_TABLE is set to
# OFF.
#
# The standard build sets the CONFIG and the SCANTABx variables to a
# blank.
#           #   Example
            # ------------
SCANTAB1= SCANTAB1.BIN
SCANTAB2=         # SCANTAB2.BIN
SCANTAB3=         # SCANTAB3.BIN
SCANTAB4=         # SCANTAB4.BIN

ROMNAME=NPCE$(CHIP_ID).ROM            # Name and extension of the ROM file.
CRISIS_ROM=KBCRISIS.ROM    # Name and extension of the crisis ROM
            # file if KBC_OFFSET is not set to AUTO.
CRISIS_NAME=KBCRISIS    # Name (excluding extension) of the
            # crisis module code.
EC_NAME=EC$(CHIP_ID)         # Name (excluding extension) of the
            # main Embedded Controller code.
DEBUG_NAME=DEBUG_EC     # Name (excluding extension) of the
            # file that can be loaded into the
            # debugger.


            # Internal keyboard (scanner) tables.
ROM_SCAN_TABLE=OUTSIDE # INSIDE  for table checksummed in ROM.
            # OUTSIDE to allow table to be appended to ROM.
            # OUTSIDE will generate the SCANTABS.BIN file.


CRISIS_PAGE=NONE          # Set to NONE or a value 0.

HDR_START=0x20000
HDR_SIZE=0x100

CRISIS_OFFSET=0x20100  # Set to a hexadecimal value.
CRISIS_START =0x2000A  # Set to a hexadecimal value.
CRISIS_SIZE  =0x1000   # Crisis size
CRISIS_END   =0x21000  # Crisis End

KBC_PAGE=0             # Set a value from 0 to 7.

! if ("$(CRISIS_PAGE)"=="NONE")
KBC_OFFSET=0x20100     # Set to AUTO or to a hexadecimal value.
SRT=$(KBC_OFFSET)
! else
KBC_OFFSET=AUTO
! endif

CHIPDIR=chip         # Directory name of Chip directory at
            # both the top level and the OEM level.

SWITCH=DEBUG         # Compiler switch.
            # NORMAL for normal code.
            # DEBUG for code with debugging info added.
            # CPP to run cpp only.


RUNASM=$(ON)         # ON  to run assembler after compiling.
            # OFF to compile but do not assemble.
            # When set OFF, object file (see OBJTYP)
            # is actually an assembly source file.


MAKE_ERR=0

!if ("$(KBC_OFFSET)"=="AUTO")
!  if ("$(CRISIS_PAGE)"=="NONE")
!  message KBC_OFFSET cannot be set to AUTO when CRISIS_PAGE is set to NONE.
   MAKE_ERR=1
!  endif
!endif


!if ( ("$(SCANTAB1)"!="") || \
      ("$(SCANTAB2)"!="") || \
      ("$(SCANTAB3)"!="") || \
      ("$(SCANTAB4)"!="") )
!  if ("$(ROM_SCAN_TABLE)"!="OUTSIDE")
!  message In order to specify the SCANTAB1, SCANTAB2, SCANTAB3, or SCANTAB4
!  message file names, the ROM_SCAN_TABLE variable must be set to OUTSIDE.
   MAKE_ERR=1
!  else
!    if ("$(SCANTAB1)"!="")
!        if !(EXIST(ROMSRC\$(SCANTAB1)))
!        message The ROMSRC\$(SCANTAB1) file specified in SCANTAB1 does not exist.
         MAKE_ERR=1
!        endif
!    endif
!    if ("$(SCANTAB2)"!="")
!       if !(EXIST(ROMSRC\$(SCANTAB2)))
!       message The ROMSRC\$(SCANTAB2) file specified in SCANTAB2 does not exist.
        MAKE_ERR=1
!       endif
!    endif
!    if ("$(SCANTAB3)"!="")
!       if !(EXIST(ROMSRC\$(SCANTAB3)))
!       message The ROMSRC\$(SCANTAB3) file specified in SCANTAB3 does not exist.
        MAKE_ERR=1
!       endif
!    endif
!    if ("$(SCANTAB4)"!="")
!       if $(RAM_SCAN_TABLE)
!       message The scanner table specified in SCANTAB4 is only available
!       message when RAM_SCAN_TABLE is set to OFF.
        MAKE_ERR=1
!       else if !(EXIST(ROMSRC\$(SCANTAB4)))
!       message The ROMSRC\$(SCANTAB4) file specified in SCANTAB4 does not exist.
        MAKE_ERR=1
!       endif
!    ENDIF
!  endif
!endif

!if $(MAKE_ERR)
!error Error in make file.
!endif

!if ("$(CRISIS_PAGE)"=="NONE")
CRISIS_CMD_HANDLER=$(OFF)
INCLUDE_CRISIS=$(OFF)
!else
INCLUDE_CRISIS=$(ON)
!endif

!if ("$(KBC_OFFSET)"=="AUTO")
KBC_PAGE=$(CRISIS_PAGE)
!endif

!if ("$(CRISIS_CMD_HANDLER)"=="OFF")
BOOTABLE_CRISIS=$(OFF)
!endif

# KBD_BLOCK_PAGE and KBD_BLOCK_START is used in the System
# Configuration Header as a pointer to the start of the Keyboard
# Controller Header.  If the crisis module is included, the Keyboard
# Controller Header is before the crisis module.  Otherwise, it is
# before the main Keyboard Controller code.
!if ($(INCLUDE_CRISIS))
KBD_BLOCK_PAGE=$(CRISIS_PAGE)
KBD_BLOCK_START=$(CRISIS_OFFSET)
!else
KBD_BLOCK_PAGE=$(KBC_PAGE)
KBD_BLOCK_START=$(KBC_OFFSET)
!endif

# The ROM goes from 0 to (PAST_LOW_ROM-1).
# dbug_dat is placed at the end of low ROM.
# Then TMON code, RAM, and I/O.
# After that there is more ROM.  (At this point in time, it is not used.)
#
# For the emulation board, the ROM is actually RAM.  The space for RAM
# is taken up by I/O.  So, the upper part of "ROM" is used for the RAM.
#
DBUG_DAT_START=0x40000
RAM_START=0x10000


LINKDEFNAME=link.def   # Linker def file name.
LINKDEFNAME1=link.def
MAKE=tools\nmake /B /C /R    # Command to invoke make utility.

KERNEL_INC =\
    -ICHIP\INC \
    -ICORE\INC \
    -IOEM\$(OEMDIR)\INC \
    -IOEM\$(OEMDIR)\CHIP\INC


V21CMP=-I.^\ $(KERNEL_INC) -mcr16cplus -ffixed-r13
V21ASM=
NSCLIB=lib16cp_Os

# For assembling after compiling.
!if ($(RUNASM))
!else
ASM=-S -n
!endif
#

# Compiler options.
CFLAGS=@temp\ccargs.fil

# Assembler options.
AFLAGS=$(V21ASM) -g

#CHIP_NAME=68AP

!if ("$(CHIP_ID)" == "68Ax")
CORE_RAM_SIZE=0x8000
CODE_RAM_SIZE=0x7FB0
TOP_RAM_O=(0x17B00 - 0x200)
!else
CORE_RAM_SIZE=0x1000
CODE_RAM_SIZE=0xFB0
TOP_RAM_O=0x10B00
!endif 

!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
RAM_SIZE=($(CORE_RAM_SIZE) - 0x700)
!else
RAM_SIZE=($(CORE_RAM_SIZE) - 0x50)
!endif



# For the DEBUG.C file.
!if ("$(SW0)"=="MESSAGES $(ON)")
USE_DEBUG_C=$(ON)
!else
USE_DEBUG_C=$(OFF)
!endif



CHIPDIR    = ..\..\CHIP
COREDIR    = ..\..\CORE
OEMCHIPDIR = OEMCHIP
MODCHIPDIR = MOD\CHIP
MODCOREDIR = MOD\CORE
ROMSRCDIR  = ROMSRC


tmpdir   = ..\..\temp
toolsdir = ..\..\tools
libdir   = ..\..\lib
objdir   = obj


all: setup kernel_core kernel_chip mod build_oem oem_chip \
!IF ($(INCLUDE_CRISIS))
!    if ("$(KBC_OFFSET)"!="AUTO")
     obj\$(CRISIS_ROM) \
!    endif
!ENDIF
     obj\$(ROMNAME) obj\$(DEBUG_NAME).x \
     restore

clean: del_obj
   if exist $(libdir)\*.*        Echo del $(libdir)\*.*
   if exist $(libdir)\*.*             del $(libdir)\*.* <y
   if exist $(libdir) rmdir $(libdir)

   if exist $(tmpdir)\*.*        Echo del $(tmpdir)\*.*
   if exist $(tmpdir)\*.*             del $(tmpdir)\*.* <y
   if exist $(tmpdir) rmdir $(tmpdir)

   if exist $(objdir)\*.*        Echo del $(objdir)\*.*
   if exist $(objdir)\*.*             del $(objdir)\*.* <y
   if exist $(objdir) rmdir $(objdir)

del_obj:
   if exist $(CHIPDIR)\obj\*.*     Echo del $(CHIPDIR)\obj\*.*
   if exist $(CHIPDIR)\obj\*.*     del $(CHIPDIR)\obj\*.* <y
   if exist $(CHIPDIR)\obj         rmdir $(CHIPDIR)\obj

   if exist $(COREDIR)\obj\*.*     Echo del $(COREDIR)\obj\*.*
   if exist $(COREDIR)\obj\*.*     del $(COREDIR)\obj\*.* <y
   if exist $(COREDIR)\obj         rmdir $(COREDIR)\obj

   if exist $(MODCHIPDIR)\obj\*.*  Echo del $(MODCHIPDIR)\obj\*.*
   if exist $(MODCHIPDIR)\obj\*.*  del $(MODCHIPDIR)\obj\*.* <y
   if exist $(MODCHIPDIR)\obj      rmdir $(MODCHIPDIR)\obj

   if exist $(MODCOREDIR)\obj\*.*  Echo del $(MODCOREDIR)\obj\*.*
   if exist $(MODCOREDIR)\obj\*.*  del $(MODCOREDIR)\obj\*.* <y
   if exist $(MODCOREDIR)\obj      rmdir $(MODCOREDIR)\obj

   if exist $(objdir)\*.*          Echo del $(objdir)\*.*
   if exist $(objdir)\*.*          del $(objdir)\*.* <y

#****************** Special character sequences ******************

# ??+GT   Greater than sign: >
# ??+LT   Less than sign:    <

#****************** setup & restore ******************

# Create or refresh CHIPDEF.H, get all the
# and setup the directories.
setup: init $(tmpdir)\ccargs.fil $(objdir)\swtchs.t1 macros dirs $(libdir)\library.fil
   cd ..\..
   echo.
   cd
init:
   echo Setting up...
   if not exist $(tmpdir) md $(tmpdir)
   if not exist $(objdir) md $(objdir)
#   if exist CHIP copy CHIP\REG$(CHIP_ID).c  REGINIT.C>NUL
   if exist CHIP copy CHIP\inc\REG$(CHIP_ID).h  $(CHIPDIR)\inc\REGS.h>NUL
   Echo OK>$(tmpdir)\output.fil

macros:
   set CFLAGS=$(CFLAGS)
   set AFLAGS=$(AFLAGS)
   set OBJTYP=O
   set SWITCH=$(SWITCH)
   set OEMDIR=$(OEMDIR)
   set I2C_SUPPORTED=$(I2C_SUPPORTED)
   set HID_OV_I2C_SUPPORTED=$(HID_OV_I2C_SUPPORTED)
   set REGINIT=REG$(CHIP_ID)


dirs: m_dirs

restore: r_msg

r_msg:
   echo Restoring...

m_dirs:
# Create directories.
    if not exist $(CHIPDIR)\obj md $(CHIPDIR)\obj
    if not exist $(COREDIR)\obj md $(COREDIR)\obj
    if not exist $(MODCHIPDIR)\obj md $(MODCHIPDIR)\obj
    if not exist $(MODCOREDIR)\obj md $(MODCOREDIR)\obj


#****************** $(tmpdir)\ccargs.fil ******************

# If OEMBLD.MAK has been modified (or if CCARGS.FIL
# file does not exist), setup a new CCARGS.FIL file.
# Compiler options.
# -Os to optimize for space.
# -J1 sets the structure-member alignment to bytes.
$(tmpdir)\ccargs.fil: oembld.mak
#
   echo. >$(tmpdir)\ccargs.fil
!  if ("$(ASM)"!="")
   echo $(ASM)>>$(tmpdir)\ccargs.fil
!  endif
!  if ("$(V21CMP)"!="")
   echo $(V21CMP)>>$(tmpdir)\ccargs.fil
!  endif
!  if ("$(SBREL)"!="")
   echo $(SBREL)>>$(tmpdir)\ccargs.fil
!  endif
#
!  if ("$(SWITCH)"=="NORMAL")
   echo -Wimplicit -Os -c>>$(tmpdir)\ccargs.fil
!  else if ("$(SWITCH)"=="DEBUG")
!  if ("$(ASM)"!="")
   echo -Wimplicit -Os>>$(tmpdir)\ccargs.fil
!  else
   echo -Wimplicit -Os -g -c >>$(tmpdir)\ccargs.fil
!  endif
!  else if ("$(SWITCH)"=="CPP")
   echo -Wimplicit -c -C -E>>$(tmpdir)\ccargs.fil
!  endif

#****************** swtchs.h ******************

# If OEMBLD.MAK has been modified (or if SWTCHS.T1
# file does not exist), setup a new SWTCHS.T1 file.
$(objdir)\swtchs.t1: oembld.mak
   echo Checking inc\SWTCHS.H file...
   echo. >$(objdir)\swtchs.t1
   echo #define CMD_BC $(CMD_BC) >>$(objdir)\swtchs.t1
   echo #define RAM_SCAN_TABLE $(RAM_SCAN_TABLE) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO44_WAKEUP $(EXT_GPIO44_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO1_WAKEUP $(EXT_GPIO1_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO46_WAKEUP $(EXT_GPIO46_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO3_WAKEUP $(EXT_GPIO3_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO4_WAKEUP $(EXT_GPIO4_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_GPIO5_WAKEUP $(EXT_GPIO5_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define EXT_IRQ40_WAKEUP $(EXT_IRQ40_WAKEUP) >>$(objdir)\swtchs.t1
   echo #define AUX_PORT3_SUPPORTED $(AUX_PORT3_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define AUX_PORT2_SUPPORTED $(AUX_PORT2_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define SXI_SUPPORTED $(SXI_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define MATRIX_18 $(MATRIX_18) >>$(objdir)\swtchs.t1
   echo #define I2C_SUPPORTED $(I2C_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define SHL_SUPPORTED $(SHL_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define HW_FLOATING_SUPPORTED $(HW_FLOATING_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define HID_OV_I2C_SUPPORTED $(HID_OV_I2C_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define ACPI_SUPPORTED $(ACPI_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define CRISIS_CMD_HANDLER $(CRISIS_CMD_HANDLER) >>$(objdir)\swtchs.t1
   echo #define BOOTABLE_CRISIS $(BOOTABLE_CRISIS) >>$(objdir)\swtchs.t1
   echo #define CRISIS_CMD_0FH $(CRISIS_CMD_0FH) >>$(objdir)\swtchs.t1
   echo #define KBD_BLOCK_PAGE $(KBD_BLOCK_PAGE) >>$(objdir)\swtchs.t1
   echo #define KBD_BLOCK_START $(KBD_BLOCK_START) >>$(objdir)\swtchs.t1
   echo #define INCLUDE_CRISIS $(INCLUDE_CRISIS) >>$(objdir)\swtchs.t1
   echo #define RUN_FROM_MRAM $(RUN_FROM_MRAM) >>$(objdir)\swtchs.t1
   echo #define EXT_HOTKEY $(EXT_HOTKEY) >>$(objdir)\swtchs.t1
   echo #define CHIP_ID $(CHIP_ID) >>$(objdir)\swtchs.t1
   echo #define SMB_FLUPDATE_SUPPORTED $(SMB_FLUPDATE_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define I2C_SLAVE_SUPPORTED $(I2C_SLAVE_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define KEY_SCANNER_SUPPORTED $(KEY_SCANNER_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define EXTEND_CMD_SUPPORTED $(EXTEND_CMD_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define EXTEND_PM1_SUPPORTED $(EXTEND_PM1_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define EXTEND_PM2_SUPPORTED $(EXTEND_PM2_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define MATRIX_SUPPORTED $(MATRIX_SUPPORTED) >>$(objdir)\swtchs.t1
   echo #define MOUSE_EMULATION $(MOUSE_EMULATION) >>$(objdir)\swtchs.t1
   echo #define KERNEL_TMRB_MFT16 $(KERNEL_TMRB_MFT16) >>$(objdir)\swtchs.t1
   echo #define I2C_GPIO_RECOVERY $(I2C_GPIO_RECOVERY) >>$(objdir)\swtchs.t1
   echo #define ESPI_SUPPORTED $(ESPI_SUPPORTED) >>$(objdir)\swtchs.t1
   if not "$(SW0)"=="" echo #define $(SW0) >>$(objdir)\swtchs.t1
   if not "$(SW1)"=="" echo #define $(SW1) >>$(objdir)\swtchs.t1
   if not "$(SW2)"=="" echo #define $(SW2) >>$(objdir)\swtchs.t1
   if not "$(SW3)"=="" echo #define $(SW3) >>$(objdir)\swtchs.t1
   if not "$(SW4)"=="" echo #define $(SW4) >>$(objdir)\swtchs.t1
   if not "$(SW5)"=="" echo #define $(SW5) >>$(objdir)\swtchs.t1
   if not "$(SW6)"=="" echo #define $(SW6) >>$(objdir)\swtchs.t1
   if not "$(SW7)"=="" echo #define $(SW7) >>$(objdir)\swtchs.t1
   if not "$(SW8)"=="" echo #define $(SW8) >>$(objdir)\swtchs.t1
   if not "$(SW9)"=="" echo #define $(SW9) >>$(objdir)\swtchs.t1
   if not "$(SW10)"=="" echo #define $(SW10) >>$(objdir)\swtchs.t1
   if not "$(SW11)"=="" echo #define $(SW11) >>$(objdir)\swtchs.t1
   if not "$(SW12)"=="" echo #define $(SW12) >>$(objdir)\swtchs.t1
   if not "$(SW13)"=="" echo #define $(SW13) >>$(objdir)\swtchs.t1
   if not "$(SW14)"=="" echo #define $(SW14) >>$(objdir)\swtchs.t1
   if not "$(SW15)"=="" echo #define $(SW15) >>$(objdir)\swtchs.t1
   if not "$(SW16)"=="" echo #define $(SW16) >>$(objdir)\swtchs.t1
#
# Switches for assembly code.
   echo #/* >>$(objdir)\swtchs.t1
   echo RUN_FROM_MRAM := $(RUN_FROM_MRAM) >>$(objdir)\swtchs.t1
   echo INCLUDE_CRISIS := $(INCLUDE_CRISIS) >>$(objdir)\swtchs.t1
   echo CRISIS_CMD_HANDLER := $(CRISIS_CMD_HANDLER) >>$(objdir)\swtchs.t1
   Echo #*/ >>$(objdir)\swtchs.t1
#
# If SWTCHS.H does not exist, create it.
#  if not exist inc\swtchs.h echo Creating inc\SWTCHS.H file...
   echo Creating inc\SWTCHS.H file...
#  if not exist inc\swtchs.h copy $(objdir)\swtchs.t1 inc\swtchs.h>NUL
   copy $(objdir)\swtchs.t1 inc\swtchs.h>NUL
# Check if SWTCHS.H is up to date.  If it is not, refresh it.
   fc inc\swtchs.h $(objdir)\swtchs.t1 >$(objdir)\swtchs.t2
   echo @echo off >$(objdir)\swtchs.bat
   echo find "no differences encountered" $(objdir)\swtchs.t2 ??+GT nul >>$(objdir)\swtchs.bat
   echo if not errorlevel 1 goto exit >>$(objdir)\swtchs.bat
   echo copy $(objdir)\swtchs.t1 inc\swtchs.h ??+GT nul >>$(objdir)\swtchs.bat
   echo echo Updated inc\SWTCHS.H file. >>$(objdir)\swtchs.bat
   echo :exit >>$(objdir)\swtchs.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < $(objdir)\swtchs.bat > $(objdir)\swtchs.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < $(objdir)\swtchs.fil > $(objdir)\swtchs.bat
   del $(objdir)\swtchs.fil
   $(objdir)\swtchs.bat
# Cleanup.
   del $(objdir)\swtchs.t2
   del $(objdir)\swtchs.bat

#****************** core ******************

kernel_core:
   echo.
   echo Building core level...
# Build CORE (pure) level.
   copy core\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL

#****************** chip ******************

kernel_chip:
   echo.
   echo Building chip level...
# Build core CHIP-specific level.
   copy chip\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL

#****************** mod ******************

mod:
# Build OEM changes to core and chip.
#
# Changes to core.
   echo.
   echo Building mod\core...
   copy oem\$(OEMDIR)\mod\core\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL
   echo building mod\core done

# Changes to chip.
   echo.
   echo Building mod\chip...
   copy oem\$(OEMDIR)\mod\chip\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL
   echo building mod\chip done

#****************** oem ******************

build_oem:
   echo.
   echo Building oem...
   copy oem\$(OEMDIR)\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL

#****************** oemchip ******************

oem_chip:
   echo.
   echo Building oem\chip...
   copy oem\$(OEMDIR)\CHIP\level.mak temp\file.mak>NUL
   $(MAKE) temp\file.mak
   find "OK" temp\OUTPUT.FIL>NUL

#
# Remove temporary file.
   del temp\file.mak
#
#****************** object ******************
# Get all object files.
# OEM object files are already in oem\$(OEMDIR)\obj directory.
   if exist core\obj\*.o copy core\obj\*.o oem\$(OEMDIR)\obj>NUL
   if exist chip\obj\*.o copy chip\obj\*.o oem\$(OEMDIR)\obj>NUL
   if exist oem\$(OEMDIR)\mod\core\obj\*.o copy oem\$(OEMDIR)\mod\core\obj\*.o oem\$(OEMDIR)\obj>NUL
   if exist oem\$(OEMDIR)\mod\chip\obj\*.o copy oem\$(OEMDIR)\mod\chip\obj\*.o oem\$(OEMDIR)\obj>NUL
#
# Go back to OEM directory.
   cd oem\$(OEMDIR)
   echo.
   cd

tmpdir   = ..\..\temp
toolsdir = ..\..\tools

#****************** ROM files ******************

obj\$(CRISIS_ROM): obj\fb_footr.bin obj\$(CRISIS_NAME).bin obj\cr_footr.bin
   copy /b obj\$(CRISIS_NAME).bin+obj\cr_footr.bin obj\$(CRISIS_ROM)>NUL

obj\$(ROMNAME): obj\$(EC_NAME).bin obj\cr_footr.bin
   if exist obj\file1.bin del obj\file1.bin
   if exist obj\file2.bin del obj\file2.bin
   if exist obj\file3.bin del obj\file3.bin
   if exist obj\file4.bin del obj\file4.bin
   if exist obj\file5.bin del obj\file5.bin
   if exist obj\file6.bin del obj\file6.bin
   if exist obj\file7.bin del obj\file7.bin
   if exist obj\file8.bin del obj\file8.bin
   if exist obj\file9.bin del obj\file9.bin
   if exist obj\filex.bin del obj\filex.bin
   if exist obj\filey.bin del obj\filey.bin
   echo Building $(ROMNAME) from:
!  if ("$(KBC_OFFSET)"=="AUTO")
   echo    OBJ\$(CRISIS_NAME).BIN
   echo    OBJ\CR_FOOTR.BIN
   copy /b obj\$(CRISIS_NAME).bin+obj\cr_footr.bin obj\file2.bin>NUL
!  endif
   echo    OBJ\$(EC_NAME).BIN
#  If SCANTAB1 is specified, use the ROMSRC\$(SCANTAB1) file instead of
#  the scantabs.bin file.
!  if ("$(SCANTAB1)"!="")
   echo    ROMSRC\$(SCANTAB1)
   copy /b ROMSRC\$(SCANTAB1) obj\file6.bin>NUL
!  else
!  if ("$(ROM_SCAN_TABLE)"=="OUTSIDE")
   echo    OBJ\SCANTABS.BIN
   copy /b obj\scantabs.bin obj\file6.bin>NUL
!  endif
!  endif
!  if ("$(SCANTAB2)"!="")
   echo    ROMSRC\$(SCANTAB2)
   copy /b ROMSRC\$(SCANTAB2) obj\file7.bin>NUL
!  endif
!  if ("$(SCANTAB3)"!="")
   echo    ROMSRC\$(SCANTAB3)
   copy /b ROMSRC\$(SCANTAB3) obj\file8.bin>NUL
!  endif
!  if ("$(SCANTAB4)"!="")
   echo    ROMSRC\$(SCANTAB4)
   copy /b ROMSRC\$(SCANTAB4) obj\file9.bin>NUL
!  endif
   copy /b obj\$(EC_NAME).bin obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file2.bin  copy /b obj\file2.bin+obj\$(ROMNAME)  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file1.bin  copy /b obj\file1.bin+obj\$(ROMNAME)  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file3.bin  copy /b obj\$(ROMNAME)+obj\file3.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file4.bin  copy /b obj\$(ROMNAME)+obj\file4.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file5.bin  copy /b obj\$(ROMNAME)+obj\file5.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file6.bin  copy /b obj\$(ROMNAME)+obj\file6.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file7.bin  copy /b obj\$(ROMNAME)+obj\file7.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file8.bin  copy /b obj\$(ROMNAME)+obj\file8.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\filex.bin  del obj\filex.bin
   if exist obj\file9.bin  copy /b obj\$(ROMNAME)+obj\file9.bin  obj\filex.bin>NUL
   if exist obj\filex.bin  copy /b obj\filex.bin                 obj\$(ROMNAME)>NUL
#
   if exist obj\file1.bin del obj\file1.bin
   if exist obj\file2.bin del obj\file2.bin
   if exist obj\file3.bin del obj\file3.bin
   if exist obj\file4.bin del obj\file4.bin
   if exist obj\file5.bin del obj\file5.bin
   if exist obj\file6.bin del obj\file6.bin
   if exist obj\file7.bin del obj\file7.bin
   if exist obj\file8.bin del obj\file8.bin
   if exist obj\file9.bin del obj\file9.bin
   if exist obj\filex.bin del obj\filex.bin
   if exist obj\filey.bin del obj\filey.bin
   echo.


!if $(RUN_FROM_MRAM)
# if run from MRAM need to copy all FLASH image to MRAM, so, bogcheck all
   echo BOGChecking $(ROMNAME) :
   $(toolsdir)\bogcheck -bin obj\$(ROMNAME)
!endif

#****************** libcore.a ******************

obj\libcore.a: oembld.mak \
      obj\PURMAIN.O  obj\PURINI.O   obj\PURDAT.O \
      obj\PURCMD.O   obj\PURSCN.O \
      obj\PURDEV.O   obj\PUREXT.O   \
      obj\PURACPI.O  obj\PURFUNCT.O
   if exist obj\libcore.a del obj\libcore.a
   crar -rc obj\libcore.a obj\PURMAIN.O
   crar -r obj\libcore.a obj\PURINI.O
   crar -r obj\libcore.a obj\PURDAT.O
   crar -r obj\libcore.a obj\PURCMD.O
   crar -r obj\libcore.a obj\PURSCN.O
   crar -r obj\libcore.a obj\PURDEV.O
   crar -r obj\libcore.a obj\PUREXT.O
   crar -r obj\libcore.a obj\PURACPI.O
   crar -r obj\libcore.a obj\PURFUNCT.O

#****************** libraries *******************

# The NSC libraries are copied into the lib directory.
# This allows accessing the library file names in the
# .def file without forcing the NSC directory to be
# on a certain disk drive.  The "NSC" definition is
# set to the NSC directory (in the BUILD.BAT file).

# Create the lib directory if it does not exist.
$(libdir)nul:
   if not exist $(libdir) md $(libdir)

# If any of the libraries are out of date, delete the library
# marker file.  This will cause all the libraries to be refreshed.
$(libdir)\libstart.a: $(NSC)\$(NSCLIB)\libstart.a
   if exist $(libdir)\library.fil del $(libdir)\library.fil>NUL

$(libdir)\libc.a: $(NSC)\$(NSCLIB)\libc.a
   if exist $(libdir)\library.fil del $(libdir)\library.fil>NUL

$(libdir)\libd.a: $(NSC)\$(NSCLIB)\libd.a
   if exist $(libdir)\library.fil del $(libdir)\library.fil>NUL

$(libdir)\libhfp.a: $(NSC)\$(NSCLIB)\libhfp.a
   if exist $(libdir)\library.fil del $(libdir)\library.fil>NUL

# Refresh all the libraries if the library marker file does not
# exist or if any library is newer than the library marker file.
$(libdir)\library.fil: $(libdir)nul \
      $(NSC)\$(NSCLIB)\libstart.a $(NSC)\$(NSCLIB)\libc.a \
      $(NSC)\$(NSCLIB)\libd.a     $(NSC)\$(NSCLIB)\libhfp.a
   echo Library marker file.>>$(libdir)\library.fil
   if exist $(NSC)\$(NSCLIB)\libstart.a copy $(NSC)\$(NSCLIB)\libstart.a $(libdir)>NUL
   if exist $(NSC)\$(NSCLIB)\libc.a     copy $(NSC)\$(NSCLIB)\libc.a     $(libdir)>NUL
   if exist $(NSC)\$(NSCLIB)\libd.a     copy $(NSC)\$(NSCLIB)\libd.a     $(libdir)>NUL
   if exist $(NSC)\$(NSCLIB)\libhfp.a   copy $(NSC)\$(NSCLIB)\libhfp.a   $(libdir)>NUL

#****************** crisis.cmd ******************

# Linker command file for the crisis module.

obj\crisis.cmd: oembld.mak
   if exist obj\crisis.cmd del obj\crisis.cmd
   if exist obj\file.ire del obj\file.ire
   if exist obj\file.ird del obj\file.ird
   echo Environment option file.>obj\file.$(ENVIRON)
   if exist obj\file.ire echo obj\IREOPT.O>>obj\crisis.cmd
   echo obj\HEADER.O>obj\crisis.cmd
   echo obj\CRSISRST.O>>obj\crisis.cmd
   if exist obj\file.ird echo obj\IRDOPT.O>>obj\crisis.cmd
   if exist obj\file.ire del obj\file.ire
   if exist obj\file.ird del obj\file.ird
   echo obj\OEMCRSIS.O>>obj\crisis.cmd
   echo obj\CRISIS.O>>obj\crisis.cmd
!  if $(CRISIS_CMD_HANDLER)
   echo obj\PURCRSIS.O>>obj\crisis.cmd
!  endif
# ------------------------------------------------
   echo obj\CRSISICU.O>>obj\crisis.cmd
   echo obj\fu_init.o>>obj\crisis.cmd
   echo obj\fu_hndl.o>>obj\crisis.cmd
!  if ("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)")
   echo obj\fu_ram.o>>obj\crisis.cmd
   echo obj\fu_utils.o>>obj\crisis.cmd
!  endif
   echo obj\shm.o>>obj\crisis.cmd
   echo -M>>obj\crisis.cmd
# -Llib specifies that the lib directory is to be searched for libraries.
   echo -Llib>>obj\crisis.cmd
#
# The following is for linking in the Application Development Board library (libstart.a):
   echo -lstart>>obj\crisis.cmd
#
# The following is for linking in the library libc.a:
   echo -lc>>obj\crisis.cmd
#
# The following is for linking in the floating point dummy library (libd.a):
   echo -ld>>obj\crisis.cmd
#
   echo -T obj\crisis.def>>obj\crisis.cmd

#****************** linker.cmd ******************

obj\linker.cmd: oembld.mak
   if exist obj\linker.cmd del obj\linker.cmd
!  if ($(INCLUDE_CRISIS))
   echo obj\CRSISOPT.O>obj\linker.cmd
   echo obj\RESET.O>>obj\linker.cmd
!  else
   echo obj\HEADER.O>>obj\linker.cmd
   echo obj\RESET.O>>obj\linker.cmd
#   echo obj\OEMCRSIS.O>>obj\linker.cmd
!  endif
   echo obj\PS2.O>>obj\linker.cmd
   echo obj\HOST_IF.O>>obj\linker.cmd
   echo obj\ICU.O>>obj\linker.cmd
   echo obj\HOOK.O>>obj\linker.cmd
   echo obj\SMB_TRAN.O>>obj\linker.cmd
   echo obj\OEMDAT.O>>obj\linker.cmd
   echo obj\OEMINI.O>>obj\linker.cmd
   echo obj\OEMMAIN.O>>obj\linker.cmd
   echo obj\OEMCMDS.O>>obj\linker.cmd
   echo obj\OEMPECI.O>>obj\linker.cmd
   echo obj\HCONFIG.O>>obj\linker.cmd
   echo obj\EVENTS.O>>obj\linker.cmd
   echo obj\PWRSEQ.O>>obj\linker.cmd
   echo obj\A_TO_D.O>>obj\linker.cmd
   echo obj\D_TO_A.O>>obj\linker.cmd
   echo obj\DEV.O>>obj\linker.cmd
   echo obj\SCAN.O>>obj\linker.cmd
   echo obj\PROC.O>>obj\linker.cmd
   echo obj\I2C.O>>obj\linker.cmd
!if $(ESPI_SUPPORTED)
   echo obj\ESPI.O>>obj\linker.cmd
!endif
#   echo obj\SELFTEST.O>>obj\linker.cmd
   echo obj\PURXLT.O>>obj\linker.cmd
   echo obj\OEM.O>>obj\linker.cmd
   echo obj\IRQ.O>>obj\linker.cmd
   echo obj\CLOCK.O>>obj\linker.cmd
   echo obj\TIMERS.O>>obj\linker.cmd
   echo obj\SIB.O>>obj\linker.cmd
   echo obj\PWM.O>>obj\linker.cmd
   echo obj\PORT80.O>>obj\linker.cmd
   echo obj\PECI.O>>obj\linker.cmd
!if ("$(SW16)"=="CR_UART_SUPPORTED $(ON)")
   echo obj\CR_UART.O>>obj\linker.cmd
!endif
!if ("$(SW14)"=="ECDT_SHM_SUPPORTED $(ON)")
   echo obj\ecdthif.O>>obj\linker.cmd
!endif
!if $(HID_OV_I2C_SUPPORTED)
   echo obj\HID.O>>obj\linker.cmd
!endif
   echo obj\PURPM.O>>obj\linker.cmd
!if $(HID_OV_I2C_SUPPORTED)
   echo obj\HID_I2C.O>>obj\linker.cmd
!endif
!if $(SHL_SUPPORTED)
   echo obj\SENSRHUB.O>>obj\linker.cmd
!endif
!if $(SMB_FLUPDATE_SUPPORTED)
   echo obj\FU_OEM.O>>obj\linker.cmd
!endif
# Add register initialization table.
   echo obj\REG$(CHIP_ID).O>>obj\linker.cmd
# Add scanner table if table is built into ROM.
!  if ("$(ROM_SCAN_TABLE)"=="INSIDE")
   echo obj\SCANTABS.O>>obj\linker.cmd
!  endif
#
!  if $(USE_DEBUG_C)
   echo obj\DEBUG.O>>obj\linker.cmd
!  endif
!  if (!$(INCLUDE_CRISIS))
   echo obj\fu_init.o>>obj\linker.cmd
   echo obj\fu_hndl.o>>obj\linker.cmd
!  if ("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)")
   echo obj\fu_ram.o>>obj\linker.cmd
   echo obj\fu_utils.o>>obj\linker.cmd
!  endif
   echo obj\shm.o>>obj\linker.cmd
!  endif
   echo -M>>obj\linker.cmd
# The LIBCORE.A library is in the obj directory.
# -Lobj specifies that the obj directory is to be searched for libraries.
# -lcore specifies that the LIBCORE.A library is to be linked in.
   echo -Lobj>>obj\linker.cmd
   echo -lcore>>obj\linker.cmd
# -Llib specifies that the lib directory is to be searched for libraries.
!  if $(HW_FLOATING_SUPPORTED)
   echo -lsfa>>obj\linker.cmd
!  endif
   echo -Llib>>obj\linker.cmd
#
# The following is for linking in the Application Development Board library (libstart.a):
   echo -lstart>>obj\linker.cmd
#
# The following is for linking in the library libc.a:
   echo -lc>>obj\linker.cmd
!  if !$(SHL_SUPPORTED)
#
# The following is for linking in the floating point dummy library (libd.a):
   echo -ld>>obj\linker.cmd
!  else
# The following is for linking in the floating point emulation library (libhfp.a):
   echo -lhfp>>obj\linker.cmd
#
!  endif
   echo -T obj\linker.def>>obj\linker.cmd


#******************* $(CRISIS_NAME).bin ********************

# The crisis module binary file.

obj\$(CRISIS_NAME).bin: obj\crisis.cmd $(LINKDEFNAME) \
!     if $(CRISIS_CMD_HANDLER)
      obj\PURCRSIS.O \
!     endif
      obj\OEMCRSIS.O obj\CRISIS.O \
!if ("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)")
      obj\fu_init.o obj\fu_hndl.o obj\shm.o \
      obj\fu_ram.o obj\fu_utils.o
!else
      obj\fu_init.o obj\fu_hndl.o obj\shm.o
!endif
# ****************** obj\crisis1.def ******************
# Setup obj\crisis1.def with most of a MEMORY statement and the
# beginning of the SECTIONS statement for the linker definition file.
   echo MEMORY {>obj\crisis1.def
#   ecpad /cp $(CRISIS_SIZE) - $(CRISIS_OFFSET) /p "rom(rx) : ORIGIN = $(CRISIS_OFFSET), LENGTH = 0x%X">>obj\crisis1.def
#   echo. >>obj\crisis1.def
   echo rom(rx) : ORIGIN = $(CRISIS_OFFSET), LENGTH = $(CRISIS_SIZE) - 0x100>>obj\crisis1.def
   echo /* next is for flash update handle function */>>obj\crisis1.def
   echo top_rom(rx)    :   ORIGIN = $(CRISIS_END) - 0x100,    LENGTH = 0x100>>obj\crisis1.def
   echo rom_sig(rx) : ORIGIN = $(HDR_START), LENGTH = $(HDR_SIZE)>>obj\crisis1.def
   echo ram(w) : ORIGIN = $(RAM_START), LENGTH = $(RAM_SIZE)>>obj\crisis1.def
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo /* next is for flash updare code and data - 4Kb RAM, 0x24 saved for buffers */>>obj\crisis1.def
   echo top_ram(w)    :   ORIGIN = $(TOP_RAM_O),    LENGTH = 0x500 - 0x24>>obj\crisis1.def
!else
   echo /* saved for flash update rom code variables */>>obj\crisis1.def
   echo /* _FLASH_jbb (rom based) - 0x10FBE (2)    */>>obj\crisis1.def
   echo /* fu variables           - 0x10FC0 (0x20) */>>obj\crisis1.def
   echo /* _FLASH_commands        - 0x10FE0 (0x10) */>>obj\crisis1.def
   echo /* _write_command_buffer  - 0x10FF0 (0x10) */>>obj\crisis1.def
   echo top_ram     : ORIGIN = $(TOP_RAM_O), LENGTH = 0x42>>obj\crisis1.def
!endif
   echo }>>obj\crisis1.def
   echo SECTIONS {>>obj\crisis1.def
# The beginning of the code or the Keyboard Controller Header is placed first.
# Start of crisis code.
#   echo .sig : { SHORT(0x8761) } ??+GTrom_sig >>obj\crisis1.def
   echo .header : {*(.header_4) *(.header_2) *(.header_1) *(.header)} ??+GTrom_sig >>obj\crisis1.def
   echo .begin :{>>obj\crisis1.def
   echo "obj\CRSISRST.O"(.text)>>obj\crisis1.def
   echo } ??+GTrom>>obj\crisis1.def
# ****************** obj\crisis2.def ******************
# Setup obj\crisis2.def with more of the MEMORY statement.
# The rest of the code and data are placed.
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo /* Flash Update Code that is run from RAM (It also allocated space in flash) */>obj\crisis2.def
   echo .fwu_ram_flash :>>obj\crisis2.def
   echo {>>obj\crisis2.def
   echo   __FWU_START = .; obj\fu_ram.o(.text) obj\fu_utils.o(.text) __FWU_END = .;>>obj\crisis2.def
   echo } ??+GT top_ram AT ??+GT rom>>obj\crisis2.def
   echo /* Flash Update data to top_ram */>>obj\crisis2.def
   echo .fwu_common : { >>obj\crisis2.def
   echo obj\fu_init.o(.bss_4)  obj\fu_init.o(.bss_2)  obj\fu_init.o(.bss_1) >>obj\crisis2.def
   echo obj\fu_init.o(.bss)    obj\fu_init.o(COMMON)                        >>obj\crisis2.def
   echo obj\fu_ram.o(.bss_4)   obj\fu_ram.o(.bss_2)   obj\fu_ram.o(.bss_1)  >>obj\crisis2.def
   echo obj\fu_ram.o(.bss)     obj\fu_ram.o(COMMON)                         >>obj\crisis2.def
   echo obj\fu_utils.o(.bss_4) obj\fu_utils.o(.bss_2) obj\fu_utils.o(.bss_1)>>obj\crisis2.def
   echo obj\fu_utils.o(.bss)   obj\fu_utils.o(COMMON)                       >>obj\crisis2.def
   echo obj\shm.o(.bss_4)      obj\shm.o(.bss_2)      obj\shm.o(.bss_1)     >>obj\crisis2.def
   echo obj\shm.o(.bss)        obj\shm.o(COMMON)                            >>obj\crisis2.def
   echo } ??+GTtop_ram>>obj\crisis2.def
   echo /* Flash Update Handle function to fixed address (top_rom) */>>obj\crisis2.def
   echo .fwu_hndl : { obj\fu_hndl.o(.text) } ??+GTtop_rom>>obj\crisis2.def
!else
   echo /* Flash Update Handle function to fixed address (top_rom) */>obj\crisis2.def
   echo .fwu_hndl : { obj\fu_hndl.o(.text) } ??+GTtop_rom>>obj\crisis2.def
!endif
   echo .text : { *(.text) } ??+GTrom>>obj\crisis2.def
   echo .init ALIGN(2) : {} ??+GTrom>>obj\crisis2.def
   echo .data :>>obj\crisis2.def
   echo { *(.data) *(.data_4) *(.data_2) *(.data_1) } ??+GTram AT ??+GTrom>>obj\crisis2.def
# Here is the beginning of the last rom section.
   echo .end :>>obj\crisis2.def
   echo { *(.rdata) *(.rdata_4) *(.rdata_2) *(.rdata_1) >>obj\crisis2.def
#
# Setup a file with a MEMORY statement.
   copy obj\crisis1.def+obj\crisis2.def obj\file.mem /b>NUL
# Setup a file with the end of the last rom section without the gap.
   echo } ??+GTrom>obj\file.end
# Setup a file with _CHECKSUM_DATA initilized to 0.
   echo _CHECKSUM_DATA = 0x00;>obj\file.chk
# Setup crisis.def with linker definition
# file and initialized _CHECKSUM_DATA.
   copy obj\file.mem+obj\file.end+$(LINKDEFNAME)+obj\file.chk obj\crisis.fil /b>NUL
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\crisis.fil > obj\crisis.def
# Get the definition of the address past the code.
   echo _OUTSIDE_ADDR = (ADDR(.end) + SIZEOF(.end) + _GAP);>>obj\crisis.def
   echo _CRISIS_END = $(CRISIS_END);>>obj\crisis.def
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo __FWU_IMAGE_START = LOADADDR(.fwu_ram_flash);>>obj\crisis.def
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\crisis.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\crisis.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\crisis.def
   echo _BOOT_BLOCK_ADDRESS = $(CRISIS_END)+0x14;>>obj\crisis.def
!else
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\crisis.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\crisis.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\crisis.def
   echo _BOOT_BLOCK_ADDRESS = $(CRISIS_END)+0x16;>>obj\crisis.def
!endif
# Link
   echo crld @obj\crisis.cmd -o obj\crisis_t.x obj\crisis_t.map
   crld @obj\crisis.cmd  -o obj\crisis_t.x >obj\crisis_t.map
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file and to binary file.
   crobjcopy -O binary obj\crisis_t.x obj\$(CRISIS_NAME).bin
#   crprom -i -l1024 -w1 -r -n -o obj\$(CRISIS_NAME).hex obj\crisis_t.x
#   h2b obj\$(CRISIS_NAME).hex obj\$(CRISIS_NAME).bin $(CRISIS_OFFSET)
#   cd obj
#   go
#   cd..
# Calculate checksum on binary file and   setup file.chk.
   $(toolsdir)\ecpad /cs obj\$(CRISIS_NAME).bin /p "_CHECKSUM_DATA = 0x%X;">obj\file.chk
   echo. >>obj\file.chk
# Setup crisis.def with linker definition file,
# correct gap size, and correct _CHECKSUM_DATA.
# The ROM size is the size of binary file expanded to a multiple of
# 256 bytes and the gap is the number of bytes needed to expand the file.
# This does not include the Keyboard Controller Header (if present).
   echo. >>obj\file.end
   copy obj\file.mem+obj\file.end+$(LINKDEFNAME)+obj\file.chk obj\crisis.fil /b>NUL
   del obj\file.mem>NUL
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\crisis.fil > obj\crisis.def
# Get the definition of the address past the code.
# Gap size is already included here.
   echo _OUTSIDE_ADDR = (ADDR(.end) + SIZEOF(.end));>>obj\crisis.def
   echo _CRISIS_END = $(CRISIS_END);>>obj\crisis.def
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo __FWU_IMAGE_START = LOADADDR(.fwu_ram_flash);>>obj\crisis.def
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\crisis.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\crisis.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\crisis.def
   echo _BOOT_BLOCK_ADDRESS = $(CRISIS_END)+0x14;>>obj\crisis.def
!else
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\crisis.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\crisis.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\crisis.def
   echo _BOOT_BLOCK_ADDRESS = $(CRISIS_END)+0x16;>>obj\crisis.def
!endif
# Link with correct gap size and _CHECKSUM_DATA.
#   del obj\$(CRISIS_NAME).map>NUL
   echo crld @obj\crisis.cmd -o obj\$(CRISIS_NAME).x obj\$(CRISIS_NAME).map
   crld @obj\crisis.cmd -o obj\$(CRISIS_NAME).x >obj\$(CRISIS_NAME).map
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file and to binary file.
   crobjcopy -O binary obj\$(CRISIS_NAME).x obj\$(CRISIS_NAME).bin
#   crprom -i -l1024 -w1 -r -n -o obj\$(CRISIS_NAME).hex obj\$(CRISIS_NAME).x
#   h2b obj\$(CRISIS_NAME).hex obj\$(CRISIS_NAME).bin $(CRISIS_OFFSET)
# The binary file can be used to check the build.
# The following is used just as a check.  The checksum should be 0.
# Calculate checksum on binary file.
#   echo At this point, the checksum on obj\$(CRISIS_NAME).bin should be 0.
#   $(toolsdir)\ecpad /cs obj\$(CRISIS_NAME).bin /p "Checksum is: %d"
   echo.
   echo.
#
   echo Align to Boot Block size.
   $(toolsdir)\ecpad /fs obj\$(CRISIS_NAME).bin /f obj\file1.bin /a $(CRISIS_SIZE)
   copy obj\file1.bin obj\$(CRISIS_NAME).bin
   echo BOGChecking $(CRISIS_NAME) :
   $(toolsdir)\bogcheck -bin obj\$(CRISIS_NAME).bin

#******************* cr_footr.bin *******************

# The crisis module's footer file.  This also generates the
# setstart.bat file.  The setstart.bat file is used to set the SRT
# environment variable to setup the start of the main Keyboard
# Controller code location.

# Room for the footer.
FOOTER_SIZE=0x100
!IF ($(INCLUDE_CRISIS))
# Generate footer file and setstart.bat file.
obj\cr_footr.bin: obj\$(CRISIS_NAME).bin obj\cr_footr.o
!else
# Only generate setstart.bat file.
obj\cr_footr.bin:
!endif
!if ($(INCLUDE_CRISIS))
# Setup a file with a MEMORY and a SECTIONS statement for the linker definition file.
   echo MEMORY {>obj\footr1.def
   echo rom : ORIGIN = 0, LENGTH = $(FOOTER_SIZE) >>obj\footr1.def
   echo }>>obj\footr1.def
   echo SECTIONS {>>obj\footr1.def
   echo .cr_footr ALIGN(2):{ *(.data) *(.data_4) *(.data_2) *(.data_1) }>>obj\footr1.def
   echo }>>obj\footr1.def
   echo _KBC_PAGE = $(KBC_PAGE);>>obj\footr1.def
# Link file
   echo obj\CR_FOOTR.O>obj\cr_footr.cmd
   echo -e _Footer>>obj\cr_footr.cmd
   echo -M>>obj\cr_footr.cmd
   echo -T obj\cr_footr.def>>obj\cr_footr.cmd
# Link with offset of 0.
   echo _KBC_OFFSET = 0;>obj\footroff.def
   copy obj\footr1.def+obj\footroff.def obj\cr_footr.def>NUL
   echo crld @obj\cr_footr.cmd -o obj\CR_FOOTR.X obj\footrtmp.map
   crld @obj\cr_footr.cmd -o obj\CR_FOOTR.X >obj\footrtmp.map
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file.
   crobjcopy -O binary obj\cr_footr.x obj\cr_footr.bin
#   crprom -i -l1024 -w1 -r -n -o obj\cr_footr.hex obj\cr_footr.x
# Convert to binary file.
#   h2b obj\cr_footr.hex obj\cr_footr.bin 0
#
!endif
!  if ("$(KBC_OFFSET)"=="AUTO")
#  The following is used when main Keyboard Controller code comes after crisis module.
   echo @Echo off>obj\setstart.bat
   echo @Echo off>obj\setsiz1.bat
   echo @Echo off>obj\setsiz2.bat
   echo @Echo off>obj\setsiz3.bat
   $(toolsdir)\ecpad /fs obj\$(CRISIS_NAME).bin /p "set SIZ=0x%X">>obj\setsiz1.bat
   echo.>>obj\setsiz1.bat
   echo $(toolsdir)\ecpad /cp $(CRISIS_START) + ^%SIZ^% /p "set SIZ=0x%%%lX"??+GT??+GTobj\setsiz2.bat>>obj\setsiz1.bat
   echo echo.??+GT??+GTobj\setsiz2.bat>>obj\setsiz1.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\setsiz1.bat > obj\setsiz1.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < obj\setsiz1.fil > obj\setsiz1.bat
   del obj\setsiz1.fil>NUL
   obj\setsiz1
   $(toolsdir)\ecpad /fs obj\fb_footr.bin /p "set SIZ1=0x%X">>obj\setsiz2.bat
   echo.>>obj\setsiz2.bat
   echo $(toolsdir)\ecpad /cp ^%SIZ^% - ^%SIZ1^% /p "set SIZ=0x%%%lX"??+GT??+GTobj\setsiz3.bat>>obj\setsiz2.bat
   echo echo.??+GT??+GTobj\setsiz3.bat>>obj\setsiz2.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\setsiz2.bat > obj\setsiz2.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < obj\setsiz2.fil > obj\setsiz2.bat
   #del obj\setsiz2.fil>NUL
   obj\setsiz2
   $(toolsdir)\ecpad /fs obj\cr_footr.bin /p "set SIZ2=0x%X">>obj\setsiz3.bat
   echo.>>obj\setsiz3.bat
   echo $(toolsdir)\ecpad /cp ^%SIZ^% + ^%SIZ2^% /p "set SRT=0x%%%lX"??+GT??+GTobj\setstart.bat>>obj\setsiz3.bat
   echo echo.??+GT??+GTobj\setstart.bat>>obj\setsiz3.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\setsiz3.bat > obj\setsiz3.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < obj\setsiz3.fil > obj\setsiz3.bat
   #del obj\setsiz3.fil>NUL
   obj\setsiz3
   echo %1>>obj\setstart.bat
!  else
#  The following is used when main Keyboard Controller code is located at KBC_OFFSET.
   echo @Echo off>obj\setstart.bat
   echo set SRT=$(KBC_OFFSET)>>obj\setstart.bat
   echo %1>>obj\setstart.bat
!  endif
#
!if ($(INCLUDE_CRISIS))
# Link with correct offset.
   echo echo _KBC_OFFSET = ^%SRT^%;??+GT??+GTobj\footroff.def>obj\tmp.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\tmp.bat > obj\tmp.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < obj\tmp.fil > obj\tmp.bat
   del obj\tmp.fil>NUL
   obj\setstart obj\tmp
   copy obj\footr1.def+obj\footroff.def obj\cr_footr.def>NUL
   echo crld @obj\cr_footr.cmd -o obj\CR_FOOTR.X obj\cr_footr.map
   crld @obj\cr_footr.cmd -o obj\CR_FOOTR.X >obj\cr_footr.map
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file.
   crobjcopy -O binary obj\cr_footr.x obj\cr_footr.bin
#   crprom -i -l1024 -w1 -r -n -o obj\cr_footr.hex obj\cr_footr.x
# Convert to binary file.
#   h2b obj\cr_footr.hex obj\cr_footr.bin 0
!  endif



#******************* scantabs.bin *******************

# The Scanner Table binary file.

# Room for the scanner tables.
# <<< 2001/4/27 Modified for 128K supported.
SCANTABS_SIZE=0x120  #0x112
# >>> 2001/4/27 Modified.
obj\scantabs.bin: obj\scantabs.x
# Convert to binary file.
   crobjcopy -O binary obj\scantabs.x obj\scantabs.bin

obj\scantabs.x: obj\linker.cmd $(LINKDEFNAME) obj\scantabs.o
# Setup a file with a MEMORY and a SECTIONS statement for the linker definition file.
   echo MEMORY {rom : ORIGIN = 0, LENGTH = $(SCANTABS_SIZE)}>obj\scantabs.def
   echo SECTIONS {>>obj\scantabs.def
   echo .scantabs ALIGN(1) :{ *(.data) *(.data_4) *(.data_2) *(.data_1) }>>obj\scantabs.def
   echo }>>obj\scantabs.def
# Link
   echo obj\SCANTABS.O>obj\scantabs.cmd
   echo -e _Rc_ROM_Tables>>obj\scantabs.cmd
   echo -M>>obj\scantabs.cmd
   echo -T obj\scantabs.def>>obj\scantabs.cmd
   echo crld @obj\scantabs.cmd -o obj\SCANTABS.X obj\scantabs.map
   crld @obj\scantabs.cmd -o obj\SCANTABS.X >obj\scantabs.map
   del obj\scantabs.def>NUL
   del obj\scantabs.cmd>NUL
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL

#****************** $(EC_NAME).bin ******************

obj\$(EC_NAME).bin: obj\cr_footr.bin obj\linker.cmd $(LINKDEFNAME1) \
# obj\$(EC_NAME).bin: obj\cr_footr.bin obj\linker.cmd $(LINKDEFNAME1) \
      obj\libcore.a \
!     if ("$(ROM_SCAN_TABLE)"=="OUTSIDE")
      obj\scantabs.bin \
!     endif
!     if $(USE_DEBUG_C)
      obj\DEBUG.O \
!     endif
!     if $(INCLUDE_CRISIS)
      obj\CRSISOPT.O \
!     endif
      obj\RESET.O obj\PS2.O \
      obj\HOST_IF.O  obj\HOOK.O \
      obj\A_TO_D.O   obj\D_TO_A.O \
      obj\DEV.O   obj\PROC.O  obj\I2C.O \
      obj\SELFTEST.O obj\OEMCRSIS.O \
!if $(ESPI_SUPPORTED)
      obj\ESPI.O \
!endif
      obj\ICU.O      \
      obj\PWRSEQ.O   \
      obj\OEMDAT.O   \
      obj\OEMMAIN.O  \
      obj\OEMCMDS.O  \
      obj\OEMPECI.O  \
      obj\HCONFIG.O  \
      obj\SMB_TRAN.O \
      obj\OEMINI.O   \
      obj\EVENTS.O   \
!if ("$(SW16)"=="CR_UART_SUPPORTED $(ON)")
      obj\CR_UART.O  \
!endif
      obj\OEM.O obj\IRQ.O obj\CLOCK.O obj\TIMERS.O obj\PWM.O obj\PORT80.O obj\PECI.O \
!if (!$(INCLUDE_CRISIS))
      obj\fu_init.o obj\fu_hndl.o obj\shm.o \
!  if ("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)")
      obj\fu_ram.o obj\fu_utils.o \
!  endif
!endif
      obj\SCAN.O     obj\PURXLT.O
# ****************** obj\link1.def ******************
# Setup obj\link1.def with most of a MEMORY statement and the
# beginning of the SECTIONS statement for the linker definition file.
# "tmon_data" and "tmon_code" are factory addresses.  The Rom
# Based Monitor resides in these addresses DO NOT CHANGE THEM.
   echo $(toolsdir)\ecpad /cp $(DBUG_DAT_START) - ^%SRT^% /p "rom(rx) : ORIGIN = "^%SRT^%", LENGTH = 0x%%%%lX"??+GTobj\link1.def>obj\tmp.bat
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\tmp.bat > obj\tmp.fil
   $(toolsdir)\ecpad /fr "??+LT" "<" < obj\tmp.fil > obj\tmp.bat
   del obj\tmp.fil>NUL
   obj\setstart obj\tmp
   echo. >>obj\link1.def
   echo rom_sig(rx) : ORIGIN = $(HDR_START), LENGTH = $(HDR_SIZE)>>obj\link1.def
   echo ram(w) :       ORIGIN = $(RAM_START),  LENGTH = $(RAM_SIZE)>>obj\link1.def
!if (!$(INCLUDE_CRISIS))
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo /* next is for flash updare code and data - 4Kb RAM, 0x24 saved for buffers */>>obj\link1.def
   REM echo top_ram(w)    :   ORIGIN = 0x10B00,    LENGTH = 0x500 - 0x24>>obj\link1.def
   echo top_ram(w)    :   ORIGIN = $(TOP_RAM_O),    LENGTH = 0x500 - 0x24>>obj\link1.def
! else
   echo /* saved for flash update rom code variables */>>obj\link1.def
   echo /* _FLASH_jbb (rom based) - 0x10FBE (2)    */>>obj\link1.def
   echo /* fu variables           - 0x10FC0 (0x20) */>>obj\link1.def
   echo /* _FLASH_commands        - 0x10FE0 (0x10) */>>obj\link1.def
   echo /* _write_command_buffer  - 0x10FF0 (0x10) */>>obj\link1.def
   echo top_ram     : ORIGIN = $(TOP_RAM_O), LENGTH = 0x42>>obj\link1.def
! endif
!endif
   echo }>>obj\link1.def
   echo SECTIONS {>>obj\link1.def
!  if $(INCLUDE_CRISIS)
# The Keyboard Controller Header used with the crisis module is placed
# first and then the start of the Keyboard Controller code.
   echo .begin :{>>obj\link1.def
   echo "obj\CRSISOPT.O"(.text)>>obj\link1.def
   echo "obj\RESET.O"(.text)>>obj\link1.def
   echo } ??+GTrom>>obj\link1.def
!  else
# The beginning of the code or the Keyboard Controller Header is placed first.
# Keyboard Controller Header (if present).
   echo .khdr :{>>obj\link1.def
   echo }??+GTrom>>obj\link1.def
# Start of Keyboard Controller code.
#   echo .sig : { SHORT(0x8761) } ??+GTrom_sig >>obj\link1.def
   echo .header : {*(.header_4) *(.header_2) *(.header_1) *(.header)} ??+GTrom_sig >>obj\link1.def
   echo .begin :{>>obj\link1.def
   echo "obj\RESET.O"(.text)>>obj\link1.def
   echo }??+GTrom>>obj\link1.def
!  endif
!  if $(USE_DEBUG_C)
# The debug code and data are placed next to
# allow the size to be monitored in the map file.
   echo .dbgcod ALIGN(2) :>>obj\link1.def
   echo    { "$(libdir)\libc.a"(.text)    "obj\DEBUG.O"(.text)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.rdata_2) "obj\DEBUG.O"(.rdata_2)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.rdata_4) "obj\DEBUG.O"(.rdata_4)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.rdata_1) "obj\DEBUG.O"(.rdata_1) }??+GTrom>>obj\link1.def
   echo .dbgdat ALIGN(2) :>>obj\link1.def
   echo    { "$(libdir)\libc.a"(.data_2)  "obj\DEBUG.O"(.data_2)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.data_4)  "obj\DEBUG.O"(.data_4)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.data_1)  "obj\DEBUG.O"(.data_1)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.bss_2)   "obj\DEBUG.O"(.bss_2)>>obj\link1.def
   echo      "$(libdir)\libc.a"(.bss_1)   "obj\DEBUG.O"(.bss_1) }??+GTram>>obj\link1.def
!  endif
# ****************** obj\link2.def ******************
# Setup obj\link2.def with more of the MEMORY statement.
# The rest of the code and data are placed.
!if (!$(INCLUDE_CRISIS))
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo /* Flash Update Code that is run from RAM (It also allocated space in flash) */>obj\link2.def
   echo .fwu_ram_flash :>>obj\link2.def
   echo {>>obj\link2.def
   echo   __FWU_START = .; obj\fu_ram.o(.text) obj\fu_utils.o(.text) __FWU_END = .;>>obj\link2.def
   echo } ??+GT top_ram AT ??+GT rom>>obj\link2.def
   echo /* Flash Update data to top_ram */>>obj\link2.def
   echo .fwu_common : { >>obj\link2.def
   echo obj\fu_init.o(.bss_4)  obj\fu_init.o(.bss_2)  obj\fu_init.o(.bss_1) >>obj\link2.def
   echo obj\fu_init.o(.bss)    obj\fu_init.o(COMMON)                        >>obj\link2.def
   echo obj\fu_ram.o(.bss_4)   obj\fu_ram.o(.bss_2)   obj\fu_ram.o(.bss_1)  >>obj\link2.def
   echo obj\fu_ram.o(.bss)     obj\fu_ram.o(COMMON)                         >>obj\link2.def
   echo obj\fu_utils.o(.bss_4) obj\fu_utils.o(.bss_2) obj\fu_utils.o(.bss_1)>>obj\link2.def
   echo obj\fu_utils.o(.bss)   obj\fu_utils.o(COMMON)                       >>obj\link2.def
   echo obj\shm.o(.bss_4)      obj\shm.o(.bss_2)      obj\shm.o(.bss_1)     >>obj\link2.def
   echo obj\shm.o(.bss)        obj\shm.o(COMMON)                            >>obj\link2.def
   echo } ??+GTtop_ram>>obj\link2.def
   echo .text : { *(.text) }??+GTrom>>obj\link2.def
!  else
   echo .text : { *(.text) }??+GTrom>obj\link2.def
!  endif
!else
   echo .text ALIGN(2) : { *(.text) }??+GTrom>obj\link2.def
!endif
   echo .init ALIGN(2) : {}??+GTrom>>obj\link2.def
   echo .data :>>obj\link2.def
   echo { *(.data) *(.data_4) *(.data_2) *(.data_1)} ??+GTram AT ??+GTrom>>obj\link2.def
# Here is the beginning of the last rom section.
   echo .end  :>>obj\link2.def
   echo { *(.rdata) *(.rdata_4) *(.rdata_2) *(.rdata_1) >>obj\link2.def
# Setup a file with a MEMORY statement.
   echo MEMORY {>obj\file1.mem
   copy obj\file1.mem+obj\link1.def+obj\link2.def obj\file.mem /b>NUL
   del obj\file1.mem>NUL
# Setup a file with the end of the last rom section without the gap.
   echo }??+GTrom>obj\file.end
# Setup a file with _CHECKSUM_DATA initilized to 0.
   echo _CHECKSUM_DATA = 0x00;>obj\file.chk
# Setup linker.def with linker definition
# file and initialized _CHECKSUM_DATA.
   copy obj\file.mem+obj\file.end+$(LINKDEFNAME)+obj\file.chk obj\linker.def /b>NUL
# Add definition of RAM initialization, static configuration,
# register initialization, and scanner tables if table is
# appended to end of ROM.  Remember to add gap size here (if used).
#
# First get size of tables.
#
# Get size of RAM initialization table.
   copy obj\linker.def obj\linker.fil>NUL
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\linker.fil > obj\linker.def
# Get size of register initialization table.
   copy obj\linker.def obj\link3.def>NUL
   del obj\link4.def
   echo _REG_INIT_SIZE = 0x00;>obj\link4.def
   copy obj\link3.def+obj\link4.def obj\linker.def>NUL
#
   del obj\link3.def
   del obj\link4.def
# Then continue getting the definitions.
!  if $(INCLUDE_CRISIS)
#  No gap is used in the main keyboard controller code when the crisis
#  module is used.  The gap is in the crisis module.
   echo _OUTSIDE_ADDR = (ADDR(.end) + SIZEOF(.end));>>obj\linker.def
!  else
   echo _OUTSIDE_ADDR = (ADDR(.end) + SIZEOF(.end));>>obj\linker.def
!  endif
#
#
!  if ("$(ROM_SCAN_TABLE)"=="OUTSIDE")
      echo _Rc_ROM_Tables = (_OUTSIDE_ADDR + _REG_INIT_SIZE);>>obj\linker.def
!  endif
!if $(INCLUDE_CRISIS)
      echo _FLASH_UPDATE_HANDLE_ADDR = $(CRISIS_END)-0x100;>>obj\linker.def
!endif
!if (!$(INCLUDE_CRISIS))
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo __FWU_IMAGE_START = LOADADDR(.fwu_ram_flash);>>obj\linker.def
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\linker.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\linker.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\linker.def
   echo _BOOT_BLOCK_ADDRESS = 0x20100;>>obj\linker.def
! else
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\linker.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\linker.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\linker.def
   echo _BOOT_BLOCK_ADDRESS = 0x20100+2;>>obj\linker.def
! endif
!endif
   copy obj\linker.def obj\link0.def>NUL
   $(toolsdir)\ecpad /fr "_CR_RAM_SIZE" "$(RAM_SIZE)" < obj\link0.def > obj\linker.def
# Link
   echo crld @obj\linker.cmd -o obj\ec_temp.x obj\ec_temp.map
   crld @obj\linker.cmd -o obj\ec_temp.x >obj\ec_temp.map
#   del obj\linker.def>NUL
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file and to binary file.
   crobjcopy -O binary obj\ec_temp.x obj\$(EC_NAME).bin
   obj\setstart obj\tmp
# Calculate checksum on binary file and   setup file.chk.
   $(toolsdir)\ecpad /cs obj\$(EC_NAME).bin /p "_CHECKSUM_DATA = 0x%X;">obj\file.chk
   echo. >>obj\file.chk
# Setup linker.def with linker definition file,
# correct gap size, and correct _CHECKSUM_DATA.
!  if $(INCLUDE_CRISIS)
# No gap is used in the main keyboard controller code when the crisis
# module is used.  The gap is in the crisis module.
   echo }>obj\file.end
!  else
# The ROM size is the size of binary file expanded to a multiple of
# 256 bytes and the gap is the number of bytes needed to expand the file.
# This does not include the Keyboard Controller Header (if present).
!  endif
#
   copy obj\file.mem+obj\file.end+$(LINKDEFNAME1)+obj\file.chk obj\linker.def /b>NUL
   del obj\file.mem>NUL
# Add definition of RAM initialization, static configuration,
# register initialization, and scanner tables if table is
# appended to end of ROM.  Gap size is already included here.
#
# First get size of tables.
#
# Get size of RAM initialization table.
   copy obj\linker.def obj\linker.fil>NUL
   $(toolsdir)\ecpad /fr "??+GT" ">" < obj\linker.fil > obj\linker.def
   copy obj\linker.def obj\link3.def>NUL
   del obj\link4.def
   echo _REG_INIT_SIZE = 0x00;>obj\link4.def
   copy obj\link3.def+obj\link4.def obj\linker.def>NUL
#
   del obj\link3.def
   del obj\link4.def
# Then continue getting the definitions.
   echo _OUTSIDE_ADDR = (ADDR(.end) + SIZEOF(.end));>>obj\linker.def
#
!  if ("$(ROM_SCAN_TABLE)"=="OUTSIDE")
      echo _Rc_ROM_Tables = (_OUTSIDE_ADDR + _REG_INIT_SIZE);>>obj\linker.def
!  endif
!if $(INCLUDE_CRISIS)
      echo _FLASH_UPDATE_HANDLE_ADDR = $(CRISIS_END)-0x100;>>obj\linker.def
!endif
!if (!$(INCLUDE_CRISIS))
!if (("$(SW8)"=="RAM_BASED_FLASH_UPDATE $(ON)") && !$(RUN_FROM_MRAM))
   echo __FWU_IMAGE_START = LOADADDR(.fwu_ram_flash);>>obj\linker.def
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\linker.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\linker.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\linker.def
   echo _BOOT_BLOCK_ADDRESS = 0x20100;>>obj\linker.def
! else
   echo _write_command_buffer = $(CORE_RAM_SIZE) + 0x10000 - 16;>>obj\linker.def
   echo _FLASH_commands = _write_command_buffer - 16;>>obj\linker.def
   echo _FLASH_jbb = _FLASH_commands -2;>>obj\linker.def
   echo _BOOT_BLOCK_ADDRESS = 0x20100+2;>>obj\linker.def
! endif
!endif
   copy obj\linker.def obj\link0.def>NUL
   $(toolsdir)\ecpad /fr "_CR_RAM_SIZE" "$(RAM_SIZE)" < obj\link0.def > obj\linker.def
# Link with correct gap size and _CHECKSUM_DATA.
   echo crld @obj\linker.cmd -s -o obj\$(EC_NAME).x obj\$(EC_NAME).map
   crld @obj\linker.cmd -o obj\$(EC_NAME).x >obj\$(EC_NAME).map
# Save of copy of the linker.def file just in case it needs to be examined.
   copy obj\linker.def obj\$(EC_NAME).def>NUL
#   del obj\linker.def>NUL
   find "OK" $(tmpdir)\OUTPUT.FIL>NUL
# Convert to Intel-hex format file and to binary file.
#   crprom -i -w1 -k1:1 -r -n obj\ec_temp.x -o obj\$(EC_NAME)_1.hex
#   crprom -i -l1024 -w1 -r -n -o obj\$(EC_NAME).hex obj\$(EC_NAME).x
   crobjcopy -O binary obj\$(EC_NAME).x obj\$(EC_NAME).bin
# The binary file can be used to check the build.
# The following is used just as a check.  The checksum should be 0.
# Calculate checksum on binary file.
! if $(INCLUDE_CRISIS)
   echo At this point, the checksum on obj\$(EC_NAME).bin should be 0.
   $(toolsdir)\ecpad /cs obj\$(EC_NAME).bin /p "Checksum is: %d"
! endif
! if (!$(INCLUDE_CRISIS))
   echo BOGChecking $(EC_NAME) :
   $(toolsdir)\bogcheck -bin obj\$(EC_NAME).bin
! endif
   echo.
   echo.

#****************** For Debugger ******************

# Generate the obj\$(DEBUG_NAME).x file.

obj\$(DEBUG_NAME).x: obj\$(ROMNAME)
#
   copy obj\$(EC_NAME).x obj\$(DEBUG_NAME).x>NUL


