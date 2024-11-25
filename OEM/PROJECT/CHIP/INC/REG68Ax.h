/*-----------------------------------------------------------------------------
 * MODULE REGS.H
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Contains C equates for the PC87421 register addresses and bit definitions.
 * The symbol names are the same as in the hardware reference manual whenever
 * possible.  Some registers and bits that have the same name in the reference
 * manual, but belong to a different function "module", are assigned the same
 * "base" name with the name of the module they belong to appended to the base
 * name.  This avoids conflicting symbol names.  Based on PC87591 code version
 * 3.01.16[2667-31]-n.
 *
 * Revision History (Started December 20, 2004 by EDH)
 *-----------------------------------------------------------------------------
 * 2005/11/28
 * 1. ADD RSTCTL_ADDR Register for WPC876xL and WPCE775x
 * 2. ADD SPI register for WPC876xL and WPCE775x
 * 3. ADD PMC Register (GPIOEV & GPIOEN)
 * -------------------------------------------------------------------------
 * 2012/6/21
 * 1. Redefine all of SFR for NPCE985L serial.
 * -------------------------------------------------------------------------
  2019/10/28
 * 1. ADD the following registers for 68Ax.
 *    DEV_CTL4_ADDR: Device Control 4.
 *    DEV_CTL5_ADDR: Device Control 5.
 *    DEVALT10_ADDR: Device Alternate Function 10.
 *    DEVALT11_ADDR: Device Alternate Function 11.
 *    LV_GPIO_CTL4_ADDR: Low-Voltage GPIO Pins Controls 4.
 *
 * ------------------------------------------------------------------------- */
#ifndef REGS_H
#define REGS_H

#include "swtchs.h"
#include "types.h"

/* Addresses of registers. */
/* System Configuration. */
#define SYS_CFG_BASE    0xFFF000
#define DEVCNT_ADDR         (SYS_CFG_BASE + 0x00)  /* Device Control. */
#define STRPST_ADDR         (SYS_CFG_BASE + 0x01)  /* Straps Status. */
#define RSTCTL_ADDR         (SYS_CFG_BASE + 0x02)  /* Reset Control and Status. */
#define DEV_CTL2_ADDR       (SYS_CFG_BASE + 0x03)  /* Device Control 2. */
#define DEV_CTL3_ADDR       (SYS_CFG_BASE + 0x04)  /* Device Control 3. */
#define DEV_CTL4_ADDR       (SYS_CFG_BASE + 0x05)  /* Device Control 4. */
#define LAST_RESET_ADDR     (SYS_CFG_BASE + 0x06)  /* Last Reset */
#define STRPST2_ADDR        (SYS_CFG_BASE + 0x07)  /* Straps Status 2. */
#define DEVALT10_ADDR       (SYS_CFG_BASE + 0x08)  /* Device Alternate Function 10. */
#define DEVALT11_ADDR       (SYS_CFG_BASE + 0x09)  /* Device Alternate Function 11. */
#define DEV_CTL5_ADDR       (SYS_CFG_BASE + 0x0A)  /* Device Control 5. */
#define DEVALT0_ADDR        (SYS_CFG_BASE + 0x10)  /* Device Alternate Function 0. */
#define DEVALT1_ADDR        (SYS_CFG_BASE + 0x11)  /* Device Alternate Function 1. */
#define DEVALT2_ADDR        (SYS_CFG_BASE + 0x12)  /* Device Alternate Function 2. */
#define DEVALT3_ADDR        (SYS_CFG_BASE + 0x13)  /* Device Alternate Function 3. */
#define DEVALT4_ADDR        (SYS_CFG_BASE + 0x14)  /* Device Alternate Function 4. */
#define DEVALT5_ADDR        (SYS_CFG_BASE + 0x15)  /* Device Alternate Function 5. */
#define DEVALT6_ADDR        (SYS_CFG_BASE + 0x16)  /* Device Alternate Function 6. */
#define DEVALT7_ADDR        (SYS_CFG_BASE + 0x17)  /* Device Alternate Function 7. */
#define DEVALT8_ADDR        (SYS_CFG_BASE + 0x18)  /* Device Alternate Function 8. */
#define DEVALT9_ADDR        (SYS_CFG_BASE + 0x19)  /* Device Alternate Function 9. */
#define DEVALTA_ADDR        (SYS_CFG_BASE + 0x1A)  /* Device Alternate Function A. */
#define DEVALTB_ADDR        (SYS_CFG_BASE + 0x1B)  /* Device Alternate Function B. */
#define DEVALTC_ADDR        (SYS_CFG_BASE + 0x1C)  /* Device Alternate Function C. */
#define DEVALTD_ADDR        (SYS_CFG_BASE + 0x1D)  /* Device Alternate Function D. */
#define DEVALTE_ADDR        (SYS_CFG_BASE + 0x1E)  /* Device Alternate Function E. */
#define DEVALTDX_ADDR       (SYS_CFG_BASE + 0x25)  /* Device Alternate Function D Extension. */
#define PWM_SEL_ADDR        (SYS_CFG_BASE + 0x26)  /* PWM Input Select. */
#define PWM_SEL2_ADDR       (SYS_CFG_BASE + 0x27)  /* PWM Input Select 2. */
#define DEVPU0_ADDR         (SYS_CFG_BASE + 0x28)  /* Device Pull-Up 0. */
#define DEVPD1_ADDR         (SYS_CFG_BASE + 0x29)  /* Device Pull-Down 1. */
#define LV_GPIO_CTL0_ADDR   (SYS_CFG_BASE + 0x2A)  /* Low-Voltage GPIO Pins Control 0. */
#define LV_GPIO_CTL1_ADDR   (SYS_CFG_BASE + 0x2B)  /* Low-Voltage GPIO Pins Control 1. */
#define LV_GPIO_CTL2_ADDR   (SYS_CFG_BASE + 0x2C)  /* Low-Voltage GPIO Pins Control 2. */
#define LV_GPIO_CTL3_ADDR   (SYS_CFG_BASE + 0x2D)  /* Low-Voltage GPIO Pins Control 3. */
#define LV_GPIO_CTL4_ADDR   (SYS_CFG_BASE + 0x0D)  /* Low-Voltage GPIO Pins Control 4. */
#define DBG_CTL_ADDR        (SYS_CFG_BASE + 0x2E)  /* Debugging Features Control. */

/* Device Control. */
#define DEVCNT      PORT_BYTE   DEVCNT_ADDR
/* --------------------------------------- */
#define SPI_TRIS     (1 << 6)
#define JENK_HEN     (1 << 4)
#define JENK_CSL     (1 << 3)
#define PECI_EN      (1 << 2)
#define PECI_Mode    (1 << 2)    /* PECI Mode               */
#define EXTCLKSL     (1 << 1)
#define CLKOM        (1 << 0)

#define Enable_PECI_Mode() (DEVCNT |= PECI_Mode)    /* Enable PECI Mode */

/* Straps Status. */
#define STRPST      PORT_BYTE   STRPST_ADDR
/* --------------------------------------- */
#define STRPST_VD1_EN       (1 << 7)
#define STRPST_TEST         (1 << 6)
#define STRPST_JENK         (1 << 5)
#define STRPST_JEN0         (1 << 4)
#define STRPST_SDP_VIS      (1 << 3)
#define STRPST_TRIST        (1 << 2)
#define STRPST_XORTR        (1 << 1)

/* Straps Status 2. */
#define STRPST2     PORT_BYTE   STRPST2_ADDR
/* ---------------------------------------- */
#define PLTRST_VW_SEL       (1 << 3) /* PLTRST Virtual Wire mode select. */
#define ESPI_RSTO_EN        (1 << 2) /* eSPI_RST I/O mode select. */
#define ESPI_STRAP          (0x03 << 0)

#define STRPST_SHBM     (1 << 0)    /* Saves SHBM# strap pin at h/w reset. */

/* Reset Control and Status. */
#define RSTCTL      PORT_BYTE   RSTCTL_ADDR
/* --------------------------------------- */
#define DBGRST_MODE     (1 << 7) /* Debugger Reset Mode Select */
#define HIPRST_MODE     (1 << 6) /* Host interface Module Reset Mode select */
#define LRESET_MODE     (1 << 5) /* Lreset mode select */
#define SFTRST_STS      (1 << 4)
#define DBGRST_STS      (1 << 1) /* Debugger Reset Statuts */
#define EXT_RST_ST      (1 << 0) /* /VCC_POR Input Status */

#define RSTCTL_DBGRST_MODE  (1 << 7)    /* Debugger Reset Mode Select */
#define RSTCTL_HIPRST_MODE  (1 << 6)    /* Host interface Module Reset Mode select */
#define RSTCTL_LRESET_MODE  (1 << 5)    /* LRESET mode select */
#define RSTCTL_SFTRST_STS   (1 << 4)    /* software reset status */
#define RSTCTL_DBGRST_STS   (1 << 1)    /* Debugger Reset Statuts */
#define RSTCTL_EXT_RST_STS  (1 << 0)    /* External Reset Status */
#define RSTCTL_VCC_POR_STS  (1 << 0)    /* /VCC_POR Input Status */

/* Device Control 2. */
#define DEV_CTL2    PORT_BYTE   DEV_CTL2_ADDR
/* ---------------------------------------- */
#define maskHPLUG_CONN  (7 << 1)

/* Device Control 3. */
#define DEV_CTL3    PORT_BYTE   DEV_CTL3_ADDR
/* ---------------------------------------- */
#define JTD_LOCK        (1 << 4)    /* JTAG Disable, Lock. */
#define WP_IF           (1 << 3)    /* Write-Protect Internal Flash. */
#define WP_GPIO81       (1 << 2)    /* Write-Protect Over GPIO81. */

/* Device Control 4. */
#define DEV_CTL4    PORT_BYTE   DEV_CTL4_ADDR
/* ---------------------------------------- */
#define VCCSPI_1P8V     (1 << 3)    /* Indicate wether VCCSPI voltage level is 1.8V or 3.3V. */
#define VDDeSPI_1P8V    (1 << 2)    /* Indicate wether VDD/VeSPI voltage level is 1.8V or 3.3V. */
#define PIN_FLASH_SL    (1 << 0)    /* Pin Flash Select. */

/* Device Control 5. */
#define DEV_CTL5    PORT_BYTE   DEV_CTL5_ADDR
/* ---------------------------------------- */
#define CCDEV_SEL       (0x0C << 6) /* Closed Case Development Select. */
#define CCDEV_DIR       (1 << 5)    /* Closed Case Development Connector Direction. */
#define HPLUG_CONN      (0x0F << 0) /* Hot-Plug JTAG Connection. */

/* Last Reset. */
#define LAST_RESET    PORT_BYTE   LAST_RESET_ADDR


/* Device Alternate Function 0. */
#define DEVALT0     PORT_BYTE   DEVALT0_ADDR
/* ---------------------------------------- */
#define SP2O_SL         (1 << 7)    /* Serial Port 2, Output Select. */
#define SP2I_SL         (1 << 6)    /* Serial Port 2, Input Select. */
#define SSPI_SL         (1 << 3)    /* SPI Select. */
#define CKOUT_SL        (1 << 2)    /* Clock Out Select. */
#define SP1O_SL         (1 << 1)    /* Serial Port 1, Output Select. */
#define SP1I_SL         (1 << 0)    /* Serial Port 1, Input Select. */

/* Device Alternate Function 1. */
#define DEVALT1     PORT_BYTE   DEVALT1_ADDR
/* ---------------------------------------- */
#define URTO3_SL        (1 << 7)    /* CR_UART Output 3 Select. */
#define DPWROK_SL       (1 << 6)    /* DPWROK Input Select. */
#define SP2CTL_SL       (1 << 5)    /* Serial Port 2, Control Select. */
#define SP1CTL_SL       (1 << 4)    /* Serial Port 1, Control Select. */
#define URTO2_SL        (1 << 3)    /* CR_UART Output 2 Select. */
#define URTO1_SL        (1 << 2)    /* CR_UART Output 1 Select. */
#define URTI_SL         (1 << 1)    /* CR_UART Input Select. */
#define URTI2_SL        (1 << 0)    /* CR_UART2 Input Select. */

/* Device Alternate Function 2. */
#define DEVALT2     PORT_BYTE   DEVALT2_ADDR
/* ---------------------------------------- */
#define SMB2_SL         (1 << 7)    /* SMBus2 Select */
#define SMB1_SL         (1 << 6)    /* SMBus1 Select */
#define ECSCI_SL        (1 << 5)    /* ECSCI Select */
#define SMI_SL          (1 << 3)    /* SMI Select */
#define CLKRN_SL        (1 << 2)    /* CLKRUN Select. */
#define LPCPD_SL        (1 << 1)    /* LPCPD Select. */

/* Device Alternate Function 3. */
#define DEVALT3     PORT_BYTE   DEVALT3_ADDR
/* ---------------------------------------- */
#define TB3_SL          (1 << 5)    /* TB3 Select. */
#define TA3_SL          (1 << 4)    /* TA3 Select. */
#define TB2_SL          (1 << 3)    /* TB1 Select. */
#define TB1_SL          (1 << 2)    /* TB1 Select. */
#define TA2_SL          (1 << 1)    /* TA2 Select. */
#define TA1_SL          (1 << 0)    /* TA1 Select. */

/* Device Alternate Function 4. */
#define DEVALT4     PORT_BYTE   DEVALT4_ADDR
/* ---------------------------------------- */
#define SMI_2_SL        (1 << 7)    /* SMI Select. */
#define PS2_2_SL        (1 << 6)    /* PS/2 #2 Select. */
#define PS2_1_SL        (1 << 5)    /* PS/2 #1 Select. */
#define PS2_3_SL        (1 << 4)    /* PS/2 #3 Select. */

/* Device Alternate Function 5. */
#define DEVALT5     PORT_BYTE   DEVALT5_ADDR
/* ---------------------------------------- */
#define A_PWM_SL        (1 << 0)    /* A_PWM Select. */
#define B_PWM_SL        (1 << 1)    /* B_PWM Select. */
#define C_PWM_SL        (1 << 2)    /* C_PWM Select. */
#define D_PWM_SL        (1 << 3)    /* D_PWM Select. */
#define E_PWM_SL        (1 << 4)    /* E_PWM Select. */
#define F_PWM_SL        (1 << 5)    /* F_PWM Select. */
#define G_PWM_SL        (1 << 6)    /* G_PWM Select. */
#define H_PWM_SL        (1 << 7)    /* H_PWM Select. */

/* Device Alternate Function 6. */
#define DEVALT6     PORT_BYTE   DEVALT6_ADDR
/* ---------------------------------------- */
#define ADC0_SL         (1 << 0)    /* AD0 Select. */
#define ADC1_SL         (1 << 1)    /* AD1 Select. */
#define ADC2_SL         (1 << 2)    /* AD2 Select. */
#define ADC3_SL         (1 << 3)    /* AD3 Select. */
#define ADC4_SL         (1 << 4)    /* AD4 Select. */
#define ADC5_SL         (1 << 5)    /* AD5 Select. */
#define ADC6_SL         (1 << 6)    /* AD6 Select. */
#define ADC7_SL         (1 << 7)    /* AD7 Select. */

/* Device Alternate Function 7. */
#define DEVALT7     PORT_BYTE   DEVALT7_ADDR
/* ---------------------------------------- */
#define DS_OVT_GPIO_EN  (1 << 1)    /* DSADC-OVT/GPIO Enable. */
#define KBO12_SL        (1 << 2)    /* KBSOUT12 Select. */
#define KBO13_SL        (1 << 3)    /* KBSOUT13 Select. */
#define KBO14_SL        (1 << 4)    /* KBSOUT14 Select. */
#define KBO15_SL        (1 << 5)    /* KBSOUT15 Select. */
#define KBO16_SL        (1 << 6)    /* KBSOUT16 Select. */
#define KBO17_SL        (1 << 7)    /* KBSOUT17 Select. */

/* Device Alternate Function 8. */
#define DEVALT8     PORT_BYTE   DEVALT8_ADDR
/* ---------------------------------------- */
#define KBO7_SL         (1 << 0)    /* KBSOUT7 Select. */
#define KBRST_SL        (1 << 2)    /* KBRST Select. */
#define KBRST2_SL       (1 << 3)    /* KBRST Select. */
#define SPI_MOSI        (1 << 4)    /* SPI_MOSI Select. */
#define SPI_MISO        (1 << 5)    /* SPI_MISO Select. */
#define SPI_SCK         (1 << 6)    /* SPI_CLK Select. */
#define SPI_CS          (1 << 7)    /* SPI_CS Select. */

/* Device Alternate Function 9. */
#define DEVALT9     PORT_BYTE   DEVALT9_ADDR
/* ---------------------------------------- */
#define GPA03_SL        (1 << 0)    /* GPIOA0-GPIOA3 Select. */
#define GPF07_SL        (1 << 1)    /* GPIOF0, GPIOF6-GPIOF7 Select. */
#define GPA45_SL        (1 << 2)    /* GPIOA4-GPIOA5 Select. */
#define GPA67_SL        (1 << 3)    /* GPIOA6-GPIOA7 Select. */
#define GPB03_SL        (1 << 4)    /* GPIOB0-GPIOB3 Select. */
#define GPB47_SL        (1 << 5)    /* GPIOB4-GPIOB7 Select. */
#define GPC01_SL        (1 << 6)    /* GPIOC0-GPIOC1 Select. */
#define GPC23_SL        (1 << 7)    /* GPIOC2-GPIOC3 Select. */

/* Device Alternate Function A. */
#define DEVALTA     PORT_BYTE   DEVALTA_ADDR
/* ---------------------------------------- */
#define SMB3_SL         (1 << 6)    /* SMBus3 Port A Select */
#define SMB4_SL         (1 << 7)    /* SMBus4 Port A Select */

/* Device Alternate Function B. */
#define DEVALTB     PORT_BYTE   DEVALTB_ADDR
/* ---------------------------------------- */
#define D_PWM_SL1       (1 << 1)    /* D_PWM Select */
#define RI1_SL          (1 << 2)    /* RI1 Select */
#define CTS1_SL         (1 << 3)    /* CTS1 Select */
#define RTS1_SL         (1 << 4)    /* RTS1 Select */
#define RI2_SL          (1 << 5)    /* RI2 Select */
#define CTS2_SL         (1 << 6)    /* CTS2 Select */
#define RTS2_SL         (1 << 7)    /* RTS2 Select */

/* Device Alternate Function C. */
#define DEVALTC     PORT_BYTE   DEVALTC_ADDR
/* ---------------------------------------- */
#define SMB3B_SL        (1 << 5)    /* SMBus3 Port B Select */
#define SMB4B_SL        (1 << 6)    /* SMBus4 Port B Select */
#define SMB5_SL         (1 << 7)    /* SMBus5 Select */

/* Device Alternate Function D. */
#define DEVALTD     PORT_BYTE   DEVALTD_ADDR
/* ---------------------------------------- */
#define PSL_IN1_AHI     (1 << 2)    /* PSL_IN1 Active-High */
#define PSL_IN1_SL      (1 << 3)    /* PSL_IN1 Select */
#define PSL_IN2_AHI     (1 << 4)    /* PSL_IN2 Active-High */
#define PSL_IN2_SL      (1 << 5)    /* PSL_IN2 Select. */

/* Device Alternate Function DX. */
#define DEVALTDX     PORT_BYTE   DEVALTDX_ADDR
/* ---------------------------------------- */
#define PSL_IN3_AHI     (1 << 2)    /* PSL_IN3 Active-High */
#define PSL_IN3_SL      (1 << 3)    /* PSL_IN3 Select */
#define PSL_IN4_AHI     (1 << 4)    /* PSL_IN4 Active-High */
#define PSL_IN4_SL      (1 << 5)    /* PSL_IN4 Select */
#define PSL_GPIO41_SL   (1 << 6)    /* PSL_GPIO41 Select */
#define PSL_GPIO66_SL   (1 << 7)    /* PSL_GPIO66 Select */

/* Device Alternate Function E. */
#define DEVALTE     PORT_BYTE   DEVALTE_ADDR
/* ---------------------------------------- */
#define VDO2_SL         (1 << 3)    /* VD_OUT2 Select */
#define VDI2_SL         (1 << 2)    /* VD_IN2 Select */
#define VDO1_SL         (1 << 1)    /* VD_OUT1 Select */
#define VDI1_SL         (1 << 0)    /* VD_IN2 Select */

/* Device Alternate Function 10. */
#define DEVALT10    PORT_BYTE   DEVALT10_ADDR
/* ---------------------------------------- */
#define KBO15_SL1       (1 << 7)    /* KBSOUT15 Select */
#define KBO14_SL1       (1 << 6)    /* KBSOUT14 Select */
#define KBO13_SL1       (1 << 5)    /* KBSOUT13 Select */
#define KBO12_SL1       (1 << 4)    /* KBSOUT12 Select */
#define KBO11_SL1       (1 << 3)    /* KBSOUT11 Select */
#define KBO10_SL1       (1 << 2)    /* KBSOUT10 Select */
#define KBO9_SL1        (1 << 1)    /* KBSOUT9 Select */
#define KBO4_SL1        (1 << 0)    /* KBSOUT4 Select */

/* Device Alternate Function 11. */
#define DEVALT11    PORT_BYTE   DEVALT11_ADDR
/* ---------------------------------------- */
#define GP74_SL         (1 << 7)    /* GPIO74/KBSIN7 Select */
#define GP22_SL         (1 << 6)    /* GPIO22/KBSIN6 Select */
#define GP17_SL         (1 << 5)    /* GPIO17/KBSIN5 Select */
#define GP00_SL         (1 << 4)    /* GPIO00/KBSIN4 Select */
#define GP40_SL         (1 << 3)    /* GPIO40/KBSIN3 Select */
#define GP45_SL         (1 << 2)    /* GPIO45/KBSIN2 Select */
#define GP07_SL         (1 << 1)    /* GPIO07/KBSIN1 Select */
#define GP14_SL         (1 << 0)    /* GPIO14/KBSIN0 Select */

/* PWM Input Select. */
#define PWM_SEL     PORT_BYTE   PWM_SEL_ADDR
/* --------------------------------------- */
#define WD_RST_EN       (1 << 7)    /* Watchdog Reset to FORCE_PWM Enable. */
#define GPIO43_POL      (1 << 5)    /* GPIO43 Input Polarity */
#define GPIO42_POL      (1 << 4)    /* GPIO42 Input Polarity */
#define GPIO_SEL        (3 << 0)    /* GPIO to FORCE_PWM Select */

/* PWM Input Select 2. */
#define PWM_SEL2    PORT_BYTE   PWM_SEL2_ADDR
/* --------------------------------------- */
#define FPWM_STS        (1 << 7)    /* FORCE_PWM Status */
#define ADC_TH3_EN      (1 << 2)    /* ADC Threshold Event 3 to FORCE_PWM, Enable */
#define ADC_TH2_EN      (1 << 1)    /* ADC Threshold Event 2 to FORCE_PWM, Enable */
#define ADC_TH1_EN      (1 << 0)    /* ADC Threshold Event 1 to FORCE_PWM, Enable */

/* Device Pull-Up 0. */
#define DEVPU0      PORT_BYTE   DEVPU0_ADDR
/* -------------------------------------- */
#define SMB1_PUE        (1 << 0)    /* SMBus1 Pull-Up */
#define SMB2_PUE        (1 << 1)    /* SMBus2 Pull-Up */
#define SMB3_PUE        (1 << 2)    /* SMBus3 Pull-Up */
#define SMB4A_PUE       (1 << 3)    /* SMBus4 Port A Pull-Up */
#define SMB4B_PUE       (1 << 6)    /* SMBus4 Port B Pull-Up */
#define SMB5_PUE        (1 << 7)    /* SMBus5 Pull-Up */

/* Device Pull-Down 1. */
#define DEVPD1      PORT_BYTE   DEVPD1_ADDR
/* -------------------------------------- */
#define F_SDI_PDE       (1 << 6)    /* Flash Serial Data Inputs Pull-Up/Down Enable */

/* Low-Voltage GPIO Pins Control 0. */
#define LV_GPIO_CTL0 PORT_BYTE   LV_GPIO_CTL0_ADDR
/* ------------------------------------------- */
#define GPIO94_LV       (1 << 7)    /* GPIO94 Low-Voltage Select. */
#define GPIOB7_LV       (1 << 6)    /* GPIOB7 Low-Voltage Select. */
#define GPIOB1_LV       (1 << 5)    /* GPIOB1 Low-Voltage Select. */
#define GPIO80_LV       (1 << 4)    /* GPIO80 Low-Voltage Select. */
#define GPIO54_LV       (1 << 3)    /* GPIO54 Low-Voltage Select. */
#define GPIO40_LV       (1 << 2)    /* GPIO40 Low-Voltage Select. */
#define GPIO21_LV       (1 << 1)    /* GPIO21 Low-Voltage Select. */
#define GPIO20_LV       (1 << 0)    /* GPIO20 Low-Voltage Select. */


/* Low-Voltage GPIO Pins Control 1. */
#define LV_GPIO_CTL1 PORT_BYTE   LV_GPIO_CTL1_ADDR
/* ------------------------------------------- */
#define GPIO86_LV       (1 << 7)    /* GPIO86 Low-Voltage Select. */
#define GPIO85_LV       (1 << 6)    /* GPIO85 Low-Voltage Select. */
#define GPIO52_LV       (1 << 5)    /* GPIO52 Low-Voltage Select. */
#define GPIO50_LV       (1 << 4)    /* GPIO50 Low-Voltage Select. */
#define GPIO44_LV       (1 << 3)    /* GPIO44 Low-Voltage Select. */
#define GPIO43_LV       (1 << 2)    /* GPIO43 Low-Voltage Select. */
#define GPIO42_LV       (1 << 1)    /* GPIO42 Low-Voltage Select. */
#define GPIO02_LV       (1 << 0)    /* GPIO02 Low-Voltage Select. */

/* Low-Voltage GPIO Pins Control 2. */
#define LV_GPIO_CTL2    PORT_BYTE   LV_GPIO_CTL2_ADDR
/* ------------------------------------------------ */
#define GPIO87_LV       (1 << 7)    /* GPIO87 Low-Voltage Select. */
#define GPIO62_LV       (1 << 6)    /* GPIO62 Low-Voltage Select. */
#define GPIO51_LV       (1 << 5)    /* GPIO51 Low-Voltage Select. */
#define GPIO45_LV       (1 << 4)    /* GPIO45 Low-Voltage Select. */
#define GPIO15_LV       (1 << 3)    /* GPIO15 Low-Voltage Select. */
#define GPIO13_LV       (1 << 2)    /* GPIO13 Low-Voltage Select. */
#define GPIO03_LV       (1 << 1)    /* GPIO03 Low-Voltage Select. */
#define GPIO01_LV       (1 << 0)    /* GPIO01 Low-Voltage Select. */

/* Low-Voltage GPIO Pins Control 3. */
#define LV_GPIO_CTL3    PORT_BYTE   LV_GPIO_CTL3_ADDR
/* ------------------------------------------------ */
#define G47_SC4A_LV     (1 << 7)    /* G47_SC4A_LV Low-Voltage Select. */
#define G53_SD4A_LV     (1 << 6)    /* G53_SD4A_LV Low-Voltage Select. */
#define G23_SC3A_LV     (1 << 5)    /* G23_SC3A_LV Low-Voltage Select. */
#define G31_SD3A_LV     (1 << 4)    /* G31_SD3A_LV Low-Voltage Select. */
#define G73_SC2_LV      (1 << 3)    /* G73_SC2_LV Low-Voltage Select. */
#define G74_SD2_LV      (1 << 2)    /* G74_SD2_LV Low-Voltage Select. */
#define G17_SC1_LV      (1 << 1)    /* G17_SC1_LV Low-Voltage Select. */
#define G22_SD1_LV      (1 << 0)    /* G22_SD1_LV Low-Voltage Select. */

/* Low-Voltage GPIO Pins Control 4. */
#define LV_GPIO_CTL4    PORT_BYTE   LV_GPIO_CTL4_ADDR
/* ------------------------------------------------ */
#define G97_SC5_LV      (1 << 2)    /* G97_SC5_LV Low-Voltage Select. */
#define G05_SD5_LV      (1 << 1)    /* G05_SD5_LV Low-Voltage Select. */
#define G46_SD4B_LV     (1 << 0)    /* G46_SD4B_LV Low-Voltage Select. */

/* Low-Voltage GPIO Pins Control 3. */
#define DBG_CTL         PORT_BYTE   DBG_CTL_ADDR
/* ------------------------------------------------ */
#define JTD             (1 << 4)    /* JTAG Disable. */

/* Flash Memory Interface unit. */
/* Common Registers. */
#define FIU_ADDR_BASE        0xFF0000
#define FIU_CFG_ADDR    (FIU_ADDR_BASE + 0x00)  /* FIU Configuration */
#define BURST_CFG_ADDR  (FIU_ADDR_BASE + 0x01)  /* Burst Configuration */
#define RESP_CFG_ADDR   (FIU_ADDR_BASE + 0x02)  /* FIU Response Configuration */
#define FWIN1_LOW_ADDR  (FIU_ADDR_BASE + 0x04)  /* Flash Access Window 1 Low Limit Reg */
#define FWIN1_HIGH_ADDR (FIU_ADDR_BASE + 0x06)  /* Flash Access Window 1 High Limit Reg */
#define FWIN2_LOW_ADDR  (FIU_ADDR_BASE + 0x08)  /* Flash Access Window 2 Low Limit Reg */
#define FWIN2_HIGH_ADDR (FIU_ADDR_BASE + 0x0A)  /* Flash Access Window 2 High Limit Reg */
#define FWIN3_LOW_ADDR  (FIU_ADDR_BASE + 0x0C)  /* Flash Access Window 3 Low Limit Reg */
#define FWIN3_HIGH_ADDR (FIU_ADDR_BASE + 0x0E)  /* Flash Access Window 3 High Limit Reg */
#define PROT_LOCK_ADDR  (FIU_ADDR_BASE + 0x10)  /* Protect Lock Register */
#define PROT_CLEAR_ADDR (FIU_ADDR_BASE + 0x11)  /* Protection and Lock Clear Register */
#define FIU_RST_ADDR    (FIU_ADDR_BASE + 0x13)  /* FIU Reset Byte */
#define SPI_FL_CFG_ADDR (FIU_ADDR_BASE + 0x14)  /* SPI Flash Configuration */
#define UMA_CODE_ADDR   (FIU_ADDR_BASE + 0x16)  /* UMA Code Byte */
#define UMA_AB0_ADDR    (FIU_ADDR_BASE + 0x17)  /* UMA Address Byte 0 */
#define UMA_AB1_ADDR    (FIU_ADDR_BASE + 0x18)  /* UMA Address Byte 1 */
#define UMA_AB2_ADDR    (FIU_ADDR_BASE + 0x19)  /* UMA Address Byte 2 */
#define UMA_DB0_ADDR    (FIU_ADDR_BASE + 0x1A)  /* UMA Data Byte 0 */
#define UMA_DB1_ADDR    (FIU_ADDR_BASE + 0x1B)  /* UMA Data Byte 1 */
#define UMA_DB2_ADDR    (FIU_ADDR_BASE + 0x1C)  /* UMA Data Byte 2 */
#define UMA_DB3_ADDR    (FIU_ADDR_BASE + 0x1D)  /* UMA Data Byte 3 */
#define UMA_CTS_ADDR    (FIU_ADDR_BASE + 0x1E)  /* UMA Control and Status */
#define UMA_ECTS_ADDR   (FIU_ADDR_BASE + 0x1F)  /* UMA Extended Control and Status */
#define UMA_DB0_3_ADDR  (FIU_ADDR_BASE + 0x20)  /* UMA Data Bytes 0-3 */
#define CRCCON_ADDR     (FIU_ADDR_BASE + 0x26)  /* CRC Control Register */
#define CRCENT_ADDR     (FIU_ADDR_BASE + 0x27)  /* CRC Entry Register */
#define CRCRSLT_ADDR    (FIU_ADDR_BASE + 0x28)  /* CRC Initialization and Result Register */

/* FIU Configratiom. */
#define FIU_CFG     PORT_BYTE   FIU_CFG_ADDR
/* --------------------------------------- */
#define FL_SIZE_P1  (0x3F << 0)     /* Flash Size Plus 4 */

/* Burst Configuration. */
#define BURST_CFG   PORT_BYTE   BURST_CFG_ADDR
/* --------------------------------------- */
#define R_BURST     (3 << 0)        /* Flash Read Burst */

/* FIU Response Configuration. */
#define RESP_CFG    PORT_BYTE   RESP_CFG_ADDR
/* --------------------------------------- */
#define QUAD_EN     (1 << 3)        /* Quad-I/O Enable */
#define IAD_EN      (1 << 0)        /* (IAD Trap Generation Enable */

/* FIU Reset Byte */
#define FIU_RST    PORT_BYTE    FIU_RST_ADDR
/* --------------------------------------- */
#define WARM_RST    (1 << 0)        /* Enable Warm Resets */

/* Flash Access Window High/Low Limit */
#define FWIN1_LOW   PORT_WORD   FWIN1_LOW_ADDR
#define FWIN1_HIGH  PORT_WORD   FWIN1_HIGH_ADDR
#define FWIN2_LOW   PORT_WORD   FWIN2_LOW_ADDR
#define FWIN2_HIGH  PORT_WORD   FWIN2_HIGH_ADDR
#define FWIN3_LOW   PORT_WORD   FWIN3_LOW_ADDR
#define FWIN3_HIGH  PORT_WORD   FWIN3_HIGH_ADDR

/* Flash Protection Lock */
#define PROT_LOCK   PORT_BYTE   PROT_LOCK_ADDR
/* ----------------------------------------- */
#define RD_MODE_LK  (1 << 4)   /* Read Mode Lock. */
#define FWIN3_LK    (1 << 2)   /* Flash Access Window 3 Limits, Lock. */
#define FWIN2_LK    (1 << 1)   /* Flash Access Window 2 Limits, Lock. */
#define FWIN1_LK    (1 << 0)   /* Flash Access Window 1 Limits, Lock. */

/* Protection and Lock Clear */
#define PROT_CLEAR  PORT_BYTE   PROT_CLEAR_ADDR

/* SPI Flash Configuration */
#define SPI_FL_CFG PORT_BYTE    SPI_FL_CFG_ADDR
/* ------------------------------------------ */
#define RD_MODE     (3 << 6)        /* Read Mode Select */

/* UMA Code Byte */
#define UMA_CODE   PORT_BYTE    UMA_CODE_ADDR
/* UMA Address Byte 0 */
#define UMA_AB0    PORT_BYTE    UMA_AB0_ADDR
/* UMA Address Byte 1 */
#define UMA_AB1    PORT_BYTE    UMA_AB1_ADDR
/* UMA Address Byte 2 */
#define UMA_AB2    PORT_BYTE    UMA_AB2_ADDR
/* UMA Data Byte 0 */
#define UMA_DB0    PORT_BYTE    UMA_DB0_ADDR
/* UMA Data Byte 1 */
#define UMA_DB1    PORT_BYTE    UMA_DB1_ADDR
/* UMA Data Byte 2 */
#define UMA_DB2    PORT_BYTE    UMA_DB2_ADDR
/* UMA Data Byte 3 */
#define UMA_DB3    PORT_BYTE    UMA_DB3_ADDR
/* UMA Control and Status */
#define UMA_CTS    PORT_BYTE    UMA_CTS_ADDR
/* --------------------------------------- */
#define EXEC_DONE   (1 << 7)    /* Operation Execute/Done */
#define RD_WR       (1 << 5)    /* Read/Write Select */
#define C_SIZE      (1 << 4)    /* Code Field Size Select */
#define A_SIZE      (1 << 3)    /* Address Field Size Select */
#define D_SIZE      (7 << 0)    /* Data Field Size Select */

/* UMA Extended Control and Status */
#define UMA_ECTS   PORT_BYTE    UMA_ECTS_ADDR
/* ---------------------------------------- */
#define SW_CS0      (1 << 0)    /* Software-Controlled Chip Select 0 */
#define Four_byte   (1 << 6)    /* Byte Count of sddress fiels */

/* UMA Data Bytes 0-3 */
#define UMA_DB0_3  PORT_DWORD   UMA_DB0_3_ADDR
/* CRC Control Register */
#define CRCCON     PORT_BYTE    CRCCON_ADDR
/* -------------------------------------- */
#define UMAENT      (1 << 2)    /* UMA Entry */
#define CKSMCRC     (1 << 1)    /* Checksum or CRC */
#define CALCEN      (1 << 0)    /* Calculation Enable */

/* CRC Entry Register */
#define CRCENT     PORT_BYTE    CRCENT_ADDR
/* CRC Initialization and Result Register */
#define CRCRSLT    PORT_DWORD   CRCRSLT_ADDR

/* Multi Input Wakeup registers. */
#define MIWU_ADDR_BASE1   0xFFF1C0
#define MIWU_ADDR_BASE2   0xFFFBC0
#define WKEDG1_ADDR     (MIWU_ADDR_BASE1 + 0x00)    /* Edge Detection 1. */
#define WKAEDG1_ADDR    (MIWU_ADDR_BASE1 + 0x01)    /* Any Edge Detection 1. */
#define WKEDG2_ADDR     (MIWU_ADDR_BASE1 + 0x02)    /* Edge Detection 2. */
#define WKAEDG2_ADDR    (MIWU_ADDR_BASE1 + 0x03)    /* Any Edge Detection 2. */
#define WKEDG3_ADDR     (MIWU_ADDR_BASE1 + 0x04)    /* Edge Detection 3. */
#define WKAEDG3_ADDR    (MIWU_ADDR_BASE1 + 0x05)    /* Any Edge Detection 3. */
#define WKEDG4_ADDR     (MIWU_ADDR_BASE1 + 0x06)    /* Edge Detection 4. */
#define WKAEDG4_ADDR    (MIWU_ADDR_BASE1 + 0x07)    /* Any Edge Detection 4. */
#define WKEDG5_ADDR     (MIWU_ADDR_BASE1 + 0x08)    /* Edge Detection 5. */
#define WKAEDG5_ADDR    (MIWU_ADDR_BASE1 + 0x09)    /* Any Edge Detection 5. */
#define WKPND1_ADDR     (MIWU_ADDR_BASE1 + 0x0A)    /* Pending 1. */
#define LOW_PND_WU_ADDR (MIWU_ADDR_BASE1 + 0x0B)    /* Lowest Pending Wake-Up */
#define WKPCL1_ADDR     (MIWU_ADDR_BASE1 + 0x0C)    /* Pending Clear 1. */
#define WKPND2_ADDR     (MIWU_ADDR_BASE1 + 0x0E)    /* Pending 2. */
#define WKPCL2_ADDR     (MIWU_ADDR_BASE1 + 0x10)    /* Pending Clear 2. */
#define PND2WKO2_ADDR   (MIWU_ADDR_BASE1 + 0x11)    /* Pending to Interrupt Output 2. */
#define WKPND3_ADDR     (MIWU_ADDR_BASE1 + 0x12)    /* Pending 3. */
#define WKPCL3_ADDR     (MIWU_ADDR_BASE1 + 0x14)    /* Pending Clear 3. */
#define WKPND4_ADDR     (MIWU_ADDR_BASE1 + 0x16)    /* Pending 4. */
#define WKPCL4_ADDR     (MIWU_ADDR_BASE1 + 0x18)    /* Pending Clear 4. */
#define WKPND5_ADDR     (MIWU_ADDR_BASE1 + 0x1A)    /* Pending 5. */
#define WKPCL5_ADDR     (MIWU_ADDR_BASE1 + 0x1C)    /* Pending Clear 5. */
#define WKEN1_ADDR      (MIWU_ADDR_BASE1 + 0x1E)    /* Enable 1. */
#define WKINEN1_ADDR    (MIWU_ADDR_BASE1 + 0x1F)    /* Wake-Up Input Enable 1. */
#define WKEN2_ADDR      (MIWU_ADDR_BASE1 + 0x20)    /* Enable 2. */
#define WKINEN2_ADDR    (MIWU_ADDR_BASE1 + 0x21)    /* Wake-Up Input Enable 2. */
#define WKEN3_ADDR      (MIWU_ADDR_BASE1 + 0x22)    /* Enable 3. */
#define WKINEN3_ADDR    (MIWU_ADDR_BASE1 + 0x23)    /* Wake-Up Input Enable 3. */
#define WKEN4_ADDR      (MIWU_ADDR_BASE1 + 0x24)    /* Enable 4. */
#define WKINEN4_ADDR    (MIWU_ADDR_BASE1 + 0x25)    /* Wake-Up Input Enable 4. */
#define WKEN5_ADDR      (MIWU_ADDR_BASE1 + 0x26)    /* Enable 5. */
#define WKINEN5_ADDR    (MIWU_ADDR_BASE1 + 0x27)    /* Wake-Up Input Enable 5. */
#define WKEDG6_ADDR     (MIWU_ADDR_BASE1 + 0x28)    /* Edge Detection 6. */
#define WKAEDG6_ADDR    (MIWU_ADDR_BASE1 + 0x29)    /* Any Edge Detection 6. */
#define WKEDG7_ADDR     (MIWU_ADDR_BASE1 + 0x2A)    /* Edge Detection 7. */
#define WKAEDG7_ADDR    (MIWU_ADDR_BASE1 + 0x2B)    /* Any Edge Detection 7. */
#define WKEDG8_ADDR     (MIWU_ADDR_BASE1 + 0x2C)    /* Edge Detection 8. */
#define WKAEDG8_ADDR    (MIWU_ADDR_BASE1 + 0x2D)    /* Any Edge Detection 8. */
#define WKPND6_ADDR     (MIWU_ADDR_BASE1 + 0x2E)    /* Pending 6. */
#define WKPCL6_ADDR     (MIWU_ADDR_BASE1 + 0x30)    /* Pending Clear 6. */
#define WKPND7_ADDR     (MIWU_ADDR_BASE1 + 0x32)    /* Pending 7. */
#define WKPCL7_ADDR     (MIWU_ADDR_BASE1 + 0x34)    /* Pending Clear 7. */
#define WKPND8_ADDR     (MIWU_ADDR_BASE1 + 0x36)    /* Pending 8. */
#define WKPCL8_ADDR     (MIWU_ADDR_BASE1 + 0x38)    /* Pending Clear 8. */
#define WKEN6_ADDR      (MIWU_ADDR_BASE1 + 0x3A)    /* Enable 6. */
#define WKINEN6_ADDR    (MIWU_ADDR_BASE1 + 0x3B)    /* Wake-Up Input Enable 6. */
#define WKEN7_ADDR      (MIWU_ADDR_BASE1 + 0x3C)    /* Enable 7. */
#define WKINEN7_ADDR    (MIWU_ADDR_BASE1 + 0x3D)    /* Wake-Up Input Enable 7. */
#define WKEN8_ADDR      (MIWU_ADDR_BASE1 + 0x3E)    /* Enable 8. */
#define WKINEN8_ADDR    (MIWU_ADDR_BASE1 + 0x3F)    /* Wake-Up Input Enable 8. */

#define WKEDG9_ADDR     (MIWU_ADDR_BASE2 + 0x00)    /* Edge Detection 9. */
#define WKAEDG9_ADDR    (MIWU_ADDR_BASE2 + 0x01)    /* Any Edge Detection 9. */
#define WKPND9_ADDR     (MIWU_ADDR_BASE2 + 0x02)    /* Pending 9. */
#define WKPCL9_ADDR     (MIWU_ADDR_BASE2 + 0x03)    /* Pending Clear 9. */
#define WKEN9_ADDR      (MIWU_ADDR_BASE2 + 0x04)    /* Enable 9. */
#define WKINEN9_ADDR    (MIWU_ADDR_BASE2 + 0x05)    /* Wake-Up Input Enable 9. */
#define WKMOD9_ADDR     (MIWU_ADDR_BASE2 + 0x06)    /* Wake-Up Detection Mode 9. */
#define WKEDGA_ADDR     (MIWU_ADDR_BASE2 + 0x08)    /* Edge Detection A. */
#define WKAEDGA_ADDR    (MIWU_ADDR_BASE2 + 0x09)    /* Any Edge Detection A. */
#define WKPNDA_ADDR     (MIWU_ADDR_BASE2 + 0x0A)    /* Pending A. */
#define WKPCLA_ADDR     (MIWU_ADDR_BASE2 + 0x0B)    /* Pending Clear A. */
#define WKENA_ADDR      (MIWU_ADDR_BASE2 + 0x0C)    /* Enable A. */
#define WKINENA_ADDR    (MIWU_ADDR_BASE2 + 0x0D)    /* Wake-Up Input Enable A. */
#define WKMODA_ADDR     (MIWU_ADDR_BASE2 + 0x0E)     /* Wake-Up Detection Mode A */
#define WKEDGB_ADDR     (MIWU_ADDR_BASE2 + 0x10)     /* Edge Detection B */
#define WKAEDGB_ADDR    (MIWU_ADDR_BASE2 + 0x11)     /* Any Edge Detection B */
#define WKPNDB_ADDR     (MIWU_ADDR_BASE2 + 0x12)     /* Pending B */
#define WKPCLB_ADDR     (MIWU_ADDR_BASE2 + 0x13)     /* Pending Clear B */
#define WKENB_ADDR      (MIWU_ADDR_BASE2 + 0x14)     /* Enable B */
#define WKINENB_ADDR    (MIWU_ADDR_BASE2 + 0x15)     /* Wake-Up Input Enable B */
#define WKMODB_ADDR     (MIWU_ADDR_BASE2 + 0x16)     /* Wake-Up Detection Mode B */
#define WKEDGC_ADDR     (MIWU_ADDR_BASE2 + 0x18)     /* Edge Detection C */
#define WKAEDGC_ADDR    (MIWU_ADDR_BASE2 + 0x19)     /* Any Edge Detection C */
#define WKPNDC_ADDR     (MIWU_ADDR_BASE2 + 0x1A)     /* Pending C */
#define WKPCLC_ADDR     (MIWU_ADDR_BASE2 + 0x1B)     /* Pending Clear C */
#define WKENC_ADDR      (MIWU_ADDR_BASE2 + 0x1C)     /* Enable C */
#define WKINENC_ADDR    (MIWU_ADDR_BASE2 + 0x1D)     /* Wake-Up Input Enable C */

#define WKMOD1_ADDR     (MIWU_ADDR_BASE2 + 0x30)     /* Wake-Up Detection Mode 1 */
#define WKMOD2_ADDR     (MIWU_ADDR_BASE2 + 0x31)     /* Wake-Up Detection Mode 2 */
#define WKMOD3_ADDR     (MIWU_ADDR_BASE2 + 0x32)     /* Wake-Up Detection Mode 3 */
#define WKMOD4_ADDR     (MIWU_ADDR_BASE2 + 0x33)     /* Wake-Up Detection Mode 4 */
#define WKMOD5_ADDR     (MIWU_ADDR_BASE2 + 0x34)     /* Wake-Up Detection Mode 5 */
#define WKMOD6_ADDR     (MIWU_ADDR_BASE2 + 0x35)     /* Wake-Up Detection Mode 6 */
#define WKMOD7_ADDR     (MIWU_ADDR_BASE2 + 0x36)     /* Wake-Up Detection Mode 7 */
#define WKMOD8_ADDR     (MIWU_ADDR_BASE2 + 0x37)     /* Wake-Up Detection Mode 8 */


/* Edge Detection 1. */
#define WKEDG1      PORT_BYTE   WKEDG1_ADDR
/* Any Edge Detection 1. */
#define WKAEDG1     PORT_BYTE   WKAEDG1_ADDR
/* Edge Detection 2. */
#define WKEDG2      PORT_BYTE   WKEDG2_ADDR
/* Any Edge Detection 2. */
#define WKAEDG2     PORT_BYTE   WKAEDG2_ADDR
/* Edge Detection 3. */
#define WKEDG3      PORT_BYTE   WKEDG3_ADDR
/* Any Edge Detection 3. */
#define WKAEDG3     PORT_BYTE   WKAEDG3_ADDR
/* Edge Detection 4. */
#define WKEDG4      PORT_BYTE   WKEDG4_ADDR
/* Any Edge Detection 4. */
#define WKAEDG4     PORT_BYTE   WKAEDG4_ADDR
/* Edge Detection 5. */
#define WKEDG5      PORT_BYTE   WKEDG5_ADDR
/* Any Edge Detection 5. */
#define WKAEDG5     PORT_BYTE   WKAEDG5_ADDR
/* Pending 1. */
#define WKPND1      PORT_BYTE   WKPND1_ADDR
/* Pending Clear 1. */
#define WKPCL1      PORT_BYTE   WKPCL1_ADDR
/* Pending 2. */
#define WKPND2      PORT_BYTE   WKPND2_ADDR
/* Pending Clear 2. */
#define WKPCL2      PORT_BYTE   WKPCL2_ADDR
/* Pending to Interrupt Output 2. */
#define PND2WKO2    PORT_BYTE   PND2WKO2_ADDR
/* Pending 3. */
#define WKPND3      PORT_BYTE   WKPND3_ADDR
/* Pending Clear 3. */
#define WKPCL3      PORT_BYTE   WKPCL3_ADDR
/* Pending 4. */
#define WKPND4      PORT_BYTE   WKPND4_ADDR
/* Pending Clear 4. */
#define WKPCL4      PORT_BYTE   WKPCL4_ADDR
/* Pending 5. */
#define WKPND5      PORT_BYTE   WKPND5_ADDR
/* Pending Clear 5. */
#define WKPCL5      PORT_BYTE   WKPCL5_ADDR
/* Enable 1. */
#define WKEN1       PORT_BYTE   WKEN1_ADDR
/* Wake-Up Input Enable 1. */
#define WKINEN1     PORT_BYTE   WKINEN1_ADDR
/* Enable 2. */
#define WKEN2       PORT_BYTE   WKEN2_ADDR
/* Wake-Up Input Enable 2. */
#define WKINEN2     PORT_BYTE   WKINEN2_ADDR
/* Enable 3. */
#define WKEN3       PORT_BYTE   WKEN3_ADDR
/* Wake-Up Input Enable 3. */
#define WKINEN3     PORT_BYTE   WKINEN3_ADDR
/* Enable 4. */
#define WKEN4       PORT_BYTE   WKEN4_ADDR
/* Wake-Up Input Enable 4. */
#define WKINEN4     PORT_BYTE   WKINEN4_ADDR
/* Enable 5. */
#define WKEN5       PORT_BYTE   WKEN5_ADDR
/* Wake-Up Input Enable 5. */
#define WKINEN5     PORT_BYTE   WKINEN5_ADDR
/* Edge Detection 6. */
#define WKEDG6      PORT_BYTE   WKEDG6_ADDR
/* Any Edge Detection 6. */
#define WKAEDG6     PORT_BYTE   WKAEDG6_ADDR
/* Edge Detection 7. */
#define WKEDG7      PORT_BYTE   WKEDG7_ADDR
/* Any Edge Detection 7. */
#define WKAEDG7     PORT_BYTE   WKAEDG7_ADDR
/* Edge Detection 8. */
#define WKEDG8      PORT_BYTE   WKEDG8_ADDR
/* Any Edge Detection 8. */
#define WKAEDG8     PORT_BYTE   WKAEDG8_ADDR
/* Pending 6. */
#define WKPND6      PORT_BYTE   WKPND6_ADDR
/* Pending Clear 6. */
#define WKPCL6      PORT_BYTE   WKPCL6_ADDR
/* Pending 7. */
#define WKPND7      PORT_BYTE   WKPND7_ADDR
/* Pending Clear 7. */
#define WKPCL7      PORT_BYTE   WKPCL7_ADDR
/* Pending 8. */
#define WKPND8      PORT_BYTE   WKPND8_ADDR
/* Pending Clear 8. */
#define WKPCL8      PORT_BYTE   WKPCL8_ADDR
/* Enable 6. */
#define WKEN6       PORT_BYTE   WKEN6_ADDR
/* Wake-Up Input Enable 6. */
#define WKINEN6     PORT_BYTE   WKINEN6_ADDR
/* Enable 7. */
#define WKEN7       PORT_BYTE   WKEN7_ADDR
/* Wake-Up Input Enable 7. */
#define WKINEN7     PORT_BYTE   WKINEN7_ADDR
/* Enable 8. */
#define WKEN8       PORT_BYTE   WKEN8_ADDR
/* Wake-Up Input Enable 8. */
#define WKINEN8     PORT_BYTE   WKINEN8_ADDR
/* Edge Detection 9. */
#define WKEDG9      PORT_BYTE   WKEDG9_ADDR
/* Any Edge Detection 9. */
#define WKAEDG9     PORT_BYTE   WKAEDG9_ADDR
/* Pending 9. */
#define WKPND9      PORT_BYTE   WKPND9_ADDR
/* Pending Clear 9. */
#define WKPCL9      PORT_BYTE   WKPCL9_ADDR
/* Enable 9. */
#define WKEN9       PORT_BYTE   WKEN9_ADDR
/* Wake-Up Input Enable 9. */
#define WKINEN9     PORT_BYTE   WKINEN9_ADDR
/* Wake-Up Detection Mode 9 */
#define  WKMOD9     PORT_BYTE   WKMOD9_ADDR
/* Edge Detection A. */
#define WKEDGA      PORT_BYTE   WKEDGA_ADDR
/* Any Edge Detection A. */
#define WKAEDGA     PORT_BYTE   WKAEDGA_ADDR
/* Pending A. */
#define WKPNDA      PORT_BYTE   WKPNDA_ADDR
/* Pending Clear A. */
#define WKPCLA      PORT_BYTE   WKPCLA_ADDR
/* Enable A. */
#define WKENA       PORT_BYTE   WKENA_ADDR
/* Wake-Up Input Enable A. */
#define WKINENA     PORT_BYTE   WKINENA_ADDR
/* Wake-Up Detection Mode A */
#define  WKMODA     PORT_BYTE   WKMODA_ADDR
/* Edge Detection B */
#define  WKEDGB     PORT_BYTE   WKEDGB_ADDR
/* Any Edge Detection B */
#define  WKAEDGB    PORT_BYTE   WKAEDGB_ADDR
/* Pending B */
#define  WKPNDB     PORT_BYTE   WKPNDB_ADDR
/* Pending Clear B */
#define  WKPCLB     PORT_BYTE   WKPCLB_ADDR
/* Enable B */
#define  WKENB      PORT_BYTE   WKENB_ADDR
/* Wake-Up Input Enable B */
#define  WKINENB    PORT_BYTE   WKINENB_ADDR
/* Wake-Up Detection Mode B */
#define  WKMODB     PORT_BYTE   WKMODB_ADDR
/* Edge Detection C */
#define  WKEDGC     PORT_BYTE   WKEDGC_ADDR
/* Any Edge Detection C */
#define  WKAEDGC    PORT_BYTE   WKAEDGC_ADDR
/* Pending C */
#define  WKPNDC     PORT_BYTE   WKPNDC_ADDR
/* Pending Clear C */
#define  WKPCLC     PORT_BYTE   WKPCLC_ADDR
/* Enable C */
#define  WKENC      PORT_BYTE   WKENC_ADDR
/* Wake-Up Input Enable C */
#define  WKINENC    PORT_BYTE   WKINENC_ADDR
/* Wake-Up Detection Mode 1 */
#define  WKMOD1     PORT_BYTE   WKMOD1_ADDR
/* Wake-Up Detection Mode 2 */
#define  WKMOD2     PORT_BYTE   WKMOD2_ADDR
/* Wake-Up Detection Mode 3 */
#define  WKMOD3     PORT_BYTE   WKMOD3_ADDR
/* Wake-Up Detection Mode 4 */
#define  WKMOD4     PORT_BYTE   WKMOD4_ADDR
/* Wake-Up Detection Mode 5 */
#define  WKMOD5     PORT_BYTE   WKMOD5_ADDR
/* Wake-Up Detection Mode 6 */
#define  WKMOD6     PORT_BYTE   WKMOD6_ADDR
/* Wake-Up Detection Mode 7 */
#define  WKMOD7     PORT_BYTE   WKMOD7_ADDR
/* Wake-Up Detection Mode 8 */
#define  WKMOD8     PORT_BYTE   WKMOD8_ADDR

/* Lowest Pending Wake-Up */
#define LOW_PND_WU  PORT_BYTE   LOW_PND_WU_ADDR
/* ------------------------------------------- */
#define PND_GRP (0x0F << 4)  /* Pending Group */
#define PND_IN  (0x07 << 0)  /* Pending Input */


/* ICU Core Registers */
#define ICU_ADDR_BASE    0xFFFE00
#define IVCT_ADDR       (ICU_ADDR_BASE + 0x00)  /* Interrupt Vector. */
#define NMISTAT_ADDR    (ICU_ADDR_BASE + 0x02)  /* NMI Status. */
#define EXNMI_ADDR      (ICU_ADDR_BASE + 0x04)  /* External NMI Interrupt Control and Status. */
#define ISTAT0_ADDR     (ICU_ADDR_BASE + 0x0A)  /* Interrupt Status 0. */
#define ISTAT1_ADDR     (ICU_ADDR_BASE + 0x0C)  /* Interrupt Status 1. */
#define IENAM0_ADDR     (ICU_ADDR_BASE + 0x0E)  /* Interrupt Enable and Mask 0. */
#define IENAM1_ADDR     (ICU_ADDR_BASE + 0x10)  /* Interrupt Enable and Mask 1. */
#define IECLR0_ADDR     (ICU_ADDR_BASE + 0x12)  /* Edge Interrupt Clear 0. */
#define IECLR1_ADDR     (ICU_ADDR_BASE + 0x14)  /* Edge Interrupt Clear 1. */
#define ICTS_ADDR       (ICU_ADDR_BASE + 0x20)  /* Interrupt Control and Status */

/* Interrupt Vector. */
#define IVCT            PORT_BYTE   IVCT_ADDR
/* NMI Status. */
#define NMISTAT         PORT_BYTE   NMISTAT_ADDR
/* ------------------------------------------- */
#define EXT         (1 << 0)    /* External Non-Maskable Interrupt Request */

/* External NMI Interrupt Control and Status. */
#define EXNMI           PORT_BYTE   EXNMI_ADDR
/* ----------------------------------------- */
#define ENLCK       (1 << 2)    /* EXNMI Interrupt Enable Lock */
#define PIN         (1 << 1)    /* EXNMI Pin Value */
#define EN          (1 << 0)    /* EXNMI Interrupt Enable */

/* Interrupt Status 0. */
#define ISTAT0          PORT_WORD   ISTAT0_ADDR
/* Interrupt Status 1. */
#define ISTAT1          PORT_WORD   ISTAT1_ADDR
/* Interrupt Enable and Mask 0. */
#define IENAM0          PORT_WORD   IENAM0_ADDR
/* Interrupt Enable and Mask 1. */
#define IENAM1          PORT_WORD   IENAM1_ADDR
/* Edge Interrupt Clear 0. */
#define IECLR0          PORT_WORD   IECLR0_ADDR
/* Edge Interrupt Clear 1. */
#define IECLR1          PORT_WORD   IECLR1_ADDR
/* Interrupt Control and Status */
#define ICTS            PORT_BYTE   ICTS_ADDR
/* ----------------------------------------- */
#define HPR_INT         (0x0F << 0) /* Highest Priority Interrupt */


/* General Purpose I/O Ports. */

#define LAST_PORT   PF_CODE

#define GPIOP0_ADDR_BASE   0xFFF200
#define GPIOP1_ADDR_BASE   0xFFF210
#define GPIOP2_ADDR_BASE   0xFFF220
#define GPIOP3_ADDR_BASE   0xFFF230
#define GPIOP4_ADDR_BASE   0xFFF240
#define GPIOP5_ADDR_BASE   0xFFF250
#define GPIOP6_ADDR_BASE   0xFFF260
#define GPIOP7_ADDR_BASE   0xFFF270
#define GPIOP8_ADDR_BASE   0xFFF280
#define GPIOP9_ADDR_BASE   0xFFF290
#define GPIOPA_ADDR_BASE   0xFFF2A0
#define GPIOPB_ADDR_BASE   0xFFF2B0
#define GPIOPC_ADDR_BASE   0xFFF2C0
#define GPIOPD_ADDR_BASE   0xFFF2D0
#define GPIOPE_ADDR_BASE   0xFFF2E0
#define GPIOPF_ADDR_BASE   0xFFF2F0

#define P0DOUT_ADDR   (GPIOP0_ADDR_BASE + 0x00) /* Port 0 data out. */
#define P0DIN_ADDR    (GPIOP0_ADDR_BASE + 0x01) /* Port 0 data in. */
#define P0DIR_ADDR    (GPIOP0_ADDR_BASE + 0x02) /* Port 0 direction. */
#define P0PULL_ADDR   (GPIOP0_ADDR_BASE + 0x03) /* Port 0 weak pull-up or pull-down enabled. */
#define P0PUD_ADDR    (GPIOP0_ADDR_BASE + 0x04) /* Port 0 pull-up/down selection. */
#define P0ENVDD_ADDR  (GPIOP0_ADDR_BASE + 0x05) /* Port 0 drive enable by VDD present. */
#define P0OTYPE_ADDR  (GPIOP0_ADDR_BASE + 0x06) /* Port 0 Output Type. */

#define P1DOUT_ADDR   (GPIOP1_ADDR_BASE + 0x00) /* Port 1 data out. */
#define P1DIN_ADDR    (GPIOP1_ADDR_BASE + 0x01) /* Port 1 data in. */
#define P1DIR_ADDR    (GPIOP1_ADDR_BASE + 0x02) /* Port 1 direction. */
#define P1PULL_ADDR   (GPIOP1_ADDR_BASE + 0x03) /* Port 1 weak pull-up or pull-down enabled. */
#define P1PUD_ADDR    (GPIOP1_ADDR_BASE + 0x04) /* Port 1 pull-up/down selection. */
#define P1ENVDD_ADDR  (GPIOP1_ADDR_BASE + 0x05) /* Port 1 drive enable by VDD present. */
#define P1OTYPE_ADDR  (GPIOP1_ADDR_BASE + 0x06) /* Port 1 Output Type. */

#define P2DOUT_ADDR   (GPIOP2_ADDR_BASE + 0x00) /* Port 2 data out. */
#define P2DIN_ADDR    (GPIOP2_ADDR_BASE + 0x01) /* Port 2 data in. */
#define P2DIR_ADDR    (GPIOP2_ADDR_BASE + 0x02) /* Port 2 direction. */
#define P2PULL_ADDR   (GPIOP2_ADDR_BASE + 0x03) /* Port 2 weak pull-up or pull-down enabled. */
#define P2PUD_ADDR    (GPIOP2_ADDR_BASE + 0x04) /* Port 2 pull-up/down selection. */
#define P2ENVDD_ADDR  (GPIOP2_ADDR_BASE + 0x05) /* Port 2 drive enable by VDD present. */
#define P2OTYPE_ADDR  (GPIOP2_ADDR_BASE + 0x06) /* Port 2 Output Type. */

#define P3DOUT_ADDR   (GPIOP3_ADDR_BASE + 0x00) /* Port 3 data out. */
#define P3DIN_ADDR    (GPIOP3_ADDR_BASE + 0x01) /* Port 3 data in. */
#define P3DIR_ADDR    (GPIOP3_ADDR_BASE + 0x02) /* Port 3 direction. */
#define P3PULL_ADDR   (GPIOP3_ADDR_BASE + 0x03) /* Port 3 weak pull-up or pull-down enabled. */
#define P3PUD_ADDR    (GPIOP3_ADDR_BASE + 0x04) /* Port 3 pull-up/down selection. */
#define P3ENVDD_ADDR  (GPIOP3_ADDR_BASE + 0x05) /* Port 3 drive enable by VDD present. */
#define P3OTYPE_ADDR  (GPIOP3_ADDR_BASE + 0x06) /* Port 3 Output Type. */

#define P4DOUT_ADDR   (GPIOP4_ADDR_BASE + 0x00) /* Port 4 data out. */
#define P4DIN_ADDR    (GPIOP4_ADDR_BASE + 0x01) /* Port 4 data in. */
#define P4DIR_ADDR    (GPIOP4_ADDR_BASE + 0x02) /* Port 4 direction. */
#define P4PULL_ADDR   (GPIOP4_ADDR_BASE + 0x03) /* Port 4 weak pull-up or pull-down enabled. */
#define P4PUD_ADDR    (GPIOP4_ADDR_BASE + 0x04) /* Port 4 pull-up/down selection. */
#define P4ENVDD_ADDR  (GPIOP4_ADDR_BASE + 0x05) /* Port 4 drive enable by VDD present. */
#define P4OTYPE_ADDR  (GPIOP4_ADDR_BASE + 0x06) /* Port 4 Output Type. */

#define P5DOUT_ADDR   (GPIOP5_ADDR_BASE + 0x00) /* Port 5 data out. */
#define P5DIN_ADDR    (GPIOP5_ADDR_BASE + 0x01) /* Port 5 data in. */
#define P5DIR_ADDR    (GPIOP5_ADDR_BASE + 0x02) /* Port 5 direction. */
#define P5PULL_ADDR   (GPIOP5_ADDR_BASE + 0x03) /* Port 5 weak pull-up or pull-down enabled. */
#define P5PUD_ADDR    (GPIOP5_ADDR_BASE + 0x04) /* Port 5 pull-up/down selection. */
#define P5ENVDD_ADDR  (GPIOP5_ADDR_BASE + 0x05) /* Port 5 drive enable by VDD present. */
#define P5OTYPE_ADDR  (GPIOP5_ADDR_BASE + 0x06) /* Port 5 Output Type. */

#define P6DOUT_ADDR   (GPIOP6_ADDR_BASE + 0x00) /* Port 6 data out. */
#define P6DIN_ADDR    (GPIOP6_ADDR_BASE + 0x01) /* Port 6 data in. */
#define P6DIR_ADDR    (GPIOP6_ADDR_BASE + 0x02) /* Port 6 direction. */
#define P6PULL_ADDR   (GPIOP6_ADDR_BASE + 0x03) /* Port 6 weak pull-up or pull-down enabled. */
#define P6PUD_ADDR    (GPIOP6_ADDR_BASE + 0x04) /* Port 6 pull-up/down selection. */
#define P6ENVDD_ADDR  (GPIOP6_ADDR_BASE + 0x05) /* Port 6 drive enable by VDD present. */
#define P6OTYPE_ADDR  (GPIOP6_ADDR_BASE + 0x06) /* Port 6 Output Type. */

#define P7DOUT_ADDR   (GPIOP7_ADDR_BASE + 0x00) /* Port 7 data out. */
#define P7DIN_ADDR    (GPIOP7_ADDR_BASE + 0x01) /* Port 7 data in. */
#define P7DIR_ADDR    (GPIOP7_ADDR_BASE + 0x02) /* Port 7 direction. */
#define P7PULL_ADDR   (GPIOP7_ADDR_BASE + 0x03) /* Port 7 weak pull-up or pull-down enabled. */
#define P7PUD_ADDR    (GPIOP7_ADDR_BASE + 0x04) /* Port 7 pull-up/down selection. */
#define P7ENVDD_ADDR  (GPIOP7_ADDR_BASE + 0x05) /* Port 7 drive enable by VDD present. */
#define P7OTYPE_ADDR  (GPIOP7_ADDR_BASE + 0x06) /* Port 7 Output Type. */

#define P8DOUT_ADDR   (GPIOP8_ADDR_BASE + 0x00) /* Port 8 data out. */
#define P8DIN_ADDR    (GPIOP8_ADDR_BASE + 0x01) /* Port 8 data in. */
#define P8DIR_ADDR    (GPIOP8_ADDR_BASE + 0x02) /* Port 8 direction. */
#define P8PULL_ADDR   (GPIOP8_ADDR_BASE + 0x03) /* Port 8 weak pull-up or pull-down enabled. */
#define P8PUD_ADDR    (GPIOP8_ADDR_BASE + 0x04) /* Port 8 pull-up/down selection. */
#define P8ENVDD_ADDR  (GPIOP8_ADDR_BASE + 0x05) /* Port 8 drive enable by VDD present. */
#define P8OTYPE_ADDR  (GPIOP8_ADDR_BASE + 0x06) /* Port 8 Output Type. */

#define P9DOUT_ADDR   (GPIOP9_ADDR_BASE + 0x00) /* Port 9 data out. */
#define P9DIN_ADDR    (GPIOP9_ADDR_BASE + 0x01) /* Port 9 data in. */
#define P9DIR_ADDR    (GPIOP9_ADDR_BASE + 0x02) /* Port 9 direction. */
#define P9PULL_ADDR   (GPIOP9_ADDR_BASE + 0x03) /* Port 9 weak pull-up or pull-down enabled. */
#define P9PUD_ADDR    (GPIOP9_ADDR_BASE + 0x04) /* Port 9 pull-up/down selection. */
#define P9ENVDD_ADDR  (GPIOP9_ADDR_BASE + 0x05) /* Port 9 drive enable by VDD present. */
#define P9OTYPE_ADDR  (GPIOP9_ADDR_BASE + 0x06) /* Port 9 Output Type. */

#define PADOUT_ADDR   (GPIOPA_ADDR_BASE + 0x00) /* Port A data out. */
#define PADIN_ADDR    (GPIOPA_ADDR_BASE + 0x01) /* Port A data in. */
#define PADIR_ADDR    (GPIOPA_ADDR_BASE + 0x02) /* Port A direction. */
#define PAPULL_ADDR   (GPIOPA_ADDR_BASE + 0x03) /* Port A weak pull-up or pull-down enabled. */
#define PAPUD_ADDR    (GPIOPA_ADDR_BASE + 0x04) /* Port A pull-up/down selection. */
#define PAENVDD_ADDR  (GPIOPA_ADDR_BASE + 0x05) /* Port A drive enable by VDD present. */
#define PAOTYPE_ADDR  (GPIOPA_ADDR_BASE + 0x06) /* Port A Output Type. */

#define PBDOUT_ADDR   (GPIOPB_ADDR_BASE + 0x00) /* Port B data out. */
#define PBDIN_ADDR    (GPIOPB_ADDR_BASE + 0x01) /* Port B data in. */
#define PBDIR_ADDR    (GPIOPB_ADDR_BASE + 0x02) /* Port B direction. */
#define PBPULL_ADDR   (GPIOPB_ADDR_BASE + 0x03) /* Port B weak pull-up or pull-down enabled. */
#define PBPUD_ADDR    (GPIOPB_ADDR_BASE + 0x04) /* Port B pull-up/down selection. */
#define PBENVDD_ADDR  (GPIOPB_ADDR_BASE + 0x05) /* Port B drive enable by VDD present. */
#define PBOTYPE_ADDR  (GPIOPB_ADDR_BASE + 0x06) /* Port B Output Type. */

#define PCDOUT_ADDR   (GPIOPC_ADDR_BASE + 0x00) /* Port C data out. */
#define PCDIN_ADDR    (GPIOPC_ADDR_BASE + 0x01) /* Port C data in. */
#define PCDIR_ADDR    (GPIOPC_ADDR_BASE + 0x02) /* Port C direction. */
#define PCPULL_ADDR   (GPIOPC_ADDR_BASE + 0x03) /* Port C weak pull-up or pull-down enabled. */
#define PCPUD_ADDR    (GPIOPC_ADDR_BASE + 0x04) /* Port C pull-up/down selection. */
#define PCENVDD_ADDR  (GPIOPC_ADDR_BASE + 0x05) /* Port C drive enable by VDD present. */
#define PCOTYPE_ADDR  (GPIOPC_ADDR_BASE + 0x06) /* Port C Output Type. */

#define PDDOUT_ADDR   (GPIOPD_ADDR_BASE + 0x00) /* Port D data out. */
#define PDDIN_ADDR    (GPIOPD_ADDR_BASE + 0x01) /* Port D data in. */
#define PDDIR_ADDR    (GPIOPD_ADDR_BASE + 0x02) /* Port D direction. */
#define PDPULL_ADDR   (GPIOPD_ADDR_BASE + 0x03) /* Port D weak pull-up or pull-down enabled. */
#define PDPUD_ADDR    (GPIOPD_ADDR_BASE + 0x04) /* Port D pull-up/down selection. */
#define PDENVDD_ADDR  (GPIOPD_ADDR_BASE + 0x05) /* Port D drive enable by VDD present. */
#define PDOTYPE_ADDR  (GPIOPD_ADDR_BASE + 0x06) /* Port D Output Type. */

#define PEDOUT_ADDR   (GPIOPE_ADDR_BASE + 0x00) /* Port E data out. */
#define PEDIN_ADDR    (GPIOPE_ADDR_BASE + 0x01) /* Port E data in. */
#define PEDIR_ADDR    (GPIOPE_ADDR_BASE + 0x02) /* Port E direction. */
#define PEPULL_ADDR   (GPIOPE_ADDR_BASE + 0x03) /* Port E weak pull-up or pull-down enabled. */
#define PEPUD_ADDR    (GPIOPE_ADDR_BASE + 0x04) /* Port E pull-up/down selection. */
#define PEENVDD_ADDR  (GPIOPE_ADDR_BASE + 0x05) /* Port E drive enable by VDD present. */
#define PEOTYPE_ADDR  (GPIOPE_ADDR_BASE + 0x06) /* Port E Output Type. */
#define PFDOUT_ADDR   (GPIOPF_ADDR_BASE + 0x00) /* Port F data out. */
#define PFDIN_ADDR    (GPIOPF_ADDR_BASE + 0x01) /* Port F data in. */
#define PFDIR_ADDR    (GPIOPF_ADDR_BASE + 0x02) /* Port F direction. */
#define PFPULL_ADDR   (GPIOPF_ADDR_BASE + 0x03) /* Port F weak pull-up or pull-down enabled. */
#define PFPUD_ADDR    (GPIOPF_ADDR_BASE + 0x04) /* Port F pull-up/down selection. */
#define PFENVDD_ADDR  (GPIOPF_ADDR_BASE + 0x05) /* Port F drive enable by VDD present. */
#define PFOTYPE_ADDR  (GPIOPF_ADDR_BASE + 0x06) /* Port F Output Type. */

/* Port 0 of chip. */
#define P0DOUT      PORT_BYTE   P0DOUT_ADDR     /* Data out register. */
#define P0DIN       PORT_BYTE   P0DIN_ADDR      /* Data in register. */
#define P0DIR       PORT_BYTE   P0DIR_ADDR      /* Direction register. */
#define P0PULL      PORT_BYTE   P0PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P0PUD       PORT_BYTE   P0PUD_ADDR      /* Pull-up/down selection. */
#define P0ENVDD     PORT_BYTE   P0ENVDD_ADDR    /* Drive enable by VDD present. */
#define P0OTYPE     PORT_BYTE   P0OTYPE_ADDR    /* Output Type. */

/* Port 1 of chip. */
#define P1DOUT      PORT_BYTE   P1DOUT_ADDR     /* Data out register. */
#define P1DIN       PORT_BYTE   P1DIN_ADDR      /* Data in register. */
#define P1DIR       PORT_BYTE   P1DIR_ADDR      /* Direction register. */
#define P1PULL      PORT_BYTE   P1PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P1PUD       PORT_BYTE   P1PUD_ADDR      /* Pull-up/down selection. */
#define P1ENVDD     PORT_BYTE   P1ENVDD_ADDR    /* Drive enable by VDD present. */
#define P1OTYPE     PORT_BYTE   P1OTYPE_ADDR    /* Output Type. */

/* Port 2 of chip. */
#define P2DOUT      PORT_BYTE   P2DOUT_ADDR     /* Data out register. */
#define P2DIN       PORT_BYTE   P2DIN_ADDR      /* Data in register. */
#define P2DIR       PORT_BYTE   P2DIR_ADDR      /* Direction register. */
#define P2PULL      PORT_BYTE   P2PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P2PUD       PORT_BYTE   P2PUD_ADDR      /* Pull-up/down selection. */
#define P2ENVDD     PORT_BYTE   P2ENVDD_ADDR    /* Drive enable by VDD present. */
#define P2OTYPE     PORT_BYTE   P2OTYPE_ADDR    /* Output Type. */

/* Port 3 of chip. */
#define P3DOUT      PORT_BYTE   P3DOUT_ADDR     /* Data out register. */
#define P3DIN       PORT_BYTE   P3DIN_ADDR      /* Data in register. */
#define P3DIR       PORT_BYTE   P3DIR_ADDR      /* Direction register. */
#define P3PULL      PORT_BYTE   P3PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P3PUD       PORT_BYTE   P3PUD_ADDR      /* Pull-up/down selection. */
#define P3ENVDD     PORT_BYTE   P3ENVDD_ADDR    /* Drive enable by VDD present. */
#define P3OTYPE     PORT_BYTE   P3OTYPE_ADDR    /* Output Type. */

/* Port 4 of chip. */
#define P4DOUT      PORT_BYTE   P4DOUT_ADDR     /* Data out register. */
#define P4DIN       PORT_BYTE   P4DIN_ADDR      /* Data in register. */
#define P4DIR       PORT_BYTE   P4DIR_ADDR      /* Direction register. */
#define P4PULL      PORT_BYTE   P4PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P4PUD       PORT_BYTE   P4PUD_ADDR      /* Pull-up/down selection. */
#define P4ENVDD     PORT_BYTE   P4ENVDD_ADDR    /* Drive enable by VDD present. */
#define P4OTYPE     PORT_BYTE   P4OTYPE_ADDR    /* Output Type. */

/* Port 5 of chip. */
#define P5DOUT      PORT_BYTE   P5DOUT_ADDR     /* Data out register. */
#define P5DIN       PORT_BYTE   P5DIN_ADDR      /* Data in register. */
#define P5DIR       PORT_BYTE   P5DIR_ADDR      /* Direction register. */
#define P5PULL      PORT_BYTE   P5PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P5PUD       PORT_BYTE   P5PUD_ADDR      /* Pull-up/down selection. */
#define P5ENVDD     PORT_BYTE   P5ENVDD_ADDR    /* Drive enable by VDD present. */
#define P5OTYPE     PORT_BYTE   P5OTYPE_ADDR    /* Output Type. */

/* Port 6 of chip. */
#define P6DOUT      PORT_BYTE   P6DOUT_ADDR     /* Data out register. */
#define P6DIN       PORT_BYTE   P6DIN_ADDR      /* Data in register. */
#define P6DIR       PORT_BYTE   P6DIR_ADDR      /* Direction register. */
#define P6PULL      PORT_BYTE   P6PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P6PUD       PORT_BYTE   P6PUD_ADDR      /* Pull-up/down selection. */
#define P6ENVDD     PORT_BYTE   P6ENVDD_ADDR    /* Drive enable by VDD present. */
#define P6OTYPE     PORT_BYTE   P6OTYPE_ADDR    /* Output Type. */

/* Port 7 of chip. */
#define P7DOUT      PORT_BYTE   P7DOUT_ADDR     /* Data out register. */
#define P7DIN       PORT_BYTE   P7DIN_ADDR      /* Data in register. */
#define P7DIR       PORT_BYTE   P7DIR_ADDR      /* Direction register. */
#define P7PULL      PORT_BYTE   P7PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P7PUD       PORT_BYTE   P7PUD_ADDR      /* Pull-up/down selection. */
#define P7ENVDD     PORT_BYTE   P7ENVDD_ADDR    /* Drive enable by VDD present. */
#define P7OTYPE     PORT_BYTE   P7OTYPE_ADDR    /* Output Type. */

/* Port 8 of chip. */
#define P8DOUT      PORT_BYTE   P8DOUT_ADDR     /* Data out register. */
#define P8DIN       PORT_BYTE   P8DIN_ADDR      /* Data in register. */
#define P8DIR       PORT_BYTE   P8DIR_ADDR      /* Direction register. */
#define P8PULL      PORT_BYTE   P8PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P8PUD       PORT_BYTE   P8PUD_ADDR      /* Pull-up/down selection. */
#define P8ENVDD     PORT_BYTE   P8ENVDD_ADDR    /* Drive enable by VDD present. */
#define P8OTYPE     PORT_BYTE   P8OTYPE_ADDR    /* Output Type. */

/* Port 9 of chip. */
#define P9DOUT      PORT_BYTE   P9DOUT_ADDR     /* Data out register. */
#define P9DIN       PORT_BYTE   P9DIN_ADDR      /* Data in register. */
#define P9DIR       PORT_BYTE   P9DIR_ADDR      /* Direction register. */
#define P9PULL      PORT_BYTE   P9PULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define P9PUD       PORT_BYTE   P9PUD_ADDR      /* Pull-up/down selection. */
#define P9ENVDD     PORT_BYTE   P9ENVDD_ADDR    /* Drive enable by VDD present. */
#define P9OTYPE     PORT_BYTE   P9OTYPE_ADDR    /* Output Type. */

/* Port A of chip. */
#define PADOUT      PORT_BYTE   PADOUT_ADDR     /* Data out register. */
#define PADIN       PORT_BYTE   PADIN_ADDR      /* Data in register. */
#define PADIR       PORT_BYTE   PADIR_ADDR      /* Direction register. */
#define PAPULL      PORT_BYTE   PAPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PAPUD       PORT_BYTE   PAPUD_ADDR      /* Pull-up/down selection. */
#define PAENVDD     PORT_BYTE   PAENVDD_ADDR    /* Drive enable by VDD present. */
#define PAOTYPE     PORT_BYTE   PAOTYPE_ADDR    /* Output Type. */

/* Port B of chip. */
#define PBDOUT      PORT_BYTE   PBDOUT_ADDR     /* Data out register. */
#define PBDIN       PORT_BYTE   PBDIN_ADDR      /* Data in register. */
#define PBDIR       PORT_BYTE   PBDIR_ADDR      /* Direction register. */
#define PBPULL      PORT_BYTE   PBPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PBPUD       PORT_BYTE   PBPUD_ADDR      /* Pull-up/down selection. */
#define PBENVDD     PORT_BYTE   PBENVDD_ADDR    /* Drive enable by VDD present. */
#define PBOTYPE     PORT_BYTE   PBOTYPE_ADDR    /* Output Type. */

/* Port C of chip. */
#define PCDOUT      PORT_BYTE   PCDOUT_ADDR     /* Data out register. */
#define PCDIN       PORT_BYTE   PCDIN_ADDR      /* Data in register. */
#define PCDIR       PORT_BYTE   PCDIR_ADDR      /* Direction register. */
#define PCPULL      PORT_BYTE   PCPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PCPUD       PORT_BYTE   PCPUD_ADDR      /* Pull-up/down selection. */
#define PCENVDD     PORT_BYTE   PCENVDD_ADDR    /* Drive enable by VDD present. */
#define PCOTYPE     PORT_BYTE   PCOTYPE_ADDR    /* Output Type. */

/* Port D of chip. */
#define PDDOUT      PORT_BYTE   PDDOUT_ADDR     /* Data out register. */
#define PDDIN       PORT_BYTE   PDDIN_ADDR      /* Data in register. */
#define PDDIR       PORT_BYTE   PDDIR_ADDR      /* Direction register. */
#define PDPULL      PORT_BYTE   PDPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PDPUD       PORT_BYTE   PDPUD_ADDR      /* Pull-up/down selection. */
#define PDENVDD     PORT_BYTE   PDENVDD_ADDR    /* Drive enable by VDD present. */
#define PDOTYPE     PORT_BYTE   PDOTYPE_ADDR    /* Output Type. */

/* Port E of chip. */
#define PEDOUT      PORT_BYTE   PEDOUT_ADDR     /* Data out register. */
#define PEDIN       PORT_BYTE   PEDIN_ADDR      /* Data in register. */
#define PEDIR       PORT_BYTE   PEDIR_ADDR      /* Direction register. */
#define PEPULL      PORT_BYTE   PEPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PEPUD       PORT_BYTE   PEPUD_ADDR      /* Pull-up/down selection. */
#define PEENVDD     PORT_BYTE   PEENVDD_ADDR    /* Drive enable by VDD present. */
#define PEOTYPE     PORT_BYTE   PEOTYPE_ADDR    /* Output Type. */

/* Port F of chip. */
#define PFDOUT      PORT_BYTE   PFDOUT_ADDR     /* Data out register. */
#define PFDIN       PORT_BYTE   PFDIN_ADDR      /* Data in register. */
#define PFDIR       PORT_BYTE   PFDIR_ADDR      /* Direction register. */
#define PFPULL      PORT_BYTE   PFPULL_ADDR     /* Weak pull-up or pull-down enabled. */
#define PFPUD       PORT_BYTE   PFPUD_ADDR      /* Pull-up/down selection. */
#define PFENVDD     PORT_BYTE   PFENVDD_ADDR    /* Drive enable by VDD present. */
#define PFOTYPE     PORT_BYTE   PFOTYPE_ADDR    /* Output Type. */



/* Keyboard Scan Core Registers */
#define KBSCAN_ADDR_BASE 0xFFF6C0

#define KBSIN_ADDR          (KBSCAN_ADDR_BASE + 0x04)   /* Keyboard Scan In. */
#define KBSINPU_ADDR        (KBSCAN_ADDR_BASE + 0x05)   /* Keyboard Scan In Pull-Up Enable. */
#define KBSOUT0_ADDR        (KBSCAN_ADDR_BASE + 0x06)   /* Keyboard Scan Out 0. */
#define KBSOUT1_ADDR        (KBSCAN_ADDR_BASE + 0x08)   /* Keyboard Scan Out 1. */
#define KBS_BUF_INDX_ADDR   (KBSCAN_ADDR_BASE + 0x0A)   /* Keyboard Scan Buffer Index. */
#define KBS_BUF_DATA_ADDR   (KBSCAN_ADDR_BASE + 0x0B)   /* Keyboard Scan Buffer Data. */
#define KBSEVT_ADDR         (KBSCAN_ADDR_BASE + 0x0C)   /* Keyboard Scan Event. */
#define KBSCTL_ADDR         (KBSCAN_ADDR_BASE + 0x0D)   /* Keyboard Scan Control. */
#define KBS_CFG_INDX_ADDR   (KBSCAN_ADDR_BASE + 0x0E)   /* Keyboard Scan Configuration Index. */
#define KBS_CFG_DATA_ADDR   (KBSCAN_ADDR_BASE + 0x0F)   /* Keyboard Scan Configuration Data. */

/* Keyboard Scan In. */
#define KBSIN           PORT_BYTE   KBSIN_ADDR
/* Keyboard Scan In Pull-Up Enable. */
#define KBSINPU         PORT_BYTE   KBSINPU_ADDR
/* Keyboard Scan Out 0. */
#define KBSOUT0         PORT_WORD   KBSOUT0_ADDR
/* Keyboard Scan Out 1. */
#define KBSOUT1         PORT_WORD   KBSOUT1_ADDR
/* Keyboard Scan Buffer Index. */
#define KBS_BUF_INDX    PORT_BYTE   KBS_BUF_INDX_ADDR
/* Keyboard Scan Buffer Data. */
#define KBS_BUF_DATA    PORT_BYTE   KBS_BUF_DATA_ADDR
/* Keyboard Scan Event. */
#define KBSEVT          PORT_BYTE   KBSEVT_ADDR
/* -------------------------------------------- */
#define KBSDONE     (1 << 0)    /* Keyboard Scan ¡§Done¡¨ Indication */
#define KBSERR      (1 << 1)    /* Keyboard Scan Error */

/* Keyboard Scan Control. */
#define KBSCTL          PORT_BYTE   KBSCTL_ADDR
/* -------------------------------------------- */
#define KBSSTART    (1 << 0)    /* Automatic Keyboard Scan Start */
#define KBSMODE     (1 << 1)    /* Keyboard Scan Mode */
#define KBSIEN      (1 << 2)    /* Keyboard Scan Interrupt Enable */
#define KBSINC      (1 << 3)    /* Keyboard Scan Index Increment */

/* Keyboard Scan Configuration Index. */
#define KBS_CFG_INDX    PORT_BYTE   KBS_CFG_INDX_ADDR
/* -------------------------------------------- */
#define KBS_DLY1    0   /* Keyboard Scan Delay 1 Register */
#define KBS_DLY2    1   /* Keyboard Scan Delay 2 Register */
#define KBS_RTYTO   2   /* Keyboard Scan Retry Timeout Register */
#define KBS_CNUM    3   /* Keyboard Scan Columns Number Register */
#define KBS_CDIV    4   /* Keyboard Scan Clock Divisor Register */

/* Keyboard Scan Configuration Data. */
#define KBS_CFG_DATA    PORT_BYTE   KBS_CFG_DATA_ADDR



/* System Glue Core Registers */
#define GLUE_ADDR_BASE 0xFFF6C0
#define IOEE_ADDR       (GLUE_ADDR_BASE + 0x00) /* Input to Output Echo Enable. */
#define IOECTL_ADDR     (GLUE_ADDR_BASE + 0x01) /* Input to Output Echo Control. */
#define SMB_SBD_ADDR    (GLUE_ADDR_BASE + 0x02) /* SMBus Start Bit Detection. */
#define SMB_EEN_ADDR    (GLUE_ADDR_BASE + 0x03) /* SMBus Event Enable. */
#define SDPD0_ADDR      (GLUE_ADDR_BASE + 0x10) /* Simple Debug Port Data 0. */
#define SDPD1_ADDR      (GLUE_ADDR_BASE + 0x12) /* Simple Debug Port Data 1. */
#define SDP_CTS_ADDR    (GLUE_ADDR_BASE + 0x14) /* Simple Debug Port Control and Status. */
#define IOECTL2_ADDR    (GLUE_ADDR_BASE + 0x18) /* Input to Output Echo Control 2. */
#define SDBC_CLK_SL_ADDR (GLUE_ADDR_BASE + 0x1B) /* Slow DBC Clock Select */
#define DBC12_CTL_ADDR  (GLUE_ADDR_BASE + 0x1C) /* DBC1 and DBC2 Control. */
#define DBC34_CTL_ADDR  (GLUE_ADDR_BASE + 0x1D) /* DBC3 and DBC4 Control. */
#define DBC56_CTL_ADDR  (GLUE_ADDR_BASE + 0x1E) /* DBC5 and DBC6 Control. */
#define DBC_SEL_ADDR    (GLUE_ADDR_BASE + 0x1F) /* DBC Signals Select. */
#define PSL_IN_MON_ADDR (GLUE_ADDR_BASE + 0x20) /* PSL Inputs Monitor. */
#define SMB_SEL_ADDR    (GLUE_ADDR_BASE + 0x21) /* SMBus Ports Select. */
#define VD_CFG_ADDR     (GLUE_ADDR_BASE + 0x24) /* Voltage Detectors Configuration */
#define PSL_CTS_ADDR    (GLUE_ADDR_BASE + 0x26) /* PSL Control and Status. */
#define PSL_CTS2_ADDR   (GLUE_ADDR_BASE + 0x27) /* PSL Control and Status 2 */
#define VD_TH1_ADDR     (GLUE_ADDR_BASE + 0x28) /* Voltage Detector 1 Threshold. */
#define VD_THU1_ADDR    (GLUE_ADDR_BASE + 0x29) /* Voltage Detector 1, Up Threshold */
#define VD_TH2_ADDR     (GLUE_ADDR_BASE + 0x2A) /* Voltage Detector 2 Threshold. */
#define VD_THU2_ADDR    (GLUE_ADDR_BASE + 0x2B) /* Voltage Detector 2, Up Threshold */
#define VD_CTL_ADDR     (GLUE_ADDR_BASE + 0x2C) /* Voltage Detectors Control. */
#define VD_CTS_ADDR     (GLUE_ADDR_BASE + 0x2D) /* Voltage Detectors Control and Status */
#define EPURST_CTL_ADDR (GLUE_ADDR_BASE + 0x30) /* External Power-Up Reset Control */
#define EPURST_DBC_ADDR (GLUE_ADDR_BASE + 0x31) /* External Power-Up Reset Debounce */
#define PSL_CTS4_ADDR   (GLUE_ADDR_BASE + 0x39) /* PSL Control and Status 4 */

/* Input to Output Echo Enable. */
#define IOEE        PORT_BYTE   IOEE_ADDR
/* ------------------------------------ */
#define EEP0    (1 << 0) /* Echo Enable Port 0 */
#define EEP1    (1 << 1) /* Echo Enable Port 1 */
#define EEP2    (1 << 2) /* Echo Enable Port 2 */
#define EEP3    (1 << 3) /* Echo Enable Port 3 */

/* Input to Output Echo Control. */
#define IOECTL      PORT_BYTE   IOECTL_ADDR

/* SMBus Start Bit Detection. */
#define SMB_SBD     PORT_BYTE   SMB_SBD_ADDR
/* --------------------------------------- */
#define SMB5SBD     (1 << 4)    /* SMBus 5 Start Bit Detection. */
#define SMB4SBD     (1 << 3)    /* SMBus 4 Start Bit Detection. */
#define SMB3SBD     (1 << 2)    /* SMBus 3 Start Bit Detection. */
#define SMB2SBD     (1 << 1)    /* SMBus 2 Start Bit Detection. */
#define SMB1SBD     (1 << 0)    /* SMBus 1 Start Bit Detection. */

/* SMBus Event Enable. */
#define SMB_EEN     PORT_BYTE   SMB_EEN_ADDR
/* --------------------------------------- */
#define maskSMB_EEN     0x1F
#define SMB5EEN     (1 << 4)    /* SMBus 5 Event Enable. */
#define SMB4EEN     (1 << 3)    /* SMBus 4 Event Enable. */
#define SMB3EEN     (1 << 2)    /* SMBus 3 Event Enable. */
#define SMB2EEN     (1 << 1)    /* SMBus 2 Event Enable. */
#define SMB1EEN     (1 << 0)    /* SMBus 1 Event Enable. */

/* Simple Debug Port Data 0. */
#define SDPD0       PORT_BYTE   SDPD0_ADDR
/* Simple Debug Port Data 1. */
#define SDPD1       PORT_BYTE   SDPD1_ADDR
/* Simple Debug Port Control and Status. */
#define SDP_CTS     PORT_BYTE   SDP_CTS_ADDR
/* ---------------------------------------- */
#define SDP_EN          (1 << 0)  /* SDP Enable. */
#define SDP_MOD         (1 << 1)  /* SDP Mode Select. */
#define DUAL_P80        (1 << 3)  /* Dual Port 80 Display. */

/* Input to Output Echo Control 2. */
#define IOECTL2     PORT_BYTE   IOECTL2_ADDR
/* --------------------------------------- */
#define DBC_CTL_00_P    0
#define DBC_CTL_00_S    3
#define FBCWK_00        3
#define DBC_CTL_02_P    4
#define DBC_CTL_02_S    3
#define FBCWK_02        7

/* Slow DBC Clock Select. */
#define SDBC_CLK_SL PORT_BYTE   SDBC_CLK_SL_ADDR
/* --------------------------------------- */
#define SDB1_CLK    (1 << 0)    /* Slow DBC1 Clock Select. */
#define SDB2_CLK    (1 << 1)    /* Slow DBC2 Clock Select. */
#define SDB3_CLK    (1 << 2)    /* Slow DBC3 Clock Select. */
#define SDB4_CLK    (1 << 3)    /* Slow DBC4 Clock Select. */
#define SDB5_CLK    (1 << 4)    /* Slow DBC5 Clock Select. */
#define SDB6_CLK    (1 << 5)    /* Slow DBC6 Clock Select. */

/* DBC1 and DBC2 Control. */
#define DBC12_CTL   PORT_BYTE   DBC12_CTL_ADDR
/* DBC3 and DBC4 Control. */
#define DBC34_CTL   PORT_BYTE   DBC34_CTL_ADDR
/* DBC5 and DBC6 Control. */
#define DBC56_CTL   PORT_BYTE   DBC56_CTL_ADDR
/* DBC Signals Select. */
#define DBC_SEL     PORT_BYTE   DBC_SEL_ADDR
/* PSL Inputs Monitor. */
#define PSL_IN_MON  PORT_BYTE   PSL_IN_MON_ADDR
/* SMBus Ports Select. */
#define SMB_SEL     PORT_BYTE   SMB_SEL_ADDR
/* Voltage Detectors Configuration */
#define VD_CFG      PORT_BYTE   VD_CFG_ADDR

/* PSL Control and Status. */
#define PSL_CTS     PORT_BYTE   PSL_CTS_ADDR
/* PSL Control and Status 2. */
#define PSL_CTS2    PORT_BYTE   PSL_CTS2_ADDR
/* Voltage Detector 1 Threshold. */
#define VD_TH1      PORT_BYTE   VD_TH1_ADDR
/* Voltage Detector 1, Up Threshold */
#define VD_THU1     PORT_BYTE   VD_THU1_ADDR
/* Voltage Detector 2 Threshold. */
#define VD_TH2      PORT_BYTE   VD_TH2_ADDR
/* Voltage Detector 2, Up Threshold */
#define VD_THU2     PORT_BYTE   VD_THU2_ADDR
/* Voltage Detectors Control. */
#define VD_CTL      PORT_BYTE   VD_CTL_ADDR
/* --------------------------------------- */
#define VD_OVT2_STS     (1 << 7)    /* Voltage Detector 2, Over-Temperature Status */
#define VD_LK2          (1 << 6)    /* Voltage Detector 2, Threshold Lock */
#define VD_EVEN2        (1 << 5)    /* Voltage Detector 2, Event Enable */
#define VD_OVT2_EV      (1 << 4)    /* Voltage Detector 2, Over-Temperature Event */
#define VD_OVT1_STS     (1 << 3)    /* Voltage Detector 1, Over-Temperature Status */
#define VD_LK1          (1 << 2)    /* Voltage Detector 1, Threshold Lock */
#define VD_EVEN1        (1 << 1)    /* Voltage Detector 1, Event Enable */
#define VD_OVT1_EV      (1 << 0)    /* Voltage Detector 1, Over-Temperature Event */

/* Voltage Detectors Control and Status */
#define VD_CTS      PORT_BYTE   VD_CTS_ADDR
/* External Power-Up Reset Control */
#define EPURST_CTL  PORT_BYTE   EPURST_CTL_ADDR
#define EPUR_LK         (1 << 7)    /* EXT_PURST Configuration Lock */
#define EPUR2_EN        (1 << 3)    /* External Power-Up Reset, Input 2 Enable */
#define EPUR2_AHI       (1 << 2)    /* External Power-Up Reset, Input 2 Active-High */
#define EPUR1_EN        (1 << 1)    /* External Power-Up Reset, Input 1 Enable */
#define EPUR1_AHI       (1 << 0)    /* External Power-Up Reset, Input 1 Active-High */

/* External Power-Up Reset Debounce */
#define EPURST_DBC  PORT_BYTE   EPURST_DBC_ADDR

/* PSL Control and Status 4. */
#define PSL_CTS4     PORT_BYTE   PSL_CTS4_ADDR
/*----------------------------------------- */
#define ADC_OVT_EN     (1 << 0)    /* ADC OVT Enable. */
#define ADC_OVT_STS    (1 << 1)    /* ADC Over-Temperature Event. */

/* PS/2 Interface Core Registers */
#define PS2_ADDR_BASE 0xFFF300
#define PSDAT_ADDR      (PS2_ADDR_BASE + 0x00)  /* PS/2 Data. */
#define PSTAT_ADDR      (PS2_ADDR_BASE + 0x02)  /* PS/2 Status. */
#define PSCON_ADDR      (PS2_ADDR_BASE + 0x04)  /* PS/2 Control. */
#define PSOSIG_ADDR     (PS2_ADDR_BASE + 0x06)  /* PS/2 Output Signal. */
#define PSISIG_ADDR     (PS2_ADDR_BASE + 0x08)  /* PS/2 Input Signal. */
#define PSIEN_ADDR      (PS2_ADDR_BASE + 0x0A)  /* PS/2 Interrupt Enable. */

/* PS/2 Data. */
#define PSDAT       PORT_BYTE   PSDAT_ADDR
/* PS/2 Status. */
#define PSTAT       PORT_BYTE   PSTAT_ADDR
/* ------------------------------------- */
#define PSTAT_SOT        (1 << 0)   /* Start of transaction. */
#define PSTAT_EOT        (1 << 1)   /* End of transaction. */
#define PSTAT_PERR       (1 << 2)   /* Parity error. */
#define PSTAT_RFERR      (1 << 6)   /* Frame error. */
#define PSTAT_ACH        (0x7 << 3) /* Active channel. */
#define AUX_PORT1_ACTIVE (1 << 3)
#define AUX_PORT2_ACTIVE (2 << 3)
#define AUX_PORT3_ACTIVE (4 << 3)
//
/* PS/2 Control. */
#define PSCON       PORT_BYTE   PSCON_ADDR
/* ------------------------------------- */
#define PSCON_EN        (1 << 0)    /* Enable shift mechanism. */
#define PSCON_XMT       (1 << 1)    /* When set - transmit mode. */
#define PSCON_IDB       (7 << 4)    /* Input debounce: */
#define PSCON_IDB_1     (0 << 4)    /* 1 cycle. */
#define PSCON_IDB_2     (1 << 4)    /* 2 cycle. */
#define PSCON_IDB_4     (2 << 4)    /* 4 cycle. */
#define PSCON_IDB_8     (3 << 4)    /* 8 cycle. */
#define PSCON_IDB_16    (4 << 4)    /* 16 cycles. */
#define PSCON_IDB_32    (5 << 4)    /* 32 cycles. */
#define PSCON_HDRV      (3 << 2)    /* High drive. */
#define PSCON_WPUEN     (1 << 7)    /* Weak pullup enable. */

/* PS/2 Output Signal. */
#define PSOSIG      PORT_BYTE   PSOSIG_ADDR
/* -------------------------------------- */
#define PSOSIG_WDAT1     (1 << 0)    /* Write data signal channel 1. */
#define PSOSIG_WDAT2     (1 << 1)    /* Write data signal channel 2. */
#define PSOSIG_WDAT3     (1 << 2)    /* Write data signal channel 3. */
#define PSOSIG_CLK1      (1 << 3)    /* Enable (or clock) channel 1. */
#define PSOSIG_CLK2      (1 << 4)    /* Enable (or clock) channel 2. */
#define PSOSIG_CLK3      (1 << 5)    /* Enable (or clock) channel 3. */

/* PS/2 Input Signal. */
#define PSISIG      PORT_BYTE   PSISIG_ADDR
/* -------------------------------------- */
#define PSISIG_RDAT1     (1 << 0)    /* Read data signal, channel 1. */
#define PSISIG_RDAT2     (1 << 1)    /* Read data signal, channel 2. */
#define PSISIG_RDAT3     (1 << 2)    /* Read data signal, channel 3. */
#define PSISIG_RCLK1     (1 << 3)    /* Read clock signal, channel 1. */
#define PSISIG_RCLK2     (1 << 4)    /* Read clock signal, channel 2. */
#define PSISIG_RCLK3     (1 << 5)    /* Read clock signal, channel 3. */


/* PS/2 Interrupt Enable. */
#define PSIEN       PORT_BYTE   PSIEN_ADDR
/* ------------------------------------- */
#define PSIEN_SOTIE     (1 << 0)    /* Start of Transaction Interrupt Enable. */
#define PSIEN_EOTIE     (1 << 1)    /* End of Transaction Interrupt Enable. */
#define PSIEN_WUE       (1 << 4)    /* PS/2 Wake-Up Enable */
#define PS2_CLK_SEL     (1 << 7)    /* PS/2 Interface Clock Select */

/* MFT16 Core Registers */
#define MFT161_ADDR_BASE 0xFFF340
#define MFT162_ADDR_BASE 0xFFF380
#define MFT163_ADDR_BASE 0xFFF3C0

#define T1CNT1_ADDR     (MFT161_ADDR_BASE + 0x00)   /* Timer/Counter 1. */
#define T1CRA_ADDR      (MFT161_ADDR_BASE + 0x02)   /* Reload/Capture A. */
#define T1CRB_ADDR      (MFT161_ADDR_BASE + 0x04)   /* Reload/Capture B. */
#define T1CNT2_ADDR     (MFT161_ADDR_BASE + 0x06)   /* Timer/Counter 2. */
#define T1PRSC_ADDR     (MFT161_ADDR_BASE + 0x08)   /* Clock Prescaler. */
#define T1CKC_ADDR      (MFT161_ADDR_BASE + 0x0A)   /* Clock Unit Control. */
#define T1MCTRL_ADDR    (MFT161_ADDR_BASE + 0x0C)   /* Timer Mode Control. */
#define T1ICTRL_ADDR    (MFT161_ADDR_BASE + 0x0E)   /* Timer Interrupt Control. */
#define T1ICLR_ADDR     (MFT161_ADDR_BASE + 0x10)   /* Timer Interrupt Clear. */
#define T1IEN_ADDR      (MFT161_ADDR_BASE + 0x12)   /* Timer Interrupt Enable. */
#define T1CPA_ADDR      (MFT161_ADDR_BASE + 0x14)   /* Compare A. */
#define T1CPB_ADDR      (MFT161_ADDR_BASE + 0x16)   /* Compare B. */
#define T1CPCFG_ADDR    (MFT161_ADDR_BASE + 0x18)   /* Compare Configuration. */
#define T1WUEN_ADDR     (MFT161_ADDR_BASE + 0x1A)   /* Timer Wake-Up Enable. */
#define T2CNT1_ADDR     (MFT162_ADDR_BASE + 0x00)   /* Timer/Counter 1. */
#define T2CRA_ADDR      (MFT162_ADDR_BASE + 0x02)   /* Reload/Capture A. */
#define T2CRB_ADDR      (MFT162_ADDR_BASE + 0x04)   /* Reload/Capture B. */
#define T2CNT2_ADDR     (MFT162_ADDR_BASE + 0x06)   /* Timer/Counter 2. */
#define T2PRSC_ADDR     (MFT162_ADDR_BASE + 0x08)   /* Clock Prescaler. */
#define T2CKC_ADDR      (MFT162_ADDR_BASE + 0x0A)   /* Clock Unit Control. */
#define T2MCTRL_ADDR    (MFT162_ADDR_BASE + 0x0C)   /* Timer Mode Control. */
#define T2ICTRL_ADDR    (MFT162_ADDR_BASE + 0x0E)   /* Timer Interrupt Control. */
#define T2ICLR_ADDR     (MFT162_ADDR_BASE + 0x10)   /* Timer Interrupt Clear. */
#define T2IEN_ADDR      (MFT162_ADDR_BASE + 0x12)   /* Timer Interrupt Enable. */
#define T2CPA_ADDR      (MFT162_ADDR_BASE + 0x14)   /* Compare A. */
#define T2CPB_ADDR      (MFT162_ADDR_BASE + 0x16)   /* Compare B. */
#define T2CPCFG_ADDR    (MFT162_ADDR_BASE + 0x18)   /* Compare Configuration. */
#define T2WUEN_ADDR     (MFT162_ADDR_BASE + 0x1A)   /* Timer Wake-Up Enable. */
#define T3CNT1_ADDR     (MFT163_ADDR_BASE + 0x00)   /* Timer/Counter 1. */
#define T3CRA_ADDR      (MFT163_ADDR_BASE + 0x02)   /* Reload/Capture A. */
#define T3CRB_ADDR      (MFT163_ADDR_BASE + 0x04)   /* Reload/Capture B. */
#define T3CNT2_ADDR     (MFT163_ADDR_BASE + 0x06)   /* Timer/Counter 2. */
#define T3PRSC_ADDR     (MFT163_ADDR_BASE + 0x08)   /* Clock Prescaler. */
#define T3CKC_ADDR      (MFT163_ADDR_BASE + 0x0A)   /* Clock Unit Control. */
#define T3MCTRL_ADDR    (MFT163_ADDR_BASE + 0x0C)   /* Timer Mode Control. */
#define T3ICTRL_ADDR    (MFT163_ADDR_BASE + 0x0E)   /* Timer Interrupt Control. */
#define T3ICLR_ADDR     (MFT163_ADDR_BASE + 0x10)   /* Timer Interrupt Clear. */
#define T3IEN_ADDR      (MFT163_ADDR_BASE + 0x12)   /* Timer Interrupt Enable. */
#define T3CPA_ADDR      (MFT163_ADDR_BASE + 0x14)   /* Compare A. */
#define T3CPB_ADDR      (MFT163_ADDR_BASE + 0x16)   /* Compare B. */
#define T3CPCFG_ADDR    (MFT163_ADDR_BASE + 0x18)   /* Compare Configuration. */
#define T3WUEN_ADDR     (MFT163_ADDR_BASE + 0x1A)   /* Timer Wake-Up Enable. */

/* Timer/Counter 1. */
#define T1CNT1      PORT_WORD   T1CNT1_ADDR
#define T2CNT1      PORT_WORD   T2CNT1_ADDR
#define T3CNT1      PORT_WORD   T3CNT1_ADDR
/* Reload/Capture A. */
#define T1CRA       PORT_WORD   T1CRA_ADDR
#define T2CRA       PORT_WORD   T2CRA_ADDR
#define T3CRA       PORT_WORD   T3CRA_ADDR
/* Reload/Capture B. */
#define T1CRB       PORT_WORD   T1CRB_ADDR
#define T2CRB       PORT_WORD   T2CRB_ADDR
#define T3CRB       PORT_WORD   T3CRB_ADDR
/* Timer/Counter 2. */
#define T1CNT2      PORT_WORD   T1CNT2_ADDR
#define T2CNT2      PORT_WORD   T2CNT2_ADDR
#define T3CNT2      PORT_WORD   T3CNT2_ADDR
/* Clock Prescaler. */
#define T1PRSC      PORT_BYTE   T1PRSC_ADDR
#define T2PRSC      PORT_BYTE   T2PRSC_ADDR
#define T3PRSC      PORT_BYTE   T3PRSC_ADDR
/* -------------------------------------- */
#define TPRSC_CLKPS     (0x1F << 0)

/* Clock Unit Control. */
#define T1CKC       PORT_BYTE   T1CKC_ADDR
#define T2CKC       PORT_BYTE   T2CKC_ADDR
#define T3CKC       PORT_BYTE   T3CKC_ADDR
/* -------------------------------------- */
#define TCKC_C1CSEL_S  3
#define TCKC_C1CSEL_P  0
#define TCKC_NOCLK1    0
#define TCKC_SYSCLK1   1
#define TCKC_TBEVNT1   2
#define TCKC_PULSE1    3
#define TCKC_SLOWCLK1  4
#define TCKC_C1CSEL    (0x7 << 0)
#define TCKC_C2CSEL_S  3
#define TCKC_C2CSEL_P  3
#define TCKC_NOCLK2    0
#define TCKC_SYSCLK2   1
#define TCKC_TBEVNT2   2
#define TCKC_PULSE2    3
#define TCKC_SLOWCLK2  4
#define TCKC_C2CSEL    (0x7 << 3)

/* Timer Mode Control. */
#define T1MCTRL     PORT_BYTE   T1MCTRL_ADDR
#define T2MCTRL     PORT_BYTE   T2MCTRL_ADDR
#define T3MCTRL     PORT_BYTE   T3MCTRL_ADDR
/* -------------------------------------- */
#define TCTRL_MDSEL     (7 << 0)
#define TCTRL_MODE1     (0)
#define TCTRL_MODE2     (1)
#define TCTRL_MODE3     (2)
#define TCTRL_MODE4     (3)
#define TCTRL_MODE5     (4)
#define TCTRL_TAEDG     (1 << 3)
#define TCTRL_TBEDG     (1 << 4)
#define TCTRL_TAEN      (1 << 5)
#define TCTRL_TBEN      (1 << 6)
#define TCTRL_TAOUT     (1 << 7)

/* Timer Interrupt Control. */
#define T1ICTRL     PORT_BYTE   T1ICTRL_ADDR
#define T2ICTRL     PORT_BYTE   T2ICTRL_ADDR
#define T3ICTRL     PORT_BYTE   T3ICTRL_ADDR
/* -------------------------------------- */
#define TICTRL_TAPND     (1 << 0)
#define TICTRL_TBPND     (1 << 1)
#define TICTRL_TCPND     (1 << 2)
#define TICTRL_TDPND     (1 << 3)
#define TICTRL_TEPND     (1 << 4)
#define TICTRL_TFPND     (1 << 5)

/* Timer Interrupt Clear. */
#define T1ICLR      PORT_BYTE   T1ICLR_ADDR
#define T2ICLR      PORT_BYTE   T2ICLR_ADDR
#define T3ICLR      PORT_BYTE   T3ICLR_ADDR
/* -------------------------------------- */
#define TICLR_TACLR     (1 << 0)
#define TICLR_TBCLR     (1 << 1)
#define TICLR_TCCLR     (1 << 2)
#define TICLR_TDCLR     (1 << 3)
#define TICLR_TECLR     (1 << 4)
#define TICLR_TFCLR     (1 << 5)

/* Timer Interrupt Enable. */
#define T1IEN       PORT_BYTE   T1IEN_ADDR
#define T2IEN       PORT_BYTE   T2IEN_ADDR
#define T3IEN       PORT_BYTE   T3IEN_ADDR
/* -------------------------------------- */
#define TIEN_TAIEN      (1 << 0)
#define TIEN_TBIEN      (1 << 1)
#define TIEN_TCIEN      (1 << 2)
#define TIEN_TDIEN      (1 << 3)
#define TIEN_TEIEN      (1 << 4)
#define TIEN_TFIEN      (1 << 5)

/* Compare A. */
#define T1CPA       PORT_WORD   T1CPA_ADDR
#define T2CPA       PORT_WORD   T2CPA_ADDR
#define T3CPA       PORT_WORD   T3CPA_ADDR
/* Compare B. */
#define T1CPB       PORT_WORD   T1CPB_ADDR
#define T2CPB       PORT_WORD   T2CPB_ADDR
#define T3CPB       PORT_WORD   T3CPB_ADDR
/* Compare Configuration. */
#define T1CPCFG     PORT_BYTE   T1CPCFG_ADDR
#define T2CPCFG     PORT_BYTE   T2CPCFG_ADDR
#define T3CPCFG     PORT_BYTE   T3CPCFG_ADDR
/* -------------------------------------- */
#define HIBEN       (1 << 7)    /* High Indication Enable for Comparator B */
#define EQBEN       (1 << 6)    /* Equal Indication Enable for Comparator B */
#define LOBEN       (1 << 5)    /* Low Indication Enable for Comparator B */
#define CPBSEL      (1 << 4)    /* Compare Function B Select */
#define HIAEN       (1 << 3)    /* High Indication Enable for Comparator A */
#define EQAEN       (1 << 2)    /* Equal Indication Enable for Comparator A */
#define LOAEN       (1 << 1)    /* Low Indication Enable for Comparator A */
#define CPASEL      (1 << 0)    /* Compare Function A Select */

/* Timer Wake-Up Enable. */
#define T1WUEN      PORT_BYTE   T1WUEN_ADDR
#define T2WUEN      PORT_BYTE   T2WUEN_ADDR
#define T3WUEN      PORT_BYTE   T3WUEN_ADDR
/* -------------------------------------- */
#define TFWEN       (1 << 5)    /* Timer Wake-Up F Enable */
#define TEWEN       (1 << 4)    /* Timer Wake-Up F Enable */
#define TDWEN       (1 << 3)    /* Timer Wake-Up F Enable */
#define TCWEN       (1 << 2)    /* Timer Wake-Up F Enable */
#define TBWEN       (1 << 1)    /* Timer Wake-Up F Enable */
#define TAWEN       (1 << 0)    /* Timer Wake-Up F Enable */

/* Core Register Map for ITIM8/ITIM16 */
#define ITIM1_ADDR_BASE 0xFFF700
#define ITIM2_ADDR_BASE 0xFFF710
#define ITIM3_ADDR_BASE 0xFFF720
#define ITIM4_ADDR_BASE 0xFFF708
#define ITIM5_ADDR_BASE 0xFFF718
#define ITIM6_ADDR_BASE 0xFFF728

#define ITCNT1_ADDR     (ITIM1_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE1_ADDR     (ITIM1_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT1_ADDR   (ITIM1_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS1_ADDR     (ITIM1_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */
#define ITCNT2_ADDR     (ITIM2_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE2_ADDR     (ITIM2_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT2_ADDR   (ITIM2_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS2_ADDR     (ITIM2_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */
#define ITCNT3_ADDR     (ITIM3_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE3_ADDR     (ITIM3_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT3_ADDR   (ITIM3_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS3_ADDR     (ITIM3_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */
#define ITCNT4_ADDR     (ITIM4_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE4_ADDR     (ITIM4_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT4_ADDR   (ITIM4_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS4_ADDR     (ITIM4_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */
#define ITCNT5_ADDR     (ITIM5_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE5_ADDR     (ITIM5_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT5_ADDR   (ITIM5_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS5_ADDR     (ITIM5_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */
#define ITCNT6_ADDR     (ITIM6_ADDR_BASE + 0x00)   /* Internal Timer Counter. */
#define ITPRE6_ADDR     (ITIM6_ADDR_BASE + 0x01)   /* Internal Timer Prescaler. */
#define IT16CNT6_ADDR   (ITIM6_ADDR_BASE + 0x02)   /* Internal 16 bits Timer Counter. */
#define ITCTS6_ADDR     (ITIM6_ADDR_BASE + 0x04)   /* Internal Timer Control and Status. */

/* Internal Timer Counter. */
#define ITCNT1      PORT_BYTE   ITCNT1_ADDR
#define ITCNT2      PORT_BYTE   ITCNT2_ADDR
#define ITCNT3      PORT_BYTE   ITCNT3_ADDR
#define ITCNT4      PORT_BYTE   ITCNT4_ADDR
#define ITCNT5      PORT_BYTE   ITCNT5_ADDR
#define ITCNT6      PORT_BYTE   ITCNT6_ADDR
/* Internal Timer Prescaler. */
#define ITPRE1      PORT_BYTE   ITPRE1_ADDR
#define ITPRE2      PORT_BYTE   ITPRE2_ADDR
#define ITPRE3      PORT_BYTE   ITPRE3_ADDR
#define ITPRE4      PORT_BYTE   ITPRE4_ADDR
#define ITPRE5      PORT_BYTE   ITPRE5_ADDR
#define ITPRE6      PORT_BYTE   ITPRE6_ADDR
/* Internal Timer Counter. */
#define IT16CNT1    PORT_WORD   IT16CNT1_ADDR
#define IT16CNT2    PORT_WORD   IT16CNT2_ADDR
#define IT16CNT3    PORT_WORD   IT16CNT3_ADDR
#define IT16CNT4    PORT_WORD   IT16CNT4_ADDR
#define IT16CNT5    PORT_WORD   IT16CNT5_ADDR
#define IT16CNT6    PORT_WORD   IT16CNT6_ADDR
/* Internal Timer Control and Status. */
#define ITCTS1      PORT_BYTE   ITCTS1_ADDR
#define ITCTS2      PORT_BYTE   ITCTS2_ADDR
#define ITCTS3      PORT_BYTE   ITCTS3_ADDR
#define ITCTS4      PORT_BYTE   ITCTS4_ADDR
#define ITCTS5      PORT_BYTE   ITCTS5_ADDR
#define ITCTS6      PORT_BYTE   ITCTS6_ADDR
/* --------------------------------------- */
#define TO_STS  (1 << 0)
#define TO_IE   (1 << 2)
#define TO_WUE  (1 << 3)
#define CKSEL   (1 << 4)
#define ITEN    (1 << 7)

/* PWM Core Registers */
#define PWM_ADDR_BASE 0xFFF440

#define PWM_MODULE_OFFSET 0x10

#define PRSC_ADDR       (PWM_ADDR_BASE + 0x00)    /* Clock Prescaler. */
#define CTR_ADDR        (PWM_ADDR_BASE + 0x02)    /* Cycle Time. */
#define CTR_RS_ADDR     (PWM_ADDR_BASE + 0x02)    /* Cycle Time for Rise Duty Cycle. */
#define PWMCTL_ADDR     (PWM_ADDR_BASE + 0x04)    /* PWM Control. */
#define PWMCTL2_ADDR    (PWM_ADDR_BASE + 0x05)    /* PWM Control 2. */
#define N_STEP_RS_ADDR  (PWM_ADDR_BASE + 0x05)    /* Number of Steps for Rise Duty Cycle. */
#define DCR_ADDR        (PWM_ADDR_BASE + 0x06)    /* Duty Cycle. */
#define MAX_DC_RS_ADDR  (PWM_ADDR_BASE + 0x06)    /* Maximum Duty-Cycle Rise. */
#define SEQL_ADDR       (PWM_ADDR_BASE + 0x08)    /* Custom Pattern Sequence Low. */
#define CTR_FL_ADDR     (PWM_ADDR_BASE + 0x08)    /* Cycle Time for Fall Duty Cycle. */
#define SEQH_ADDR       (PWM_ADDR_BASE + 0x0A)    /* Custom Pattern Sequence High. */
#define MAX_DC_FL_ADDR  (PWM_ADDR_BASE + 0x0A)    /* Maximum Duty-Cycle Fall. */
#define PWMCTLEX_ADDR   (PWM_ADDR_BASE + 0x0C)    /* PWM Extended Control. */
#define N_STEP_FL_ADDR  (PWM_ADDR_BASE + 0x0D)    /* Number of Steps for Fall Duty Cycle. */
#define EXT_ON_ADDR     (PWM_ADDR_BASE + 0x0E)    /* Extend ON Time. */
#define EXT_OFF_ADDR    (PWM_ADDR_BASE + 0x0F)    /* Extend OFF Time. */


#define PWM_PRSC        PRSC_ADDR
#define PWM_CTR         CTR_ADDR
#define PWM_CTR_RS      CTR_RS_ADDR
#define PWM_PWMCTL      PWMCTL_ADDR
#define PWM_PWMCTL2     PWMCTL2_ADDR
#define PWM_N_STEP_RS   N_STEP_RS_ADDR
#define PWM_DCR         DCR_ADDR
#define PWM_MAX_DC_RS   MAX_DC_RS_ADDR
#define PWM_SEQL        SEQL_ADDR
#define PWM_CTR_FL      CTR_FL_ADDR
#define PWM_SEQH        SEQH_ADDR
#define PWM_MAX_DC_FL   MAX_DC_FL_ADDR
#define PWM_PWMCTLEX    PWMCTLEX_ADDR
#define PWM_N_STEP_FL   N_STEP_FL_ADDR
#define PWM_EXT_ON      EXT_ON_ADDR
#define PWM_EXT_OFF     EXT_OFF_ADDR



/* Clock Prescaler. */
#define PRSC_A         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 0)
#define PRSC_B         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 1)
#define PRSC_C         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 2)
#define PRSC_D         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 3)
#define PRSC_E         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 4)
#define PRSC_F         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 5)
#define PRSC_G         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 6)
#define PRSC_H         PORT_WORD    (PRSC_ADDR + PWM_MODULE_OFFSET * 7)

/* Cycle Time. When HBNK_SEL = 0. */
#define CTR_A          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 0)
#define CTR_B          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 1)
#define CTR_C          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 2)
#define CTR_D          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 3)
#define CTR_E          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 4)
#define CTR_F          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 5)
#define CTR_G          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 6)
#define CTR_H          PORT_WORD    (CTR_ADDR + PWM_MODULE_OFFSET * 7)

/* Cycle Time for Rise Duty Cycle. When HBNK_SEL = 1. */
#define CTR_RS_A        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 0)
#define CTR_RS_B        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 1)
#define CTR_RS_C        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 2)
#define CTR_RS_D        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 3)
#define CTR_RS_E        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 4)
#define CTR_RS_F        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 5)
#define CTR_RS_G        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 6)
#define CTR_RS_H        PORT_WORD    (CTR_RS_ADDR + PWM_MODULE_OFFSET * 7)

/* PWM Control. */
#define PWMCTL_A       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 0)
#define PWMCTL_B       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 1)
#define PWMCTL_C       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 2)
#define PWMCTL_D       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 3)
#define PWMCTL_E       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 4)
#define PWMCTL_F       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 5)
#define PWMCTL_G       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 6)
#define PWMCTL_H       PORT_BYTE    (PWMCTL_ADDR + PWM_MODULE_OFFSET * 7)
/* ------------------------------------------- */
#define PWMCTL_PWR          (1 << 7)    /* Power Mode */
#define PWMCTL_HBNK_SEL     (1 << 6)    /* Heartbeat Bank Select */
#define PWMCTL_SYNC         (1 << 4)    /* Operation Synchronization */
#define mskHB_DC_CTL        (3 << 2)    /* Heartbeat Duty-Cycle Control */
#define PWM_Mode            (0 << 2)    /* Normal PWM operation mode */
#define VDC_Mode            (1 << 2)    /* Heartbeat PWM operation mode with Visual duty-cycle control */
#define SDC_Mode            (2 << 2)    /* Heartbeat PWM operation mode with Standard duty-cycle control */
#define PWMCTL_CKSEL        (1 << 1)    /* Input Clock Select */
#define PWMCTL_INVP         (1 << 0)    /* Inverse PWM Output */

/* PWM Control 2. When HBNK_SEL = 0.*/
#define PWMCTL2_A      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 0)
#define PWMCTL2_B      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 1)
#define PWMCTL2_C      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 2)
#define PWMCTL2_D      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 3)
#define PWMCTL2_E      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 4)
#define PWMCTL2_F      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 5)
#define PWMCTL2_G      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 6)
#define PWMCTL2_H      PORT_BYTE    (PWMCTL2_ADDR + PWM_MODULE_OFFSET * 7)
/* ------------------------------------------- */


/* Number of Steps for Rise Duty Cycle. When HBNK_SEL = 1.*/
#define N_STEP_RS_A      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 0)
#define N_STEP_RS_B      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 1)
#define N_STEP_RS_C      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 2)
#define N_STEP_RS_D      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 3)
#define N_STEP_RS_E      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 4)
#define N_STEP_RS_F      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 5)
#define N_STEP_RS_G      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 6)
#define N_STEP_RS_H      PORT_BYTE    (N_STEP_RS_ADDR + PWM_MODULE_OFFSET * 7)
/* ------------------------------------------- */


/* Duty Cycle. When HBNK_SEL = 0. */
#define DCR_A          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 0)
#define DCR_B          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 1)
#define DCR_C          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 2)
#define DCR_D          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 3)
#define DCR_E          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 4)
#define DCR_F          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 5)
#define DCR_G          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 6)
#define DCR_H          PORT_WORD    (DCR_ADDR + PWM_MODULE_OFFSET * 7)

/* Maximum Duty-Cycle Rise. When HBNK_SEL = 1 */
#define MAX_DC_RS_A    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 0)
#define MAX_DC_RS_B    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 1)
#define MAX_DC_RS_C    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 2)
#define MAX_DC_RS_D    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 3)
#define MAX_DC_RS_E    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 4)
#define MAX_DC_RS_F    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 5)
#define MAX_DC_RS_G    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 6)
#define MAX_DC_RS_H    PORT_WORD    (MAX_DC_RS_ADDR + PWM_MODULE_OFFSET * 7)


/* Custom Pattern Sequence Low. When HBNK_SEL = 0. */
#define SEQL_A         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 0)
#define SEQL_B         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 1)
#define SEQL_C         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 2)
#define SEQL_D         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 3)
#define SEQL_E         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 4)
#define SEQL_F         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 5)
#define SEQL_G         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 6)
#define SEQL_H         PORT_WORD    (SEQL_ADDR + PWM_MODULE_OFFSET * 7)

/* Cycle Time for Fall Duty Cycle. When HBNK_SEL = 1. */
#define CTR_FL_A       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 0)
#define CTR_FL_B       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 1)
#define CTR_FL_C       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 2)
#define CTR_FL_D       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 3)
#define CTR_FL_E       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 4)
#define CTR_FL_F       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 5)
#define CTR_FL_G       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 6)
#define CTR_FL_H       PORT_WORD    (CTR_FL_ADDR + PWM_MODULE_OFFSET * 7)

/* Custom Pattern Sequence High. When HBNK_SEL = 0. */
#define SEQH_A         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 0)
#define SEQH_B         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 1)
#define SEQH_C         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 2)
#define SEQH_D         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 3)
#define SEQH_E         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 4)
#define SEQH_F         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 5)
#define SEQH_G         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 6)
#define SEQH_H         PORT_WORD    (SEQH_ADDR + PWM_MODULE_OFFSET * 7)

/* Maximum Duty-Cycle Fall. When HBNK_SEL = 1. */
#define MAX_DC_FL_A    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 0)
#define MAX_DC_FL_B    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 1)
#define MAX_DC_FL_C    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 2)
#define MAX_DC_FL_D    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 3)
#define MAX_DC_FL_E    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 4)
#define MAX_DC_FL_F    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 5)
#define MAX_DC_FL_G    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 6)
#define MAX_DC_FL_H    PORT_WORD    (MAX_DC_FL_ADDR + PWM_MODULE_OFFSET * 7)

/* PWM Extended Control. */
#define PWMCTLEX_A     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 0)
#define PWMCTLEX_B     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 1)
#define PWMCTLEX_C     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 2)
#define PWMCTLEX_D     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 3)
#define PWMCTLEX_E     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 4)
#define PWMCTLEX_F     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 5)
#define PWMCTLEX_G     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 6)
#define PWMCTLEX_H     PORT_BYTE    (PWMCTLEX_ADDR + PWM_MODULE_OFFSET * 7)
/* --------------------------------------------- */
#define OD_OUT          (1 << 7)    /* Open-Drain Output */
#define mskFCK_SEL      (3 << 4)
#define FCK_CORE_32K    (0 << 4)    /* Core clock (CLK) or the 32KHz Clock (LFCLK) */
#define FCK_FXCLK       (1 << 4)    /* Fixed Frequency clock (FXCLK) FMCLK/2 */
#define FCK_FRCLK       (2 << 4)    /* Free-Running clock (FRCLK) */
#define mskONE_SHOT     (3 << 2)
#define ONE_SHOT        (1 << 1)
#define FORCE_EN        (1 << 0)
/* Number of Steps for Fall Duty Cycle. When HBNK_SEL = 1 */
#define N_STEP_FL_A    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 0)
#define N_STEP_FL_B    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 1)
#define N_STEP_FL_C    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 2)
#define N_STEP_FL_D    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 3)
#define N_STEP_FL_E    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 4)
#define N_STEP_FL_F    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 5)
#define N_STEP_FL_G    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 6)
#define N_STEP_FL_H    PORT_BYTE    (N_STEP_FL_ADDR + PWM_MODULE_OFFSET * 7)
/* Extend ON Time. When HBNK_SEL = 1 */
#define EXT_ON_A       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 0)
#define EXT_ON_B       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 1)
#define EXT_ON_C       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 2)
#define EXT_ON_D       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 3)
#define EXT_ON_E       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 4)
#define EXT_ON_F       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 5)
#define EXT_ON_G       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 6)
#define EXT_ON_H       PORT_BYTE    (EXT_ON_ADDR + PWM_MODULE_OFFSET * 7)
/* Extend OFF Time. When HBNK_SEL = 1 */
#define EXT_OFF_A      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 0)
#define EXT_OFF_B      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 1)
#define EXT_OFF_C      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 2)
#define EXT_OFF_D      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 3)
#define EXT_OFF_E      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 4)
#define EXT_OFF_F      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 5)
#define EXT_OFF_G      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 6)
#define EXT_OFF_H      PORT_BYTE    (EXT_OFF_ADDR + PWM_MODULE_OFFSET * 7)

/* CR_UART Core Registers */
#define CR_UART_BASE 0xFFF680
#define UTBUF_ADDR      (CR_UART_BASE + 0x00)   /* Transmit Data Buffer. */
#define URBUF_ADDR      (CR_UART_BASE + 0x02)   /* Receive Data Buffer. */
#define UICTRL_ADDR     (CR_UART_BASE + 0x04)   /* Interrupt Control. */
#define USTAT_ADDR      (CR_UART_BASE + 0x06)   /* Status. */
#define UFRS_ADDR       (CR_UART_BASE + 0x08)   /* Frame Select. */
#define UMDSL_ADDR      (CR_UART_BASE + 0x0A)   /* Mode Select. */
#define UBAUD_ADDR      (CR_UART_BASE + 0x0C)   /* Baud Rate Divisor. */
#define UPSR_ADDR       (CR_UART_BASE + 0x0E)   /* Baud Rate Prescaler. */

/* Transmit Data Buffer. */
#define UTBUF       PORT_BYTE   UTBUF_ADDR
/* Receive Data Buffer. */
#define URBUF       PORT_BYTE   URBUF_ADDR
/* Interrupt Control. */
#define UICTRL      PORT_BYTE   UICTRL_ADDR
/* --------------------------------------- */
#define UICTRL_TBE		(1 << 0)		/* RO -> 1: Transmit buffer empty.*/
#define UICTRL_RBF		(1 << 1)		/* RO -> 1: Receive buffer full. */
#define UICTRL_ETI		(1 << 5)		/* 1: Enable transmitter interrupt. */
#define UICTRL_ERI		(1 << 6)		/* 1: Enable receiver interrupt. */
#define UICTRL_EEI		(1 << 7)		/* 1: Enable receive error interrupt. */

/* Status. */
#define USTAT       PORT_BYTE   USTAT_ADDR
/* -------------------------------------- */
#define USART_PE        (1 << 0)        /* RO -> 1: Parity error detected. */
#define USART_FE        (1 << 1)        /* RO -> 1: Framing error detected. */
#define USART_DOE       (1 << 2)        /* RO -> 1: Data overrun error detected. */
#define USART_ERR       (1 << 3)        /* RO -> 1: A DOE, FE or PE error has occured. */
#define USART_BKD       (1 << 4)        /* RO -> 1: A break Confition detected on SIN_CR */
#define USART_RB9       (1 << 5)        /* RO -> 1: '1' received in ninth bit position. */
#define USART_XMIP      (1 << 6)        /* RO -> 1: USART is transmitting. */

/* Frame Select. */
#define UFRS        PORT_BYTE   UFRS_ADDR
/* ------------------------------------- */
#define UFRD_CHAR       (3 << 0)        /* Select the number of data bits per fram. */
#define UFRD_STP        (1 << 2)        /* 1: 2 stop bit transmitted. 0: 1 stop bit. */
#define UFRD_XB9        (1 << 3)        /* 1: Transmit 1 as 9th data bit. */
#define UFRD_PSEL       (3 << 4)        /* Parity Select */
#define UFRD_PEN        (1 << 6)        /* 1: Parity enabled. */

/* Mode Select. */
#define UMDSL       PORT_BYTE   UMDSL_ADDR
/* -------------------------------------- */
#define UMDSL_ATN		(1 << 1)		/* 1: Enable attention mode. */
#define UMDSL_BRK		(1 << 2)        /* 1: Transmit Line break (SOUT_CR at low level). */

/* Baud Rate Divisor. */
#define UBAUD       PORT_BYTE   UBAUD_ADDR
/* Baud Rate Prescaler. */
#define UPSR        PORT_BYTE   UPSR_ADDR

/* TWD Core Registers */
#define TWD_ADDR_BASE 0xFFF4C0

#define TWCFG_ADDR      (TWD_ADDR_BASE + 0x00)  /* Timer and Watchdog Configuration Byte. */
#define TWCP_ADDR       (TWD_ADDR_BASE + 0x02)  /* Timer and Watchdog Clock Prescaler. */
#define TWDT0_ADDR      (TWD_ADDR_BASE + 0x04)  /* TWD Timer 0. */
#define T0CSR_ADDR      (TWD_ADDR_BASE + 0x06)  /* TWDT0 Control and Status. */
#define WDCNT_ADDR      (TWD_ADDR_BASE + 0x08)  /* Watchdog Count. */
#define WDSDM_ADDR      (TWD_ADDR_BASE + 0x0A)  /* Watchdog Service Data Match. */
#define TWMT0_ADDR      (TWD_ADDR_BASE + 0x0C)  /* TWD Timer 0 Counter. */
#define TWMWD_ADDR      (TWD_ADDR_BASE + 0x0E)  /* Watchdog Counter. */
#define WDCP_ADDR       (TWD_ADDR_BASE + 0x10)  /* Watchdog Clock Prescaler. */

/* TWCFG register bit fields */
#define WDRST_MODE_P    0x6
#define WDRST_MODE_S    0x2

/* Timer and Watchdog Configuration Byte. */
#define TWCFG       PORT_BYTE   TWCFG_ADDR
/* -------------------------------------- */
#define TWCFG_WDST_MODE (1 << 6)    /* Watchdog Reset Mode Select */
#define TWCFG_WDSDME    (1 << 5)    /* Watchdog Touch Select */
#define TWCFG_WDCT0I    (1 << 4)    /* Watchdog Clock Select */
#define TWCFG_LWDCNT    (1 << 3)    /* Lock Watchdog Counter */
#define TWCFG_LTWDT0    (1 << 2)    /* Lock T0 Timer */
#define TWCFG_LTECP     (1 << 1)    /* (Lock Prescalers */
#define TWCFG_LTWCFG    (1 << 0)    /* Lock Watchdog Configuration */

/* Timer and Watchdog Clock Prescaler. */
#define TWCP        PORT_BYTE   TWCP_ADDR
/* -------------------------------------- */
#define TWCP_MDIV      (0x0F << 0)

/* TWD Timer 0. */
#define TWDT0       PORT_WORD   TWDT0_ADDR
/* TWDT0 Control and Status. */
#define T0CSR       PORT_BYTE   T0CSR_ADDR
/* -------------------------------------- */
#define T0CSR_RST       (1 << 0)   /* Reset. */
#define T0CSR_TC        (1 << 1)   /* Terminal Count. */
#define T0WDLTD         (1 << 3)   /* Watchdog Last Touch Delay. */
#define WDRST_STS       (1 << 4)   /* Watchdog Reset Status. */
#define WD_RUN          (1 << 5)   /* Watchdog Run Status. */
#define TESDIS          (1 << 6)   /* Too Early Service Disable. */

/* Watchdog Count. */
#define WDCNT       PORT_BYTE   WDCNT_ADDR
/* Watchdog Service Data Match. */
#define WDSDM       PORT_BYTE   WDSDM_ADDR
/* TWD Timer 0 Counter */
#define TWMT0       PORT_BYTE   TWMT0_ADDR
/* Watchdog Counter Register */
#define TWMWD       PORT_BYTE   TWMWD_ADDR
/* ------------------------------------- */
#define TWMWD_WDIV   (0x0F >> 0)

/* Watchdog Clock Prescaler. */
#define WDCP        PORT_BYTE   WDCP_ADDR

/* LCT Core Registers */
#define LCT_ADDR_BASE 0xFFF9C0
#define LCTCONT_ADDR    (LCT_ADDR_BASE + 0x02)  /* LCT Control. */
#define LCTSTAT_ADDR    (LCT_ADDR_BASE + 0x04)  /* LCT Status. */
#define LCTMINUT_ADDR   (LCT_ADDR_BASE + 0x06)  /* LCT Minutes. */
#define LCTHOUR_ADDR    (LCT_ADDR_BASE + 0x08)  /* LCT Hours. */
#define LCTDAY_ADDR     (LCT_ADDR_BASE + 0x0A)  /* LCT Days. */
#define LCTWEEK_ADDR    (LCT_ADDR_BASE + 0x0C)  /* LCT Weeks. */

/* LCT Control. */
#define LCTCONT    PORT_BYTE     LCTCONT_ADDR
/* -------------------------------------- */
#define LCTEN           (1 << 0)    /* LCT Enable */
#define LCTEVEN         (1 << 1)    /* LCT Event Enable */
#define LCT_PSL_EN      (1 << 2)    /* LCT PSL Event Enable */
#define LCT_VSBY_PWR    (1 << 7)    /* LCT, VSBY Power Select */

/* LCT Status. */
#define LCTSTAT    PORT_BYTE     LCTSTAT_ADDR
/* -------------------------------------- */
#define LCTEVST         (1 << 0)    /* LCT Event Status */

/* LCT Minutes. */
#define LCTMINUT   PORT_BYTE     LCTMINUT_ADDR
/* LCT Hours. */
#define LCTHOUR    PORT_BYTE     LCTHOUR_ADDR
/* LCT Days. */
#define LCTDAY     PORT_BYTE     LCTDAY_ADDR
/* LCT Weeks. */
#define LCTWEEK    PORT_BYTE     LCTWEEK_ADDR

/* ADC Core Registers */
#define ADC_ADDR_BASE 0xFFF800
#define ADCSTS_ADDR     (ADC_ADDR_BASE + 0x00)  /* ADC Status. */
#define ADCCNF_ADDR     (ADC_ADDR_BASE + 0x02)  /* ADC Configuration. */
#define ATCTL_ADDR      (ADC_ADDR_BASE + 0x04)  /* ADC Timing Control. */
#define ASCADD_ADDR     (ADC_ADDR_BASE + 0x06)  /* ADC Single Channel Address. */
#define ADCCS_ADDR      (ADC_ADDR_BASE + 0x08)  /* ADC Scan Channels Select. */
#define THRCTL1_ADDR    (ADC_ADDR_BASE + 0x14)  /* Threshold Control 1. */
#define THRCTL2_ADDR    (ADC_ADDR_BASE + 0x16)  /* Threshold Control 2. */
#define THRCTL3_ADDR    (ADC_ADDR_BASE + 0x18)  /* Threshold Control 3. */
#define THRCTS_ADDR     (ADC_ADDR_BASE + 0x1A)  /* Threshold Status. */
#define THR_DCTL1_ADDR  (ADC_ADDR_BASE + 0x3A)  /* Deassertion Threshold Control 1 */
#define THR_DCTL2_ADDR  (ADC_ADDR_BASE + 0x3C)  /* Deassertion Threshold Control 2 */
#define THR_DCTL3_ADDR  (ADC_ADDR_BASE + 0x3E)  /* Deassertion Threshold Control 3 */
#define CHN0DAT_ADDR    (ADC_ADDR_BASE + 0x40)  /* Channel 0 Data Buffer. */
#define CHN1DAT_ADDR    (ADC_ADDR_BASE + 0x42)  /* Channel 1 Data Buffer. */
#define CHN2DAT_ADDR    (ADC_ADDR_BASE + 0x44)  /* Channel 2 Data Buffer. */
#define CHN3DAT_ADDR    (ADC_ADDR_BASE + 0x46)  /* Channel 3 Data Buffer. */
#define CHN4DAT_ADDR    (ADC_ADDR_BASE + 0x48)  /* Channel 4 Data Buffer. */
#define CHN5DAT_ADDR    (ADC_ADDR_BASE + 0x4A)  /* Channel 5 Data Buffer. */
#define CHN6DAT_ADDR    (ADC_ADDR_BASE + 0x4C)  /* Channel 6 Data Buffer. */
#define CHN7DAT_ADDR    (ADC_ADDR_BASE + 0x4E)  /* Channel 7 Data Buffer. */
#define CHN12DAT_ADDR   (ADC_ADDR_BASE + 0x58)  /* Channel 12 Data Buffer. */
#define CHN15DAT_ADDR   (ADC_ADDR_BASE + 0x5E)  /* Channel 15 Data Buffer. */

/* ADC Status. */
#define ADCSTS      PORT_WORD   ADCSTS_ADDR
/* ------------------------------------- */
#define EOCEV 	    (1 << 0)	    /* End of Conversion Event. */
#define EOCCEV      (1 << 1)      /* End of Conversion Cycle Event. */
#define EOTCEV      (1 << 2)      /* End of Timer Triggered Conversion Event. */
#define ADC_OVFEV   (1 << 3)      /* Data Overflow Event. */

/* ADC Configuration. */
#define ADCCNF      PORT_WORD   ADCCNF_ADDR
/* ---------------------------------------- */
#define ADCEN       (1 << 0)      /* ADC Module Enable. */
#define ADCMD       (3 << 1)      /* ADC Operation Mode. */
#define ADCRPTC     (1 << 3)      /* ADC Repetitive Conversion. */
#define ADCSTART    (1 << 4)      /* ADC Start Conversion. (R/W 1S) */
#define ADCTTE      (1 << 5)      /* ADC Timer Triggered Conversion Enable. */
#define INTECEN     (1 << 6)      /* Interrupt from End-of-Conversion Event Enable. */
#define INTECCEN    (1 << 7)      /* Interrupt from End-of-Cyclic Conversion Event Enable. */
#define INTETCEN    (1 << 8)      /* Interrupt from End-of-Timer Triggered Conversion Event Enable. */
#define INTOVFEN    (1 << 9)      /* Interrupt from Overflow Event Enable. */
#define STOP        (1 << 11)     /* ADC STOP Conversion */

/* ADC Timing Control. */
#define ATCTL       PORT_WORD   ATCTL_ADDR
/* ---------------------------------------- */
#define SCLKDIV_S   6       /* 6 bits for Core Clock Division Factor. */
#define SCLKDIV_P   0       /* Position of Core Clock Division Factor. */

/* ADC Single Channel Address. */
#define ASCADD      PORT_WORD   ASCADD_ADDR
/* ---------------------------------------- */
#define SADDR   (0x1F << 0)     /* Software Trigger Address. */
#define TTADDR  (0x1F << 8)     /* Timer Trigger Address. */

/* ADC Scan Channels Select. */
#define ADCCS       PORT_WORD   ADCCS_ADDR
/* ---------------------------------------- */
#define ADC_CC0      (1 << 0)        /* Convert Channel 0 */
#define ADC_CC1      (1 << 1)        /* Convert Channel 1 */
#define ADC_CC2      (1 << 2)        /* Convert Channel 2  */
#define ADC_CC3      (1 << 3)        /* Convert Channel 3  */
#define ADC_CC4      (1 << 4)        /* Convert Channel 4  */
#define ADC_CC5      (1 << 5)        /* Convert Channel 5  */
#define ADC_CC6      (1 << 6)        /* Convert Channel 6  */
#define ADC_CC7      (1 << 7)        /* Convert Channel 7  */
#define ADC_CC12     (1 << 12)       /* Convert Channel 12 */
#define ADC_CC15     (1 << 15)       /* Convert Channel 15 */

/* Threshold Control 1. */
#define THRCTL1     PORT_WORD   THRCTL1_ADDR
/* Threshold Control 2. */
#define THRCTL2     PORT_WORD   THRCTL2_ADDR
/* Threshold Control 3. */
#define THRCTL3     PORT_WORD   THRCTL3_ADDR
/* ---------------------------------------- */
#define THEN        (1 << 15)       /* Threshold Enable */
#define L_H         (1 << 14)       /* Lower or Higher */
#define CHNSEL      (0x0F << 10)    /* Channel Select */
#define THRVAL      (0xFF << 2)     /* Threshold Value */

/* Threshold Status. */
#define THRCTS      PORT_WORD   THRCTS_ADDR
/* ---------------------------------------- */
#define ADC_WKEN    (1 << 15)   /* ADC Wake-Up Enable. */
#define THR3_IEN    (1 << 10)   /* Threshold 3 Interrupt Enable. */
#define THR2_IEN    (1 << 9)    /* Threshold 2 Interrupt Enable. */
#define THR1_IEN    (1 << 8)    /* Threshold 1 Interrupt Enable) */
#define ADC_EVENT   (1 << 7)    /* ADC Event Status. */
#define THR3_STS    (1 << 2)    /* Threshold 3 Status. */
#define THR2_STS    (1 << 1)    /* Threshold 2 Status. */
#define THR1_STS    (1 << 0)    /* Threshold 1 Status. */

/* Deassertion Threshold Control 1 */
#define THR_DCTL1   PORT_WORD   THR_DCTL1_ADDR
/* Deassertion Threshold Control 2 */
#define THR_DCTL2   PORT_WORD   THR_DCTL2_ADDR
/* Deassertion Threshold Control 3 */
#define THR_DCTL3   PORT_WORD   THR_DCTL3_ADDR
/* ---------------------------------------- */
#define THRD_EN     (1 << 15)       /* Deassertion Threshold Enable. */
#define THR_DVAL    (0xFF << 2)    /* Deassertion Threshold Value. */


/* Channel 0 Data Buffer. */
#define CHN0DAT     PORT_WORD   CHN0DAT_ADDR
/* Channel 1 Data Buffer. */
#define CHN1DAT     PORT_WORD   CHN1DAT_ADDR
/* Channel 2 Data Buffer. */
#define CHN2DAT     PORT_WORD   CHN2DAT_ADDR
/* Channel 3 Data Buffer. */
#define CHN3DAT     PORT_WORD   CHN3DAT_ADDR
/* Channel 4 Data Buffer. */
#define CHN4DAT     PORT_WORD   CHN4DAT_ADDR
/* Channel 5 Data Buffer. */
#define CHN5DAT     PORT_WORD   CHN5DAT_ADDR
/* Channel 6 Data Buffer. */
#define CHN6DAT     PORT_WORD   CHN6DAT_ADDR
/* Channel 7 Data Buffer. */
#define CHN7DAT     PORT_WORD   CHN7DAT_ADDR
/* Channel 12 Data Buffer.*/
#define CHN12DAT    PORT_WORD   CHN12DAT_ADDR
/* Channel 15 Data Buffer.*/
#define CHN15DAT    PORT_WORD   CHN15DAT_ADDR
#define ADC_NEW     (1 << 15)
#define CHDAT       (0xFF << 2)

/* Delta-Sigma Analog to Digital Converter (DSADC) */
#define DSADC_ADDR_BASE 0xFFFD00
#define DSADCCTRL0_ADDR   (DSADC_ADDR_BASE + 0x02)  /* DSADC Control Register 0. */
#define ADCTM_ADDR        (DSADC_ADDR_BASE + 0x0E)  /* Operation Mode Select. */
#define ADCTD10PO_ADDR    (DSADC_ADDR_BASE + 0x12)  /* Offset setting for td0 and td1. */
#define ADCTD32PO_ADDR    (DSADC_ADDR_BASE + 0x14)  /* Offset setting for td2 and td3. */
#define ADCTD54PO_ADDR    (DSADC_ADDR_BASE + 0x16)  /* Offset setting for td4 and td5. */
#define ADCACTRL2_ADDR    (DSADC_ADDR_BASE + 0x22)  /* DSADC Analog Power Down Control. */
#define DSADCCFG_ADDR     (DSADC_ADDR_BASE + 0x78)  /* DSADC Configuration */
#define DSADCCS_ADDR      (DSADC_ADDR_BASE + 0x7A)  /* DSADC Channel select */
#define DSADCSTS_ADDR     (DSADC_ADDR_BASE + 0x7C)  /* DSADC global status */
#define TCHN0DAT_ADDR     (DSADC_ADDR_BASE + 0x80)  /* Temperature Channel0 Data */
#define TCHN1DAT_ADDR     (DSADC_ADDR_BASE + 0x82)  /* Temperature Channel1 Data */
#define TCHN2DAT_ADDR     (DSADC_ADDR_BASE + 0x84)  /* Temperature Channel2 Data */
#define TCHN3DAT_ADDR     (DSADC_ADDR_BASE + 0x86)  /* Temperature Channel3 Data */
#define TCHN4DAT_ADDR     (DSADC_ADDR_BASE + 0x88)  /* Temperature Channel4 Data */
#define TCHN5DAT_ADDR     (DSADC_ADDR_BASE + 0x8A)  /* Temperature Channel5 Data */
#define DS_THRCTL0_ADDR   (DSADC_ADDR_BASE + 0x90)  /* DSADC Channel0 Threshold conrol. */
#define DS_THRCTL1_ADDR   (DSADC_ADDR_BASE + 0x92)  /* DSADC Channel1 Threshold conrol. */
#define DS_THRCTL2_ADDR   (DSADC_ADDR_BASE + 0x94)  /* DSADC Channel2 Threshold conrol. */
#define DS_THRCTL3_ADDR   (DSADC_ADDR_BASE + 0x96)  /* DSADC Channel3 Threshold conrol. */
#define DS_THRCTL4_ADDR   (DSADC_ADDR_BASE + 0x98)  /* DSADC Channel4 Threshold conrol. */
#define DS_THRCTL5_ADDR   (DSADC_ADDR_BASE + 0x9A)  /* DSADC Channel5 Threshold conrol. */
#define DS_THR_DCTL0_ADDR (DSADC_ADDR_BASE + 0x9E)  /* ADC Deassertion Threshold control 0. */
#define DS_THR_DCTL1_ADDR (DSADC_ADDR_BASE + 0xA0)  /* ADC Deassertion Threshold control 1. */
#define DS_THR_DCTL2_ADDR (DSADC_ADDR_BASE + 0xA2)  /* ADC Deassertion Threshold control 2. */
#define DS_THR_DCTL3_ADDR (DSADC_ADDR_BASE + 0xA4)  /* ADC Deassertion Threshold control 3. */
#define DS_THR_DCTL4_ADDR (DSADC_ADDR_BASE + 0xA6)  /* ADC Deassertion Threshold control 4. */
#define DS_THR_DCTL5_ADDR (DSADC_ADDR_BASE + 0xA8)  /* ADC Deassertion Threshold control 5. */

/* DSADC Control */
#define DSADCCTRL0   PORT_WORD   DSADCCTRL0_ADDR
/* ADC Temperature Mode */
#define ADCTM        PORT_WORD   ADCTM_ADDR
/* --------------------------------------- */
#define T_MODE_0     (3 << 0)     /* DSADC Channel0 operation mode select. */
#define T_MODE_1     (3 << 2)     /* DSADC Channel1 operation mode select. */
#define T_MODE_2     (3 << 4)     /* DSADC Channel2 operation mode select. */
#define T_MODE_3     (3 << 8)     /* DSADC Channel3 operation mode select. */
#define T_MODE_4     (3 << 10)    /* DSADC Channel4 operation mode select. */
#define T_MODE_5     (3 << 12)    /* DSADC Channel5 operation mode select. */

/* ADC TD1P-TD0P Post Offset */
#define ADCTD10PO    PORT_WORD   ADCTD10PO_ADDR
#define ADCTD32PO    PORT_WORD   ADCTD32PO_ADDR
#define ADCTD54PO    PORT_WORD   ADCTD54PO_ADDR

/* ADC Analog Control 2 */
#define ADCACTRL2    PORT_BYTE   ADCACTRL2_ADDR
/* ---------------------------------------- */
#define PD_RG          (1 << 1)   /* Power down analog regulator circuit. */
#define PD_ISEN        (1 << 2)   /* Power down analog current sense circuit. */
#define PD_IREF        (1 << 3)   /* Power down analog bias current circuit. */
#define PD_DVBE        (1 << 4)   /* Power down analog delta Vbe circuit. */
#define PD_DSM         (1 << 5)   /* Power down analog delta-sigma circuit. */
#define PD_BG          (1 << 6)   /* Power down analog band gap circuit. */
#define PD_ANA         (1 << 7)   /* Power down whole analog circuit. */

/* ADC Configuration */
#define DSADCCFG    PORT_WORD   DSADCCFG_ADDR
/* ---------------------------------------- */
#define DSADCEN        (1 << 0)   /* DSADC Module Enable. */
#define DSADCCMD       (3 << 1)   /* DSADC Conversion Mode. */
#define DSADCPRTC      (1 << 3)   /* DSADC Repetitive Mode. */
#define DSADC_START    (1 << 4)   /* DSADC Start Conversion. */
#define DSADCICEN      (1 << 5)   /* DSADC Interrupt from End of Conversion Event Enable. */
#define DSADCICCEN     (1 << 6)   /* DSADC Interrupt from End of Cyclic-Conversion Event Enable. */
#define DSADCTHR_IEN   (1 << 7)   /* Threshold Interrupt Enable. */
#define DSADC_OVT_EN   (1 << 8)   /* OVT function enable setting. */
#define DSADC_OVT_MODE (1 << 9)   /* OVT function mode setting. */

/* ADC Scan Channels Select */
#define DSADCCS     PORT_BYTE   DSADCCS_ADDR
/* ---------------------------------------- */
#define DSADC_CC0    (1 << 0)
#define DSADC_CC1    (1 << 1)
#define DSADC_CC2    (1 << 2)
#define DSADC_CC3    (1 << 3)
#define DSADC_CC4    (1 << 4)
#define DSADC_CC5    (1 << 5)

/* ADC Status */
#define DSADCSTS    PORT_WORD   DSADCSTS_ADDR
/* ---------------------------------------- */
#define EOCEV          (1 << 0)    /* End of Conversion Event */
#define EOCCEV         (1 << 1)    /* End of Conversion Cycle Event */
#define DS_OVFEV       (1 << 2)    /* Data Overflow Event */
#define THR_STS0       (1 << 3)    /* Threshold detector0 Status */
#define THR_STS1       (1 << 4)    /* Threshold detector1 Status */
#define THR_STS2       (1 << 5)    /* Threshold detector2 Status */
#define THR_STS3       (1 << 6)    /* Threshold detector3 Status */
#define THR_STS4       (1 << 7)    /* Threshold detector4 Status */
#define THR_STS5       (1 << 8)    /* Threshold detector5 Status */

/* ADC Channel Data Buffer */
#define TCHN0DAT    PORT_WORD   TCHN0DAT_ADDR
#define TCHN1DAT    PORT_WORD   TCHN1DAT_ADDR
#define TCHN2DAT    PORT_WORD   TCHN2DAT_ADDR
#define TCHN3DAT    PORT_WORD   TCHN3DAT_ADDR
#define TCHN4DAT    PORT_WORD   TCHN4DAT_ADDR
#define TCHN5DAT    PORT_WORD   TCHN5DAT_ADDR
/* ADC Threshold Control */
#define DS_THRCTL0  PORT_WORD   DS_THRCTL0_ADDR
#define DS_THRCTL1  PORT_WORD   DS_THRCTL1_ADDR
#define DS_THRCTL2  PORT_WORD   DS_THRCTL2_ADDR
#define DS_THRCTL3  PORT_WORD   DS_THRCTL3_ADDR
#define DS_THRCTL4  PORT_WORD   DS_THRCTL4_ADDR
#define DS_THRCTL5  PORT_WORD   DS_THRCTL5_ADDR
/* ADC Deassertion Threshold Control */
#define DS_THR_DCTL0 PORT_WORD  DS_THR_DCTL0_ADD
#define DS_THR_DCTL1 PORT_WORD  DS_THR_DCTL1_ADD
#define DS_THR_DCTL2 PORT_WORD  DS_THR_DCTL2_ADD
#define DS_THR_DCTL3 PORT_WORD  DS_THR_DCTL3_ADD
#define DS_THR_DCTL4 PORT_WORD  DS_THR_DCTL4_ADD
#define DS_THR_DCTL5 PORT_WORD  DS_THR_DCTL5_ADD


/* SMB Core Registers */
#define SMB1_ADDR_BASE 0xFFF500
#define SMB2_ADDR_BASE 0xFFF540
#define SMB3_ADDR_BASE 0xFFF580
#define SMB4_ADDR_BASE 0xFFF5C0
#define SMB5_ADDR_BASE 0xFFF600

/* Addresses of registers for the ACCESS.bus1 interface. */
#define SMB1SDA_ADDR     (SMB1_ADDR_BASE + 0x00)    /* SMB Serial Data. */
#define SMB1ST_ADDR      (SMB1_ADDR_BASE + 0x02)    /* SMB Status. */
#define SMB1CST_ADDR     (SMB1_ADDR_BASE + 0x04)    /* SMB Control Status. */
#define SMB1CTL1_ADDR    (SMB1_ADDR_BASE + 0x06)    /* SMB Control 1. */
#define SMB1ADDR1_ADDR   (SMB1_ADDR_BASE + 0x08)    /* SMB Own Address 1. */
#define SMB1CTL2_ADDR    (SMB1_ADDR_BASE + 0x0A)    /* SMB Control 2. */
#define SMB1ADDR2_ADDR   (SMB1_ADDR_BASE + 0x0C)    /* SMB Own Address 2. */
#define SMB1CTL3_ADDR    (SMB1_ADDR_BASE + 0x0E)    /* SMB Control 3. */
#define SMB1ADDR3_ADDR   (SMB1_ADDR_BASE + 0x10)    /* SMB Own Address 3. */
#define SMB1ADDR7_ADDR   (SMB1_ADDR_BASE + 0x11)    /* SMB Own Address 7. */
#define SMB1ADDR4_ADDR   (SMB1_ADDR_BASE + 0x12)    /* SMB Own Address 4. */
#define SMB1ADDR8_ADDR   (SMB1_ADDR_BASE + 0x13)    /* SMB Own Address 8. */
#define SMB1ADDR5_ADDR   (SMB1_ADDR_BASE + 0x14)    /* SMB Own Address 5. */
#define SMB1ADDR9_ADDR   (SMB1_ADDR_BASE + 0x15)    /* SMB Own Address 9. */
#define SMB1ADDR6_ADDR   (SMB1_ADDR_BASE + 0x16)    /* SMB Own Address 6. */
#define SMB1ADDR10_ADDR  (SMB1_ADDR_BASE + 0x17)    /* SMB Own Address 10. */
#define SMB1CST2_ADDR    (SMB1_ADDR_BASE + 0x18)    /* SMB Control Status 2. */
#define SMB1CST3_ADDR    (SMB1_ADDR_BASE + 0x19)    /* SMB Control Status 3. */
#define SMB1CTL4_ADDR    (SMB1_ADDR_BASE + 0x1A)    /* SMB Control 4. */
#define SMB1CTL5_ADDR    (SMB1_ADDR_BASE + 0x1B)    /* SMB Control 5. */
#define SMB1SCLLT_ADDR   (SMB1_ADDR_BASE + 0x1C)    /* SMB SCL Low Time. */
#define SMB1SCLHT_ADDR   (SMB1_ADDR_BASE + 0x1E)    /* SMB SCL High Time. */
#define SMB1PEC_ADDR     (SMB1_ADDR_BASE + 0x2B)    /* SMB PEC Data. */

#define TIMEOUT_ST_ADDR  (SMB1_ADDR_BASE + 0x09)    /* Timeout Status. */
#define TIMEOUT_EN_ADDR  (SMB1_ADDR_BASE + 0x09)    /* Timeout Enable. */
#define DMA_CTRL_ADDR    (SMB1_ADDR_BASE + 0x0F)    /* DMA Control. */
#define DMA_ADDR1_ADDR   (SMB1_ADDR_BASE + 0x20)    /* DMA Address Byte 1. */
#define DMA_ADDR2_ADDR   (SMB1_ADDR_BASE + 0x21)    /* DMA Address Byte 2. */
#define DMA_ADDR3_ADDR   (SMB1_ADDR_BASE + 0x22)    /* DMA Address Byte 3. */
#define DMA_ADDR4_ADDR   (SMB1_ADDR_BASE + 0x23)    /* DMA Address Byte 4. */
#define DATA_LEN1_ADDR   (SMB1_ADDR_BASE + 0x24)    /* Data Lenth Byte 1. */
#define DATA_LEN2_ADDR   (SMB1_ADDR_BASE + 0x25)    /* Data Lenth Byte 2. */
#define DATA_CNT1_ADDR   (SMB1_ADDR_BASE + 0x26)    /* Data Counter Byte 1. */
#define DATA_CNT2_ADDR   (SMB1_ADDR_BASE + 0x27)    /* Data Counter Byte 2. */
#define TIMEOUT_CTL1_ADDR (SMB1_ADDR_BASE + 0x29)   /* Timeout Control 1. */
#define TIMEOUT_CTL2_ADDR (SMB1_ADDR_BASE + 0x2A)   /* Timeout Control 2. */

/* Addresses of registers for the ACCESS.bus2 interface. */
#define SMB2SDA_ADDR     (SMB2_ADDR_BASE + 0x00)    /* SMB Serial Data. */
#define SMB2ST_ADDR      (SMB2_ADDR_BASE + 0x02)    /* SMB Status. */
#define SMB2CST_ADDR     (SMB2_ADDR_BASE + 0x04)    /* SMB Control Status. */
#define SMB2CTL1_ADDR    (SMB2_ADDR_BASE + 0x06)    /* SMB Control 1. */
#define SMB2ADDR1_ADDR   (SMB2_ADDR_BASE + 0x08)    /* SMB Own Address 1. */
#define SMB2CTL2_ADDR    (SMB2_ADDR_BASE + 0x0A)    /* SMB Control 2. */
#define SMB2ADDR2_ADDR   (SMB2_ADDR_BASE + 0x0C)    /* SMB Own Address 2. */
#define SMB2CTL3_ADDR    (SMB2_ADDR_BASE + 0x0E)    /* SMB Control 3. */
#define SMB2T_OUT_ADDR   (SMB2_ADDR_BASE + 0x0F)    /* SMB Bus Timeout. */
#define SMB2ADDR3_ADDR   (SMB2_ADDR_BASE + 0x10)    /* SMB Own Address 3. */
#define SMB2ADDR7_ADDR   (SMB2_ADDR_BASE + 0x11)    /* SMB Own Address 7. */
#define SMB2ADDR4_ADDR   (SMB2_ADDR_BASE + 0x12)    /* SMB Own Address 4. */
#define SMB2ADDR8_ADDR   (SMB2_ADDR_BASE + 0x13)    /* SMB Own Address 8. */
#define SMB2ADDR5_ADDR   (SMB2_ADDR_BASE + 0x14)    /* SMB Own Address 5. */
#define SMB2ADDR9_ADDR   (SMB2_ADDR_BASE + 0x15)    /* SMB Own Address 9. */
#define SMB2ADDR6_ADDR   (SMB2_ADDR_BASE + 0x16)    /* SMB Own Address 6. */
#define SMB2ADDR10_ADDR  (SMB2_ADDR_BASE + 0x17)    /* SMB Own Address 10. */
#define SMB2CST2_ADDR    (SMB2_ADDR_BASE + 0x18)    /* SMB Control Status 2. */
#define SMB2CST3_ADDR    (SMB2_ADDR_BASE + 0x19)    /* SMB Control Status 3. */
#define SMB2CTL4_ADDR    (SMB2_ADDR_BASE + 0x1A)    /* SMB Control 4. */
#define SMB2CTL5_ADDR    (SMB2_ADDR_BASE + 0x1B)    /* SMB Control 5. */
#define SMB2SCLLT_ADDR   (SMB2_ADDR_BASE + 0x1C)    /* SMB SCL Low Time. */
#define SMB2SCLHT_ADDR   (SMB2_ADDR_BASE + 0x1E)    /* SMB SCL High Time. */
#define SMB2PEC_ADDR     (SMB2_ADDR_BASE + 0x2B)    /* SMB PEC Data. */

/* Addresses of registers for the ACCESS.bus3 interface. */
#define SMB3SDA_ADDR     (SMB3_ADDR_BASE + 0x00)    /* SMB Serial Data. */
#define SMB3ST_ADDR      (SMB3_ADDR_BASE + 0x02)    /* SMB Status. */
#define SMB3CST_ADDR     (SMB3_ADDR_BASE + 0x04)    /* SMB Control Status. */
#define SMB3CTL1_ADDR    (SMB3_ADDR_BASE + 0x06)    /* SMB Control 1. */
#define SMB3ADDR1_ADDR   (SMB3_ADDR_BASE + 0x08)    /* SMB Own Address 1. */
#define SMB3CTL2_ADDR    (SMB3_ADDR_BASE + 0x0A)    /* SMB Control 2. */
#define SMB3ADDR2_ADDR   (SMB3_ADDR_BASE + 0x0C)    /* SMB Own Address 2. */
#define SMB3CTL3_ADDR    (SMB3_ADDR_BASE + 0x0E)    /* SMB Control 3. */
#define SMB3T_OUT_ADDR   (SMB3_ADDR_BASE + 0x0F)    /* SMB Bus Timeout. */
#define SMB3ADDR3_ADDR   (SMB3_ADDR_BASE + 0x10)    /* SMB Own Address 3. */
#define SMB3ADDR7_ADDR   (SMB3_ADDR_BASE + 0x11)    /* SMB Own Address 7. */
#define SMB3ADDR4_ADDR   (SMB3_ADDR_BASE + 0x12)    /* SMB Own Address 4. */
#define SMB3ADDR8_ADDR   (SMB3_ADDR_BASE + 0x13)    /* SMB Own Address 8. */
#define SMB3ADDR5_ADDR   (SMB3_ADDR_BASE + 0x14)    /* SMB Own Address 5. */
#define SMB3ADDR9_ADDR   (SMB3_ADDR_BASE + 0x15)    /* SMB Own Address 9. */
#define SMB3ADDR6_ADDR   (SMB3_ADDR_BASE + 0x16)    /* SMB Own Address 6. */
#define SMB3ADDR10_ADDR  (SMB3_ADDR_BASE + 0x17)    /* SMB Own Address 10. */
#define SMB3CST2_ADDR    (SMB3_ADDR_BASE + 0x18)    /* SMB Control Status 2. */
#define SMB3CST3_ADDR    (SMB3_ADDR_BASE + 0x19)    /* SMB Control Status 3. */
#define SMB3CTL4_ADDR    (SMB3_ADDR_BASE + 0x1A)    /* SMB Control 4. */
#define SMB3CTL5_ADDR    (SMB3_ADDR_BASE + 0x1B)    /* SMB Control 5. */
#define SMB3SCLLT_ADDR   (SMB3_ADDR_BASE + 0x1C)    /* SMB SCL Low Time. */
#define SMB3SCLHT_ADDR   (SMB3_ADDR_BASE + 0x1E)    /* SMB SCL High Time. */
#define SMB3PEC_ADDR     (SMB3_ADDR_BASE + 0x2B)    /* SMB PEC Data. */

/* Addresses of registers for the ACCESS.bus4 interface. */
#define SMB4SDA_ADDR     (SMB4_ADDR_BASE + 0x00)    /* SMB Serial Data. */
#define SMB4ST_ADDR      (SMB4_ADDR_BASE + 0x02)    /* SMB Status. */
#define SMB4CST_ADDR     (SMB4_ADDR_BASE + 0x04)    /* SMB Control Status. */
#define SMB4CTL1_ADDR    (SMB4_ADDR_BASE + 0x06)    /* SMB Control 1. */
#define SMB4ADDR1_ADDR   (SMB4_ADDR_BASE + 0x08)    /* SMB Own Address 1. */
#define SMB4CTL2_ADDR    (SMB4_ADDR_BASE + 0x0A)    /* SMB Control 2. */
#define SMB4ADDR2_ADDR   (SMB4_ADDR_BASE + 0x0C)    /* SMB Own Address 2. */
#define SMB4CTL3_ADDR    (SMB4_ADDR_BASE + 0x0E)    /* SMB Control 3. */
#define SMB4T_OUT_ADDR   (SMB4_ADDR_BASE + 0x0F)    /* SMB Bus Timeout. */
#define SMB4ADDR3_ADDR   (SMB4_ADDR_BASE + 0x10)    /* SMB Own Address 3. */
#define SMB4ADDR7_ADDR   (SMB4_ADDR_BASE + 0x11)    /* SMB Own Address 7. */
#define SMB4ADDR4_ADDR   (SMB4_ADDR_BASE + 0x12)    /* SMB Own Address 4. */
#define SMB4ADDR8_ADDR   (SMB4_ADDR_BASE + 0x13)    /* SMB Own Address 8. */
#define SMB4ADDR5_ADDR   (SMB4_ADDR_BASE + 0x14)    /* SMB Own Address 5. */
#define SMB4ADDR9_ADDR   (SMB4_ADDR_BASE + 0x15)    /* SMB Own Address 9. */
#define SMB4ADDR6_ADDR   (SMB4_ADDR_BASE + 0x16)    /* SMB Own Address 6. */
#define SMB4ADDR10_ADDR  (SMB4_ADDR_BASE + 0x17)    /* SMB Own Address 10. */
#define SMB4CST2_ADDR    (SMB4_ADDR_BASE + 0x18)    /* SMB Control Status 2. */
#define SMB4CST3_ADDR    (SMB4_ADDR_BASE + 0x19)    /* SMB Control Status 3. */
#define SMB4CTL4_ADDR    (SMB4_ADDR_BASE + 0x1A)    /* SMB Control 4. */
#define SMB4CTL5_ADDR    (SMB4_ADDR_BASE + 0x1B)    /* SMB Control 5. */
#define SMB4SCLLT_ADDR   (SMB4_ADDR_BASE + 0x1C)    /* SMB SCL Low Time. */
#define SMB4SCLHT_ADDR   (SMB4_ADDR_BASE + 0x1E)    /* SMB SCL High Time. */
#define SMB4PEC_ADDR     (SMB4_ADDR_BASE + 0x2B)    /* SMB PEC Data. */

/* Addresses of registers for the ACCESS.bus4 interface. */
#define SMB5SDA_ADDR     (SMB5_ADDR_BASE + 0x00)    /* SMB Serial Data. */
#define SMB5ST_ADDR      (SMB5_ADDR_BASE + 0x02)    /* SMB Status. */
#define SMB5CST_ADDR     (SMB5_ADDR_BASE + 0x04)    /* SMB Control Status. */
#define SMB5CTL1_ADDR    (SMB5_ADDR_BASE + 0x06)    /* SMB Control 1. */
#define SMB5ADDR1_ADDR   (SMB5_ADDR_BASE + 0x08)    /* SMB Own Address 1. */
#define SMB5CTL2_ADDR    (SMB5_ADDR_BASE + 0x0A)    /* SMB Control 2. */
#define SMB5ADDR2_ADDR   (SMB5_ADDR_BASE + 0x0C)    /* SMB Own Address 2. */
#define SMB5CTL3_ADDR    (SMB5_ADDR_BASE + 0x0E)    /* SMB Control 3. */
#define SMB5T_OUT_ADDR   (SMB5_ADDR_BASE + 0x0F)    /* SMB Bus Timeout. */
#define SMB5ADDR3_ADDR   (SMB5_ADDR_BASE + 0x10)    /* SMB Own Address 3. */
#define SMB5ADDR7_ADDR   (SMB5_ADDR_BASE + 0x11)    /* SMB Own Address 7. */
#define SMB5ADDR4_ADDR   (SMB5_ADDR_BASE + 0x12)    /* SMB Own Address 4. */
#define SMB5ADDR8_ADDR   (SMB5_ADDR_BASE + 0x13)    /* SMB Own Address 8. */
#define SMB5ADDR5_ADDR   (SMB5_ADDR_BASE + 0x14)    /* SMB Own Address 5. */
#define SMB5ADDR9_ADDR   (SMB5_ADDR_BASE + 0x15)    /* SMB Own Address 9. */
#define SMB5ADDR6_ADDR   (SMB5_ADDR_BASE + 0x16)    /* SMB Own Address 6. */
#define SMB5ADDR10_ADDR  (SMB5_ADDR_BASE + 0x17)    /* SMB Own Address 10. */
#define SMB5CST2_ADDR    (SMB5_ADDR_BASE + 0x18)    /* SMB Control Status 2. */
#define SMB5CST3_ADDR    (SMB5_ADDR_BASE + 0x19)    /* SMB Control Status 3. */
#define SMB5CTL4_ADDR    (SMB5_ADDR_BASE + 0x1A)    /* SMB Control 4. */
#define SMB5CTL5_ADDR    (SMB5_ADDR_BASE + 0x1B)    /* SMB Control 5. */
#define SMB5SCLLT_ADDR   (SMB5_ADDR_BASE + 0x1C)    /* SMB SCL Low Time. */
#define SMB5SCLHT_ADDR   (SMB5_ADDR_BASE + 0x1E)    /* SMB SCL High Time. */
#define SMB5PEC_ADDR     (SMB5_ADDR_BASE + 0x2B)    /* SMB PEC Data. */


/* SMB Serial Data. */
#define SMB1SDA     PORT_BYTE   SMB1SDA_ADDR
#define SMB2SDA     PORT_BYTE   SMB2SDA_ADDR
#define SMB3SDA     PORT_BYTE   SMB3SDA_ADDR
#define SMB4SDA     PORT_BYTE   SMB4SDA_ADDR
#define SMB5SDA     PORT_BYTE   SMB5SDA_ADDR
/* SMB Status. */
#define SMB1ST      PORT_BYTE   SMB1ST_ADDR
#define SMB2ST      PORT_BYTE   SMB2ST_ADDR
#define SMB3ST      PORT_BYTE   SMB3ST_ADDR
#define SMB4ST      PORT_BYTE   SMB4ST_ADDR
#define SMB5ST      PORT_BYTE   SMB5ST_ADDR
/* ---------------------------------------- */
#define SMBST_XMIT   (1 << 0)       /* Transmit Mode. */
#define SMBST_MASTER (1 << 1)       /* Master Mode. */
#define SMBST_NMATCH (1 << 2)       /* New Match. */
#define SMBST_STASTR (1 << 3)       /* Stall After Start. */
#define SMBST_NEGACK (1 << 4)       /* Negative Acknowledge. */
#define SMBST_BER    (1 << 5)       /* Bus Error. */
#define SMBST_SDAST  (1 << 6)       /* SDA status. */
#define SMBST_SLVSTP (1 << 7)       /* Slave Stop. */

/* SMB Control Status. */
#define SMB1CST     PORT_BYTE   SMB1CST_ADDR
#define SMB2CST     PORT_BYTE   SMB2CST_ADDR
#define SMB3CST     PORT_BYTE   SMB3CST_ADDR
#define SMB4CST     PORT_BYTE   SMB4CST_ADDR
#define SMB5CST     PORT_BYTE   SMB5CST_ADDR
/* ---------------------------------------- */
#define SMBCST_BUSY     (1 << 0)    /* Busy. */
#define SMBCST_BB       (1 << 1)    /* Bus Busy. */
#define SMBCST_MATCH    (1 << 2)    /* Address Match. */
#define SMBCST_GCMTCH   (1 << 3)    /* Global Call Match. */
#define SMBCST_TSDA     (1 << 4)    /* Test SDA Line. */
#define SMBCST_TGSCL    (1 << 5)    /* Toggle SCL Line. */
#define SMBCST_MATCHAF  (1 << 6)    /* Match Address Field. */
#define SMBCST_ARPMATCH (1 << 7)    /* ARP Address Match. */

/* SMB Control 1. */
#define SMB1CTL1    PORT_BYTE   SMB1CTL1_ADDR
#define SMB2CTL1    PORT_BYTE   SMB2CTL1_ADDR
#define SMB3CTL1    PORT_BYTE   SMB3CTL1_ADDR
#define SMB4CTL1    PORT_BYTE   SMB4CTL1_ADDR
#define SMB5CTL1    PORT_BYTE   SMB5CTL1_ADDR
/* ---------------------------------------- */
#define SMBCTL1_START   (1 << 0)    /* Start. */
#define SMBCTL1_STOP    (1 << 1)    /* Stop. */
#define SMBCTL1_INTEN   (1 << 2)    /* Interrupt. */
#define SMBCTL1_EOBINTE (1 << 3)    /* End of 'Busy' Interrupt Enable. */
#define SMBCTL1_ACK     (1 << 4)    /* Acknowledge. */
#define SMBCTL1_GCMEN   (1 << 5)    /* Global Call Match Enable. */
#define SMBCTL1_NMINTE  (1 << 6)    /* New Match Interrupt Enable. */
#define SMBCTL1_STASTRE (1 << 7)    /* Stall After Start Enable. */


/* SMB Own Address 1. */
#define SMB1ADDR1   PORT_BYTE   SMB1ADDR1_ADDR
#define SMB2ADDR1   PORT_BYTE   SMB2ADDR1_ADDR
#define SMB3ADDR1   PORT_BYTE   SMB3ADDR1_ADDR
#define SMB4ADDR1   PORT_BYTE   SMB4ADDR1_ADDR
#define SMB5ADDR1   PORT_BYTE   SMB5ADDR1_ADDR
/* ---------------------------------------- */
/* Bits 0 through 6 holds the bus address of the chip.
   Bit 7 is the address enable bit. */
#define SMBADDR_SAEN (1 << 7)       /* Slave Address Enable. */

/* SMB Control 2. */
#define SMB1CTL2    PORT_BYTE   SMB1CTL2_ADDR
#define SMB2CTL2    PORT_BYTE   SMB2CTL2_ADDR
#define SMB3CTL2    PORT_BYTE   SMB3CTL2_ADDR
#define SMB4CTL2    PORT_BYTE   SMB4CTL2_ADDR
#define SMB5CTL2    PORT_BYTE   SMB5CTL2_ADDR
/* ---------------------------------------- */
#define SMBCTL2_ENABLE  (1 << 0)
#define SMBCTL2_SCLFRQ  (0x7F << 1)

/* SMB Own Address 2. */
#define SMB1ADDR2   PORT_BYTE   SMB1ADDR2_ADDR
#define SMB2ADDR2   PORT_BYTE   SMB2ADDR2_ADDR
#define SMB3ADDR2   PORT_BYTE   SMB3ADDR2_ADDR
#define SMB4ADDR2   PORT_BYTE   SMB4ADDR2_ADDR
#define SMB5ADDR2   PORT_BYTE   SMB5ADDR2_ADDR
/* ---------------------------------------- */
/* Bits 0 through 6 holds the bus address of the chip.
   Bit 7 is the address enable bit. */
#define SMBADDR2_SAEN (1 << 7)       /* Slave Address Enable. */

/* SMB Control 3. */
#define SMB1CTL3    PORT_BYTE   SMB1CTL3_ADDR
#define SMB2CTL3    PORT_BYTE   SMB2CTL3_ADDR
#define SMB3CTL3    PORT_BYTE   SMB3CTL3_ADDR
#define SMB4CTL3    PORT_BYTE   SMB4CTL3_ADDR
#define SMB5CTL3    PORT_BYTE   SMB5CTL3_ADDR
/* ---------------------------------------- */
#define SMBCTL3_SCLFRQ      (0x03 << 0)
#define SMBCTL3_ARPMEN      (1 << 2)    /* ARP Match Enable. */
#define SMBCTL3_IDL_START   (1 << 3)    /* IDL_START */
#define SMBCTL3_400K_MODE   (1 << 4)    /* This bit should be set for working in 400KHz */
#define SMBCTL3_SDA_LVL     (1 << 6)    /* SDA Current Level. */
#define SMBCTL3_SCL_LVL     (1 << 7)    /* SCL Current Level. */

/* SMB Own Address 3. */
#define SMB1ADDR3   PORT_BYTE   SMB1ADDR3_ADDR
#define SMB2ADDR3   PORT_BYTE   SMB2ADDR3_ADDR
#define SMB3ADDR3   PORT_BYTE   SMB3ADDR3_ADDR
#define SMB4ADDR3   PORT_BYTE   SMB4ADDR3_ADDR
#define SMB5ADDR3   PORT_BYTE   SMB5ADDR3_ADDR
/* SMB Own Address 7. */
#define SMB1ADDR7   PORT_BYTE   SMB1ADDR7_ADDR
#define SMB2ADDR7   PORT_BYTE   SMB2ADDR7_ADDR
#define SMB3ADDR7   PORT_BYTE   SMB3ADDR7_ADDR
#define SMB4ADDR7   PORT_BYTE   SMB4ADDR7_ADDR
#define SMB5ADDR7   PORT_BYTE   SMB5ADDR7_ADDR
/* SMB Own Address 4. */
#define SMB1ADDR4   PORT_BYTE   SMB1ADDR4_ADDR
#define SMB2ADDR4   PORT_BYTE   SMB2ADDR4_ADDR
#define SMB3ADDR4   PORT_BYTE   SMB3ADDR4_ADDR
#define SMB4ADDR4   PORT_BYTE   SMB4ADDR4_ADDR
#define SMB5ADDR4   PORT_BYTE   SMB5ADDR4_ADDR
/* SMB Own Address 8. */
#define SMB1ADDR8   PORT_BYTE   SMB1ADDR8_ADDR
#define SMB2ADDR8   PORT_BYTE   SMB2ADDR8_ADDR
#define SMB3ADDR8   PORT_BYTE   SMB3ADDR8_ADDR
#define SMB4ADDR8   PORT_BYTE   SMB4ADDR8_ADDR
#define SMB5ADDR8   PORT_BYTE   SMB5ADDR8_ADDR
/* SMB Own Address 5. */
#define SMB1ADDR5   PORT_BYTE   SMB1ADDR5_ADDR
#define SMB2ADDR5   PORT_BYTE   SMB2ADDR5_ADDR
#define SMB3ADDR5   PORT_BYTE   SMB3ADDR5_ADDR
#define SMB4ADDR5   PORT_BYTE   SMB4ADDR5_ADDR
#define SMB5ADDR5   PORT_BYTE   SMB5ADDR5_ADDR
/* SMB Own Address 6. */
#define SMB1ADDR6   PORT_BYTE   SMB1ADDR6_ADDR
#define SMB2ADDR6   PORT_BYTE   SMB2ADDR6_ADDR
#define SMB3ADDR6   PORT_BYTE   SMB3ADDR6_ADDR
#define SMB4ADDR6   PORT_BYTE   SMB4ADDR6_ADDR
#define SMB5ADDR6   PORT_BYTE   SMB5ADDR6_ADDR
/* SMB Control Status 2. */
#define SMB1CST2    PORT_BYTE   SMB1CST2_ADDR
#define SMB2CST2    PORT_BYTE   SMB2CST2_ADDR
#define SMB3CST2    PORT_BYTE   SMB3CST2_ADDR
#define SMB4CST2    PORT_BYTE   SMB4CST2_ADDR
#define SMB5CST2    PORT_BYTE   SMB5CST2_ADDR
/* ---------------------------------------- */
#define INTSTS      (1 << 7)    /* Interrupt Status */
#define MATCHA7F    (1 << 6)    /* Match Address 7 Field */
#define MATCHA6F    (1 << 5)    /* Match Address 6 Field */
#define MATCHA5F    (1 << 4)    /* Match Address 5 Field */
#define MATCHA4F    (1 << 3)    /* Match Address 4 Field */
#define MATCHA3F    (1 << 2)    /* Match Address 3 Field */
#define MATCHA2F    (1 << 1)    /* Match Address 2 Field */
#define MATCHA1F    (1 << 0)    /* Match Address 1 Field */

/* SMB Control Status 3. */
#define SMB1CST3    PORT_BYTE   SMB1CST3_ADDR
#define SMB2CST3    PORT_BYTE   SMB2CST3_ADDR
#define SMB3CST3    PORT_BYTE   SMB3CST3_ADDR
#define SMB4CST3    PORT_BYTE   SMB4CST3_ADDR
#define SMB5CST3    PORT_BYTE   SMB5CST3_ADDR
/* ---------------------------------------- */
#define MATCHA8F    (1 << 0)    /* Match Address 8 Field */
#define MATCHA9F    (1 << 1)    /* Match Address 9 Field */
#define MATCHA10F   (1 << 2)    /* Match Address 10 Field */
#define EO_BUSY     (1 << 7)    /* End of ¡§Busy¡¨ */

/* SMB Control 4. */
#define SMB1CTL4    PORT_BYTE   SMB1CTL4_ADDR
#define SMB2CTL4    PORT_BYTE   SMB2CTL4_ADDR
#define SMB3CTL4    PORT_BYTE   SMB3CTL4_ADDR
#define SMB4CTL4    PORT_BYTE   SMB4CTL4_ADDR
#define SMB5CTL4    PORT_BYTE   SMB5CTL4_ADDR
/* SMB Control 5. */
#define SMB1CTL5    PORT_BYTE   SMB1CTL5_ADDR
#define SMB2CTL5    PORT_BYTE   SMB2CTL5_ADDR
#define SMB3CTL5    PORT_BYTE   SMB3CTL5_ADDR
#define SMB4CTL5    PORT_BYTE   SMB4CTL5_ADDR
#define SMB5CTL5    PORT_BYTE   SMB5CTL5_ADDR
/* SMB SCL Low Time. */
#define SMB1SCLLT   PORT_BYTE   SMB1SCLLT_ADDR
#define SMB2SCLLT   PORT_BYTE   SMB2SCLLT_ADDR
#define SMB3SCLLT   PORT_BYTE   SMB3SCLLT_ADDR
#define SMB4SCLLT   PORT_BYTE   SMB4SCLLT_ADDR
#define SMB5SCLLT   PORT_BYTE   SMB5SCLLT_ADDR
/* SMB SCL High Time. */
#define SMB1SCLHT   PORT_BYTE   SMB1SCLHT_ADDR
#define SMB2SCLHT   PORT_BYTE   SMB2SCLHT_ADDR
#define SMB3SCLHT   PORT_BYTE   SMB3SCLHT_ADDR
#define SMB4SCLHT   PORT_BYTE   SMB4SCLHT_ADDR
#define SMB5SCLHT   PORT_BYTE   SMB5SCLHT_ADDR
/* SMB PEC */
#define SMB1PEC     PORT_BYTE   SMB1PEC_ADDR
#define SMB2PEC     PORT_BYTE   SMB2PEC_ADDR
#define SMB3PEC     PORT_BYTE   SMB3PEC_ADDR
#define SMB4PEC     PORT_BYTE   SMB4PEC_ADDR
#define SMB5PEC     PORT_BYTE   SMB5PEC_ADDR

/* Timeout Status */
#define TIMEOUT_ST  PORT_BYTE   TIMEOUT_ST_ADDR
/* ----------------------------------------- */
#define T_OUTST       (1 << 0)

/* Timeout Status */
#define TIMEOUT_EN  PORT_BYTE   TIMEOUT_EN_ADDR
/* ----------------------------------------- */
#define TIMEOUT_EN_FLG    (1 << 0)
#define TO_CKDIV          (0x3F << 2)

/* DMA Control */
#define DMA_CTRL    PORT_BYTE   DMA_CTRL_ADDR
/* ----------------------------------------- */
#define DMA_INT_CLR   (1 << 0)    /* DMA Interrupt Clear. */
#define DMA_EN        (1 << 1)    /* DMA Enable. */
#define LAST_PEC      (1 << 2)    /* Last Byte or PEC Byte. */
#define DMA_STALL     (1 << 3)    /* DMA Stall. */
#define DMA_IRQ       (1 << 7)    /* DMA IRQ status. */

/* DMA Address Byte */
#define DMA_ADDR1     PORT_BYTE   DMA_ADDR1_ADDR
#define DMA_ADDR2     PORT_BYTE   DMA_ADDR2_ADDR
#define DMA_ADDR3     PORT_BYTE   DMA_ADDR3_ADDR
#define DMA_ADDR4     PORT_BYTE   DMA_ADDR4_ADDR
#define DMA_DATA_LEN1 PORT_BYTE   DATA_LEN1_ADDR
#define DMA_DATA_LEN2 PORT_BYTE   DATA_LEN2_ADDR
#define DMA_DATA_CNT1 PORT_BYTE   DATA_CNT1_ADDR
#define DMA_DATA_CNT2 PORT_BYTE   DATA_CNT2_ADDR

/* Timeour Control */
#define TIMEOUT_CTL1  PORT_BYTE   TIMEOUT_CTL1_ADDR
#define TIMEOUT_CTL2  PORT_BYTE   TIMEOUT_CTL2_ADDR


/* PECI Core Registers */
#define PECI_ADDR_BASE       0xFFF900
#define PECI_CTL_STS_ADDR    (PECI_ADDR_BASE + 0x00)        /* PECI Control Status. */
#define PECI_RD_LENGTH_ADDR  (PECI_ADDR_BASE + 0x01)        /* PECI Read Length. */
#define PECI_ADDR_ADDR       (PECI_ADDR_BASE + 0x02)        /* PECI Address. */
#define PECI_CMD_ADDR        (PECI_ADDR_BASE + 0x03)        /* PECI Command. */
#define PECI_CTL2_ADDR       (PECI_ADDR_BASE + 0x04)        /* PECI Control 2. */
#define PECI_INDEX_ADDR      (PECI_ADDR_BASE + 0x05)        /* PECI Index. */
#define PECI_IDATA_ADDR      (PECI_ADDR_BASE + 0x06)        /* PECI Index Data. */
#define PECI_WR_LENGTH_ADDR  (PECI_ADDR_BASE + 0x07)        /* PECI Write Length. */
#define PECI_WR_FCS_ADDR     (PECI_ADDR_BASE + 0x0B)        /* PECI Write FCS. */
#define PECI_RD_FCS_ADDR     (PECI_ADDR_BASE + 0x0C)        /* PECI Read FCS. */
#define PECI_AW_FCS_ADDR     (PECI_ADDR_BASE + 0x0D)        /* PECI Assured Write FCS. */
#define PECI_RATE_ADDR       (PECI_ADDR_BASE + 0x0F)        /* PECI Transfer Rate. */
#define PECI_DATA_IN_ADDR    (PECI_ADDR_BASE + 0x10 + (i))  /* PECI Data IN (16 bytes). */
#define PECI_DATA_OUT_ADDR   (PECI_ADDR_BASE + 0x10 + (i))  /* PECI Data OUT (16 bytes). */

/* PECI Control Status. */
#define PECI_CTL_STS        PORT_BYTE   PECI_CTL_STS_ADDR
/* ----------------------------------------------------- */
#define START_BUSY     (1 << 0)
#define PECI_DONE      (1 << 1)
#define AVL_ERR        (1 << 2)
#define CRC_ERR        (1 << 3)
#define ABRT_ERR       (1 << 4)
#define AWFCS_EN       (1 << 5)
#define DONE_EN        (1 << 6)
#define PECI_TRANS_BUSY()   ((PECI_CTL_STS & START_BUSY) ? 1:0)

/* PECI Read Length. */
#define PECI_RD_LENGTH      PORT_BYTE   PECI_RD_LENGTH_ADDR
/* PECI Address. */
#define PECI_ADDR           PORT_BYTE   PECI_ADDR_ADDR
/* PECI Command. */
#define PECI_CMD            PORT_BYTE   PECI_CMD_ADDR
/* PECI Control 2. */
#define PECI_CTL2           PORT_BYTE   PECI_CTL2_ADDR
/* PECI Index. */
#define PECI_INDEX          PORT_BYTE   PECI_INDEX_ADDR
/* PECI Index Data. */
#define PECI_IDATA          PORT_BYTE   PECI_IDATA_ADDR
/* PECI Write Length. */
#define PECI_WR_LENGTH      PORT_BYTE   PECI_WR_LENGTH_ADDR
/* PECI Write FCS. */
#define PECI_WR_FCS         PORT_BYTE   PECI_WR_FCS_ADDR
/* PECI Read FCS. */
#define PECI_RD_FCS         PORT_BYTE   PECI_RD_FCS_ADDR
/* PECI Assured Write FCS. */
#define PECI_AW_FCS         PORT_BYTE   PECI_AW_FCS_ADDR
/* PECI Transfer Rate. */
#define PECI_RATE           PORT_BYTE   PECI_RATE_ADDR
/* -------------------------------------------------- */
#define EHSP_EN       (1 << 7)
#define HLOAD_EN      (1 << 6)

#define PECI_RATE_MAX_BIT_RATE_P      0
#define PECI_RATE_MAX_BIT_RATE_S      5

/* PECI Data IN (16 bytes). */
#define PECI_DATA_IN(i)     PORT_BYTE(PECI_ADDR_BASE + 0x10 + (i))   //PECI_DATA_IN_ADDR
/* PECI Data OUT (16 bytes). */
#define PECI_DATA_OUT(i)    PORT_BYTE(PECI_ADDR_BASE + 0x10 + (i))   //PECI_DATA_OUT_ADDR


/* SPI Core Registers */
#define SPI_ADDR_BASE 0xFFF0C0
#define SPI_DATA_ADDR   (SPI_ADDR_BASE + 0x00)  /* SPI Data In/Out.*/
#define SPI_CTL1_ADDR   (SPI_ADDR_BASE + 0x02)  /* SPI Control 1. */
#define SPI_STAT_ADDR   (SPI_ADDR_BASE + 0x04)  /* SPI Status. */

/* SPI Data In/Out.*/
#define SPI_DATA    PORT_BYTE   SPI_DATA_ADDR
/* SPI Control 1. */
#define SPI_CTL1    PORT_WORD   SPI_CTL1_ADDR
/* SPI Status. */
#define SPI_STAT    PORT_BYTE   SPI_STAT_ADDR

/* BBRM CORE Registers */
#define BBRM_ADDR_BASE 0xFFF8BF
#define BKUP_STS_ADDR   (BBRM_ADDR_BASE + 0x00) /* BBRM Status. */

/* BBRM Status. */
#define BKUP_STS    PORT_BYTE   BKUP_STS_ADDR


/* MRC Core Registers */
#define MDC_ADDR_BASE 0xFFF0E0
#define SRST_CTL_ADDR       (MDC_ADDR_BASE + 0x00)  /* Software Reset Control. */
#define SRST_LOCK_ADDR      (MDC_ADDR_BASE + 0x02)  /* Software Reset Lock. */
#define MDC_BSC0_ADDR       (MDC_ADDR_BASE + 0x04)  /* Booter Scratch 0. */
#define SRST_UNLOCK_ADDR    (MDC_ADDR_BASE + 0x06)  /* Software Reset Unlock. */
#define MDC_BSC1_ADDR       (MDC_ADDR_BASE + 0x08)  /* Booter Scratch 1. */
#define HEAD_BASEL_ADDR     (MDC_ADDR_BASE + 0x0A)  /* Header Base Low. */
#define HEAD_BASEH_ADDR     (MDC_ADDR_BASE + 0x0C)  /* Header Base High. */

/* Software Reset Control. */
#define SRST_CTL        PORT_BYTE   SRST_CTL_ADDR
/* ------------------------------------------- */
#define SOFT_RST      (1 << 0)
#define CRIS_MODE     (1 << 1)
#define LD_CRIS       (1 << 5)

/* Software Reset Lock. */
#define SRST_LOCK       PORT_BYTE   SRST_LOCK_ADDR
/* Booter Scratch 0. */
#define MCD_BSC0        PORT_BYTE   MDC_BSC0_ADDR
/* Software Reset Unlock. */
#define SRST_UNLOCK     PORT_BYTE   SRST_UNLOCK_ADDR
/* Booter Scratch 1. */
#define MCD_BSC1        PORT_BYTE   MDC_BSC1_ADDR
/* Header Base Low. */
#define HEAD_BASEL      PORT_BYTE   HEAD_BASEL_ADDR
/* Header Base High. */
#define HEAD_BASEH      PORT_BYTE   HEAD_BASEH_ADDR

/* General-Purpose Window */
#define GP_WIN_BASE 0xFFFB80
#define PW_GP1_BOT_ADDR      (GP_WIN_BASE + 0x00)  /* General-Purpose Window 1 Bottom Address. */
#define PW_GP1_TOP_ADDR      (GP_WIN_BASE + 0x02)  /* General-Purpose Window 1 Top Address. */
#define PW_GP1_RW_CTL_ADDR   (GP_WIN_BASE + 0x04)  /* General-Purpose Window 1 Read/Write Control. */
#define PW_GP1_CTS_ADDR      (GP_WIN_BASE + 0x05)  /* General-Purpose Window 1 Control/Status. */
#define PW_GP1_UNLK_DAT_ADDR (GP_WIN_BASE + 0x06)  /* General-Purpose Window 1 Unlock Data. */
#define PW_GP2_BOT_ADDR      (GP_WIN_BASE + 0x08)  /* General-Purpose Window 2 Bottom Address. */
#define PW_GP2_TOP_ADDR      (GP_WIN_BASE + 0x0A)  /* General-Purpose Window 2 Top Address. */
#define PW_GP2_RW_CTL_ADDR   (GP_WIN_BASE + 0x0C)  /* General-Purpose Window 2 Read/Write Control. */
#define PW_GP2_CTS_ADDR      (GP_WIN_BASE + 0x0D)  /* General-Purpose Window 2 Control/Status. */
#define PW_GP2_UNLK_DAT_ADDR (GP_WIN_BASE + 0x0E)  /* General-Purpose Window 2 Unlock Data. */
#define PW_GP3_BOT_ADDR      (GP_WIN_BASE + 0x10)  /* General-Purpose Window 3 Bottom Address. */
#define PW_GP3_TOP_ADDR      (GP_WIN_BASE + 0x12)  /* General-Purpose Window 3 Top Address. */
#define PW_GP3_RW_CTL_ADDR   (GP_WIN_BASE + 0x14)  /* General-Purpose Window 3 Read/Write Control. */
#define PW_GP3_CTS_ADDR      (GP_WIN_BASE + 0x15)  /* General-Purpose Window 3 Control/Status. */
#define PW_GP3_UNLK_DAT_ADDR (GP_WIN_BASE + 0x16)  /* General-Purpose Window 3 Unlock Data. */
#define PW_GP4_BOT_ADDR      (GP_WIN_BASE + 0x18)  /* General-Purpose Window 4 Bottom Address. */
#define PW_GP4_TOP_ADDR      (GP_WIN_BASE + 0x1A)  /* General-Purpose Window 4 Top Address. */
#define PW_GP4_RW_CTL_ADDR   (GP_WIN_BASE + 0x1C)  /* General-Purpose Window 4 Read/Write Control. */
#define PW_GP4_CTS_ADDR      (GP_WIN_BASE + 0x1D)  /* General-Purpose Window 4 Control/Status. */
#define PW_GP4_UNLK_DAT_ADDR (GP_WIN_BASE + 0x1E)  /* General-Purpose Window 4 Unlock Data. */
#define PW_UMA_RW_CTL_ADDR   (GP_WIN_BASE + 0x2C)  /* UMA Window Read/Write Control. */
#define PW_UMA_CTS_ADDR      (GP_WIN_BASE + 0x2D)  /* UMA Window Control/Status. */
#define PW_UMA_UNLK_DAT_ADDR (GP_WIN_BASE + 0x2E)  /* UMA Window Unlock Data. */


/* General-Purpose Window 1-4, Read/Write Control */
#define PW_GP1_RW_CTL    PORT_BYTE   PW_GP1_RW_CTL_ADDR
#define PW_GP2_RW_CTL    PORT_BYTE   PW_GP2_RW_CTL_ADDR
#define PW_GP3_RW_CTL    PORT_BYTE   PW_GP3_RW_CTL_ADDR
#define PW_GP4_RW_CTL    PORT_BYTE   PW_GP4_RW_CTL_ADDR
/* ------------------------------------------- */
#define PW_HRP      (1 << 0)      /* Host Read Protect. */
#define PW_HWP      (1 << 1)      /* Host Write Protect. */
#define PW_JRP      (1 << 2)      /* JTAG Read Protect. */
#define PW_JWP      (1 << 3)      /* JTAG Write Protect. */
#define PW_CRP      (1 << 4)      /* Core Data Access Read Protect. */
#define PW_CWP      (1 << 5)      /* Core Data Access Write Protect. */
#define PW_IRP      (1 << 6)      /* I2C Access General-Purpose Window Read Protect. */
#define PW_IWP      (1 << 7)      /* I2C Access General-Purpose Window Write Protect. */

/* General-Purpose Window 1-4, Control/Status */
#define PW_GP1_CTS       PORT_BYTE   PW_GP1_CTS_ADDR
#define PW_GP2_CTS       PORT_BYTE   PW_GP2_CTS_ADDR
#define PW_GP3_CTS       PORT_BYTE   PW_GP3_CTS_ADDR
#define PW_GP4_CTS       PORT_BYTE   PW_GP4_CTS_ADDR
/* ------------------------------------------ */
#define PW_HP_VL    (1 << 0)      /* Host Access Protect Violation. */
#define PW_JP_VL    (1 << 1)      /* JTAG Access Protection Violation. */
#define PW_CP_VL    (1 << 2)      /* Core Data Access Protection Violation. */
#define PW_IP_VL    (1 << 3)      /* I2C Access General-Purpose Window Protection Violation. */
#define PW_CFG_LK   (1 << 4)      /* Protection Window Configuration Lock. */
#define PW_UNLK_CTL (1 << 5)      /* Protection Window Unlock Control. */


/* General-Purpose Window 1-4, Unlock Data */
#define PW_GP1_UNLK_DAT  PORT_BYTE   PW_GP1_UNLK_DAT_ADDR
#define PW_GP2_UNLK_DAT  PORT_BYTE   PW_GP2_UNLK_DAT_ADDR
#define PW_GP3_UNLK_DAT  PORT_BYTE   PW_GP3_UNLK_DAT_ADDR
#define PW_GP4_UNLK_DAT  PORT_BYTE   PW_GP4_UNLK_DAT_ADDR

/* UMA Window Read/Write Control. */
#define PW_UMA_RW_CTL    PORT_BYTE   PW_UMA_RW_CTL_ADDR
/* ------------------------------------------- */
#define UMA_HAP     (1 << 0)      /* Host Access Protect. */
#define UMA_JAP     (1 << 2)      /* JTAG Access Protect. */
#define UMA_CAP     (1 << 4)      /* Core Data Access Protect. */
#define UMA_IAP     (1 << 6)      /* I2C Access UMA Window Protection Control. */

/* UMA Window Control/Status. */
#define PW_UMA_CTS       PORT_BYTE   PW_UMA_CTS_ADDR
/* UMA Window Unlock Data. */
#define PW_UMA_UNLK_DAT  PORT_BYTE   PW_UMA_UNLK_DAT_ADDR


/* PMC Core Registers */
#define PMC_ADDR_BASE 0xFFF100
#define PMCSR_ADDR      (PMC_ADDR_BASE + 0x00)  /* Power Management Controller Status. */
#define ENIDL_CTL_ADDR  (PMC_ADDR_BASE + 0x03)  /* Enable in Idle Control. */
#define DISWT_CTL_ADDR  (PMC_ADDR_BASE + 0x04)  /* Disable in Wait Control. */
#define DISWT_CTL1_ADDR (PMC_ADDR_BASE + 0x05)  /* Disable in Wait Control 1. */
#define PWDWN_CTL1_ADDR (PMC_ADDR_BASE + 0x08)  /* Power-Down Control 1. */
#define PWDWN_CTL2_ADDR (PMC_ADDR_BASE + 0x09)  /* Power-Down Control 2. */
#define PWDWN_CTL3_ADDR (PMC_ADDR_BASE + 0x0A)  /* Power-Down Control 3. */
#define PWDWN_CTL4_ADDR (PMC_ADDR_BASE + 0x0B)  /* Power-Down Control 4. */
#define PWDWN_CTL5_ADDR (PMC_ADDR_BASE + 0x0C)  /* Power-Down Control 5. */
#define PWDWN_CTL6_ADDR (PMC_ADDR_BASE + 0x0D)  /* Power-Down Control 6. */

/* Power Management Controller Status. */
#define PMCSR       PORT_BYTE PMCSR_ADDR
#define PMCSR_DI_INSTW  (1 << 0)    /* Deep Idle Instant Wake-Up. */
#define PMCSR_DHF       (1 << 1)    /* Disable High Frequency oscillator. */
#define PMCSR_IDLE      (1 << 2)    /* Idle mode. */
#define PMCSR_NWBI      (1 << 3)    /* No WAIT Before Idle. */
#define PMCSR_VCORF_DIS (1 << 5)    /* VCORF Disable */
#define PMCSR_OHFC      (1 << 6)    /* Oscillating High Frequency Clock. */
#define PMCSR_OLFC      (1 << 7)    /* Oscillating Low Frequency Clock. */

/* Enable in Idle Control. */
#define ENIDL_CTL   PORT_BYTE   ENIDL_CTL_ADDR
/* ----------------------------------------- */
#define ADC_LFSL        (1 << 7)    /* ADC Low-Frequency Clock Select. */
#define LP_WK_CTL       (1 << 6)    /* Low-Power Wake-Up Control. */
#define PECI_ENI        (1 << 2)    /* PECI Enable in Idle. */
#define ADC_ACC_DIS     (1 << 1)    /* ADC Access is Disabled. */

/* Disable in Wait Control. */
#define DISWT_CTL   PORT_BYTE   DISWT_CTL_ADDR
/* ----------------------------------------- */
#define ACC_DW          (1 << 7)    /* Access Disable in Wait. */
#define ROM_DW          (1 << 6)    /* ROM Disable in Wait. */
#define RAM_DW          (1 << 5)    /* Core RAM Disable in Wait. */
#define SPIP_DW         (1 << 4)    /* SPI Peripheral Disable in Wait. */
#define PECI_DW         (1 << 2)    /* PECI Disable in Wait. */
#define ADC_DW          (1 << 1)    /* ADC Disable in Wait. */
#define FIU_DW          (1 << 0)    /* FIU Disable in Wait. */

/* Disable in Wait Control 1. */
#define DISWT_CTL1  PORT_BYTE   DISWT_CTL1_ADDR
/* ----------------------------------------- */
#define SHM_ACC_DW      (1 << 7)    /* SHM Access Disable in Wait. */
#define CR_LPW          (1 << 1)    /* Core Low Power in Wait. */
#define CB_DW           (1 << 0)    /* Core Bus Disable in Wait. */

/* Power-Down Control 1. */
#define PWDWN_CTL1  PORT_BYTE   PWDWN_CTL1_ADDR
/* ----------------------------------------- */
#define MFT3_PD         (1 << 7)    /* MFT16-3 Power-Down. */
#define MFT2_PD         (1 << 6)    /* MFT16-2 Power-Down. */
#define MFT1_PD         (1 << 5)    /* MFT16-1 Power-Down. */
#define UART_PD         (1 << 4)    /* CR_UART Power-Down. */
#define PS2_PD          (1 << 3)    /* PS/2 Power-Down. */
#define FIU_PD          (1 << 2)    /* FIU Power-Down. */
#define SDP_PD          (1 << 1)    /* SDP Power-Down. */
#define KBS_PD          (1 << 0)    /* Keyboard Scan Power-Down. */

/* Power-Down Control 2. */
#define PWDWN_CTL2  PORT_BYTE   PWDWN_CTL2_ADDR
/* ----------------------------------------- */
#define H_PWM_PD        (1 << 7)    /* H_PWM Power-Down. */
#define G_PWM_PD        (1 << 6)    /* G_PWM Power-Down. */
#define F_PWM_PD        (1 << 5)    /* F_PWM Power-Down. */
#define E_PWM_PD        (1 << 4)    /* E_PWM Power-Down. */
#define D_PWM_PD        (1 << 3)    /* D_PWM Power-Down. */
#define C_PWM_PD        (1 << 2)    /* C_PWM Power-Down. */
#define B_PWM_PD        (1 << 1)    /* B_PWM Power-Down. */
#define A_PWM_PD        (1 << 0)    /* A_PWM Power-Down. */

/* Power-Down Control 3. */
#define PWDWN_CTL3  PORT_BYTE   PWDWN_CTL3_ADDR
/* ----------------------------------------- */
#define USB_PD          (1 << 5)    /* USB Power-Down. */
#define SMB5_PD         (1 << 4)    /* SMB5 Power-Down. */
#define SMB4_PD         (1 << 3)    /* SMB4 Power-Down. */
#define SMB3_PD         (1 << 2)    /* SMB3 Power-Down. */
#define SMB2_PD         (1 << 1)    /* SMB2 Power-Down. */
#define SMB1_PD         (1 << 0)    /* SMB1 Power-Down. */

/* Power-Down Control 4. */
#define PWDWN_CTL4  PORT_BYTE   PWDWN_CTL4_ADDR
/* ----------------------------------------- */
#define SPIP_PD         (1 << 7)    /* SPI Peripheral Power-Down. */
#define PECI_PD         (1 << 5)    /* PECI Power-Down. */
#define ADC_PD          (1 << 4)    /* ADC Power-Down. */
#define SMB_DMA_PD      (1 << 3)    /* SMB DMA Power-Down. */
#define ITIM3_PD        (1 << 2)    /* ITIM16-3 Power-Down. */
#define ITIM2_PD        (1 << 1)    /* ITIM16-2 Power-Down. */
#define ITIM1_PD        (1 << 0)    /* ITIM16-1 Power-Down. */

/* Power-Down Control 5. */
#define PWDWN_CTL5  PORT_BYTE   PWDWN_CTL5_ADDR
/* ----------------------------------------- */
#define MSWC_PD         (1 << 7)    /* MSWC Power-Down. */
#define DP80_PD         (1 << 6)    /* Debug Port 80 Power-Down. */
#define SHM_PD          (1 << 5)    /* SHM Power-Down. */
#define C2HACC_PD       (1 << 3)    /* Core-to-Host Module Power-Down. */

/* Power-Down Control 6. */
#define PWDWN_CTL6  PORT_BYTE   PWDWN_CTL6_ADDR
/* ----------------------------------------- */
#define ITIM6_PD        (1 << 2)    /* ITIM16-6 Power-Down. */
#define ITIM5_PD        (1 << 1)    /* ITIM16-5 Power-Down. */
#define ITIM4_PD        (1 << 0)    /* ITIM16-4 Power-Down. */


/* CDCG Core Registers */
#define LFCG_ADDR_BASE 0xFFF090
#define HFCG_ADDR_BASE 0xFFF080

#define LFCGCTL_ADDR    (LFCG_ADDR_BASE + 0x00) /* LFCG Control. */
#define HFRDI_ADDR      (LFCG_ADDR_BASE + 0x02) /* High-Frequency Reference Divisor I. */
#define HFRDF_ADDR      (LFCG_ADDR_BASE + 0x04) /* High-Frequency Reference Divisor F. */
#define FRCDIV_ADDR     (LFCG_ADDR_BASE + 0x06) /* FRCLK Clock Divisor. */
#define DIVCOR1_ADDR    (LFCG_ADDR_BASE + 0x08) /* Divisor Correction Value 1. */
#define DIVCOR2_ADDR    (LFCG_ADDR_BASE + 0x0A) /* Divisor Correction Value 2. */
#define LFCGCTL2_ADDR   (LFCG_ADDR_BASE + 0x14) /* LFCG Control 2. */

#define HFCGCTRL_ADDR   (HFCG_ADDR_BASE + 0x00) /* HFCG Control. */
#define HFCGML_ADDR     (HFCG_ADDR_BASE + 0x02) /* HFCG M Low. */
#define HFCGMH_ADDR     (HFCG_ADDR_BASE + 0x04) /* HFCG M High. */
#define HFCGN_ADDR      (HFCG_ADDR_BASE + 0x06) /* HFCG N Value. */
#define HFCGP_ADDR      (HFCG_ADDR_BASE + 0x08) /* HFCG Prescaler. */

/* LFCG Control. */
#define LFCGCTL     PORT_BYTE   LFCGCTL_ADDR
/* --------------------------------------- */
#define LREFEN          (1 << 2)    /* LPC Clock Reference Enable. */
#define LFLER           (1 << 3)    /* LFCG Locked on External Reference. */
#define UDCP            (1 << 4)    /* Update Calibration Parameters. */
#define LFLOC           (1 << 5)    /* LFCG Register Write Lock. */

/* High-Frequency Reference Divisor I. */
#define HFRDI       PORT_WORD   HFRDI_ADDR
/* High-Frequency Reference Divisor F. */
#define HFRDF       PORT_WORD   HFRDF_ADDR
/* FRCLK Clock Divisor. */
#define FRCDIV      PORT_WORD   FRCDIV_ADDR
/* Divisor Correction Value 1. */
#define DIVCOR1     PORT_WORD   DIVCOR1_ADDR
/* Divisor Correction Value 2. */
#define DIVCOR2     PORT_WORD   DIVCOR2_ADDR
/* LFCG Control 2. */
#define LFCGCTL2    PORT_BYTE   LFCGCTL2_ADDR
/* ---------------------------------------- */
#define AUDP_EN      (1 << 3)    /* Automatic Update Enable. */
#define STOPCAL      (1 << 4)    /* Stop Calibration to External Reference. */
#define FCLKRUN      (1 << 5)    /* Force LCLK Running. */

/* HFCG Control. */
#define HFCGCTRL    PORT_BYTE   HFCGCTRL_ADDR
/* ---------------------------------------- */
#define HFCGCTRL_LOAD       (1 << 0)  /* Load M and N Values. */
#define HFCGCTRL_LOCK       (1 << 2)  /* Disable Writing to all HFCG register. */
#define HFCGCTRL_CLK_CHNG   (1 << 7)  /* Clock Changing. */

/* HFCG M Low. */
#define HFCGML      PORT_BYTE   HFCGML_ADDR
/* HFCG M High. */
#define HFCGMH      PORT_BYTE   HFCGMH_ADDR
/* HFCG N Value. */
#define HFCGN       PORT_BYTE   HFCGN_ADDR
/* HFCG Prescaler. */
#define HFCGP       PORT_BYTE   HFCGP_ADDR


/* Debugger Interface Core Registers */
#define DBGI_ADDR_BASE 0xFFF030
#define DBG_DID_ADDR    (DBGI_ADDR_BASE + 0x00) /* Debugger Interface JTAG Device ID. */
#define DBGFRZEN1_ADDR  (DBGI_ADDR_BASE + 0x04) /* Debug Freeze Enable 1. */
#define DBGFRZEN2_ADDR  (DBGI_ADDR_BASE + 0x05) /* Debug Freeze Enable 2. */
#define DBGFRZEN3_ADDR  (DBGI_ADDR_BASE + 0x06) /* Debug Freeze Enable 3. */

/* Debugger Interface JTAG Device ID. */
#define DBG_DID     PORT_DWORD    DBG_DID_ADDR
/* Debug Freeze Enable 1. */
#define DBGFRZEN1   PORT_BYTE     DBGFRZEN1_ADDR
/* Debug Freeze Enable 2. */
#define DBGFRZEN2   PORT_BYTE     DBGFRZEN2_ADDR
/* Debug Freeze Enable 3. */
#define DBGFRZEN3   PORT_BYTE     DBGFRZEN3_ADDR


/* USB DC Core Registers */
#define USBD_ADDR_BASE 0xFF4000
#define USBD_INTEN_ADDR     (USBD_ADDR_BASE + 0x00)  /* USB Device Interrupt Enable. */
#define USBD_INTSTS_ADDR    (USBD_ADDR_BASE + 0x04)  /* USB Device Interrupt Event Status. */
#define USBD_FADDR_ADDR     (USBD_ADDR_BASE + 0x08)  /* USB Device Function Address. */
#define USBD_EPSTS_ADDR     (USBD_ADDR_BASE + 0x0C)  /* USB Device Endpoint Status. */
#define USBD_ATTR_ADDR      (USBD_ADDR_BASE + 0x10)  /* USB Deivce Bus Status and Attribution. */
#define USBD_STBUFSEG_ADDR  (USBD_ADDR_BASE + 0x18)  /* Setup Token Buffer Segmentation. */
#define USBD_EPSTS0_ADDR    (USBD_ADDR_BASE + 0x20)  /* USB Devide Endpoint Status 0. */
#define USBD_EPSTS1_ADDR    (USBD_ADDR_BASE + 0x24)  /* USB Devide Endpoint Status 1. */
#define USBD_FN_ADDR        (USBD_ADDR_BASE + 0x8C)  /* USB Frame number. */
#define USBD_SE0_ADDR       (USBD_ADDR_BASE + 0x90)  /* USB Device Drive SE0 Control. */
#define USBD_BUFSEG0_ADDR   (USBD_ADDR_BASE + 0x100) /* Endpoint 0 Buffer Segmentation. */
#define USBD_MXPLD0_ADDR    (USBD_ADDR_BASE + 0x104) /* Endpoint 0 Maximal Payload. */
#define USBD_CFG0_ADDR      (USBD_ADDR_BASE + 0x108) /* Endpoint 0 Configuration. */
#define USBD_CFGP0_ADDR     (USBD_ADDR_BASE + 0x10C) /* Endpoint 0 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG1_ADDR   (USBD_ADDR_BASE + 0x110) /* Endpoint 1 Buffer Segmentation. */
#define USBD_MXPLD1_ADDR    (USBD_ADDR_BASE + 0x114) /* Endpoint 1 Maximal Payload. */
#define USBD_CFG1_ADDR      (USBD_ADDR_BASE + 0x118) /* Endpoint 1 Configuration. */
#define USBD_CFGP1_ADDR     (USBD_ADDR_BASE + 0x11C) /* Endpoint 1 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG2_ADDR   (USBD_ADDR_BASE + 0x120) /* Endpoint 2 Buffer Segmentation. */
#define USBD_MXPLD2_ADDR    (USBD_ADDR_BASE + 0x124) /* Endpoint 2 Maximal Payload. */
#define USBD_CFG2_ADDR      (USBD_ADDR_BASE + 0x128) /* Endpoint 2 Configuration. */
#define USBD_CFGP2_ADDR     (USBD_ADDR_BASE + 0x12C) /* Endpoint 2 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG3_ADDR   (USBD_ADDR_BASE + 0x130) /* Endpoint 3 Buffer Segmentation. */
#define USBD_MXPLD3_ADDR    (USBD_ADDR_BASE + 0x134) /* Endpoint 3 Maximal Payload. */
#define USBD_CFG3_ADDR      (USBD_ADDR_BASE + 0x138) /* Endpoint 3 Configuration. */
#define USBD_CFGP3_ADDR     (USBD_ADDR_BASE + 0x13C) /* Endpoint 3 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG4_ADDR   (USBD_ADDR_BASE + 0x140) /* Endpoint 4 Buffer Segmentation. */
#define USBD_MXPLD4_ADDR    (USBD_ADDR_BASE + 0x144) /* Endpoint 4 Maximal Payload. */
#define USBD_CFG4_ADDR      (USBD_ADDR_BASE + 0x148) /* Endpoint 4 Configuration. */
#define USBD_CFGP4_ADDR     (USBD_ADDR_BASE + 0x14C) /* Endpoint 4 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG5_ADDR   (USBD_ADDR_BASE + 0x150) /* Endpoint 5 Buffer Segmentation. */
#define USBD_MXPLD5_ADDR    (USBD_ADDR_BASE + 0x154) /* Endpoint 5 Maximal Payload. */
#define USBD_CFG5_ADDR      (USBD_ADDR_BASE + 0x158) /* Endpoint 5 Configuration. */
#define USBD_CFGP5_ADDR     (USBD_ADDR_BASE + 0x15C) /* Endpoint 5 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG6_ADDR   (USBD_ADDR_BASE + 0x160) /* Endpoint 6 Buffer Segmentation. */
#define USBD_MXPLD6_ADDR    (USBD_ADDR_BASE + 0x164) /* Endpoint 6 Maximal Payload. */
#define USBD_CFG6_ADDR      (USBD_ADDR_BASE + 0x168) /* Endpoint 6 Configuration. */
#define USBD_CFGP6_ADDR     (USBD_ADDR_BASE + 0x16C) /* Endpoint 6 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG7_ADDR   (USBD_ADDR_BASE + 0x170) /* Endpoint 7 Buffer Segmentation. */
#define USBD_MXPLD7_ADDR    (USBD_ADDR_BASE + 0x174) /* Endpoint 7 Maximal Payload. */
#define USBD_CFG7_ADDR      (USBD_ADDR_BASE + 0x178) /* Endpoint 7 Configuration. */
#define USBD_CFGP7_ADDR     (USBD_ADDR_BASE + 0x17C) /* Endpoint 7 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG8_ADDR   (USBD_ADDR_BASE + 0x180) /* Endpoint 8 Buffer Segmentation. */
#define USBD_MXPLD8_ADDR    (USBD_ADDR_BASE + 0x184) /* Endpoint 8 Maximal Payload. */
#define USBD_CFG8_ADDR      (USBD_ADDR_BASE + 0x188) /* Endpoint 8 Configuration. */
#define USBD_CFGP8_ADDR     (USBD_ADDR_BASE + 0x18C) /* Endpoint 8 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG9_ADDR   (USBD_ADDR_BASE + 0x190) /* Endpoint 9 Buffer Segmentation. */
#define USBD_MXPLD9_ADDR    (USBD_ADDR_BASE + 0x194) /* Endpoint 9 Maximal Payload. */
#define USBD_CFG9_ADDR      (USBD_ADDR_BASE + 0x198) /* Endpoint 9 Configuration. */
#define USBD_CFGP9_ADDR     (USBD_ADDR_BASE + 0x19C) /* Endpoint 9 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG10_ADDR  (USBD_ADDR_BASE + 0x1A0) /* Endpoint 10 Buffer Segmentation. */
#define USBD_MXPLD10_ADDR   (USBD_ADDR_BASE + 0x1A4) /* Endpoint 10 Maximal Payload. */
#define USBD_CFG10_ADDR     (USBD_ADDR_BASE + 0x1A8) /* Endpoint 10 Configuration. */
#define USBD_CFGP10_ADDR    (USBD_ADDR_BASE + 0x1AC) /* Endpoint 10 Set Stall and Clear I/O Ready Control. */
#define USBD_BUFSEG11_ADDR  (USBD_ADDR_BASE + 0x1B0) /* Endpoint 11 Buffer Segmentation. */
#define USBD_MXPLD11_ADDR   (USBD_ADDR_BASE + 0x1B4) /* Endpoint 11 Maximal Payload. */
#define USBD_CFG11_ADDR     (USBD_ADDR_BASE + 0x1B8) /* Endpoint 11 Configuration. */
#define USBD_CFGP11_ADDR    (USBD_ADDR_BASE + 0x1BC) /* Endpoint 11 Set Stall and Clear I/O Ready Control. */
#define USBD_SRAM_ADDR      (USBD_ADDR_BASE + 0x200) /* USB Buffer. (Lenth: 1KB) */
#define USBD_VERSION_ADDR   (USBD_ADDR_BASE + 0x7FC) /* USB Version Number. */

/* USB Interrupt Enable */
#define USBD_INTEN        PORT_DWORD    USBD_INTEN_ADDR
/* ------------------------------------------------- */
#define USBD_BUSIEN         (1 << 0)    /* Bus Event Interrupt Enable. */
#define USBD_USBIEN         (1 << 1)    /* USB Event Interrupt Enable. */
#define USBD_SOFIEN         (1 << 2)    /* Start of Frame Interrupt Enable. */
#define USBD_WKEN           (1 << 8)    /* Wake-up Function Enable. */
#define USBD_INNAKEN        (1 << 15)   /* Active NAK Function and Its Status in IN Token. */

/* USB Interrupt Event Status */
#define USBD_INTSTS       PORT_DWORD    USBD_INTSTS_ADDR
/* ------------------------------------------------- */
#define USBD_EPEVT0         (1 << 16)    /* Endpoint 0's USB Event Status. */
#define USBD_EPEVT1         (1 << 17)    /* Endpoint 1's USB Event Status. */
#define USBD_EPEVT2         (1 << 18)    /* Endpoint 2's USB Event Status. */
#define USBD_EPEVT3         (1 << 19)    /* Endpoint 3's USB Event Status. */
#define USBD_EPEVT4         (1 << 20)    /* Endpoint 4's USB Event Status. */
#define USBD_EPEVT5         (1 << 21)    /* Endpoint 5's USB Event Status. */
#define USBD_EPEVT6         (1 << 22)    /* Endpoint 6's USB Event Status. */
#define USBD_EPEVT7         (1 << 23)    /* Endpoint 7's USB Event Status. */
#define USBD_EPEVT8         (1 << 24)    /* Endpoint 8's USB Event Status. */
#define USBD_EPEVT9         (1 << 25)    /* Endpoint 9's USB Event Status. */
#define USBD_EPEVT10        (1 << 26)    /* Endpoint 10's USB Event Status. */
#define USBD_EPEVT11        (1 << 27)    /* Endpoint 11's USB Event Status. */
#define USBD_SETUP          (1 << 31)    /* Setup Event Status. */

/* USB Device Function Address */
#define USBD_FADDR        PORT_DWORD     USBD_FADDR_ADDR
/* USB Device Endpoint Status. */
#define USBD_EPSTS        PORT_DWORD     USBD_EPSTS_ADDR
/* ------------------------------------------------- */
#define USBD_OV             (1 << 7)     /* Overrun */

/* USB Deivce Bus Status and Attribution. */
#define USBD_ATTR         PORT_DWORD     USBD_ATTR_ADDR
/* ------------------------------------------------- */
#define USBD_USBRST         (1 << 0)     /* USB Reset Status. */
#define USBD_SUSPEND        (1 << 1)     /* Suspend Status. */
#define USBD_RESUME         (1 << 2)     /* Resume Status. */
#define USBD_TOUT           (1 << 3)     /* Time-out Status. */
#define USBD_PHYEN          (1 << 4)     /* PHY Transceiver Function Enable. */
#define USBD_RWAKEUP        (1 << 5)     /* Remote Wake-up. */
#define USBD_SPEED          (1 << 6)     /* Speed Selection. */
#define USBD_USBEN          (1 << 7)     /* USB Controller Enable. */
#define USBD_DPPUEN         (1 << 8)     /* Pull-Up Resistor Enable. */
#define USBD_BYTEM          (1 << 10)    /* CPU Access USB SRAM Size Mode Selection. */

/* Setup Token Buffer Segmentation. */
#define USBD_STBUFSEG     PORT_DWORD     USBD_STBUFSEG_ADDR

/* USB Devide Endpoint Status 0. */
#define USBD_EPSTS0       PORT_DWORD     USBD_EPSTS0_ADDR
/* ------------------------------------------------- */
#define EPSTS0            (0x0F << 0)    /* Endpoint 0 Status */
#define EPSTS1            (0x0F << 4)    /* Endpoint 1 Status */
#define EPSTS2            (0x0F << 8)    /* Endpoint 2 Status */
#define EPSTS3            (0x0F << 12)   /* Endpoint 3 Status */
#define EPSTS4            (0x0F << 16)   /* Endpoint 4 Status */
#define EPSTS5            (0x0F << 20)   /* Endpoint 5 Status */
#define EPSTS6            (0x0F << 24)   /* Endpoint 6 Status */
#define EPSTS7            (0x0F << 28)   /* Endpoint 7 Status */

/* USB Devide Endpoint Status 1. */
#define USBD_EPSTS1       PORT_DWORD     USBD_EPSTS1_ADDR
/* ------------------------------------------------- */
#define EPSTS8            (0x0F << 0)    /* Endpoint 8 Status */
#define EPSTS9            (0x0F << 4)    /* Endpoint 9 Status */
#define EPSTS10           (0x0F << 8)    /* Endpoint 10 Status */
#define EPSTS11           (0x0F << 12)   /* Endpoint 11 Status */

/* USB Frame number. */
#define USBD_FN           PORT_DWORD     USBD_FN_ADDR
/* USB Device Drive SE0 Control. */
#define USBD_SE0          PORT_DWORD     USBD_SE0_ADDR

/* USB Buffer Segmentation 0-11 */
#define USBD_BUFSEG0      PORT_DWORD     USBD_BUFSEG0_ADDR
#define USBD_BUFSEG1      PORT_DWORD     USBD_BUFSEG1_ADDR
#define USBD_BUFSEG2      PORT_DWORD     USBD_BUFSEG2_ADDR
#define USBD_BUFSEG3      PORT_DWORD     USBD_BUFSEG3_ADDR
#define USBD_BUFSEG4      PORT_DWORD     USBD_BUFSEG4_ADDR
#define USBD_BUFSEG5      PORT_DWORD     USBD_BUFSEG5_ADDR
#define USBD_BUFSEG6      PORT_DWORD     USBD_BUFSEG6_ADDR
#define USBD_BUFSEG7      PORT_DWORD     USBD_BUFSEG7_ADDR
#define USBD_BUFSEG8      PORT_DWORD     USBD_BUFSEG8_ADDR
#define USBD_BUFSEG9      PORT_DWORD     USBD_BUFSEG9_ADDR
#define USBD_BUFSEG10     PORT_DWORD     USBD_BUFSEG10_ADDR
#define USBD_BUFSEG11     PORT_DWORD     USBD_BUFSEG10_ADDR

/* USB Maximal Payload 0-11 */
#define USBD_MXPLD0       PORT_DWORD     USBD_MXPLD0_ADDR
#define USBD_MXPLD1       PORT_DWORD     USBD_MXPLD1_ADDR
#define USBD_MXPLD2       PORT_DWORD     USBD_MXPLD2_ADDR
#define USBD_MXPLD3       PORT_DWORD     USBD_MXPLD3_ADDR
#define USBD_MXPLD4       PORT_DWORD     USBD_MXPLD4_ADDR
#define USBD_MXPLD5       PORT_DWORD     USBD_MXPLD5_ADDR
#define USBD_MXPLD6       PORT_DWORD     USBD_MXPLD6_ADDR
#define USBD_MXPLD7       PORT_DWORD     USBD_MXPLD7_ADDR
#define USBD_MXPLD8       PORT_DWORD     USBD_MXPLD8_ADDR
#define USBD_MXPLD9       PORT_DWORD     USBD_MXPLD9_ADDR
#define USBD_MXPLD10      PORT_DWORD     USBD_MXPLD10_ADDR
#define USBD_MXPLD11      PORT_DWORD     USBD_MXPLD11_ADDR

/* USB Configuration */
#define USBD_CFG0         PORT_DWORD     USBD_CFG0_ADDR
#define USBD_CFG1         PORT_DWORD     USBD_CFG1_ADDR
#define USBD_CFG2         PORT_DWORD     USBD_CFG2_ADDR
#define USBD_CFG3         PORT_DWORD     USBD_CFG3_ADDR
#define USBD_CFG4         PORT_DWORD     USBD_CFG4_ADDR
#define USBD_CFG5         PORT_DWORD     USBD_CFG5_ADDR
#define USBD_CFG6         PORT_DWORD     USBD_CFG6_ADDR
#define USBD_CFG7         PORT_DWORD     USBD_CFG7_ADDR
#define USBD_CFG8         PORT_DWORD     USBD_CFG8_ADDR
#define USBD_CFG9         PORT_DWORD     USBD_CFG9_ADDR
#define USBD_CFG10        PORT_DWORD     USBD_CFG10_ADDR
#define USBD_CFG11        PORT_DWORD     USBD_CFG11_ADDR
/* -------------------------------------------------- */
#define USBD_EPNUM        (0x0F << 0)    /* Endpoint Number. */
#define USBD_ISOCH        (1 << 4)       /* Isochronous Endpoint. */
#define USBD_STATE        (3 << 5)       /* Endpoint STATE. */
#define USBD_DSQSYNC      (1 << 7)       /* Data Sequence Synchronization. */
#define USBD_CSTALL       (1 << 9)       /* Clear STALL Response. */


/* USB Extra Configuration */
#define USBD_CFGP0        PORT_DWORD     USBD_CFGP0_ADDR
#define USBD_CFGP1        PORT_DWORD     USBD_CFGP1_ADDR
#define USBD_CFGP2        PORT_DWORD     USBD_CFGP2_ADDR
#define USBD_CFGP3        PORT_DWORD     USBD_CFGP3_ADDR
#define USBD_CFGP4        PORT_DWORD     USBD_CFGP4_ADDR
#define USBD_CFGP5        PORT_DWORD     USBD_CFGP5_ADDR
#define USBD_CFGP6        PORT_DWORD     USBD_CFGP6_ADDR
#define USBD_CFGP7        PORT_DWORD     USBD_CFGP7_ADDR
#define USBD_CFGP8        PORT_DWORD     USBD_CFGP8_ADDR
#define USBD_CFGP9        PORT_DWORD     USBD_CFGP9_ADDR
#define USBD_CFGP10       PORT_DWORD     USBD_CFGP10_ADDR
#define USBD_CFGP11       PORT_DWORD     USBD_CFGP11_ADDR
/* -------------------------------------------------- */
#define USBD_CCLRRDY      (1 << 0)       /* Clear Ready. */
#define USBD_SSTALL       (1 << 1)       /* Set STALL. */

/* USB Device Version Number */
#define USBD_VERSION      PORT_DWORD     USBD_VERSION_ADDR


/* Keyboard & Mouse CORE Registers */
#define KBC_ADDR_BASE 0xFFF780
#define HICTRL_ADDR     (KBC_ADDR_BASE + 0x00)  /* Host Interface Control. */
#define HIIRQC_ADDR     (KBC_ADDR_BASE + 0x02)  /* Host Interface IRQ Control. */
#define HIKMST_ADDR     (KBC_ADDR_BASE + 0x04)  /* Host Interface Keyboard/Mouse Status. */
#define HIKDO_ADDR      (KBC_ADDR_BASE + 0x06)  /* Host Interface Keyboard Data Out Buffer. */
#define HIMDO_ADDR      (KBC_ADDR_BASE + 0x08)  /* Host Interface Mouse Data Out Buffer. */
#define HIKMDI_ADDR     (KBC_ADDR_BASE + 0x0A)  /* Host Interface Keyboard/Mouse Data In Buffer. */
#define SHIKMDI_ADDR    (KBC_ADDR_BASE + 0x0B)  /* Host Interface Keyboard/Mouse Shadow Data In Buffer. */

/* Host Interface Control. */
#define HICTRL      PORT_BYTE   HICTRL_ADDR
/* -------------------------------------- */
#define HICTRL_OBFKIE    (1 << 0)   /* OBF Keyboard Interrupt Enable (1 = auto IRQ1). */
#define HICTRL_OBFMIE    (1 << 1)   /* OBF Mouse    Interrupt Enable (1 = auto IRQ12). */
#define HICTRL_OBECIE    (1 << 2)   /* OBE Core Interrupt Enable. */
#define HICTRL_IBFCIE    (1 << 3)   /* IBF Core Interrupt Enable. */
#define HICTRL_PMHIE     (1 << 4)   /* OBF Power Management Interrupt Enable (1 = auto IRQ11). */
#define HICTRL_PMECIE    (1 << 5)   /* OBE Power Management Core Interrupt Enable. */
#define HICTRL_PMICIE    (1 << 6)   /* IBF Power Management Core Interrupt Enable. */

/* Host Interface IRQ Control. */
#define HIIRQC      PORT_BYTE   HIIRQC_ADDR
/* -------------------------------------- */
#define HIIRQC_IRQ1B     (1 << 0)   /* IRQ1  */
#define HIIRQC_IRQ12B    (1 << 1)   /* IRQ12 */
#define HIIRQC_IRQ11B    (1 << 2)   /* IRQ11 */
#define HIIRQC_IRQM      (7 << 3)   /* IRQ Mode - level or pulse, pulse width */
#define HIIRQC_IRQNPOL   (1 << 6)   /* IRQ Negative Polarity */
#define HIIRQC_PSPE      (1 << 7)   /* Push Pull Enable */

/* Host Interface Keyboard/Mouse Status. */
#define HIKMST      PORT_BYTE   HIKMST_ADDR
/* -------------------------------------- */
/* Bit fields: */
#define HIST_OBF        (1 << 0)  /* Output Buffer Full (1 = full) */
#define HIST_IBF        (1 << 1)  /* Input Buffer Full (1 = full) */
#define HIST_SYS        (1 << 2)  /* System: 1 means CPU virtual->real mode*/
#define HIST_CD         (1 << 3)  /* Command/Data: 0 = data, 1 = command */
#define HIST_KBEN       (1 << 4)  /* Keyboard Enable (1 = enabled) */
#define HIST_ODS        (1 << 5)  /* Output buffer Data Source (1 = mouse) */

/* Status register definitions.

   NOTE:
   Bit 5 is used for two purposes, depending on whether the Keyboard
   Controller is in PS/2 compatible mode or AT compatible mode.  That
   is why there are 2 definitions. */

#define maskSTATUS_PORT_OBF     (1 << 0) /* Output buffer full, PC read 60. */
#define maskSTATUS_PORT_IBF     (1 << 1) /* Input buffer full, PC write 60/64. */
#define maskSTATUS_PORT_SYSTEM  (1 << 2) /* System Flag checked by BIOS on reset.
                                            (0 - reset was caused by power on) */
#define maskSTATUS_PORT_CMD     (1 << 3) /* 1 = Command, 0 = Data received. */

/* INHIBIT = 1 and ENABLE = 1 have the same meaning.
   The keyboard is not inhibited but is enabled. */
#define maskSTATUS_PORT_INHIBIT (1 << 4) /* 0 = Inhibit switch on. */
#define maskSTATUS_PORT_ENABLE  (1 << 4) /* 1 = Keyboard enabled. */

#define maskSTATUS_PORT_AUXOBF  (1 << 5) /* 1 = Data from Aux device (PS/2 mode). */
#define maskSTATUS_PORT_SENDTMO (1 << 5) /* Send time out error (AT mode). */
#define maskSTATUS_PORT_GENTMO  (1 << 6) /* General time out error. */
#define maskSTATUS_PORT_PARITY  (1 << 7) /* 1 = parity error. */


/* Host Interface Keyboard Data Out Buffer. */
#define HIKDO       PORT_BYTE   HIKDO_ADDR
/* Host Interface Mouse Data Out Buffer. */
#define HIMDO       PORT_BYTE   HIMDO_ADDR
/* Host Interface Keyboard/Mouse Data In Buffer. */
#define HIKMDI      PORT_BYTE   HIKMDI_ADDR
/* Host Interface Keyboard/Mouse Shadow Data In Buffer. */
#define SHIKMDI     PORT_BYTE   SHIKMDI_ADDR

/* PM Core Registers */
#define PM1_ADDR_BASE 0xFFF78C
#define PM2_ADDR_BASE 0xFFF79E
#define PM3_ADDR_BASE 0xFFF7C0
#define PM4_ADDR_BASE 0xFFF7D2

#define HIPM1ST_ADDR      (PM1_ADDR_BASE + 0x00)    /* Host Interface PM n Status. */
#define HIPM1DO_ADDR      (PM1_ADDR_BASE + 0x02)    /* Host Interface PM n Data Out Buffer. */
#define HIPM1DI_ADDR      (PM1_ADDR_BASE + 0x04)    /* Host Interface PM n Data In Buffer. */
#define SHIP1nDI_ADDR     (PM1_ADDR_BASE + 0x05)    /* Host Interface PM n Shadow Data In Buffer. */
#define HIPM1DOC_ADDR     (PM1_ADDR_BASE + 0x06)    /* Host Interface PM n Data Out Buffer with SCI. */
#define HIPM1DOM_ADDR     (PM1_ADDR_BASE + 0x08)    /* Host Interface PM n Data Out Buffer with SMI. */
#define HIPM1DIC_ADDR     (PM1_ADDR_BASE + 0x0A)    /* Host Interface PM n Data In Buffer with SCI. */
#define HIPM1CTL_ADDR     (PM1_ADDR_BASE + 0x0C)    /* Host Interface PM n Control. */
#define HIPM1CTL2_ADDR    (PM1_ADDR_BASE + 0x0D)    /* Host Interface PM n Control 2. */
#define HIPM1IC_ADDR      (PM1_ADDR_BASE + 0x0E)    /* Host Interface PM n Interrupt Control. */
#define HIPM1IE_ADDR      (PM1_ADDR_BASE + 0x10)    /* Host Interface PM n Interrupt Enable. */

#define HIPM2ST_ADDR      (PM2_ADDR_BASE + 0x00)    /* Host Interface PM n Status. */
#define HIPM2DO_ADDR      (PM2_ADDR_BASE + 0x02)    /* Host Interface PM n Data Out Buffer. */
#define HIPM2DI_ADDR      (PM2_ADDR_BASE + 0x04)    /* Host Interface PM n Data In Buffer. */
#define SHIP2nDI_ADDR     (PM2_ADDR_BASE + 0x05)    /* Host Interface PM n Shadow Data In Buffer. */
#define HIPM2DOC_ADDR     (PM2_ADDR_BASE + 0x06)    /* Host Interface PM n Data Out Buffer with SCI. */
#define HIPM2DOM_ADDR     (PM2_ADDR_BASE + 0x08)    /* Host Interface PM n Data Out Buffer with SMI. */
#define HIPM2DIC_ADDR     (PM2_ADDR_BASE + 0x0A)    /* Host Interface PM n Data In Buffer with SCI. */
#define HIPM2CTL_ADDR     (PM2_ADDR_BASE + 0x0C)    /* Host Interface PM n Control. */
#define HIPM2CTL2_ADDR    (PM2_ADDR_BASE + 0x0D)    /* Host Interface PM n Control 2. */
#define HIPM2IC_ADDR      (PM2_ADDR_BASE + 0x0E)    /* Host Interface PM n Interrupt Control. */
#define HIPM2IE_ADDR      (PM2_ADDR_BASE + 0x10)    /* Host Interface PM n Interrupt Enable. */

#define HIPM3ST_ADDR      (PM3_ADDR_BASE + 0x00)    /* Host Interface PM n Status. */
#define HIPM3DO_ADDR      (PM3_ADDR_BASE + 0x02)    /* Host Interface PM n Data Out Buffer. */
#define HIPM3DI_ADDR      (PM3_ADDR_BASE + 0x04)    /* Host Interface PM n Data In Buffer. */
#define SHIP3nDI_ADDR     (PM3_ADDR_BASE + 0x05)    /* Host Interface PM n Shadow Data In Buffer. */
#define HIPM3DOC_ADDR     (PM3_ADDR_BASE + 0x06)    /* Host Interface PM n Data Out Buffer with SCI. */
#define HIPM3DOM_ADDR     (PM3_ADDR_BASE + 0x08)    /* Host Interface PM n Data Out Buffer with SMI. */
#define HIPM3DIC_ADDR     (PM3_ADDR_BASE + 0x0A)    /* Host Interface PM n Data In Buffer with SCI. */
#define HIPM3CTL_ADDR     (PM3_ADDR_BASE + 0x0C)    /* Host Interface PM n Control. */
#define HIPM3CTL2_ADDR    (PM3_ADDR_BASE + 0x0D)    /* Host Interface PM n Control 2. */
#define HIPM3IC_ADDR      (PM3_ADDR_BASE + 0x0E)    /* Host Interface PM n Interrupt Control. */
#define HIPM3IE_ADDR      (PM3_ADDR_BASE + 0x10)    /* Host Interface PM n Interrupt Enable. */

#define HIPM4ST_ADDR      (PM4_ADDR_BASE + 0x00)    /* Host Interface PM n Status. */
#define HIPM4DO_ADDR      (PM4_ADDR_BASE + 0x02)    /* Host Interface PM n Data Out Buffer. */
#define HIPM4DI_ADDR      (PM4_ADDR_BASE + 0x04)    /* Host Interface PM n Data In Buffer. */
#define SHIP4nDI_ADDR     (PM4_ADDR_BASE + 0x05)    /* Host Interface PM n Shadow Data In Buffer. */
#define HIPM4DOC_ADDR     (PM4_ADDR_BASE + 0x06)    /* Host Interface PM n Data Out Buffer with SCI. */
#define HIPM4DOM_ADDR     (PM4_ADDR_BASE + 0x08)    /* Host Interface PM n Data Out Buffer with SMI. */
#define HIPM4DIC_ADDR     (PM4_ADDR_BASE + 0x0A)    /* Host Interface PM n Data In Buffer with SCI. */
#define HIPM4CTL_ADDR     (PM4_ADDR_BASE + 0x0C)    /* Host Interface PM n Control. */
#define HIPM4CTL2_ADDR    (PM4_ADDR_BASE + 0x0D)    /* Host Interface PM n Control 2. */
#define HIPM4IC_ADDR      (PM4_ADDR_BASE + 0x0E)    /* Host Interface PM n Interrupt Control. */
#define HIPM4IE_ADDR      (PM4_ADDR_BASE + 0x10)    /* Host Interface PM n Interrupt Enable. */

/* Host Interface PM n Status. */
#define HIPM1ST     PORT_BYTE   HIPM1ST_ADDR
#define HIPM2ST     PORT_BYTE   HIPM2ST_ADDR
#define HIPM3ST     PORT_BYTE   HIPM3ST_ADDR
#define HIPM4ST     PORT_BYTE   HIPM4ST_ADDR
/* --------------------------------------- */
#define HIPMnCTL_IBFIE   (1 << 0)   /* Input Buffer Full Interrupt Enable. */
#define HIPMnCTL_OBEIE   (1 << 1)   /* Output Buffer Empty Interrupt Enable. */
#define HIPMnCTL_PLMS    (7 << 3)   /* Pulse Level Mode SCI. */
#define HIPMnCTL_SCIPOL  (1 << 6)   /* SCI Negative Polarity. */
#define HIPMnCTL_EME     (1 << 7)   /* Enhance Mode Enable. */

/* Status register definitions for secondary Host interface for ACPI. */
#define maskSTATUS_PORT2_BURST  (1 << 4) /* Burst mode flag. */
#define maskSTATUS_PORT2_SCIEVT (1 << 5) /* SCI event flag. */
#define maskSTATUS_PORT2_SMIEVT (1 << 6) /* SMI event flag. */

/* Host Interface PM n Data Out Buffer. */
#define HIPM1DO     PORT_BYTE   HIPM1DO_ADDR
#define HIPM2DO     PORT_BYTE   HIPM2DO_ADDR
#define HIPM3DO     PORT_BYTE   HIPM3DO_ADDR
#define HIPM4DO     PORT_BYTE   HIPM4DO_ADDR
/* Host Interface PM n Data In Buffer. */
#define HIPM1DI     PORT_BYTE   HIPM1DI_ADDR
#define HIPM2DI     PORT_BYTE   HIPM2DI_ADDR
#define HIPM3DI     PORT_BYTE   HIPM3DI_ADDR
#define HIPM4DI     PORT_BYTE   HIPM4DI_ADDR
/* Host Interface PM n Shadow Data In Buffer. */
#define SHIPM1DI    PORT_BYTE   SHIPM1DI_ADDR
#define SHIPM2DI    PORT_BYTE   SHIPM2DI_ADDR
#define SHIPM3DI    PORT_BYTE   SHIPM3DI_ADDR
#define SHIPM4DI    PORT_BYTE   SHIPM4DI_ADDR
/* Host Interface PM n Data Out Buffer with SCI. */
#define HIPM1DOC    PORT_BYTE   HIPM1DOC_ADDR
#define HIPM2DOC    PORT_BYTE   HIPM2DOC_ADDR
#define HIPM3DOC    PORT_BYTE   HIPM3DOC_ADDR
#define HIPM4DOC    PORT_BYTE   HIPM4DOC_ADDR
/* Host Interface PM n Data Out Buffer with SMI. */
#define HIPM1DOM    PORT_BYTE   HIPM1DOM_ADDR
#define HIPM2DOM    PORT_BYTE   HIPM2DOM_ADDR
#define HIPM3DOM    PORT_BYTE   HIPM3DOM_ADDR
#define HIPM4DOM    PORT_BYTE   HIPM4DOM_ADDR
/* Host Interface PM n Data In Buffer with SCI. */
#define HIPM1DIC    PORT_BYTE   HIPM1DIC_ADDR
#define HIPM2DIC    PORT_BYTE   HIPM2DIC_ADDR
#define HIPM3DIC    PORT_BYTE   HIPM3DIC_ADDR
#define HIPM4DIC    PORT_BYTE   HIPM4DIC_ADDR
/* Host Interface PM n Control. */
#define HIPM1CTL    PORT_BYTE   HIPM1CTL_ADDR
#define HIPM2CTL    PORT_BYTE   HIPM2CTL_ADDR
#define HIPM3CTL    PORT_BYTE   HIPM3CTL_ADDR
#define HIPM4CTL    PORT_BYTE   HIPM4CTL_ADDR

#define HIPMnCTL_OBEIE   (1 << 1)   /* Output Buffer Empty Interrupt Enable. */
#define HIPMnCTL_PLMS    (7 << 3)   /* Pulse Level Mode SCI. */
#define HIPMnCTL_SCIPOL  (1 << 6)   /* SCI Negative Polarity. */
#define HIPMnCTL_EME     (1 << 7)   /* Enhance Mode Enable. */

/* Host Interface PM n Control 2. */
#define HIPM1CTL2   PORT_BYTE   HIPM1CTL2_ADDR
#define HIPM2CTL2   PORT_BYTE   HIPM2CTL2_ADDR
#define HIPM3CTL2   PORT_BYTE   HIPM3CTL2_ADDR
#define HIPM4CTL2   PORT_BYTE   HIPM4CTL2_ADDR

#define HIPMnCTL2_FW_IBF    (1 << 0)
#define HIPMnCTL2_FW_OBF    (1 << 1)
#define HIPMnCTL2_FW_FLAG   (1 << 2)
#define SCIPOL              (1 << 6)

/* Host Interface PM n Interrupt Control. */
#define HIPM1IC     PORT_BYTE   HIPM1IC_ADDR
#define HIPM2IC     PORT_BYTE   HIPM2IC_ADDR
#define HIPM3IC     PORT_BYTE   HIPM3IC_ADDR
#define HIPM4IC     PORT_BYTE   HIPM4IC_ADDR

#define HIPMnIC_IRQB     (1 << 0)   /* Host Interrupt Request Control Bit. */
#define HIPMnIC_SMIB     (1 << 1)   /* Host SMI Request Control Bit. */
#define HIPMnIC_SCIB     (1 << 2)    /* Host SCI Request Control Bit. */
#define HIPMNIC_PLMM     (7 << 3)   /* Pulse Level Mode SMI. */
#define HIPMnIC_SMIPOL   (1 << 6)   /* SMI Negative Pollarity. */
#define HIPMnIC_SCIIS    (1 << 7)   /* SCI on IBF Start. */
#define SMIPOL           (1 << 6)

/* Host Interface PM n Interrupt Enable. */
#define HIPM1IE     PORT_BYTE   HIPM1IE_ADDR
#define HIPM2IE     PORT_BYTE   HIPM2IE_ADDR
#define HIPM3IE     PORT_BYTE   HIPM3IE_ADDR
#define HIPM4IE     PORT_BYTE   HIPM4IE_ADDR

#define HSMIE       ( 1 << 5)
#define HSCIE       ( 1 << 4)
#define IESCIE      ( 1 << 1)
#define IESMIE      ( 1 << 2)

#define STATUS_PORT  PORT_BYTE HIKMST_ADDR /* Register (read/write). */
#define STATUS_PORT2 PORT_BYTE HIPM1ST_ADDR /* Register (read/write). */
#define STATUS_PORT3 PORT_BYTE HIPM2ST_ADDR /* Register (read/write). */
#define STATUS_PORT4 PORT_BYTE HIPM3ST_ADDR /* Register (read/write). */
#define STATUS_PORT5 PORT_BYTE HIPM4ST_ADDR /* Register (read/write). */


/* Core Access to Host Module Registers */
#define SIB_ADDR_BASE 0xFFF740
#define IHIOA_ADDR      (SIB_ADDR_BASE + 0x00)  /* Indirect Host I/O Address. */
#define IHD_ADDR        (SIB_ADDR_BASE + 0x02)  /* Indirect Host Data. */
#define LKSIOHA_ADDR    (SIB_ADDR_BASE + 0x04)  /* Lock Host Access. */
#define SIOLV_ADDR      (SIB_ADDR_BASE + 0x06)  /* Access Lock Violation. */
#define CRSMAE_ADDR     (SIB_ADDR_BASE + 0x08)  /* Core-to-Host Modules Access Enable. */
#define SIBCTRL_ADDR    (SIB_ADDR_BASE + 0x0A)  /* Module Control. */

/* Indirect Host I/O Address. */
#define IHIOA       PORT_WORD   IHIOA_ADDR
#define OFFSET_P         0x0
#define OFFSET_S         0x8
/* Indirect Host Data. */
#define IHD         PORT_BYTE   IHD_ADDR
/* Lock Host Access. */
#define LKSIOHA     PORT_WORD   LKSIOHA_ADDR

#define LKCFG       (1 << 0)    /* Lock Configuration Registers Host Access. */
#define LKSP2HA     (1 << 1)    /* Lock CIR PORT 2 Host Access. */
#define LKSP1HA     (1 << 2)    /* Lock CIR PORT 1 Host Access. */
#define LKESMEM     (1 << 3)    /* Lock Extended Shared Memory Host Access. */
#define LKMSWC      (1 << 8)    /* Lock MSWC Host Access. */
#define LKSMEM      (1 << 10)   /* Lock Shared Memoery Host Access. */

/* Access Lock Violation. */
#define SIOLV       PORT_WORD   SIOLV_ADDR

#define CFGLV   (1 << 0)    /* Configuration Register Lock Violation. */
#define SP2LV   (1 << 1)    /* CIR Port 2 Lock violation. */
#define SP1LV   (1 << 2)    /* CIR Port 1 Lock violation. */       
#define ESMEMLV (1 << 3)    /* External Shared Memory Lock Violation. */
#define MSWCLV  (1 << 8)    /* MSWC Lock Violation. */
#define SMEMLV  (1 << 10)   /* Shared Memory Lock Violation. */

/* Core-to-Host Modules Access Enable. */
#define CRSMAE      PORT_WORD   CRSMAE_ADDR

#define CFGAE   (1 << 0)    /* Configuration Register Core Access Enable. */
#define SP2AE   (1 << 1)    /* Serial Port 2 (SP2) Core Access Enable. */        
#define SP1AE   (1 << 2)    /* Serial Port 1 (SP1) Core Access Enable. */
#define ESMEMAE (1 << 3)    /* ESHM Core Access Enable. */
#define MSWCAE  (1 << 8)    /* MSWC Core Access Enable. */
#define SMEMAE  (1 << 10)   /* Shared Memory Core Access Enable. */

/* Module Control. */
#define SIBCTRL     PORT_BYTE   SIBCTRL_ADDR

#define CSAE	(1 << 0)    /* Core to SIB Access Enabled. */
#define CSRD	(1 << 1)    /* Core Read from SIB. */
#define CSWR	(1 << 2)    /* Core Write to SIB. */


/* Shared Memory Contrl */
#define SHM_ADDR_BASE     0xFF0400
#define SMC_STS_ADDR      (SHM_ADDR_BASE + 0x00)  /* Shared Memory Core Status. (R/W1C)*/
#define SMC_CTRL_ADDR     (SHM_ADDR_BASE + 0x01)  /* Shared Memory Core Control. */
#define STALL_CTL_ADDR    (SHM_ADDR_BASE + 0x02)  /* Stall Host Control. */
#define WIN1_WR_PROT_ADDR (SHM_ADDR_BASE + 0x06)  /* Shared Memory Window 1 Write Protet. */
#define WIN_SIZE_ADDR     (SHM_ADDR_BASE + 0x07)  /* RAM Windows Size. */
#define SHAW1_SEM_ADDR    (SHM_ADDR_BASE + 0x08)  /* Shared Access Window 1, Semaphore1. */
#define SHAW2_SEM_ADDR    (SHM_ADDR_BASE + 0x09)  /* Shared Access Window 2, Semaphore2. */
#define WIN_BASE1_ADDR    (SHM_ADDR_BASE + 0x0A)  /* RAM Windows 1 Base Address. */
#define WIN_BASE2_ADDR    (SHM_ADDR_BASE + 0x0C)  /* RAM Windows 2 Base Address. */
#define SHCFG_ADDR        (SHM_ADDR_BASE + 0x0E)  /* Shared Memory Configuration. */
#define RST_CFG_ADDR      (SHM_ADDR_BASE + 0x3A)  /* Reset Configuration. */
#define DP80BUF3_ADDR     (SHM_ADDR_BASE + 0x3E)  /* Debug Port 80 Buffered Data 3. */
#define DP80BUF_ADDR      (SHM_ADDR_BASE + 0x40)  /* Debug Port 80 Buffered Data. */
#define DP80STS_ADDR      (SHM_ADDR_BASE + 0x42)  /* Debug Port 80 Status. */
#define DP80CTL_ADDR      (SHM_ADDR_BASE + 0x44)  /* Debug Port 80 Control. */
#define DP80BUF1_ADDR     (SHM_ADDR_BASE + 0x46)  /* Debug Port 80 Buffered Data 1. */
#define DP80BUF2_ADDR     (SHM_ADDR_BASE + 0x47)  /* Debug Port 80 Buffered Data 2. */
#define HOFS_STS_ADDR     (SHM_ADDR_BASE + 0x48)  /* Host_Offset in Windows 1, 2 Status. */
#define HOFS_CTL_ADDR     (SHM_ADDR_BASE + 0x49)  /* Host_Offset in Windows 1, 2 Control. */
#define COFS2_ADDR        (SHM_ADDR_BASE + 0x4A)  /* Core_Offset in Window 2 Address. */
#define COFS1_ADDR        (SHM_ADDR_BASE + 0x4C)  /* Core_Offset in Window 1 Address. */
#define WIN2_WR_PORT_ADDR (SHM_ADDR_BASE + 0x54)  /* Shared Memeory Window 2 Write Protect. */
#define WIN2_RD_PORT_ADDR (SHM_ADDR_BASE + 0x55)  /* Shared Memeory Window 2 Read Protect. */
#define WIN3_WR_PORT_ADDR (SHM_ADDR_BASE + 0x56)  /* Shared Memeory Window 3 Write Protect. */ 
#define WIN3_RD_PORT_ADDR (SHM_ADDR_BASE + 0x57)  /* Shared Memeory Window 3 Read Protect. */ 
#define WIN4_WR_PORT_ADDR (SHM_ADDR_BASE + 0x58)  /* Shared Memeory Window 4 Write Protect. */
#define WIN4_RD_PORT_ADDR (SHM_ADDR_BASE + 0x59)  /* Shared Memeory Window 4 Read Protect. */ 
#define WIN5_WR_PORT_ADDR (SHM_ADDR_BASE + 0x5A)  /* Shared Memeory Window 5 Write Protect. */
#define WIN5_RD_PORT_ADDR (SHM_ADDR_BASE + 0x5B)  /* Shared Memeory Window 5 Read Protect. */ 
#define SMCE_STS_ADDR     (SHM_ADDR_BASE + 0x60)  /* Extended Shared Memory Core Status. */
#define SMCE_CTL_ADDR     (SHM_ADDR_BASE + 0x61)  /* Extended Shared Memory Core Control. */
#define WIN1_RD_PORT_ADDR (SHM_ADDR_BASE + 0x62)  /* Shared Memeory Window 1 Read Protect. */
#define WINE_SIZE_ADDR    (SHM_ADDR_BASE + 0x63)  /* Extended RAM Windows Size. */
#define WIN_BASE3_ADDR    (SHM_ADDR_BASE + 0x64)  /* RAM Window 3 Base. */
#define COFS_AIM_ADDR     (SHM_ADDR_BASE + 0x66)  /* Core_Offset in Additional Indirect Memory. */
#define ESHCFG_ADDR       (SHM_ADDR_BASE + 0x67)  /* Extended Shared Memory Configuration. */
#define HOFSE_STS_ADDR    (SHM_ADDR_BASE + 0x68)  /* Host_Offset in Window 3 Status. */
#define HOFSE_CTL_ADDR    (SHM_ADDR_BASE + 0x69)  /* Host_Offset in Window 3 Control. */
#define COFS3_ADDR        (SHM_ADDR_BASE + 0x6A)  /* Core_Offset in Window 3 Address. */
#define AMISE_STS_ADDR    (SHM_ADDR_BASE + 0x6C)  /* Additional Memory Indirect Access Status. */
#define AMISE_CTL_ADDR    (SHM_ADDR_BASE + 0x6D)  /* Additional Memory Indirect Access Control. */
#define SHAW3_SEM_ADDR    (SHM_ADDR_BASE + 0x70)  /* Shared Access Window 3, Semaphore3. */
#define SHAW4_SEM_ADDR    (SHM_ADDR_BASE + 0x71)  /* Shared Access Window 4, Semaphore4. */ 
#define WIN_BASE4_ADDR    (SHM_ADDR_BASE + 0x72)  /* RAM Window 4 Base. */
#define COFS4_ADDR        (SHM_ADDR_BASE + 0x74)  /* Core_Offset in Window 4 Address. */
#define WINE2_SIZE_ADDR   (SHM_ADDR_BASE + 0x79)  /* Extended 2 RAM Windows Size. */
#define SHAW5_SEM_ADDR    (SHM_ADDR_BASE + 0x7A)  /* Shared Access Window 5, Semaphore5. */ 
#define WIN_BASE5_ADDR    (SHM_ADDR_BASE + 0x7C)  /* RAM Window 5 Base. */
#define COFS5_ADDR        (SHM_ADDR_BASE + 0x80)  /* Core_Offset Window 5 Address. */
#define IHOFS5_ADDR       (SHM_ADDR_BASE + 0x82)  /* Host_Offset in Window 5 Address. */


/* Shared Memory Core Status. */
#define SMC_STS     PORT_BYTE   SMC_STS_ADDR
/* --------------------------------------- */
#define SHM_ACC          (1 << 6)
#define HSEM2W           (1 << 5)
#define HSEM1W           (1 << 4)
#define HWERR            (1 << 1)
#define HRERR            (1 << 0)

/* Extended Shared Memory Core Status. */
#define SMCE_STS    PORT_BYTE   SMCE_STS_ADDR
/* --------------------------------------- */ 
#define HSEM5W           (1 << 6)             
#define HSEM4W           (1 << 5)             
#define HSEM3W           (1 << 4)             

/* Shared Memory Core Control. */
#define SMC_CTRL    PORT_BYTE   SMC_CTRL_ADDR
/* ---------------------------------------- */
#define HOSTWAIT        (1 << 7)
#define PREF_EN         (1 << 6)
#define SHM_ACC_IE      (1 << 5)
#define HSEM2_IE        (1 << 4)
#define HSEM1_IE        (1 << 3)
#define HERR_IE         (1 << 2)
#define HERES           (3 << 0)

#define CTL_HERES_P          0
#define CTL_HERES_S          2

/* Extended Shared Memory Core Control. */    
#define SMCE_CTL    PORT_BYTE   SMCE_CTL_ADDR
/* --------------------------------------- */
#define HSEM5_IE        (1 << 6)            
#define HSEM4_IE        (1 << 5)            
#define HSEM3_IE        (1 << 4)            

/* Stall Host Control */
#define STALL_CTL   PORT_BYTE   STALL_CTL_ADDR
#define FLASH_SIZE  PORT_BYTE   STALL_CTL_ADDR
/* ---------------------------------------- */
#define STALL_HOST      (1 << 6)

/* Shared Memory Window 1-5 Write Protect */
#define WIN_PROT     PORT_BYTE   WIN1_WR_PROT_ADDR
#define WIN2_PROT    PORT_BYTE   WIN2_WR_PROT_ADDR
#define WIN3_PROT    PORT_BYTE   WIN3_WR_PROT_ADDR
#define WIN4_PROT    PORT_BYTE   WIN4_WR_PROT_ADDR
#define WIN5_PROT    PORT_BYTE   WIN5_WR_PROT_ADDR
#define WIN1_WR_PROT PORT_BYTE   WIN1_WR_PROT_ADDR
#define WIN2_WR_PROT PORT_BYTE   WIN2_WR_PROT_ADDR
#define WIN3_WR_PROT PORT_BYTE   WIN3_WR_PROT_ADDR
#define WIN4_WR_PROT PORT_BYTE   WIN4_WR_PROT_ADDR
#define WIN5_WR_PROT PORT_BYTE   WIN5_WR_PROT_ADDR

/* Shared Memory Window 1-5 Read Protect */      
#define WIN1_RD_PROT PORT_BYTE   WIN1_RD_PROT_ADDR
#define WIN2_RD_PROT PORT_BYTE   WIN2_RD_PROT_ADDR
#define WIN3_RD_PROT PORT_BYTE   WIN3_RD_PROT_ADDR
#define WIN4_RD_PROT PORT_BYTE   WIN4_RD_PROT_ADDR
#define WIN5_RD_PROT PORT_BYTE   WIN5_RD_PROT_ADDR

/* RAM Windows Size. */
#define WIN_SIZE    PORT_BYTE   WIN_SIZE_ADDR
/* Extended RAM Windows Size. */
#define WINE_SIZE   PORT_BYTE   WINE_SIZE_ADDR
/* Extended RAM Windows 2 Size. */              
#define WINE2_SIZE  PORT_BYTE   WINE2_SIZE_ADDR

/* Shared Access Window 1, Semaphore1. */
#define SHAW1_SEM   PORT_BYTE   SHAW1_SEM_ADDR
/* Shared Access Window 2, Semaphore2. */
#define SHAW2_SEM   PORT_BYTE   SHAW2_SEM_ADDR
/* Shared Access Window 1, Semaphore1. */     
#define SHAW3_SEM   PORT_BYTE   SHAW3_SEM_ADDR
/* Shared Access Window 2, Semaphore2. */     
#define SHAW4_SEM   PORT_BYTE   SHAW4_SEM_ADDR
/* Shared Access Window 1, Semaphore1. */     
#define SHAW5_SEM   PORT_BYTE   SHAW5_SEM_ADDR

#define FU_SEM_EXE               (1 << 0)
#define FU_SEM_DBG               (1 << 5)
#define FU_SEM_ERR               (1 << 6)
#define FU_SEM_RDY               (1 << 7)

/* RAM Windows 1 Base Address. */
#define WIN_BASE1   PORT_WORD   WIN_BASE1_ADDR
/* RAM Windows 2 Base Address. */
#define WIN_BASE2   PORT_WORD   WIN_BASE2_ADDR
/* RAM Windows 3 Base Address. */             
#define WIN_BASE3   PORT_WORD   WIN_BASE3_ADDR
/* RAM Windows 4 Base Address. */             
#define WIN_BASE4   PORT_WORD   WIN_BASE4_ADDR
/* RAM Windows 5 Base Address. */             
#define WIN_BASE5   PORT_WORD   WIN_BASE5_ADDR

/* Shared MEmeory Configuration */
#define SHCFG       PORT_BYTE   SHCFG_ADDR
/* ---------------------------------------- */
#define IMAEN               (1 << 0)
#define FL_SIZE_EX          (1 << 4)
#define SEMW2_DIS           (1 << 6)
#define SEMW1_DIS           (1 << 7)

/* Extended Shared Memory Configuration. */
#define ESHCFG      PORT_BYTE   ESHCFG_ADDR
/* ---------------------------------------- */
#define SEMW5_DIS           (1 << 2)
#define SEMW4_DIS           (1 << 1)
#define SEMW3_DIS           (1 << 0)

/* Reset Configuration */
#define RST_CFG     PORT_BYTE   RST_CFG_ADDR
/* ---------------------------------------- */
#define LRESET_DIS          (1 << 7)

/* Debug Port 80 Buffered Data. */
#define DP80BUF         PORT_WORD   DP80BUF_ADDR
/* Debug Port 80 Status. */
#define DP80STS         PORT_BYTE   DP80STS_ADDR
/* -------------------------------------------- */
#define FWR                 (1 << 5)
#define FNE                 (1 << 6)
#define FOR                 (1 << 7)

/* Debug Port 80 Control. */
#define DP80CTL         PORT_BYTE   DP80CTL_ADDR
/* -------------------------------------------- */
#define DP80EN              (1 << 0)
#define SYNCEN              (1 << 1)
#define ADV                 (1 << 2)
#define RAA                 (1 << 3)
#define RFIFO               (1 << 4)
#define CIEN                (1 << 5)

/* Debug Port 80 Buffered Data 1 */
#define DP80BUF1        PORT_BYTE   DP80BUF1_ADDR
/* Debug Port 80 Buffered Data 2 */
#define DP80BUF2        PORT_BYTE   DP80BUF2_ADDR
/* Debug Port 80 Buffered Data 3 */
#define DP80BUF3        PORT_BYTE   DP80BUF3_ADDR

/* Host_Offset in Windows 1, 2 Status. */
#define HOFS_STS        PORT_BYTE   HOFS_STS_ADDR
/* -------------------------------------------- */
#define HOFS1R               (1 << 0)
#define HOFS1W               (1 << 1)
#define HOFS2R               (1 << 2)
#define HOFS2W               (1 << 3)

/* Host_Offset in Windows 3, 4, 5 Status. */
#define HOFSE_STS       PORT_BYTE   HOFSE_STS_ADDR
/* -------------------------------------------- */
#define HOFS3R               (1 << 0)
#define HOFS3W               (1 << 1)
#define HOFS4R               (1 << 2)
#define HOFS4W               (1 << 3)
#define HOFS5R               (1 << 4)
#define HOFS5W               (1 << 5)

/* Additional Memory Indirect Access Status */
#define AMISE_STS       PORT_BYTE   AMISE_STS_ADDR
/* -------------------------------------------- */
#define HAMILW               (1 << 0)

/* Host_Offset in Windows 1, 2 Control. */
#define HOFS_CTL        PORT_BYTE   HOFS_CTL_ADDR
/* -------------------------------------------- */
#define HOFS1R_IE            (1 << 0)
#define HOFS1W_IE            (1 << 1)
#define HOFS2R_IE            (1 << 2)
#define HOFS2W_IE            (1 << 3)

/* Host_Offset in Windows 3, 4, 5 Control. */
#define HOFSE_CTL       PORT_BYTE   HOFSE_CTL_ADDR
/* -------------------------------------------- */
#define HOFS3R_IE            (1 << 0)
#define HOFS3W_IE            (1 << 1)
#define HOFS4R_IE            (1 << 2)
#define HOFS4W_IE            (1 << 3)
#define HOFS5R_IE            (1 << 4)
#define HOFS5W_IE            (1 << 5)

/* Additional Memory Indirect Access Status */
#define AMISE_CTL       PORT_BYTE   AMISE_CTL_ADDR
/* -------------------------------------------- */
#define HAMILW_IE            (1 << 0)

/* Additional Memory Indirect Access Status */
#define COFS_AIM        PORT_BYTE   COFS_AMI_ADDR

/* Core_Offset in Window 2 Address. */
#define COFS2           PORT_WORD   COFS2_ADDR
/* Core_Offset in Window 1 Address. */
#define COFS1           PORT_WORD   COFS1_ADDR
/* Core_Offset in Window 3 Address. */
#define COFS3           PORT_WORD   COFS3_ADDR
/* Core_Offset in Window 4 Address. */
#define COFS4           PORT_WORD   COFS4_ADDR
/* Core_Offset in Window 5 Address. */
#define COFS5           PORT_WORD   COFS5_ADDR

/* WIN_PROT register bit fields */
#define WIN_PROT_RWIN1_P      0
#define WIN_PROT_RWIN1_S      4
#define WIN_PROT_RWIN2_P      4
#define WIN_PROT_RWIN2_S      4

/* WIN_SIZE register bit fields */
#define WIN_SIZE_RWIN1_SIZE_P 0
#define WIN_SIZE_RWIN1_SIZE_S 4
#define WIN_SIZE_RWIN2_SIZE_P 4
#define WIN_SIZE_RWIN2_SIZE_S 4


/* Mobile System Wake-Up Control */
#define MSWC_ADDR_BASE  0xFFF040
#define MSWCTL1_ADDR    (MSWC_ADDR_BASE + 0x00)   /* MSWC Control Status Register1. */
#define MSWCTL2_ADDR    (MSWC_ADDR_BASE + 0x02)   /* MSWC Control Status Register2. */
#define HCFGBAL_ADDR    (MSWC_ADDR_BASE + 0x08)   /* Host Configuration Base Address Low. */
#define HCFGBAH_ADDR    (MSWC_ADDR_BASE + 0x0A)   /* Host Configuration Base Address High. */
#define MSIEN2_ADDR     (MSWC_ADDR_BASE + 0x0C) /* MSWC Interrupt Register2. */
#define MSHES0_ADDR     (MSWC_ADDR_BASE + 0x0E) /* MSWC Host Event Status Register0. */
#define MSHEIE0_ADDR    (MSWC_ADDR_BASE + 0x10) /* MSWC Host Event Interrupt Enable Register. */
#define HOST_CTL_ADDR   (MSWC_ADDR_BASE + 0x12) /* Host Control. */
#define SMIP_LEN_ADDR   (MSWC_ADDR_BASE + 0x14) /* SMI Pulse Length. */
#define SCIP_LEN_ADDR   (MSWC_ADDR_BASE + 0x16) /* SCI Pulse Length. */
#define SRID_CR_ADDR    (MSWC_ADDR_BASE + 0x1C) /* SRID Core Access. */
#define SID_CR_ADDR     (MSWC_ADDR_BASE + 0x20) /* SID Core Access. */
#define DID_CR_ADDR     (MSWC_ADDR_BASE + 0x22) /* DEVICE_ID Core Access. */

#define MSWCTL1     PORT_BYTE   MSWCTL1_ADDR
#define MSWCTL2     PORT_BYTE   MSWCTL2_ADDR
#define HCFGBAL     PORT_BYTE   HCFGBAL_ADDR
#define HCFGBAH     PORT_BYTE   HCFGBAH_ADDR
#define MSIEN2      PORT_BYTE   MSIEN2_ADDR
#define MSHES0      PORT_BYTE   MSHES0_ADDR
#define MSHEIE0     PORT_BYTE   MSHEIE0_ADDR
#define HOST_CTL    PORT_BYTE   HOST_CTL_ADDR
#define SMIP_LEN    PORT_BYTE   SMIP_LEN_ADDR
#define SCIP_LEN    PORT_BYTE   SCIP_LEN_ADDR
#define SRID_CR     PORT_BYTE   SRID_CR_ADDR
#define SID_CR      PORT_BYTE   SID_CR_ADDR
#define DID_CR      PORT_BYTE   DID_CR_ADDR


#define MSWCTL1_HRSTOB (1 << 0)
#define MSWCTL1_VHCFGA (1 << 3)


/* Return a non-zero value if setup for shared memory configuration. */
#define Shared_Memory_Cfg() (!(STRPST & STRPST_SHBM))

/* Return a non-zero value if External Memory bus width is 16 bit. */
#define Wide_Ext_Memory_Bus() (BUSS_WIDTH_PORT & BUSS_WIDTH_BIT)

/*------------------------------------------------------------------------------
 * MOBILE SYSTEM WAKE-UP CONTROL Host Registers
 *----------------------------------------------------------------------------*/

/* registers which are common to all banks */
#define WK_STS0               0
#define WK_EN0                2
#define WK_CFG                4
#define WK_SIGV               6
#define WK_STATE              7

/* WK_EN0 register bit fields */
// Same as MSHEIE0 bits

/* WK_CFG register bit fields */
#define CONF_BANK_SELECT_P    0
#define CONF_BANK_SELECT_S    2

/* Bank 1 */
#define MSWC_BANK_1           1

/* Bank 1 registers */
#define WOC_CTL               0x13
#define WOC_STS               0x14
#define WOC_EV_EN             0x15
#define WOC_CNTL              0x16
#define WOC_CNTH              0x17
#define WOC_INDEX             0x18
#define WOC_DATA              0x19
#define WOC_CSL               0x1A
#define WOC_CFG1              0x1B
#define WOC_CFG2              0x1C

/* WOC_EV_EN register bit fields */
#define MATCH_EN              0
#define END_EN                1
#define BUF_EN                2

/* WOC_STS register bit fields */
#define MATCH                 0
#define MSG_END               1
#define BUFF                  2
#define NEC_REP               4
#define BUFO                  6
#define ERR                   7

/* WOC_CTL register bit fields */
#define WOC_EN                0
#define RX_INV                3
#define PROT_SEL_P            4
#define PROT_SEL_S            2

/* WOC_INDEX register bit fields */
#define B_INDEX_P             0
#define B_INDEX_S             4
#define REG_SEL_P             4
#define REG_SEL_S             2

/* WOC_CSL register bit fields */
#define RC6_CSL_P             0
#define RC6_CSL_S             7

/* WOC_CFG1 register bit fields */
#define RC5_CELL_P            0
#define RC5_CELL_S            7

/* Bank 2 */
#define MSWC_BANK_2           2

/* Bank 2 registers */
#define WK_SMIEN0             0x13
#define WK_IRQEN0             0x15

/* WK_IRQEN0 register bit fields */
#define WOC_IRQ_EN            4

/*------------------------------------------------------------------------------
 * MOBILE SYSTEM WAKE-UP CONTROL Core Registers
 *----------------------------------------------------------------------------*/

/* MSWCTL1 register bit fields */
#define HPWRON                      (1 << 1)

/* MSWCTL2 register bit fields */
#define ACPIS0                      (1 << 0)
#define ACPIS1                      (1 << 1)
#define ACPIS2                      (1 << 2)
#define ACPIS3                      (1 << 3)
#define ACPIS4                      (1 << 4)
#define ACPIS5                      (1 << 5)
#define CFGPBM                      (1 << 6)
#define CFGPSO                      (1 << 7)

/* MSIEN2 register bit fields */
#define EIACPIS1                    (1 << 1)
#define EIACPIS2                    (1 << 2)
#define EIACPIS3                    (1 << 3)
#define EIACPIS4                    (1 << 4)
#define EIACPIS5                    (1 << 5)
#define EICFGPBM                    (1 << 6)
#define EICFGPSO                    (1 << 7)

/* MSHES0 register bit fields */
#define EWOC_EVENT_STATUS           (1 << 3)
#define WOC_EVENT_STATUS            (1 << 4)
#define MODULE_IRQ_EVENT_STATUS     (1 << 7)

/* MSHEIE0 register bit fields */
#define EWOC_EVENT_ENABLE           (1 << 3)
#define WOC_EVENT_ENABLE            (1 << 4)
#define EIR_CTS_EVENT_ENABLE        (1 << 5)
#define SW_EVENT_ENABLE             (1 << 6)
#define MODULE_IRQ_EVENT_ENABLE     (1 << 7)

#define EIR_CTS         3
#define EIR_GPS1        8
#define EIR_GPS2        9

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                           Enhanced Serial Peripheral Interface (eSPI) Core Registers                         */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Common and alias registers */
#define ESPI_BASE_ADDR                 	0xFF0800

#define ESPIID_ADDR                 (ESPI_BASE_ADDR + 0x000)
#define ESPICFG_ADDR                (ESPI_BASE_ADDR + 0x004)
#define ESPISTS_ADDR                (ESPI_BASE_ADDR + 0x008)
#define ESPIIE_ADDR                 (ESPI_BASE_ADDR + 0x00C)
#define ESPIWE_ADDR                 (ESPI_BASE_ADDR + 0x010)
#define VWREGIDX_ADDR               (ESPI_BASE_ADDR + 0x014)
#define VWREGDATA_ADDR              (ESPI_BASE_ADDR + 0x018)
#define OOBRXRDHEAD_ADDR            (ESPI_BASE_ADDR + 0x01C)
#define OOBTXWRHEAD_ADDR            (ESPI_BASE_ADDR + 0x020)
#define FLASHRXRDHEAD_ADDR          (ESPI_BASE_ADDR + 0x028)
#define FLASHTXWRHEAD_ADDR          (ESPI_BASE_ADDR + 0x02C)
#define ESPIERR_ADDR                (ESPI_BASE_ADDR + 0x03C)

#define ESPIID                 PORT_DWORD   ESPIID_ADDR
/*---------------------------------------------------------------------------------------------------------*/
/* ESPICFG register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPICFG                 PORT_DWORD   ESPICFG_ADDR
#define PCHANEN                 ((DWORD) 1 << 0)    /* Core Peripheral Channel Enable */ //0,  1
#define VWCHANEN                ((DWORD) 1 << 1)    /* Core Virtual Wire Channel Enable */ //1,  1
#define OOBCHANEN               ((DWORD) 1 << 2)    /* Core OOB Channel Enable */ //2,  1
#define FLASHCHANEN             ((DWORD) 1 << 3)    /* Core Flash Access Channel Enable */ //3,  1
#define HPCHANEN                ((DWORD) 1 << 4)    /* Host Peripheral Channel Enable */ //4,  1
#define HVWCHANEN               ((DWORD) 1 << 5)    /* Host Virtual Wire Channel Enable */ //5,  1
#define HOOBCHANEN              ((DWORD) 1 << 6)    /* Host OOB Channel Enable */ //6,  1
#define HFLASHCHANEN            ((DWORD) 1 << 7)    /* Host Flash Access Channel Enable */ //7,  1
#define mskIOMODE               ((DWORD) 0x03 << 8)    /* I/O mode support */ //8,  2
#define SINGLE_IO_SUPPORT       ((DWORD) 0 << 20)
#define DUAL_IO_SUPPORT         ((DWORD) 1 << 20)
#define QUAD_IO_SUPPORT         ((DWORD) 2 << 20)
#define ALL_IO_SUPPORT          ((DWORD) 3 << 8)
#define mskMAXFREQ              ((DWORD) 0x07 << 10)/* Maximum Frequency Supported */ //10, 3
#define MAXFREQ_20M             ((DWORD) 0 << 10)
#define MAXFREQ_25M             ((DWORD) 1 << 10)
#define MAXFREQ_33M             ((DWORD) 2 << 10)
#define MAXFREQ_50M             ((DWORD) 3 << 10)
#define MAXFREQ_66M             ((DWORD) 4 << 10)
#define mskOPFREQ               ((DWORD) 0x07 << 17)/* Operating Frequency */ //17, 3
#define OPFREQ_20M              ((DWORD) 0 << 17)
#define OPFREQ_25M              ((DWORD) 1 << 17)
#define OPFREQ_33M              ((DWORD) 2 << 17)
#define OPFREQ_50M              ((DWORD) 3 << 17)
#define OPFREQ_66M              ((DWORD) 4 << 17)
#define mskIOMODESEL            ((DWORD) 0x03 << 20) /* I/O Mode Select */ //20, 2
#define SINGLE_IO               ((DWORD) 0 << 20)
#define DUAL_IO                 ((DWORD) 1 << 20)
#define QUAD_IO                 ((DWORD) 2 << 20)
#define ALERTMODE               ((DWORD) 1 << 22)   /* ALERT Mode */ //22, 1
#define PCCHN_SUPP              ((DWORD) 1 << 24)   /* Peripheral Channel Supported */ //24, 1
#define VWCHN_SUPP              ((DWORD) 1 << 25)   /* Virtual Wire Channel Supported */ //25, 1
#define OOBCHN_SUPP             ((DWORD) 1 << 26)   /* OOB Channel Supported */ //26, 1
#define FLASHCHN_SUPP           ((DWORD) 1 << 27)   /* Flash Access Channel Supported */ //27, 1
#define CH_SUPP				    ((DWORD) 0x0F << 24)//24, 4
#define RSTO                    ((DWORD) 1 << 31)   /* Reset Output *///31, 1

/*---------------------------------------------------------------------------------------------------------*/
/* ESPISTS register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPISTS                 PORT_DWORD   ESPISTS_ADDR
#define IBRST                   ((DWORD) 1 << 0)    /* In-Band Reset Command Received */ //0,  1
#define CFGUPD                  ((DWORD) 1 << 1)    /* eSPI Configuration Updated */ //1,  1
#define BERR                    ((DWORD) 1 << 2)    /* eSPI Bus Error */ //2,  1
#define OOBRX                   ((DWORD) 1 << 3)    /* OOB Data Received */ //3,  1
#define FLASHRX                 ((DWORD) 1 << 4)    /* Flash Data Received */ //4,  1
#define PERACC                  ((DWORD) 1 << 6)    /* Peripheral Channel Access Detected */ //6,  1
#define DFRD                    ((DWORD) 1 << 7)    /* Peripheral Channel Transaction Deferred */ //7,  1
#define VWUPD                   ((DWORD) 1 << 8)    /* Virtual Wire Updated */ //8,  1
#define ESPIRST                 ((DWORD) 1 << 9)    /* eSPI_RST Activated */ //9,  1
#define PLTRST                  ((DWORD) 1 << 10)   /* PLTRST Activated */ //10, 1
#define VW1                     ((DWORD) 1 << 11)   /* VW1 ¡§Floating¡¨ Virtual Wire Event */ //11, 1
#define VW2                     ((DWORD) 1 << 12)   /* VW2 ¡§Floating¡¨ Virtual Wire Event */ //12, 1
#define VW3                     ((DWORD) 1 << 13)   /* VW3 ¡§Floating¡¨ Virtual Wire Event */ //13, 1
#define VW4                     ((DWORD) 1 << 14)   /* VW4 ¡§Floating¡¨ Virtual Wire Event */ //14, 1
#define AMERR                   ((DWORD) 1 << 15)   /* Automatic Mode Transfer Error */ //15, 1
#define AMDONE                  ((DWORD) 1 << 16)   /* Automatic Mode Transfer Done */ //16, 1
#define ESPIRST_DEASSERT        ((DWORD) 1 << 17)
#define PLTRST_DEASSERT         ((DWORD) 1 << 18)

/*---------------------------------------------------------------------------------------------------------*/
/* ESPIIE register fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIIE                  PORT_DWORD   ESPIIE_ADDR
#define IBRSTIE                 ((DWORD) 1 << 0)   /* IBRST Interrupt Enable */ //0,  1
#define CFGUPDIE                ((DWORD) 1 << 1)   /* CFGUPD Interrupt Enable */ //1,  1
#define BERRIE                  ((DWORD) 1 << 2)   /* BERR Interrupt Enable */ //2,  1
#define OOBRXIE                 ((DWORD) 1 << 3)   /* OOBRX Interrupt Enable */ //3,  1
#define FLASHRXIE               ((DWORD) 1 << 4)   /* FLASHRX Interrupt Enable */ //4,  1
#define PERACCIE                ((DWORD) 1 << 6)   /* PERACC Interrupt Enable */ //6,  1
#define DFRDIE                  ((DWORD) 1 << 7)   /* DFRD Interrupt Enable */ //7,  1
#define VWUPDIE                 ((DWORD) 1 << 8)   /* VWUPD Interrupt Enable */ //8,  1
#define ESPIRSTIE               ((DWORD) 1 << 9)   /* eSPI_RST Interrupt Enable */ //9,  1
#define PLTRSTIE                ((DWORD) 1 << 10)  /* PLTRST Interrupt Enable */ //10, 1
#define VW1IE                   ((DWORD) 1 << 11)  /* VW1 Interrupt Enable */ //11, 1
#define VW2IE                   ((DWORD) 1 << 12)  /* VW2 Interrupt Enable */ //12, 1
#define VW3IE                   ((DWORD) 1 << 13)  /* VW3 Interrupt Enable */ //13, 1
#define VW4IE                   ((DWORD) 1 << 14)  /* VW4 Interrupt Enable */ //14, 1
#define AMERRIE                 ((DWORD) 1 << 15)  /* AMERR Interrupt Enable */ //15, 1
#define AMDONEIE                ((DWORD) 1 << 16)  /* AMDONE Interrupt Enable */ //16, 1
#define ESPIRSTIE2              ((DWORD) 1 << 17)  /* eSPI_RST Interrupt Enable 2 */ //15, 1
#define PLTRSTIE2               ((DWORD) 1 << 18)  /* PLTRST Interrupt Enable 2 */ //16, 1

/*---------------------------------------------------------------------------------------------------------*/
/* ESPIWE (ESPI Wake-Up Enable) register fields                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIWE                  PORT_DWORD   ESPIWE_ADDR
#define IBRSTWE                 ((DWORD) 1 << 0)    /* IBRST Wake-Up Enable */  //0,  1
#define CFGUPDWE                ((DWORD) 1 << 1)    /* CFGUPD Wake-Up Enable */  //1,  1
#define BERRWE                  ((DWORD) 1 << 2)    /* BERR Wake-Up Enable */  //2,  1
#define OOBRXWE                 ((DWORD) 1 << 3)    /* OOBRX Wake-Up Enable */  //3,  1
#define FLASHRXWE               ((DWORD) 1 << 4)    /* FLASHRX Wake-Up Enable */  //4,  1
#define PERACCWE                ((DWORD) 1 << 6)    /* PERACC Wake-Up Enable */  //6,  1
#define DFRDWE                  ((DWORD) 1 << 7)    /* DFRD Wake-Up Enable */  //7,  1
#define VWUPDWE                 ((DWORD) 1 << 8)    /* VWUPD Wake-Up Enable */  //8,  1
#define ESPIRSTWE               ((DWORD) 1 << 9)    /* eSPI_RST Wake-Up Enable */  //9,  1

/*---------------------------------------------------------------------------------------------------------*/
/* VWREGIDX register fields                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define VWREGIDX                PORT_DWORD   VWREGIDX_ADDR
#define VWREGINDEX              ((VWREGIDX & 0xFFF) //2, 10

#define VWREGDATA               PORT_DWORD   VWREGDATA_ADDR

#define OOBRXRDHEAD             PORT_DWORD   OOBRXRDHEAD_ADDR
#define OOBTXWRHEAD             PORT_DWORD   OOBTXWRHEAD_ADDR
#define FLASHRXRDHEAD           PORT_DWORD   FLASHRXRDHEAD_ADDR
#define FLASHTXWRHEAD           PORT_DWORD   FLASHTXWRHEAD_ADDR
/*---------------------------------------------------------------------------------------------------------*/
/* ESPIERR register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIERR                 PORT_DWORD   ESPIERR_ADDR
#define INVCMD                  ((DWORD) 1 << 0)    /* Invalid Cycle Type */ //0,  1
#define INVCYC                  ((DWORD) 1 << 1)    /* Invalid Cycle Type */ //1,  1
#define CRCERR                  ((DWORD) 1 << 2)    /* Transaction CRC Error */ //2,  1
#define ABCOMP                  ((DWORD) 1 << 3)    /* Abnormal Completion */ //3,  1
#define PROTERR                 ((DWORD) 1 << 4)    /* Protocol Error */ //4,  1
#define BADSIZE                 ((DWORD) 1 << 5)    /* Bad Size */ //5,  1
#define NPBADALN                ((DWORD) 1 << 6)    /* Non-Posted Peripheral Channel Bad Address Alignment */ //6,  1
#define PCBADALN                ((DWORD) 1 << 7)    /* Posted Peripheral Channel Bad Address Alignment */ //7,  1
#define UNCMD                   ((DWORD) 1 << 9)    /* Unsupported Command or Cycle Type */ //9,  1
#define EXTRACYC                ((DWORD) 1 << 10)   /* Extra eSPI Clock Cycles */ //10, 1


/* Virtual Wire Channel Registers */
#define VWEVSM_ADDR(n)              (ESPI_BASE_ADDR + 0x100 + (n * 4))
#define VWEVMS_ADDR(n)              (ESPI_BASE_ADDR + 0x140 + (n * 4))

#define VWGPSM_ADDR(n)              (ESPI_BASE_ADDR + 0x180 + (n * 4))
#define VWGPMS_ADDR(n)              (ESPI_BASE_ADDR + 0x1C0 + (n * 4))

#define VWPING_ADDR                 (ESPI_BASE_ADDR + 0x2F8)
#define VWCTL_ADDR                  (ESPI_BASE_ADDR + 0x2FC)

/*---------------------------------------------------------------------------------------------------------*/
/* VWEVSM and VWGPSM register fields                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define VWEVSM(n)                   PORT_DWORD  VWEVSM_ADDR(n)
#define VWGPSM(n)                   PORT_DWORD  VWGPSM_ADDR(n)
#define mskWIRE_3_0                 ((DWORD)0x0F << 0)     /*  */
#define mskWIRE_3_0_VALID           ((DWORD)0x0F << 4)     /*  */
#define mskINDEX                    ((DWORD)0x7F << 8)     /* Index Value */
#define DIRTY                       ((DWORD)1 << 16)       /*  */
#define ENPLTRST                    ((DWORD)1 << 17)       /* Enable PLTRST */
#define ENCDRST                     ((DWORD)1 << 19)       /* Enable Core Reset */

/*---------------------------------------------------------------------------------------------------------*/
/* VWEVMS register fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define VWEVMS(n)                   PORT_DWORD  VWEVMS_ADDR(n)
#define VWGPMS(n)                   PORT_DWORD  VWGPMS_ADDR(n)
#define INDEX_EN                    ((DWORD) 1 << 15)       /* Index Enable */
#define MODIFIED                    ((DWORD) 1 << 16)       /*  */
#define IE_EN                       ((DWORD) 1 << 18)       /* (Interrupt/Wake-up Enable */


/*---------------------------------------------------------------------------------------------------------*/
/* VWPING register fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define VWPING                      PORT_DWORD  VWPING_ADDR
#define mskVWPING_DATA              ((DWORD) 0x0F << 0)     /*  */
#define mskVWPING_VALID             ((DWORD) 0x0F << 4)     /* DATA is Valid */
#define mskVWPING_INDEX             ((DWORD) 0xFF << 8)     /* Index Value */
#define VWPING_DIRTY                ((DWORD) 1 << 16)       /*  */

/*---------------------------------------------------------------------------------------------------------*/
/* VWCTL register fields                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define VWCTL                       PORT_DWORD  VWCTL_ADDR
#define mskINTWIN                   ((DWORD) 0x03 << 0)     /* Interrupt Window Select */
#define IRQ_LINE_15_0               ((DWORD) 0 << 0)
#define IRQ_LINE_31_16              ((DWORD) 1 << 0)
#define IRQ_LINE_47_32              ((DWORD) 2 << 0)
#define IRQ_LINE_63_48              ((DWORD) 3 << 0)

#define mskGPVWMAP                  ((DWORD) 0x03 << 2)     /* GPIO Virtual Wire Indices Mapping */
#define INDEX_128_159               ((DWORD) 0 << 2)
#define INDEX_160_191               ((DWORD) 1 << 2)
#define INDEX_192_223               ((DWORD) 2 << 2)
#define INDEX_224_255               ((DWORD) 3 << 2)

/* OOB Channel Registers */
#define OOBRXBUF_ADDR(n)            (ESPI_BASE_ADDR + 0x300 + (n * 4))
#define OOBTXBUF_ADDR(n)            (ESPI_BASE_ADDR + 0x380 + (n * 4))
#define OOBCTL_ADDR                 (ESPI_BASE_ADDR + 0x3FC)


#define OOBRXBUF(n)                 PORT_DWORD  OOBRXBUF_ADDR(n)
#define OOBTXBUF(n)                 PORT_DWORD  OOBTXBUF_ADDR(n)
/*---------------------------------------------------------------------------------------------------------*/
/* OOBCTL register fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define OOBCTL                      PORT_DWORD  OOBCTL_ADDR
#define OOB_FREE                    ((DWORD) 1 << 0)        /* OOB Receive Queue Free */  //0,  1
#define OOB_AVAIL                   ((DWORD) 1 << 1)        /* OOB Transmit Queue Available */  //1,  1
#define OOBCTL_RSTBUFHEADS          ((DWORD) 1 << 2)        /* Reset Buffer Heads */  //2,  1
#define mskOOBPLSIZE                ((DWORD) 0x07 << 10)    /* OOB Channel Maximum Payload Size */  //10, 3
#define OOBPLSIZE_64                ((DWORD) 1 << 10)
#define OOBPLSIZE_128               ((DWORD) 2 << 10)
#define OOBPLSIZE_256               ((DWORD) 3 << 10)

/* Flash Access Channel Registers */
#define FLASHRXBUF_ADDR(n)          (ESPI_BASE_ADDR + 0x400 + (n * 4))
#define FLASHTXBUF_ADDR(n)          (ESPI_BASE_ADDR + 0x480 + (n * 4))

#define FLASHCRC_ADDR               (ESPI_BASE_ADDR + 0x4F4)
#define FLASHCFG_ADDR               (ESPI_BASE_ADDR + 0x4F8)
#define FLASHCTL_ADDR               (ESPI_BASE_ADDR + 0x4FC)

#define FLASHRXBUF(n)               PORT_DWORD  FLASHRXBUF_ADDR(n)
#define FLASHTXBUF(n)               PORT_DWORD  FLASHTXBUF_ADDR(n)


#define FLASHCRC                    PORT_DWORD  FLASHCRC_ADDR
/*---------------------------------------------------------------------------------------------------------*/
/* FLASHCFG register fields                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASHCFG                PORT_DWORD  FLASHCFG_ADDR
#define mskFLASHBLERSSIZE       ((DWORD) 0x07 << 7)     /* Flash Access Channel Block Erase Size */ //7,  3
#define FLASHBLERS_4K           ((DWORD) 1 << 7)
#define FLASHBLERS_64K          ((DWORD) 2 << 7)
#define FLASHBLERS_4_64K        ((DWORD) 3 << 7)
#define FLASHBLERS_128K         ((DWORD) 4 << 7)
#define FLASHBLERS_256K         ((DWORD) 5 << 7)

#define mskFLASHPLSIZE          ((DWORD) 0x07 << 10)    /* Flash Access Channel Maximum Payload Size */ //10, 3
#define FLASHPL_64B             ((DWORD) 1 << 10)
#define FLASHPL_128B            ((DWORD) 2 << 10)
#define FLASHPL_256B            ((DWORD) 3 << 10)

#define mskFLASHREQSIZE         ((DWORD) 0x07 << 13)    /* Flash Access Channel Maximum Read Request Size */ //13, 3
#define FLASHREQ_64B            ((DWORD) 1 << 13)
#define FLASHREQ_128B           ((DWORD) 2 << 13)
#define FLASHREQ_256B           ((DWORD) 3 << 13)
#define FLASHREQ_512B           ((DWORD) 4 << 13)
#define FLASHREQ_1024B          ((DWORD) 5 << 13)
#define FLASHREQ_2048B          ((DWORD) 6 << 13)
#define FLASHREQ_4096B          ((DWORD) 7 << 13)


/*---------------------------------------------------------------------------------------------------------*/
/* FLASHCTL register fields                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASHCTL                PORT_DWORD  FLASHCTL_ADDR
#define FLASH_TX_AVAIL          ((DWORD) 1 << 1)    /* Flash Access Transmit Queue Available */ //1,  1
#define STRPHDR                 ((DWORD) 1 << 2)    /* Strip Header */ //2,  1
#define mskDMATHRESH            ((DWORD) 0x03 << 3)    /* DMA Request Threshold */ //3,  2
#define DMA_DISABLED            ((DWORD) 0 << 3)
#define DMATHRESH_4B            ((DWORD) 2 << 3)
#define DMATHRESH_16B           ((DWORD) 3 << 3)

#define mskAMTSIZE              ((DWORD) 0xFF << 5)    /* Automatic Mode Transfer Size */ //5,  8
#define FLASHCTL_RSTBUFHEADS    ((DWORD) 1 << 13)   /* Reset Buffer Heads */ //13, 1
#define CRCEN                   ((DWORD) 1 << 14)   /* CRC/Checksum Enable */ //14, 1
#define CHKSUMSEL               ((DWORD) 1 << 15)   /* Checksum Select */ //15, 1
#define AMTEN				    ((DWORD) 1 << 16)   /* Automatic Mode Enable */ //16, 1



/*------------------------------------------------------------------------------
 * Consumer Electronic Infra Red (CEIR) Registers
 *----------------------------------------------------------------------------*/
/* FIR specific Configuration registers */
#define FIR_CONF_REG        0xF0
/* FIR config register bit fields */
#define BANK_SELECT_EN      0x80
#define POWER_MODE_CTL      0x02

/* Bank 0 */
#define IR_BANK_0          0
/* Bank 0 registers */
#define RXD         0
#define IER         1
#define EIR         2
#define FCR         2
#define LCR         3
#define BSR         3
#define MCR         4
#define LSR         5
#define ASCR        7

/* PMCSR register bit fields */
#define BKSE_MASK   0xe0

/* Bank 2 */
#define IR_BANK_2           2
/* Bank 0 registers */
#define BGDL        0
#define BGDH        1
#define EXCR1       2
#define EXCR2       4

/* Bank 7 */
#define IR_BANK_7           7
#define IRRXDC      0
#define IRTXMC      1
#define RCCFG       2
#define IRCFG1      4
#define IRCFG4      7


/*------------------------------------------------------------------------------
 * CONFIG and CONTROL Host Registers
 *----------------------------------------------------------------------------*/
#define LDN         0x07    /* Logic Device Number selector. */
#define BASE_ADDR   0x60    /* Base Address MSB register. */
#define INT         0x70    /* Interrupt Number. */
#define INT_TYPE    0x71    /* Interrupt Type. */

#define LDN_CEIR    0x03    /* CEIR */
#define LDN_MSWC    0x04    /* MSWC */
#define LDN_MOUSE   0x05    /* Mouse */
#define LDN_KBD     0x06    /* Keyboard */
#define LDN_HGPIO   0x07    /* Host GPIO. */
#define LDN_SHM     0x0F    /* Shared Memory. */

#define SHM_CFG     0xF0    /* Shared Memory Configuration Register. */
#define WIN_CFG     0xF1    /* Shared Access Windows Configuration Register. */
#define SHAW1BA0    0xF4    /* Windows 1 Base address 0 register. */
#define SHAW1BA1    0xF5    /* Windows 1 Base address 1 register. */
#define SHAW1BA2    0xF6    /* Windows 1 Base address 2 register. */
#define SHAW1BA3    0xF7    /* Windows 1 Base address 3 register. */

#define SHAW2BA0    0xF8    /* Windows 2 Base address 0 register. */
#define SHAW2BA1    0xF9    /* Windows 2 Base address 1 register. */
#define SHAW2BA2    0xFA    /* Windows 2 Base address 2 register. */
#define SHAW2BA3    0xFB    /* Windows 2 Base address 3 register. */

#define LDN_PM1     0x11
#define LDN_PM2     0x12
#define LDN_PM3     0x17
#define LDN_PM4     0x1E

#define LDN_SP1     0x03
#define LDN_SP2     0x02

#define LDN_CIR_PORT2       0x02
#define LDN_CIR_PORT        0x03
#define LDN_MSWC            0x04
#define LDN_KBC_MOUSE       0x05
#define LDN_KBC_KEYBOARD    0x06
#define LDN_HOST_GPIO       0x07
#define LDN_SHARED_MEMORY   0x0F
#define LDN_PM_Channel1     0x11
#define LDN_PM_Channel2     0x12
#define LDN_PM_Channel3     0x17
#define LDN_ESHM            0x1D
#define LDN_PM_Channel4     0x1E

#endif /* ifndef REGS_H */

