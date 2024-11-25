/* ----------------------------------------------------------------------------
 * MODULE ICU.H
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
 * Interrupt Control Unit (ICU).
 * Based on PC87570 code version 2.00.07[2667-21]-n.
 *
 * Revision History (Started March 15, 2000 by GLP)
 *
 * ---------------------------------------------------------------------------- */

#ifndef ICU_H
#define ICU_H

#include "swtchs.h"
#include "types.h"

#define ICU_ADDR_BASE 0xFFFE00

void Icu_Init(void);

/* PSR bit. */
#define PSR_I_BIT 11

extern void int_hnd(void);
extern void int_hnd2(void);
#define Svc       0
#define Dvz       0
#define Flg       0
#define Bpt       0
#define Trc       0
#define Und       0
#define Ise       0

/* ------------------ external interrupts ------------------ */

typedef enum {

   /* formal names */
   ICU_EINT0 = 0,
   ICU_EINT1,
   ICU_EINT2,
   ICU_EINT3,
   ICU_EINT4,
   ICU_EINT5,
   ICU_EINT6,
   ICU_EINT7,
   ICU_EINT8,
   ICU_EINT9,
   ICU_EINT10,
   ICU_EINT11,
   ICU_EINT12,
   ICU_EINT13,
   ICU_EINT14,
   ICU_EINT15,

   ICU_EINT16,
   ICU_EINT17,
   ICU_EINT18,
   ICU_EINT19,
   ICU_EINT20,
   ICU_EINT21,
   ICU_EINT22,
   ICU_EINT23,
   ICU_EINT24,
   ICU_EINT25,
   ICU_EINT26,
   ICU_EINT27,
   ICU_EINT28,
   ICU_EINT29,
   ICU_EINT30,
   ICU_EINT31,

   /* functionality */
                                    /* GLP NOTE: CORRECT EDGES? */

   ICU_EINT_PS2_1    = ICU_EINT21,  /* Falling edge. */

   ICU_EINT_SPI      = ICU_EINT12,  /* Falling edge. */
   ICU_EINT_KBD_SCAN = ICU_EINT15,  /* High level. */
   ICU_EINT_T0OUT    = ICU_EINT31,  /* Rising edge. */
   ICU_EINT_MFT161    = ICU_EINT9,   /* High level. */
   ICU_EINT_MFT162   = ICU_EINT23,   /* High level. */
   ICU_EINT_IBF      = ICU_EINT25,  /* High level. */
   ICU_EINT_OBE      = ICU_EINT2,   /* High level. */
   ICU_EINT_IBF2     = ICU_EINT26,  /* High level. */

   ICU_EINT_ADC      = ICU_EINT10,

   ICU_EINT_I2C      = ICU_EINT13,   /* High level. */
   ICU_EINT_I2C2     = ICU_EINT14,   /* High level. */
   ICU_EINT_I2C3     = ICU_EINT4,   /* High level. */
   ICU_EINT_I2C4     = ICU_EINT5,   /* High level. */

   ICU_EINT_GPIO44   = ICU_EINT1,
   ICU_EINT_GPIO1    = ICU_EINT30,
   ICU_EINT_GPIO46   = ICU_EINT22,
   ICU_EINT_GPIO3    = ICU_EINT8,
   ICU_EINT_GPIO4    = ICU_EINT20,
   ICU_EINT_GPIO5    = ICU_EINT19,  /* Falling edge. */
   ICU_EINT_MSWC     = ICU_EINT16,
   ICU_EINT_MIWU3    = ICU_EINT11,
   ICU_EINT_ITIM81   = ICU_EINT28,
   ICU_EINT_ITIM82   = ICU_EINT27,
   ICU_EINT_ITIM83   = ICU_EINT17,
#if CR_UART_SUPPORTED
   ICU_EINT_UART	 = ICU_EINT18,
#endif // CR_UART_SUPPORTED
   ICU_EINT_ESPI     = ICU_EINT7,
   ICU_EINT_MIWU2    = ICU_EINT7
} ext_int_t;

/* Interrupt number register. */
#define IVCT PORT_BYTE IVCT_ADDR   /* Read only. */
#define IVCT_INTVECT   (0xF << 0)  /* Value of highest priority
                                      enabled pending interrupt. */

/* Non-Maskable Interrupt Status register
   Holds the current pending interrupt.
   It is cleared when read. */
#define NMISTAT PORT_BYTE NMISTAT_ADDR   /* Read only. */
#define NMISTAT_PFAIL     (1 << 0)       /* Power fail input pending. */

/* Power Fail Control register. */
#define EXNMI PORT_BYTE EXNMI_ADDR       /* Read/Write. */
#define PFAIL_EN        (1 << 0)         /* Enable power fail pin to
                                            generate non-maskable interrupt. */
#define PFAIL_PIN       (1 << 1)         /* Value of power fail pin. */

/* Interrupt Status registers (tells which interrupts are pending). */
#define ISTAT0 PORT_WORD ISTAT0_ADDR /* Read only. */
#define ISTAT1 PORT_WORD ISTAT1_ADDR /* Read only. */

/* Interrupt Enable and Mask registers. */
#define IENAM0 PORT_WORD IENAM0_ADDR  /* Read/Write. */
#define IENAM1 PORT_WORD IENAM1_ADDR  /* Read/Write. */

/* Edge Interrupt Clear registers. */
#define IECLR0 PORT_WORD IECLR0_ADDR /* Write only. */
#define IECLR1 PORT_WORD IECLR1_ADDR /* Write only. */

#define ICU_EINT_MASK(int_num)       ((int_num < 16) ? ((IENAM0) &= ~((WORD) 1 << (int_num & 0xF))) : ((IENAM1) &= ~((WORD) 1 << ((int_num - 16) & 0xF))))
#define ICU_EINT_UNMASK(int_num)     ((int_num < 16) ? ((IENAM0) |=  ((WORD) 1 << (int_num & 0xF))) : ((IENAM1) |=  ((WORD) 1 << ((int_num - 16) & 0xF))))
#define ICU_EINT_IS_ENABLED(int_num) ((int_num < 16) ? ((IENAM0) &   ((WORD) 1 << (int_num & 0xF))) : ((IENAM1) &   ((WORD) 1 << ((int_num - 16) & 0xF))))
#define ICU_EINT_CLEAR(int_num)      ((int_num < 16) ? ((IECLR0)  =  ((WORD) 1 << (int_num & 0xF))) : ((IECLR1)  =  ((WORD) 1 << ((int_num - 16) & 0xF))))
#define ICU_EINT_PENDING(int_num)    ((int_num < 16) ? ((ISTAT0) &   ((WORD) 1 << (int_num & 0xF))) : ((ISTAT1) &   ((WORD) 1 << ((int_num - 16) & 0xF))))

/* Disable the interrupt. */
#define ICU_Disable_Irq(int_num)          ICU_EINT_MASK(int_num)

/* Enable the interrupt. */
#define ICU_Enable_Irq(int_num)           ICU_EINT_UNMASK(int_num)

/* Returns non-zero (not FALSE) if interrupt is enabled. */
#define ICU_Irq_Is_Enabled(int_num)       ICU_EINT_IS_ENABLED(int_num)

/* Clear pending edge-triggered interrupt. */
#define ICU_Clr_Pending_Irq(int_num)      ICU_EINT_CLEAR(int_num)

/* Returns non-zero (not FALSE) if interrupt is pending. */
#define ICU_Irq_Is_Pending(int_num)       ICU_EINT_PENDING(int_num)

/* Returns non-zero (not FALSE) if interrupt is enabled and pending. */
#define ICU_Irq_Is_Active(int_num) (ICU_Irq_Is_Enabled(int_num) && ICU_Irq_Is_Pending(int_num))






#endif   /* ICU_H */

