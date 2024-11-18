/* ----------------------------------------------------------------------------
 * MODULE REGINIT.C
 *
 * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Contains initialization for registers.
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 20, 2000 by GLP)
 *
 * EDH
 * 25 Jan 05  Defined Device Alternate Function.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * Mark
 * 23 Apr 06  Revise some Register bits name and del some Register bits.
 * ------------------------------------------------------------------------- */

#include "swtchs.h"
#include "reginit.h"
#include "a_to_d.h"
#include "oem.h"
#include "ps2.h"
#include "ini.h"
#include "scan.h"
#include "d_to_a.h"
#include "i2c.h"

#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif

#ifndef EXT_GPIO44_WAKEUP
#error EXT_GPIO44_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO1_WAKEUP
#error EXT_GPIO1_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO46_WAKEUP
#error EXT_GPIO46_WAKEUP switch is not defined.
#endif

#ifndef EXT_GPIO3_WAKEUP
#error EXT_GPIO3_WAKEUP switch is not defined.
#endif

#ifndef I2C_SUPPORTED
#error I2C_SUPPORTED switch is not defined.
#endif

#define Enable  1
#define Disable 0

/* ---------------------------- DEVCNT OEM Functions ----------------------

      Standard definition of DEVCNT:

      Bit     Function
      --- -----------------
       0  Clock Out Mode
       1  External Clock Select
       2  PECI Pin Enable
       3  JTAG over KBSOUT, Select by Core
       4  JTAG over KBSOUT, Host Enable
       5  Reserved
       6  Flash SPI Interface TRI-STATE
       7  Reserved              	*/

#define DEVCNT_OEM  0x04

/* ---------------------------- RSTCTL OEM Functions ----------------------

      Standard definition of RSTCTL:

      Bit     Function
      --- -----------------
       0  EXT_RST Status
       1  Debugger Reset Status
       2  Reserved
       3  Reserved
       4  Software Reset Status
       5  LRESET Mode Select
       6  Host Interface Modules Reset Mode Select
       7  Debugger Reset Mode Select              	*/

#define RSTCTL_OEM  0x00

/* ---------------------------- DEV_CTL3 OEM Functions ----------------------

      Standard definition of DEV_CTL3:

      Bit     Function
      --- -----------------
       0  Write-Protect Over GPIO30
       1  Write-Protect Over GPIO41
       2  Write-Protect Over GPIO81
       3  Reserved
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved              	*/

#define DEV_CTL3_OEM  0x00




/* ---------------------------- DEVALT0 OEM Functions ----------------------

      Standard definition of DEVALT0:

      Bit     Function
      --- -----------------
       0  Reserved
       1  Reserved
       2  Clock Out Select
       3  SPI Select
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved              	*/

#define SSPI_SL          (Disable << 3)
#define SCKOUT_SL        (Disable << 2)
#define DEVALT0_OEM     (SSPI_SL  | SCKOUT_SL)

/* ---------------------------- DEVALT1 OEM Functions ----------------------

      Standard definition of DEVALT1:

      Bit     Function
      --- -----------------
       0  Reserved
       1  CR_UART Input Select
       2  CR_UART Output 1 Select
       3  CR_UART Output 2 Select
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved 	*/

#define SURTI_SL      (Disable << 1)
#define SURTO1_SL     (Disable << 2)
#define SURTO2_SL     (Disable << 3)

#define DEVALT1_OEM (SURTI_SL | SURTO1_SL | SURTO2_SL)

/* ---------------------------- DEVALT2 OEM Functions ----------------------

      Standard definition of DEVALT2:

      Bit     Function
      --- -----------------
       0  Reserved
       1  LPCPD Select
       2  CLKRUN Select
       3  SMI Select
       4  Reserved
       5  ECSCI Select
       6  SMBus1 Select
       7  SMBus2 Select              	*/

#define SLPCPD_SL    (Disable << 1)
#define SCLKRN_SL    (Enable << 2)
#define SSMI_SL      (Enable  << 3)
#define SECSCI_SL    (Enable  << 5)
#define SSMBUS1_SL   (Enable  << 6)
#define SSMBUS2_SL   (Enable  << 7)

#define DEVALT2_OEM (SLPCPD_SL | SCLKRN_SL | SSMI_SL | SECSCI_SL | SSMBUS1_SL | SSMBUS2_SL)
/* ---------------------------- DEVALT3 OEM Functions ----------------------

      Standard definition of DEVALT3:

      Bit     Function
      --- -----------------
       0  TA1 Select
       1  TA2 Select
       2  TB1 Select
       3  TB2 Select
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved              	*/

#define STA1_SL      (Disable << 0)
#define STA2_SL      (Disable << 1)
#define STB1_SL      (Disable << 2)
#define STB2_SL      (Disable << 3)

#define DEVALT3_OEM (STA1_SL | STA2_SL | STB1_SL | STB2_SL)

/* ---------------------------- DEVALT4 OEM Functions ----------------------

      Standard definition of DEVALT4:

      Bit     Function
      --- -----------------
       0  DAC0 Select
       1  DAC1 Select
       2  DAC2 Select
       3  DAC3 Select
       4  PS/2 #3 Select
       5  PS/2 #1 Select
       6  PS/2 #2 Select
       7  Reserved              	*/

#define SDAC0_SL     (Enable << 0)
#define SDAC1_SL     (Disable << 1)
#define SDAC2_SL     (Disable << 2)
#define SDAC3_SL     (Disable << 3)
#if AUX_PORT3_SUPPORTED
#define SPS2_3_SL    (Enable << 4)
#else
#define SPS2_3_SL    (Disable << 4)
#endif
#define SPS2_1_SL    (Enable  << 5)
#if AUX_PORT2_SUPPORTED
#define SPS2_2_SL    (Enable << 6)
#else
#define SPS2_2_SL    (Disable << 6)
#endif

#define DEVALT4_OEM (SDAC0_SL | SDAC1_SL | SDAC2_SL | SDAC3_SL | SPS2_1_SL | SPS2_2_SL | SPS2_3_SL)

/* ---------------------------- DEVALT5 OEM Functions ----------------------

      Standard definition of DEVALT5:

      Bit     Function
      --- -----------------
       0  A_PWM Select
       1  B_PWM Select
       2  C_PWM Select
       3  D_PWM Select
       4  E_PWM Select
       5  F_PWM Select
       6  G_PWM Select
       7  H_PWM Select         	*/

#define SA_PWM_SL     (Disable << 0)
#define SB_PWM_SL     (Disable << 1)
#define SC_PWM_SL     (Disable << 2)
#define SD_PWM_SL     (Disable << 3)
#define SE_PWM_SL     (Disable << 4)
#define SF_PWM_SL     (Disable << 5)
#define SG_PWM_SL     (Disable << 6)
#define SH_PWM_SL     (Disable << 7)

#define DEVALT5_OEM (SA_PWM_SL | SB_PWM_SL | SC_PWM_SL | SD_PWM_SL | SE_PWM_SL \
                         | SF_PWM_SL | SG_PWM_SL | SH_PWM_SL)

/* ---------------------------- DEVALT6 OEM Functions ----------------------

      Standard definition of DEVALT6:

      Bit     Function
      --- -----------------
       0  ADC0 Select
       1  ADC1 Select
       2  ADC2 Select
       3  ADC3 Select
       4  ADC4 Select
       5  ADC5 Select
       6  ADC6 Select
       7  ADC7 Select              	*/

#define SADC0_SL    (Disable << 0)
#define SADC1_SL    (Disable << 1)
#define SADC2_SL    (Disable << 2)
#define SADC3_SL    (Disable << 3)
#define SADC4_SL    (Disable << 4)
#define SADC5_SL    (Disable << 5)
#define SADC6_SL    (Disable << 6)
#define SADC7_SL    (Disable << 7)

#define DEVALT6_OEM (SADC0_SL | SADC1_SL | SADC2_SL | SADC3_SL | SADC4_SL \
                        | SADC5_SL | SADC6_SL | SADC7_SL)

/* ---------------------------- DEVALT7 OEM Functions ----------------------

      Standard definition of DEVALT7:

      Bit     Function
      --- -----------------
       0  Reserved
       1  Reserved
       2  KBSOUT12 Select
       3  KBSOUT13 Select
       4  KBSOUT14 Select
       5  KBSOUT15 Select
       6  KBSOUT16 Select
       7  KBSOUT17 Select              	*/

#define SKBO12_SL    (Enable << 2)
#define SKBO13_SL    (Enable << 3)
#define SKBO14_SL    (Enable << 4)
#define SKBO15_SL    (Enable << 5)
#if MATRIX_18
#define SKBO16_SL    (Enable << 6)
#define SKBO17_SL    (Enable << 7)
#else
#define SKBO16_SL    (Disable << 6)
#define SKBO17_SL    (Disable << 7)
#endif

#define DEVALT7_OEM (SKBO12_SL | SKBO13_SL | SKBO14_SL | SKBO15_SL | SKBO16_SL \
                        | SKBO17_SL)

/* ---------------------------- DEVALT8 OEM Functions ----------------------

      Standard definition of DEVALT8:

      Bit     Function
      --- -----------------
       0  Reserved
       1  GA20 Select
       2  KBRST Select
       3  Reserved
       4  IOX Serial Data Output Select
       5  IOX Serial Data Input/Output 2 Select
       6  IOX Serial Data Input/Output 1 Select
       7  IOX Load/Clock Select              	*/

#define SGA20_SL     (Enable << 1)
#define SKBRST_SL    (Enable << 2)
#define SIOXDO_SL    (Disable << 4)
#define SIOXIO2_SL   (Disable << 5)
#define SIOXIO1_SL   (Disable << 6)
#define SIOXLC_SL    (Disable << 7)


#define DEVALT8_OEM (SGA20_SL | SKBRST_SL | SIOXDO_SL | SIOXIO2_SL | SIOXIO1_SL \
                        | SIOXLC_SL)

/* ---------------------------- DEVALT9 OEM Functions ----------------------

      Standard definition of DEVALT9:

      Bit     Function
      --- -----------------
       0  GPIOA0-GPIOA3 Select
       1  GPIOF0-GPIOF7 Select
       2  GPIOA4-GPIOA5 Select
       3  GPIOA6-GPIOA7 Select
       4  GPOB0-GPIOB3 Select
       5  GPOB4-GPIOB7 Select
       6  GPIOC0-C1 Select
       7  GPIOC2-C3 Select              	*/

#define SGPA03_SL    (Disable << 0)
#define SGPF07_SL    (Disable << 1)
#define SGPA45_SL    (Disable << 2)
#define SGPA67_SL    (Disable << 3)
#define SGPB03_SL    (Disable << 4)
#define SGPB47_SL    (Disable << 5)
#define SGPC01_SL    (Disable << 6)
#define SGPC23_SL    (Disable << 7)

#define DEVALT9_OEM (SGPA03_SL | SGPF07_SL | SGPA45_SL | SGPA67_SL | SGPB03_SL \
                        | SGPB47_SL | SGPC01_SL | SGPC23_SL)

/* ---------------------------- DEVALTA OEM Functions ----------------------

      Standard definition of DEVALTA:

      Bit     Function
      --- -----------------
       0  Reserved
       1  Reserved
       2  Reserved
       3  CIRRL  Select
       4  CIRRM2 Select
       5  CIRRM1 Select
       6  SMB3   Select
       7  SMB3   Select           	 */

#define SCIRRL_SL     (Disable << 3)
#define SCIRRM2_SL    (Disable << 4)
#define SCIRRM1_SL    (Disable << 5)
#define SSMB3_SL      (Disable << 6)
#define SSMB4_SL      (Disable << 7)
#define DEVALTA_OEM (SCIRRL_SL | SCIRRM2_SL | SCIRRM1_SL | SSMB3_SL | SSMB4_SL)

/* ---------------------------- DEVALTD OEM Functions ----------------------

      Standard definition of DEVALTD:

      Bit     Function
      --- -----------------
       0  Flash Quad SPI Signal Select
       1  Reserved
       2  Reserved
       3  Reserved
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved           	 */

#define SF_IO23_SL     (Disable << 0)
#define DEVALTD_OEM (SF_IO23_SL)

/* ---------------------------- DEVALTE OEM Functions ----------------------

      Standard definition of DEVALTE:

      Bit     Function
      --- -----------------
       0  VD_IN1 Select
       1  VD_OUT1 Select
       2  VD_IN2 Select
       3  VD_OUT2 Select
       4  1_WIRE Select
       5  Reserved
       6  Reserved
       7  Reserved           	 */

#define SOWI_SL          (Disable << 4)
#define SVDO2_SL         (Disable << 3)
#define SVDI2_SL         (Disable << 2)
#define SVDO1_SL         (Disable << 1)
#define SVDI1_SL         (Disable << 0)
#define DEVALTE_OEM (SOWI_SL | SVDO2_SL | SVDI2_SL | SVDO1_SL | SVDI1_SL)


/* ---------------------------- PWM_SEL ----------------------

      Standard definition of PWM_SEL:

      Bit     Function
      --- -----------------
       0  GPIO to FORCE_PWM Select
       1  GPIO to FORCE_PWM Select
       2  Reserved
       3  Reserved
       4  GPIO42 Input Polarity
       5  GPIO43 Input Polarity
       6  Reserved
       7  Watchdog Reset to FORCE_PWM, Enable              	*/

#define SWD_RST_EN       (Disable << 7)
#define SGPIO43_POL      (Disable << 5)
#define SGPIO42_POL      (Disable << 4)
#define SGPIO_SEL        (0 << 0)
#define PWM_SEL_OEM (SWD_RST_EN | SGPIO43_POL | SGPIO42_POL | SGPIO_SEL)

/* ---------------------------- PWM_SEL2 ----------------------

      Standard definition of PWM_SEL2:

      Bit     Function
      --- -----------------
       0  ADC Threshold Event 1 to FORCE_PWM, Enable
       1  ADC Threshold Event 2 to FORCE_PWM, Enable
       2  ADC Threshold Event 3 to FORCE_PWM, Enable
       3  Reserved
       4  Reserved
       5  Reserved
       6  Reserved
       7  FORCE_PWM Status              	*/

#define SFPWM_STS        (Disable << 7)
#define SADC_TH3_EN      (Disable << 2)
#define SADC_TH2_EN      (Disable << 1)
#define SADC_TH1_EN      (Disable << 0)

#define PWM_SEL2_OEM (SFPWM_STS | SADC_TH3_EN | SADC_TH2_EN | SADC_TH1_EN)


/* ---------------------------- DEVPU0 ----------------------

      Standard definition of DEVPU0:

      Bit     Function
      --- -----------------
       0  SMBus1 Pull-Up
       1  SMBus2 Pull-Up
       2  SMBus3 Pull-Up
       3  SMBus4 Pull-Up
       4  Reserved
       5  Reserved
       6  Reserved
       7  Reserved              	*/

#define SSMB1_PUE    (Disable << 0)
#define SSMB2_PUE    (Disable << 1)
#define SSMB3_PUE    (Disable << 2)
#define SSMB4_PUE    (Disable << 3)

#define DEVPU0_OEM (SSMB1_PUE | SSMB2_PUE | SSMB3_PUE | SSMB4_PUE)

/* ---------------------------- DEVPD1 ----------------------

      Standard definition of DEVPD1:

      Bit     Function
      --- -----------------
       0  Reserved
       1  Reserved
       2  Reserved
       3  Reserved
       4  Reserved
       5  Reserved
       6  F_SDI Pull-Down
       7  Reserved              	*/

#define SF_SDI_PDE    (Disable << 6)

#define DEVPD1_OEM (SF_SDI_PDE)

/* ---------------------------- LV_GPIO_CTL ----------------------

      Standard definition of LV_GPIO_CTL:

      Bit     Function
      --- -----------------
       0  GPIO02 Low-Voltage Select
       1  GPIO42 Low-Voltage Select
       2  GPIO43 Low-Voltage Select
       3  GPIO44 Low-Voltage Select
       4  GPIO50 Low-Voltage Select
       5  GPIO52 Low-Voltage Select
       6  GPIO85 Low-Voltage Select
       7  GPIO86 Low-Voltage Select              	*/

#define SGPIO86_LV       (Disable << 7)
#define SGPIO85_LV       (Disable << 6)
#define SGPIO52_LV       (Disable << 5)
#define SGPIO50_LV       (Disable << 4)
#define SGPIO44_LV       (Disable << 3)
#define SGPIO43_LV       (Disable << 2)
#define SGPIO42_LV       (Disable << 1)
#define SGPIO02_LV       (Disable << 0)


#define LV_GPIO_CTL_OEM (SGPIO86_LV | SGPIO85_LV | SGPIO52_LV | SGPIO50_LV \
                            | SGPIO44_LV | SGPIO43_LV | SGPIO42_LV | SGPIO02_LV)

/* ---------------------------- LV_GPIO_CTL2 ----------------------

      Standard definition of LV_GPIO_CTL2:

      Bit     Function
      --- -----------------
       0  GPIO01 Low-Voltage Select
       1  GPIO03 Low-Voltage Select
       2  GPIO13 Low-Voltage Select
       3  GPIO15 Low-Voltage Select
       4  GPIO45 Low-Voltage Select
       5  GPIO51 Low-Voltage Select
       6  GPIO62 Low-Voltage Select
       7  GPIO87 Low-Voltage Select              	*/

#define SGPIO87_LV       (Disable << 7)
#define SGPIO62_LV       (Disable << 6)
#define SGPIO51_LV       (Disable << 5)
#define SGPIO45_LV       (Disable << 4)
#define SGPIO15_LV       (Disable << 3)
#define SGPIO13_LV       (Disable << 2)
#define SGPIO03_LV       (Disable << 1)
#define SGPIO01_LV       (Disable << 0)


#define LV_GPIO_CTL2_OEM (SGPIO87_LV | SGPIO62_LV | SGPIO51_LV | SGPIO45_LV \
                            | SGPIO15_LV | SGPIO13_LV | SGPIO03_LV | SGPIO01_LV)

/* ---------------------------- LV_GPIO_CTL3 ----------------------

      Standard definition of LV_GPIO_CTL3:

      Bit     Function
      --- -----------------
       0  GPIO02 Low-Voltage Select
       1  GPIO42 Low-Voltage Select
       2  GPIO43 Low-Voltage Select
       3  GPIO44 Low-Voltage Select
       4  GPIO50 Low-Voltage Select
       5  GPIO52 Low-Voltage Select
       6  GPIO85 Low-Voltage Select
       7  GPIO86 Low-Voltage Select              	*/

#define SG47_SC4A_LV       (Disable << 7)
#define SG53_SD4A_LV       (Disable << 6)
#define SG23_SC3A_LV       (Disable << 5)
#define SG31_SD3A_LV       (Disable << 4)
#define SG73_SC2_LV        (Disable << 3)
#define SG74_SD2_LV        (Disable << 2)
#define SG17_SC1_LV        (Disable << 1)
#define SG22_SD1_LV        (Disable << 0)


#define LV_GPIO_CTL3_OEM (SG47_SC4A_LV | SG53_SD4A_LV | SG23_SC3A_LV | SG31_SD3A_LV \
                            | SG73_SC2_LV | SG74_SD2_LV | SG17_SC1_LV | SG22_SD1_LV)

/* --------------------------- I2C --------------------------- */

#if I2C_SUPPORTED

/* Pullups for Bits 4 and 3 of Port B. */
#define I2C_PU  0x18		/* For PC87591 */

/* Alternate function for Bits 4 and 3 of Port B, SDA and SCL. */
#define I2C_ALT 0x18		/* For PC87591 */

/* Pullups for Bits 2 and 1 of Port C. */
#define I2C2_PU  0x06		/* For PC87591 */

/* Alternate function for Bits 2 and 1 of Port C, SDA2 and SCL2. */
#define I2C2_ALT 0x06		/* For PC87591 */

#else
#define I2C_PU  0x00
#define I2C_ALT 0x00

#define I2C2_PU  0x00
#define I2C2_ALT 0x00
#endif

/* ----------------------- PS/2 Port 3 ----------------------- */

//#if AUX_PORT3_SUPPORTED
//
///* Alternate function for Bits 5 and 4 of Port F, PSDAT3 and PSCLK3. */
//#define AUX3_ALT 0x30
//
//#else
//#define AUX3_ALT 0x00
//#endif

/* ---------------------- External IRQ x --------------------- */

#if EXT_GPIO3_WAKEUP
/* Alternate function for Bit 6 of Port C, External IRQ 23. */
#define IRQ23_ALT 0x40
#else
#define IRQ23_ALT 0x00
#endif

#if EXT_GPIO46_WAKEUP
/* Alternate function for Bit 4 of Port C, External IRQ 22. */
#define IRQ22_ALT 0x10
#else
#define IRQ22_ALT 0x00
#endif

#if EXT_GPIO44_WAKEUP
/* Alternate function for Bit 0 of Port D, External IRQ 20. */
#define IRQ20_ALT 0x01
#else
#define IRQ20_ALT 0x00
#endif

#if EXT_GPIO1_WAKEUP
/* Alternate function for Bit 1 of Port D, External IRQ 21. */
#define IRQ21_ALT 0x02
#else
#define IRQ21_ALT 0x00
#endif

#if EXT_GPIO4_WAKEUP
/* Alternate function for Bit 2 of Port D, External IRQ 24. */
#define IRQ24_ALT 0x04
#else
#define IRQ24_ALT 0x00
#endif

#if EXT_IRQ40_WAKEUP
/* Alternate function for Bit 5 of Port E, External IRQ 40. */
#define IRQ40_ALT 0x02
#else
#define IRQ40_ALT 0x00
#endif

/* -------------------------- P0OUT --------------------------
    Port 0 output data. */
#define P0DOUT_INIT     0x00
/* -------------------------- P0DIR --------------------------
    Port 0 direction. */
#define P0DIR_INIT      0x00
/* -------------------------- P0PULL -------------------------
   Port 0 Pull-Up or Pull-Down enable. */
#define P0PULL_INIT     0x00
/* -------------------------- P0PUD --------------------------
   Port 0 Pull-Up or Down selection. */
#define P0PUD_INIT      0x00
/* -------------------------- P0ENVDD ------------------------
   Port 0 Output enable by VDD contrl.. */
#define P0ENVDD_INIT    0x00
/* -------------------------- P0OTYPE ------------------------
    Port 0 Output Type Selection. */
#define P0OTYPE_INIT    0x00

/* -------------------------- P1OUT --------------------------
    Port 1 output data. */
#define P1DOUT_INIT     0x00
/* -------------------------- P1DIR --------------------------
    Port 1 direction. */
#define P1DIR_INIT      0x00
/* -------------------------- P1PULL -------------------------
   Port 1 Pull-Up or Pull-Down enable. */
#define P1PULL_INIT     0x00
/* -------------------------- P1PUD --------------------------
   Port 1 Pull-Up or Down selection. */
#define P1PUD_INIT      0x00
/* -------------------------- P1ENVDD ------------------------
   Port 1 Output enable by VDD contrl.. */
#define P1ENVDD_INIT    0x00
/* -------------------------- P1OTYPE ------------------------
    Port 1 Output Type Selection. */
#define P1OTYPE_INIT    0x00

/* -------------------------- P2OUT --------------------------
    Port 2 output data. */
#define P2DOUT_INIT     0x00
/* -------------------------- P2DIR --------------------------
    Port 2 direction. */
#define P2DIR_INIT      0x00
/* -------------------------- P2PULL -------------------------
   Port 2 Pull-Up or Pull-Down enable. */
#define P2PULL_INIT     0x00
/* -------------------------- P2PUD --------------------------
   Port 2 Pull-Up or Down selection. */
#define P2PUD_INIT      0x00
/* -------------------------- P2ENVDD ------------------------
   Port 2 Output enable by VDD contrl.. */
#define P2ENVDD_INIT    0x00
/* -------------------------- P2OTYPE ------------------------
    Port 2 Output Type Selection. */
#define P2OTYPE_INIT    0x00

/* -------------------------- P3OUT --------------------------
    Port 3 output data. */
#define P3DOUT_INIT     0x00
/* -------------------------- P3DIR --------------------------
    Port 3 direction. */
#define P3DIR_INIT      0x00
/* -------------------------- P3PULL -------------------------
   Port 3 Pull-Up or Pull-Down enable. */
#define P3PULL_INIT     0x00
/* -------------------------- P3PUD --------------------------
   Port 3 Pull-Up or Down selection. */
#define P3PUD_INIT      0x00
/* -------------------------- P3ENVDD ------------------------
    Port 3 Output enable by VDD contrl.. */
#define P3ENVDD_INIT    0x00
/* -------------------------- P3OTYPE ------------------------
    Port 3 Output Type Selection. */
#define P3OTYPE_INIT    0x00

/* -------------------------- P4OUT --------------------------
    Port 4 output data. */
#define P4DOUT_INIT     0x00
/* -------------------------- P4DIR --------------------------
    Port 4 direction. */
#define P4DIR_INIT      0x00
/* -------------------------- P4PULL -------------------------
    Port 4 Pull-Up or Pull-Down enable. */
#define P4PULL_INIT     0x00
/* -------------------------- P4PUD --------------------------
    Port 4 Pull-Up or Down selection. */
#define P4PUD_INIT      0x00
/* -------------------------- P4ENVDD ------------------------
    Port 4 Output enable by VDD contrl.. */
#define P4ENVDD_INIT    0x00
/* -------------------------- P4OTYPE ------------------------
    Port 4 Output Type Selection. */
#define P4OTYPE_INIT    0x00

/* -------------------------- P5OUT --------------------------
    Port 5 output data. */
#define P5DOUT_INIT     0x00
/* -------------------------- P5DIR --------------------------
    Port 5 direction. */
#define P5DIR_INIT      0x00
/* -------------------------- P5PULL -------------------------
    Port 5 Pull-Up or Pull-Down enable. */
#define P5PULL_INIT     0x00
/* -------------------------- P5PUD --------------------------
    Port 5 Pull-Up or Down selection. */
#define P5PUD_INIT      0x00
/* -------------------------- P5ENVDD ------------------------
    Port 5 Output enable by VDD contrl.. */
#define P5ENVDD_INIT    0x00
/* -------------------------- P5OTYPE ------------------------
    Port 5 Output Type Selection. */
#define P5OTYPE_INIT    0x00

/* -------------------------- P6OUT --------------------------
    Port 6 output data. */
#define P6DOUT_INIT     0x00
/* -------------------------- P6DIR --------------------------
    Port 6 direction. */
#define P6DIR_INIT      0x00
/* -------------------------- P6PULL -------------------------
    Port 6 Pull-Up or Pull-Down enable. */
#define P6PULL_INIT     0x00
/* -------------------------- P6PUD --------------------------
    Port 6 Pull-Up or Down selection. */
#define P6PUD_INIT      0x00
/* -------------------------- P6ENVDD ------------------------
    Port 6 Output enable by VDD contrl.. */
#define P6ENVDD_INIT    0x00
/* -------------------------- P6OTYPE ------------------------
    Port 6 Output Type Selection. */
#define P6OTYPE_INIT    0x00

/* -------------------------- P7OUT --------------------------
    Port 7 output data. */
#define P7DOUT_INIT     0x00
/* -------------------------- P7DIR --------------------------
    Port 7 direction. */
#define P7DIR_INIT      0x00
/* -------------------------- P7PULL -------------------------
    Port 7 Pull-Up or Pull-Down enable. */
#define P7PULL_INIT     0x00
/* -------------------------- P7PUD --------------------------
    Port 7 Pull-Up or Down selection. */
#define P7PUD_INIT      0x00
/* -------------------------- P7ENVDD ------------------------
    Port 7 Output enable by VDD contrl.. */
#define P7ENVDD_INIT    0x00
/* -------------------------- P7OTYPE ------------------------
    Port 7 Output Type Selection. */
#define P7OTYPE_INIT    0x00

/* -------------------------- P8OUT --------------------------
    Port 8 output data. */
#define P8DOUT_INIT     0x00
/* -------------------------- P8DIR --------------------------
    Port 8 direction. */
#define P8DIR_INIT      0x00
/* -------------------------- P8PULL -------------------------
    Port 8 Pull-Up or Pull-Down enable. */
#define P8PULL_INIT     0x00
/* -------------------------- P8PUD --------------------------
    Port 8 Pull-Up or Down selection. */
#define P8PUD_INIT      0x00
/* -------------------------- P8ENVDD ------------------------
    Port 8 Output enable by VDD contrl.. */
#define P8ENVDD_INIT    0x00
/* -------------------------- P8OTYPE ------------------------
    Port 8 Output Type Selection. */
#define P8OTYPE_INIT    0x00

/* -------------------------- P9OUT --------------------------
    Port 9 output data. */
#define P9DOUT_INIT     0x00
/* -------------------------- P9DIR --------------------------
    Port 9 direction. */
#define P9DIR_INIT      0x00
/* -------------------------- P9PULL -------------------------
    Port 9 Pull-Up or Pull-Down enable. */
#define P9PULL_INIT     0x00
/* -------------------------- P9PUD --------------------------
    Port 9 Pull-Up or Down selection. */
#define P9PUD_INIT      0x00
/* -------------------------- P9ENVDD ------------------------
    Port 9 Output enable by VDD contrl. */
#define P9ENVDD_INIT    0x00
/* -------------------------- P9OTYPE ------------------------
    Port 9 Output Type Selection. */
#define P9OTYPE_INIT    0x00

/* -------------------------- PAOUT --------------------------
    Port A output data. */
#define PADOUT_INIT     0x00
/* -------------------------- PADIR --------------------------
    Port A direction. */
#define PADIR_INIT      0x00
/* -------------------------- PAPULL -------------------------
    Port A Pull-Up or Pull-Down enable. */
#define PAPULL_INIT     0x00
/* -------------------------- PAPUD --------------------------
    Port A Pull-Up or Down selection. */
#define PAPUD_INIT      0x00
/* -------------------------- PAENVDD ------------------------
    Port A Output enable by VDD contrl. */
#define PAENVDD_INIT    0x00
/* -------------------------- PAOTYPE ------------------------
    Port A Output Type Selection. */
#define PAOTYPE_INIT    0x00

/* -------------------------- PBOUT --------------------------
    Port B output data. */
#define PBDOUT_INIT     0x00
/* -------------------------- PBDIR --------------------------
    Port B direction. */
#define PBDIR_INIT      0x00
/* -------------------------- PBPULL -------------------------
    Port B Pull-Up or Pull-Down enable. */
#define PBPULL_INIT     0x00
/* -------------------------- PBPUD --------------------------
    Port B Pull-Up or Down selection. */
#define PBPUD_INIT      0x00
/* -------------------------- PBENVDD ------------------------
    Port B Output enable by VDD contrl. */
#define PBENVDD_INIT    0x00
/* -------------------------- PBOTYPE ------------------------
    Port B Output Type Selection. */
#define PBOTYPE_INIT    0x00

/* -------------------------- PCOUT --------------------------
    Port C output data. */
#define PCDOUT_INIT     0x00
/* -------------------------- PCDIR --------------------------
    Port C direction. */
#define PCDIR_INIT      0x00
/* -------------------------- PCPULL -------------------------
    Port C Pull-Up or Pull-Down enable. */
#define PCPULL_INIT     0x00
/* -------------------------- PCPUD --------------------------
    Port C Pull-Up or Down selection. */
#define PCPUD_INIT      0x00
/* -------------------------- PCENVDD ------------------------
    Port C Output enable by VDD contrl. */
#define PCENVDD_INIT    0x00
/* -------------------------- PCOTYPE ------------------------
    Port C Output Type Selection. */
#define PCOTYPE_INIT    0x00

/* -------------------------- PFOUT --------------------------
    Port F output data. */
#define PFDOUT_INIT     0x00
/* -------------------------- PFDIR --------------------------
    Port F direction. */
#define PFDIR_INIT      0x00
/* -------------------------- PFPULL -------------------------
    Port F Pull-Up or Pull-Down enable. */
#define PFPULL_INIT     0x00
/* -------------------------- PFPUD --------------------------
    Port F Pull-Up or Down selection. */
#define PFPUD_INIT      0x00
/* -------------------------- PFENVDD ------------------------
    Port F Output enable by VDD contrl. */
#define PFENVDD_INIT    0x00
/* -------------------------- PFOTYPE ------------------------
    Port F Output Type Selection. */
#define PFOTYPE_INIT    0x00


/* ----- Device Alternate Function #0 Register. ----- */
#define DEVALT0_INIT    DEVALT0_OEM
/* Device Alternate Function #1 Register. */
#define DEVALT1_INIT    DEVALT1_OEM
/* ----- Device Alternate Function #2 Register. ----- */
#define DEVALT2_INIT    DEVALT2_OEM
/* ----- Device Alternate Function #3 Register. ----- */
#define DEVALT3_INIT    DEVALT3_OEM
/* Device Alternate Function #4 Register. */
#define DEVALT4_INIT    DEVALT4_OEM
/* Device Alternate Function #5 Register. */
#define DEVALT5_INIT    DEVALT5_OEM
/* Device Alternate Function #6 Register. */
#define DEVALT6_INIT    DEVALT6_OEM
/* Device Alternate Function #7 Register. */
#define DEVALT7_INIT    DEVALT7_OEM
/* Device Alternate Function #8 Register. */
#define DEVALT8_INIT    DEVALT8_OEM
/* Device Alternate Function #9 Register. */
#define DEVALT9_INIT    DEVALT9_OEM
/* Device Alternate Function #A Register. */
#define DEVALTA_INIT    DEVALTA_OEM
/* Device Alternate Function #D Register. */
#define DEVALTD_INIT    DEVALTD_OEM
/* Device Alternate Function #E Register. */
#define DEVALTE_INIT    DEVALTE_OEM
/* Device Pull-Up Register 0. */
#define DEVPU0_INIT     DEVPU0_OEM
/* Device Pull-Up Register 1 */
#define DEVPD1_INIT     DEVPD1_OEM
/* Device PWM_SEL Register. */
#define PWM_SEL_INIT    PWM_SEL_OEM
/* Device PWM_SEL2 Register 1 */
#define PWM_SEL2_INIT    PWM_SEL2_OEM
/* Low-Voltage GPIO Pins Control Register */
#define LV_GPIO_CTL_INIT     LV_GPIO_CTL_OEM
/* Low-Voltage GPIO Pins Control 2 Register */
#define LV_GPIO_CTL2_INIT    LV_GPIO_CTL2_OEM
/* Low-Voltage GPIO Pins Control 3 Register */
#define LV_GPIO_CTL3_INIT    LV_GPIO_CTL3_OEM


/* -------------------------- A to D ------------------------- */

#define ADLYCTL_INIT ((TMPDLY_INIT << 4) + VOLDLY_INIT)
#define ADCCNF_INIT     0x0700
#define ATCTL_INIT      0x0300 /* bit11-8 must be 011 */
#define ADCCS_INIT      0x00
/* ------------------------- DACCTRL -------------------------
   D to A control. */

 /* DA0 and DA1 enabled. */
 #define DA0_ON 1
 #define DA1_ON 1
 #define DA2_ON 0
 #define DA3_ON 0

#define DACCTRL_INIT ((DA3_ON << 3) | (DA2_ON << 2) | (DA1_ON << 1) | (DA0_ON << 0))

/* ------------------------- DACDATx -------------------------
   D to A outputs. */

 #define DACDAT0_INIT 0
 #define DACDAT1_INIT 0
 #define DACDAT2_INIT 0
 #define DACDAT3_INIT 0

/* -------------------------- PSCON --------------------------

   Bit     Function
   --- -----------------
    0  EN
    1  XMT
    2  High drive
    3  High drive
    4  Input debouce bit 0
    5  Input debouce bit 1
    6  Input debouce bit 2
    7  Weak pullup enable

   Input debounce: 0 =  1 cycle.
                   1 =  2 cycles.
                   2 =  4 cycles.
                   3 =  8 cycles.
                   4 = 16 cycles.
                   5 = 32 cycles.

   Bit 1 and 0 must be clear. */

#define PSCON_OEM  0x40 /* Input De-Bounce set to 16 cycles. */
#define PSCON_INIT (PSCON_OEM & ~(PSCON_XMT | PSCON_EN))

/* ------------------------- SMB1CTL2 -------------------------
   SMB1 Control 2.

   Bits 7 through 1 set the SCL frequency (8 - 127 valid).
   Bit 0 set enables the module.
   Bit 0 must be set if PB_Alt bits 4 and 3 are set, else must be 0. */

#define SMB1CTL2_OEM  0xFE // 50KHz

 #define SMB1CTL2_INIT (SMB1CTL2_OEM | 1)

/* ------------------------- SMB2CTL2 -------------------------
   SMB2 Control 2.

   Bits 7 through 1 set the SCL frequency (8 - 127 valid).
   Bit 0 set enables the module.
   Bit 0 must be set if PC_Alt bits 2 and 1 are set, else must be 0. */

#define SMB2CTL2_OEM  0xFE // 50KHz

 #define SMB2CTL2_INIT (SMB2CTL2_OEM | 1)

/* ------------------------- KBSOUT0-1 ------------------------------------- */
#define KBSOUT0_INIT    0xFFFF
#define KBSOUT1_INIT    0x0003
#define KBSINPU_INIT    0xFF

/* ------------------------------------------------------------------------- */
/* Module - PMC                                                              */
/* ------------------------------------------------------------------------- */
/* ---------------------------- ENIDL_CTL ----------------------
      Standard definition of ENIDL_CTL:

      Bit     Function
      --- -----------------
       0  Reserved
       1  ADC Access is Disabled
       2  PECI Enable in Idle
       3  Reserved
       4  Reserved
       5  Reserved
       6  Reserved
       7  ADC Low-Frequency Clock Select              	*/



#define ADC_ACC_DIS_SL         (Disable << 1)
#define PECI_ENIP_SL           (Enable << 2)
#define ADC_LFSL_SL            (Disable << 7)

#define ENIDL_CTL_INIT       (ADC_ACC_DIS_SL | PECI_ENIP_SL | ADC_LFSL_SL)


/* ---------------------------- DISWT_CTL ----------------------
      Standard definition of DISWT_CTL:

      Bit     Function
      --- -----------------
       0  FIU Disable in Wait
       1  ADC Disable in Wait
       2  PECI Disable in Wait
       3  One-Wire Disable in Wait
       4  SPI Peripheral Disable in Wait
       5  Core RAM Disable in Wait
       6  ROM Disable in Wait
       7  Access Disable in Wait              	*/

#define FIU_DW_SL              (Disable << 0)
#define ADC_DW_SL              (Disable << 1)
#define PECI_DW_SL             (Disable << 2)
#define OWI_DW_SL              (Disable << 3)
#define SPIP_DW_SL             (Disable << 4)
#define RAM_DW_SL              (Disable << 5)
#define ROM_DW_SL              (Disable << 6)
#define ACC_DW_SL              (Disable << 7)

#define DISWT_CTL_INIT      (FIU_DW_SL | ADC_DW_SL | PECI_DW_SL | OWI_DW_SL \
                                | SPIP_DW_SL | RAM_DW_SL | ROM_DW_SL | ACC_DW_SL)


/* ---------------------------- DISWT_CTL1 ----------------------
    Standard definition of DISWT_CTL1:

    Note: The CB_DW_SL should not set as "Enable" if SHM or Indirect
          Memory Access is enabled.

    Bit     Function
    --- -----------------
     0  Core Bus Disable in Wait
     1  Core Low Power in Wait
     2  Reserved
     3  Reserved
     4  Reserved
     5  Reserved
     6  Reserved
     7  Reserved              	*/

#define CB_DW_SL                (Disable << 0)
#define CR_LPW_SL               (Enable << 1)

#define DISWT_CTL1_INIT      (CB_DW_SL | CR_LPW_SL)

/* ---------------------------- PWDWN_CTL1 ----------------------
      Standard definition of PWDWN_CTL1:

      Bit     Function
      --- -----------------
       0  Keyboard Scan Power-Down
       1  SDP Power-Down
       2  FIU Power-Down
       3  PS/2 Power-Down
       4  CR_UART Power-Down
       5  MFT16-1 Power-Down
       6  MFT16-2 Power-Down
       7  Reserved              	*/

#if HW_KB_SCN_SUPPORTED
#define KBS_PD_SL               (Disable << 0)
#else // HW_KB_SCN_SUPPORTED
#define KBS_PD_SL               (Enable << 0)
#endif // HW_KB_SCN_SUPPORTED
#define SDP_PD_SL               (Disable << 1)
#define FIU_PD_SL               (Disable << 2)
#define PS2_PD_SL               (Disable << 3)
#define UART_PD_SL              (Enable << 4)
#define MFT1_PD_SL              (Disable << 5)
#define MFT2_PD_SL              (Disable << 6)

#define PWDWN_CTL1_INIT (0x80 | KBS_PD_SL | SDP_PD_SL | FIU_PD_SL | PS2_PD_SL \
                            | UART_PD_SL | MFT1_PD_SL | MFT2_PD_SL)

/* ---------------------------- PWDWN_CTL2 ----------------------
      Standard definition of PWDWN_CTL2:

      Bit     Function
      --- -----------------
       0  A_PWM Power-Down
       1  B_PWM Power-Down
       2  C_PWM Power-Down
       3  D_PWM Power-Down
       4  E_PWM Power-Down
       5  F_PWM Power-Down
       6  G_PWM Power-Down
       7  H_PWM Power-Down   */


#define A_PWM_PD_SL            (Disable << 0)
#define B_PWM_PD_SL            (Disable << 1)
#define C_PWM_PD_SL            (Disable << 2)
#define D_PWM_PD_SL            (Disable << 3)
#define E_PWM_PD_SL            (Disable << 4)
#define F_PWM_PD_SL            (Disable << 5)
#define G_PWM_PD_SL            (Disable << 6)
#define H_PWM_PD_SL            (Disable << 7)

#define PWDWN_CTL2_INIT (A_PWM_PD_SL | B_PWM_PD_SL | C_PWM_PD_SL | D_PWM_PD_SL \
                            | E_PWM_PD_SL | F_PWM_PD_SL |G_PWM_PD_SL | H_PWM_PD_SL)

/* ---------------------------- PWDWN_CTL3 ----------------------
      Standard definition of PWDWN_CTL3:

      Bit     Function
      --- -----------------
       0  SMB1_PD Power-Down
       1  SMB2_PD Power-Down
       2  SMB3_PD Power-Down
       3  SMB4_PD Power-Down
       4  SMBF_PD Power-Down
       5  Reserved
       6  IOXIF Power-Down
       7  Reserved                      */

#define SMB1_PD_SL          (Disable << 0)
#define SMB2_PD_SL          (Disable << 1)
#define SMB3_PD_SL          (Enable << 2)
#define SMB4_PD_SL          (Disable << 3)
#define SMBF_PD_SL          (Enable << 4)
#define IOXIF_PD_SL         (Enable << 6)

#define PWDWN_CTL3_INIT (0x80 | SMB1_PD_SL | SMB2_PD_SL | SMB3_PD_SL | SMB4_PD_SL \
                            | SMBF_PD_SL | IOXIF_PD_SL)


/* ---------------------------- PWDWN_CTL4 ----------------------
      Standard definition of PWDWN_CTL4:

      Bit     Function
      --- -----------------
       0  ITIM8-1 Power-Down
       1  ITIM8-2 Power-Down
       2  ITIM8-3 Power-Down
       3  Reserved
       4  ADC Power-Down
       5  PECI Power-Down
       6  One-Wire Power-Down
       7  SPI Peripheral Power-Down                      */


#define ITIM1_PD_SL             (Disable << 0)
#define ITIM2_PD_SL             (Disable << 1)
#define ITIM3_PD_SL             (Disable << 2)
#define ADC_PD_SL               (Disable << 4)
#define PECI_PD_SL              (Disable << 5)
#define OWI_PD_SL               (Enable << 6)
#define SPIP_PD_SL              (Enable << 7)

#define PWDWN_CTL4_INIT (ITIM1_PD_SL | ITIM2_PD_SL | ITIM3_PD_SL | ADC_PD_SL \
                            | PECI_PD_SL | OWI_PD_SL | SPIP_PD_SL)


/* ---------------------------- PWDWN_CTL5 ----------------------
      Standard definition of PWDWN_CTL5:

      Bit     Function
      --- -----------------
       0  Reserved
       1  Reserved
       2  Reserved
       3  Core-to-Host Module Power-Down
       4  Reserved
       5  SHM Power-Down
       6  Debug Port 80 Power-Down
       7  MSWC Power-Down                      */


#define C2HACC_PD_SL                (Disable << 3)
#define SHM_PD_SL                   (Disable << 5)
#define DP80_PD_SL                  (Enable << 6)
#define MSWC_PD_SL                  (Enable << 7)

#define PWDWN_CTL5_INIT (C2HACC_PD_SL | SHM_PD_SL | DP80_PD_SL | MSWC_PD_SL)



/* The Reg_Init table is used to initialize the registers. */
const REG_INIT_DESCRIPTOR Reg_Init[] =
{
    { P0DIR_ADDR,    Byte_type,  P0DIR_INIT     },      /* Port 0 Direction. */
    { P0DOUT_ADDR,   Byte_type,  P0DOUT_INIT    },      /* Port 0 Output Data. */
    { P0PULL_ADDR,   Byte_type,  P0PULL_INIT    },      /* Port 0 Weak pull-up or pull-down enabled. */
    { P0PUD_ADDR,    Byte_type,  P0PUD_INIT     },      /* Port 0 Pull-up/down selection. */
    { P0ENVDD_ADDR,  Byte_type,  P0ENVDD_INIT   },      /* Port 0 Drive enable by VDD present. */
    { P0OTYPE_ADDR,  Byte_type,  P0OTYPE_INIT   },      /* Port 0 Output Type. */
    { P1DIR_ADDR,    Byte_type,  P1DIR_INIT     },      /* Port 1 Direction. */
    { P1DOUT_ADDR,   Byte_type,  P1DOUT_INIT    },      /* Port 1 Output Data. */
    { P1PULL_ADDR,   Byte_type,  P1PULL_INIT    },      /* Port 1 Weak pull-up or pull-down enabled. */
    { P1PUD_ADDR,    Byte_type,  P1PUD_INIT     },      /* Port 1 Pull-up/down selection. */
    { P1ENVDD_ADDR,  Byte_type,  P1ENVDD_INIT   },      /* Port 1 Drive enable by VDD present. */
    { P1OTYPE_ADDR,  Byte_type,  P1OTYPE_INIT   },      /* Port 1 Output Type. */
    { P2DIR_ADDR,    Byte_type,  P2DIR_INIT     },      /* Port 2 Direction. */
    { P2DOUT_ADDR,   Byte_type,  P2DOUT_INIT    },      /* Port 2 Output Data. */
    { P2PULL_ADDR,   Byte_type,  P2PULL_INIT    },      /* Port 2 Weak pull-up or pull-down enabled. */
    { P2PUD_ADDR,    Byte_type,  P2PUD_INIT     },      /* Port 2 Pull-up/down selection. */
    { P2ENVDD_ADDR,  Byte_type,  P2ENVDD_INIT   },      /* Port 2 Drive enable by VDD present. */
    { P2OTYPE_ADDR,  Byte_type,  P2OTYPE_INIT   },      /* Port 2 Output Type. */
    { P3DIR_ADDR,    Byte_type,  P3DIR_INIT     },      /* Port 3 Direction. */
    { P3DOUT_ADDR,   Byte_type,  P3DOUT_INIT    },      /* Port 3 Output Data. */
    { P3PULL_ADDR,   Byte_type,  P3PULL_INIT    },      /* Port 3 Weak pull-up or pull-down enabled. */
    { P3PUD_ADDR,    Byte_type,  P3PUD_INIT     },      /* Port 3 Pull-up/down selection. */
    { P3ENVDD_ADDR,  Byte_type,  P3ENVDD_INIT   },      /* Port 3 Drive enable by VDD present. */
    { P3OTYPE_ADDR,  Byte_type,  P3OTYPE_INIT   },      /* Port 3 Output Type. */
    { P4DIR_ADDR,    Byte_type,  P4DIR_INIT     },      /* Port 4 Direction. */
    { P4DOUT_ADDR,   Byte_type,  P4DOUT_INIT    },      /* Port 4 Output Data. */
    { P4PULL_ADDR,   Byte_type,  P4PULL_INIT    },      /* Port 4 Weak pull-up or pull-down enabled. */
    { P4PUD_ADDR,    Byte_type,  P4PUD_INIT     },      /* Port 4 Pull-up/down selection. */
    { P4ENVDD_ADDR,  Byte_type,  P4ENVDD_INIT   },      /* Port 4 Drive enable by VDD present. */
    { P4OTYPE_ADDR,  Byte_type,  P4OTYPE_INIT   },      /* Port 4 Output Type. */
    { P5DIR_ADDR,    Byte_type,  P5DIR_INIT     },      /* Port 5 Direction. */
    { P5DOUT_ADDR,   Byte_type,  P5DOUT_INIT    },      /* Port 5 Output Data. */
    { P5PULL_ADDR,   Byte_type,  P5PULL_INIT    },      /* Port 5 Weak pull-up or pull-down enabled. */
    { P5PUD_ADDR,    Byte_type,  P5PUD_INIT     },      /* Port 5 Pull-up/down selection. */
    { P5ENVDD_ADDR,  Byte_type,  P5ENVDD_INIT   },      /* Port 5 Drive enable by VDD present. */
    { P5OTYPE_ADDR,  Byte_type,  P5OTYPE_INIT   },      /* Port 5 Output Type. */
    { P6DIR_ADDR,    Byte_type,  P6DIR_INIT     },      /* Port 6 Direction. */
    { P6DOUT_ADDR,   Byte_type,  P6DOUT_INIT    },      /* Port 6 Output Data. */
    { P6PULL_ADDR,   Byte_type,  P6PULL_INIT    },      /* Port 6 Weak pull-up or pull-down enabled. */
    { P6PUD_ADDR,    Byte_type,  P6PUD_INIT     },      /* Port 6 Pull-up/down selection. */
    { P6ENVDD_ADDR,  Byte_type,  P6ENVDD_INIT   },      /* Port 6 Drive enable by VDD present. */
    { P6OTYPE_ADDR,  Byte_type,  P6OTYPE_INIT   },      /* Port 6 Output Type. */
    { P7DIR_ADDR,    Byte_type,  P7DIR_INIT     },      /* Port 7 Direction. */
    { P7DOUT_ADDR,   Byte_type,  P7DOUT_INIT    },      /* Port 7 Output Data. */
    { P7PULL_ADDR,   Byte_type,  P7PULL_INIT    },      /* Port 7 Weak pull-up or pull-down enabled. */
    { P7PUD_ADDR,    Byte_type,  P7PUD_INIT     },      /* Port 7 Pull-up/down selection. */
    { P7ENVDD_ADDR,  Byte_type,  P7ENVDD_INIT   },      /* Port 7 Drive enable by VDD present. */
    { P7OTYPE_ADDR,  Byte_type,  P7OTYPE_INIT   },      /* Port 7 Output Type. */
    { P8DIR_ADDR,    Byte_type,  P8DIR_INIT     },      /* Port 8 Direction. */
    { P8DOUT_ADDR,   Byte_type,  P8DOUT_INIT    },      /* Port 8 Output Data. */
    { P8PULL_ADDR,   Byte_type,  P8PULL_INIT    },      /* Port 8 Weak pull-up or pull-down enabled. */
    { P8PUD_ADDR,    Byte_type,  P8PUD_INIT     },      /* Port 8 Pull-up/down selection. */
    { P8ENVDD_ADDR,  Byte_type,  P8ENVDD_INIT   },      /* Port 8 Drive enable by VDD present. */
    { P8OTYPE_ADDR,  Byte_type,  P8OTYPE_INIT   },      /* Port 8 Output Type. */
    { P9DIR_ADDR,    Byte_type,  P9DIR_INIT     },      /* Port 9 Direction. */
    { P9DOUT_ADDR,   Byte_type,  P9DOUT_INIT    },      /* Port 9 Output Data. */
    { P9PULL_ADDR,   Byte_type,  P9PULL_INIT    },      /* Port 9 Weak pull-up or pull-down enabled. */
    { P9PUD_ADDR,    Byte_type,  P9PUD_INIT     },      /* Port 9 Pull-up/down selection. */
    { P9ENVDD_ADDR,  Byte_type,  P9ENVDD_INIT   },      /* Port 9 Drive enable by VDD present. */
    { P9OTYPE_ADDR,  Byte_type,  P9OTYPE_INIT   },      /* Port 9 Output Type. */
    { PADIR_ADDR,    Byte_type,  PADIR_INIT     },      /* Port A Direction. */
    { PADOUT_ADDR,   Byte_type,  PADOUT_INIT    },      /* Port A Output Data. */
    { PAPULL_ADDR,   Byte_type,  PAPULL_INIT    },      /* Port A Weak pull-up or pull-down enabled. */
    { PAPUD_ADDR,    Byte_type,  PAPUD_INIT     },      /* Port A Pull-up/down selection. */
    { PAENVDD_ADDR,  Byte_type,  PAENVDD_INIT   },      /* Port A Drive enable by VDD present. */
    { PAOTYPE_ADDR,  Byte_type,  PAOTYPE_INIT   },      /* Port A Output Type. */
    { PBDIR_ADDR,    Byte_type,  PBDIR_INIT     },      /* Port B Direction. */
    { PBDOUT_ADDR,   Byte_type,  PBDOUT_INIT    },      /* Port B Output Data. */
    { PBPULL_ADDR,   Byte_type,  PBPULL_INIT    },      /* Port B Weak pull-up or pull-down enabled. */
    { PBPUD_ADDR,    Byte_type,  PBPUD_INIT     },      /* Port B Pull-up/down selection. */
    { PBENVDD_ADDR,  Byte_type,  PBENVDD_INIT   },      /* Port B Drive enable by VDD present. */
    { PBOTYPE_ADDR,  Byte_type,  PBOTYPE_INIT   },      /* Port B Output Type. */
    { PCDIR_ADDR,    Byte_type,  PCDIR_INIT     },      /* Port C Direction. */
    { PCDOUT_ADDR,   Byte_type,  PCDOUT_INIT    },      /* Port C Output Data. */
    { PCPULL_ADDR,   Byte_type,  PCPULL_INIT    },      /* Port C Weak pull-up or pull-down enabled. */
    { PCPUD_ADDR,    Byte_type,  PCPUD_INIT     },      /* Port C Pull-up/down selection. */
    { PCENVDD_ADDR,  Byte_type,  PCENVDD_INIT   },      /* Port C Drive enable by VDD present. */
    { PCOTYPE_ADDR,  Byte_type,  PCOTYPE_INIT   },      /* Port C Output Type. */
    { PFDIR_ADDR,    Byte_type,  PFDIR_INIT     },      /* Port F Direction. */
    { PFDOUT_ADDR,   Byte_type,  PFDOUT_INIT    },      /* Port F Output Data. */
    { PFPULL_ADDR,   Byte_type,  PFPULL_INIT    },      /* Port F Weak pull-up or pull-down enabled. */
    { PFPUD_ADDR,    Byte_type,  PFPUD_INIT     },      /* Port F Pull-up/down selection. */
    { PFENVDD_ADDR,  Byte_type,  PFENVDD_INIT   },      /* Port F Drive enable by VDD present. */
    { PFOTYPE_ADDR,  Byte_type,  PFOTYPE_INIT   },      /* Port F Output Type. */
    { DEVALT0_ADDR,  Byte_type,  DEVALT0_INIT   },      /* Device Alternate Function #0 Register. */
    { DEVALT1_ADDR,  Byte_type,  DEVALT1_INIT   },      /* Device Alternate Function #1 Register. */
    { DEVALT2_ADDR,  Byte_type,  DEVALT2_INIT   },      /* Device Alternate Function #2 Register. */
    { DEVALT3_ADDR,  Byte_type,  DEVALT3_INIT   },      /* Device Alternate Function #3 Register. */
    { DEVALT4_ADDR,  Byte_type,  DEVALT4_INIT   },      /* Device Alternate Function #4 Register. */
    { DEVALT5_ADDR,  Byte_type,  DEVALT5_INIT   },      /* Device Alternate Function #5 Register. */
    { DEVALT6_ADDR,  Byte_type,  DEVALT6_INIT   },      /* Device Alternate Function #6 Register. */
    { DEVALT7_ADDR,  Byte_type,  DEVALT7_INIT   },      /* Device Alternate Function #7 Register. */
    { DEVALT8_ADDR,  Byte_type,  DEVALT8_INIT   },      /* Device Alternate Function #8 Register. */
    { DEVALT9_ADDR,  Byte_type,  DEVALT9_INIT   },      /* Device Alternate Function #9 Register. */
    { DEVALTA_ADDR,  Byte_type,  DEVALTA_INIT   },      /* Device Alternate Function #A Register. */
    { DEVALTE_ADDR,  Byte_type,  DEVALTE_INIT   },      /* Device Alternate Function #A Register. */

    { PWM_SEL_ADDR,  Byte_type,  PWM_SEL_INIT   },      /* PWM Input Select. */
    { PWM_SEL2_ADDR, Byte_type,  PWM_SEL2_INIT  },      /* PWM Input Select 2. */
    { DEVPU0_ADDR,   Byte_type,  DEVPU0_INIT    },      /* Device Pull-Up Register 0. */
    { DEVPD1_ADDR,   Byte_type,  DEVPD1_INIT    },      /* Device Pull-Down Register 1. */

    { LV_GPIO_CTL_ADDR, Byte_type,  LV_GPIO_CTL_INIT }, /* Low-Voltage GPIO Pins Control. */
    { LV_GPIO_CTL2_ADDR,    Byte_type,  LV_GPIO_CTL2_INIT }, /* Low-Voltage GPIO Pins Control. */
    { LV_GPIO_CTL3_ADDR,    Byte_type,  LV_GPIO_CTL3_INIT }, /* Low-Voltage GPIO Pins Control. */

    { DACCTRL_ADDR,  Byte_type,  DACCTRL_INIT   },      /* DAC Control. */
    { DACDAT0_ADDR,  Byte_type,  DACDAT0_INIT   },      /* DAC Data 0. */
    { DACDAT1_ADDR,  Byte_type,  DACDAT1_INIT   },      /* DAC Data 1. */
    { DACDAT2_ADDR,  Byte_type,  DACDAT2_INIT   },      /* DAC Data 2. */

    { SMB1CTL2_ADDR, Byte_type,  SMB1CTL2_INIT  },      /* SMB1 Control 2. */
    { SMB2CTL2_ADDR, Byte_type,  SMB2CTL2_INIT  },      /* SMB2 Control 2. */

    { ADCCNF_ADDR,   Word_type,  ADCCNF_INIT    },      /* ADC Configuration Register. */
    { ATCTL_INIT,    Word_type,  ATCTL_INIT     },      /* ADC Clock Control Register. */

    { KBSOUT0_ADDR,  Word_type,  KBSOUT0_INIT   },      /* KBS_OUT0 KBSOUT Data. */
    { KBSOUT1_ADDR,  Word_type,  KBSOUT1_INIT   },      /* KBS_OUT1 KBSOUT Data. */
    { KBSINPU_ADDR,  Byte_type,  KBSINPU_INIT   },       /* KBSINPU Data. */
    { ENIDL_CTL_ADDR,  Byte_type,  ENIDL_CTL_INIT   },   /* ENIDL_CTL Data. */
    { DISWT_CTL_ADDR,  Byte_type,  DISWT_CTL_INIT   },   /* DISWT_CTL Data. */
    { DISWT_CTL1_ADDR,  Byte_type, DISWT_CTL1_INIT  },   /* DISWT_CTL1 Data. */
    { PWDWN_CTL2_ADDR,  Byte_type,  PWDWN_CTL2_INIT },   /* PWDWN_CTL2 Data. */
    { PWDWN_CTL3_ADDR,  Byte_type,  PWDWN_CTL3_INIT },   /* PWDWN_CTL3 Data. */
    { PWDWN_CTL4_ADDR,  Byte_type,  PWDWN_CTL4_INIT },   /* PWDWN_CTL4 Data. */
    { PWDWN_CTL5_ADDR,  Byte_type,  PWDWN_CTL5_INIT }    /* PWDWN_CTL5 Data. */
};


void Init_Regs(void)
{
   WORD * Word_Data_Pntr;
   BYTE * Byte_Data_Pntr;
   WORD   init_reg_cnt;
   WORD   index;

   init_reg_cnt = sizeof(Reg_Init) / sizeof(REG_INIT_DESCRIPTOR);

   for(index = 0; index < init_reg_cnt; index++) {
       if(Reg_Init[index].Reg_Type == Word_type)
       {
           Word_Data_Pntr = (WORD *) (Reg_Init[index].Reg_Addr);
           *Word_Data_Pntr = Reg_Init[index].Reg_Value;
       }
       else if(Reg_Init[index].Reg_Type == Byte_type)
       {
           Byte_Data_Pntr = (BYTE *) (Reg_Init[index].Reg_Addr);
           *Byte_Data_Pntr = (BYTE) Reg_Init[index].Reg_Value;
       }
   }


  /*
  Initialize PS/2 register (PSCON) in hardware shift mode or in
  legacy mode.  Put in receive mode.  Use data from Reg_Init.PS_Con.

  It is assumed that PSCON_XMT and PSCON_EN bits in Reg_Init.PS_Con
  are clear so that PSCON will be set to receive mode and disabled. */

  Ps2_Init(PSCON_INIT);

}
