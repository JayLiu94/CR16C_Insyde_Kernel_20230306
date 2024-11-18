-----------------------------------------------------------------------------
Nuvoton Technology Corporation - Confidential
Embedded Controller Firmware
Release Letter
Copyright (c) 2011 Nuvoton Technology Corporation. All Rights Reserved.
-----------------------------------------------------------------------------

Embedded Controller Firmware for the WPC876xL, WPCE775x, NPCE781x/3x, NPCE791x/5x
and NPCE8mnx NPCE9mnx NPCE2mnx NPCE3mnx Nuvoton Notebook Embedded Controllers.

Release Letter
Version: 2.0.12
Date:    March 2018

1. PACKAGE CONTENTS
-------------------
   This zip file contains the following files and subdirectories:
   - CHIP  folder                       - CHIP sources of EC FW
   - CORE  folder                       - CORE sources of EC FW
   - OEM   folder                       - OEM  sources of EC FW
   - TOOLS folder                       - Building utilities
   - REBUILD.BAT/BUILD.BAT/CLEAN.BAT    - Building scripts
   - ECFW.*                             - winIDEA files
   - Insyde_EC_FW_Reference_Manual.pdf  - Insyde Reference Manual for EC FW
   - EC_FW_UserGuide.pdf                - User Guide for EC FW
   - ReadMe.txt                         - This Release Letter

2. DESCRIPTION
--------------
   This package contains the Nuvoton Embedded Controller Firmware source code
   for the WPC876xL, WPCE775x, NPCE781x/3x, NPCE791x/5x and NPCE8mnx Nuvoton
   Notebook Embedded Controllers.
   This package is released in Alpha quality.

3. LICENSE
----------
   This Nuvoton Embedded Controller Firmware source code is provided under the strict
   terms of a license agreement signed by the receiving company and Nuvoton Technology
   Corporation. Please read the license agreement and ensure the firmware code is handled
   accordingly. If there is no such license agreement in place between the receiving
   company and Nuvoton, or should the license agreement terminate, immediately stop using
   this firmware code fully or partly in any way, including earlier versions of this
   firmware delivered to you, and discard any copy you may have of the firmware.

4. REQUIREMENTS
---------------
   - Software:
     - Microsoft(R) nmake.exe
     - NSC CompactRISC Toolset   - Version 4.1
   - Hardware:
     - A platform containing a Nuvoton Notebook Embedded Controller

5. INSTALLATION PROCEDURE
-------------------------
   a. Install NSC CompactRISC Toolset - Version 4.1 - to C:\Program Files\CR164.1
   b. Copy libc.a, libd.a, libhfp.a and libstart.a
      From: C:\Program Files\CR164.1\lib16cp_Os
      To:   C:\Program Files\CR164.1\lib
   c. Copy (rename) file  crobjcopy.exe
      From: C:\Program Files\CR164.1\crobjcopy.exe
      To:   C:\Program Files\CR164.1\crobjcpy.exe
   d. Create <EC_FW> directory and extract this package to the <EC_FW> directory
   e. Place Microsoft nmake.exe in the <EC_FW>\TOOLS directory
   f. Open CR16C 4.1 command prompt at <EC_FW> directory
   g. To build the Embedded Controller Firmware, run <EC_FW>\REBUILD.BAT

6. ENHANCEMENTS
---------------
    None

7. FIXED ISSUES
---------------
    - Fixed PS/2 keyboard yellow mark in device manager when KERNEL_TMRB_MFT16 is off in OEMBLD.MAK.
    - Get actual clock from HFCG module for I2C and Timer B of traffic control.
    - Fixed wrong register definition for T3CKC register in MFT16 module in NPCE285x and NPCE385x.
    - Fixed wrong register definition for ITCTS6 register in ITIM8/16 module in NPCE285x and NPCE385x.
    - Fixed wrong register initialize from LV_GPIO_CTL3_INIT to LV_GPIO_CTL2.

8. KNOWN ISSUES
---------------
   None

9. HISTORY
----------
Version 2.0.12 January 2018:

ENHANCEMENTS
   - Enabled eSPI interface.

FIXED ISSUES
    - Fixed PS/2 keyboard yellow mark in device manager when KERNEL_TMRB_MFT16 is off in OEMBLD.MAK.
    - Get actual clock from HFCG module for I2C and Timer B of traffic control.
    - Fixed wrong register definition for T3CKC register in MFT16 module in NPCE285x and NPCE385x.
    - Fixed wrong register definition for ITCTS6 register in ITIM8/16 module in NPCE285x and NPCE385x.
    - Fixed wrong register initialize from LV_GPIO_CTL3_INIT to LV_GPIO_CTL2.

Version 2.0.10 July 2016:

ENHANCEMENTS
   - Moved PM channels command/data for seperate Host infterface handling.
   - Add simple sensor hub lite firmware for uRider notebook embedded controller peripherals.
   - Added LPC to I2C module support for Sensor Hub Interface.

FIXED ISSUES
   - Fixed EC jump to IAD trap when AUX_PORT2_SUPPORTED and AUX_PORT3_SUPPORTED
     switchs are turned off.
   - Fixed NPCE2mnx I2C SCL could not toggle when SDA latched low by device.

Version 2.0.7 July 2013:
   ENHANCEMENTS
   - Add support HID over I2C module.
   - Add function irq_i2c_getcmd2() in I2C.C.
   - Add function I2C_Slave_Init() in I2C.C.
   - Add to support make, repeat and break key in System_Control_Function().

Version 2.0.5 Jan 2012:
   ENHANCEMENTS
   - Enabel SPI flash update through SMBus interface.
   - Enable HID over I2C support.
   - Integrated PEC flag into I2C variables structure.
   - Enable keyboard scan code output through HID over I2C.


Version 2.0.4 October 2012:
   ENHANCEMENTS
   - Add a switch in OEMBLD.MAK for kernel time ticker is 1 or 5msec.
   - Add a switch in OEMBLD.MAK for deep idle in S0 state.
   - Add function Irq_Timer_PS2() in IRQ.C.
   - 150usec timer change from MFT16-1 to MFT16-2 in TIMERS.C.
   - Move MFT16-1 for handle PS2 timer.
   - Move Scan_Init() from warm_reset() to Reset_Initialization() in PURINI.C.
   - Add function Transfer_Finished() in SMB_TRAN.C.

Version 2.0.3 October 2012:
   ENHANCEMENTS
   - Support EC runtime enter IDLE mode to support connect standby function.
   - Reenable Timer_A for I2C timeout in I2C.C.
   - Add MIWU table and function Write_To_MIWU() in PROC.C.
   - Add power management controller regiester init in REGINIT.C.
   - Reduce SMBus transaction time by removing Init_SMB_Transfer() and
     SMB_Transfer_1mS()in SMB_TRAN.C.

   FIXED ISSUES
   - Fix host interface command lost issued in IRQ.C.


Version 2.0.2 November 2012:
   ENHANCEMENTS
   - Redefined SFR for NPCE9mnx.
   - Removed unused code for code side reduced.
   - Removed Insyde Configuration table.

   FIXED ISSUES
   None

Version 2.0.0 November 2012:
   ENHANCEMENTS
   None

   FIXED ISSUES
   - EC might issue Host Interrupt Request two times
   - Extended_Write_Port() does not support GPIOs ports 9 ~ F
   - unified any-key (KBS) and GPIOs handlers
   - failed to build the code if chip is not "NPCE8mnx"


Version 1.9.1 June 2011:
   ENHANCEMENTS
   - Supports compilation on Windows 64 bits
   - Add definition to all ROM code flash update functions
   - Runs from MRAM support       - under RUN_FROM_MRAM       compilation switch
   - NPCE8mnx Nuvoton ECs support - under NPCE8mnx            compilation switch
   - HW keyboard scan support     - under HW_KB_SCN_SUPPORTED compilation switch

   FIXED ISSUES
   - Duplicate naming for variables
   - Need to add boundary check to CIR shift buffer
   - If running from MRAM - need to calculate checksum on all the binary image (not just on CRISIS)
   - Need to remove test variables in I2C slave mode
   - Entering Deep Idle should be atomic sequence
   - Microsecond_delay is not accurate
   - Wrong scan codes in some key combinations
   - Need to disable cache when running from MRAM


Version 1.8.1 November 2010:
   ENHANCEMENTS
   - EC debug tool host interface support - under ECDT_SHM_SUPPORTED     compilation switch
   - USB2JTAG-2 hot plug support          - under UJA_HOT_PLUG_SUPPORTED compilation switch
   - MFT CEIR (NEC) support               - under MFT_CEIR_SUPPORTED     compilation switch

   FIXED ISSUES
   None

Version 1.7.0 July 2010:
   ENHANCEMENTS
   - Enhanced LFCG_calibrate() function
   - Port80\SDP bypass for a chip issue??
   - Removed unnecessary files - CHIP\IRDOPT.S, CHIP\IREOPT.S, OEM\PROJECT\SYSCFG.C
   - Merged OEMRESET.C and OEMLEDS.C into OEM.C

   FIXED ISSUES
   - No I2C Handling for SMBnCTL4 register
   - NEC repeat key causes the system to wake up
   - RC6 message header is not checked in CEIR interrupt handler

Version 1.6.0 June 2010:
   ENHANCEMENTS
   - NPCE791x/5x Nuvoton ECs support - under NPCE79nx compilation switch
   - Simplified the OEM directory structure
   - Removed CPK support
   - Merged chip.h into regs.h to have one central register definition file
   - Some coding style changes

   FIXED ISSUES
   - Wrong assignment of external interrupt from GPIOs in the interrupt table
   - FW should change the uncalibrated value of FRCDIV
   - Compilation fails if setting OEM_SMB_EXAMPLE to OFF
   - DEVALT6 bits are not defined properly for the WPCE775x and latter chips

Version 1.5.0 February 2010:
   ENHANCEMENTS
   - PECI support - under PECI_SUPPORTED  compilation switch

   FIXED ISSUES
   None

Version 1.4.2 October 2009:
   ENHANCEMENTS
   - Upgraded to CPK HAL version 1.1.0
   - Set SMB frequency to 50 KHz

   FIXED ISSUES
   - Wrong disabling of SMBus module during SMBus functionality initializations
   - SDP functionality should be disabled during keyboard scan to avoid
     performance degradation

Version 1.3.2 June 2009:
   ENHANCEMENTS
   - Added CapKey Analysis Tool communication protocol support
   - Upgraded to CPK HAL version 1.0.1
   - Set SMB frequency to 100 KHz
   - Minor changes to PWM and PORT80 APIs

   FIXED ISSUES
   None

Version 1.2.0 June 2009:
   ENHANCEMENTS
   - NPCE781x/3x Nuvoton ECs support                 - under NPCE78mx         compilation switch
   - Capacitive Keys support (CPK HAL version 1.0.0) - under CPK_SUPPORTED    compilation switch
   - Internal 32K clock calibration support          - under INTERNAL_32K     compilation switch
   - PWM / HEARTBEAT support                         - under PWM_SUPPORTED    compilation switch
   - PORT 80 support                                 - under PORT80_SUPPORTED compilation switch
   - IOX support                                     - under IOX_SUPPORTED    compilation switch

   FIXED ISSUES
   - Compilation fails when running on Vista(R)
   - Touchpad sometimes malfunctions after resume from S3
   - System sometimes hangs on power on/off cycling to DOS
   - Redundant increase in code (flash) size
   - FW might process the I2C interrupt too late during POST

Version 1.1.0 February 2008:
   ENHANCEMENTS
   - Added flash update support when compiling without CRISIS module
     (CRISIS_PAGE=NONE in OEMBLD.MAK)

   FIXED ISSUES
   - FW sometimes resets the SMBus START bit after HW clears it
   - Sometimes, the mouse malfunctions after a resume from S3 with Active PS/2 Multiplexing driver

Version 1.0.0 December 2007:
   ENHANCEMENTS
   - New directory structure for OEM code
   - Added EC FW User Guide document
   - SMBus FW driver enhancements -
     - Added up to 4 SMBus channels support
     - Added support for parallel SMBus transactions on different SMBus channels
     - Removed obsolete SMBus level support
     - Added SMBus slave mode support
   - CEIR FW driver enhancement for WPCE775x -
     - Added support for FW level CEIR functionality while Host system is in S0
   - CRISIS module enhancements -
     - Added support for variable CRISIS module size
     - Added new CRISIS OEM Hook functions
   - New switches in OEMBLD.MAK -
     - WPC876xL               - If ON, specifies WPC876xL chip
     - WPCE775x               - If ON, specifies WPCE775x chip
     - RAM_BASED_FLASH_UPDATE - If ON, specifies that flash update code is in RAM (otherwise in ROM)
     - CEIR_SUPPORTED         - If ON, CEIR functionality is included

   FIXED ISSUES
   None


-----------------------------------------------------------
For contact information see "Contact Us" at www.nuvoton.com
-----------------------------------------------------------



