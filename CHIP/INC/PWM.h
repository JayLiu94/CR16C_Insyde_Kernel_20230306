/*-----------------------------------------------------------------------------
 * Copyright (c) 2006-2010 by Nuvoton Technology Corporation
 * All rights reserved.
 *<<<--------------------------------------------------------------------------
 * File Contents:
 *     pwm.h - Pulse width modulator module
 *
 * Project:
 *     Driver Set for uRider Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------>>>*/

#ifndef PWM_H
#define PWM_H

/*------------------------------------------------------------------------------
 * PULSE WIDTH MODULATOR (PWM) Registers
 *----------------------------------------------------------------------------*/
#define PWM_HB_ONOFF_PRESCALAR  1664UL
#define PWM_HB_CTR              0x7F
/* PWM registers access */
#define PWM_REG(reg_name)                       (PORT_BYTE(reg_name))
#define BYTE_PWM_REG(reg_name, pwm_module)      (PORT_BYTE(PWM_MODULE_OFFSET * pwm_module + reg_name))
#define WORD_PWM_REG(reg_name, pwm_module)      (PORT_WORD(PWM_MODULE_OFFSET * pwm_module + reg_name))

#define PWM_SEL_GPIO43_POL              0x05
#define PWM_SEL_GPIO42_POL              0x05

/* PWM usefull registers fields */
#define PRSC_PRSC_P                     0x00
#define PRSC_PRSC_S                     0x10

#define CTR_CTR_P                       0x00
#define CTR_CTR_S                       0x10

#define CTR_CTR_HEART_BIT_MODE_P        0x00
#define CTR_CTR_HEART_BIT_MODE_S        0x07

#define PWMCTL_EXT_OFF_P                0x05
#define PWMCTL_EXT_OFF_S                0x02

#define PWMCTL_HBSEL_P                  0x02
#define PWMCTL_HBSEL_S                  0x02

#define DCR_DCR_P                       0x00
#define DCR_DCR_S                       0x10

#define SEQL_SLT_15_0_P                 0x00
#define SEQL_SLT_15_0_S                 0x10

#define SEQH_SLT_31_16_P                0x00
#define SEQH_SLT_31_16_S                0x10

#define PWMCTLEX_RATE_DIV_P             0x02
#define PWMCTLEX_RATE_DIV_S             0x02

#define PWM_SEL_FPWM_SEL_P              0x00
#define PWM_SEL_FPWM_SEL_S              0x02

#define PWM_HB_DC_CTL_P                 0x02
#define PWM_HB_DC_CTL_S                 0x02

#define PWM_FCK_SEL_P                   0x04
#define PWM_FCK_SEL_S                   0x02
/*------------------------------------------------------------------------------
 * System Configuration Registers
 *----------------------------------------------------------------------------*/
//#define SYSC_REGS_BASE  0xFFF000

//#define PWM_SEL         (PORT_BYTE(SYSC_REGS_BASE + 0x26))

/*---------------------------------------------------------------------------------------------------------*/
/* Typedefs.                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

/* PWM modules enum   */
typedef enum
{
    PWM_MODULE_A = 0,
    PWM_MODULE_B = 1,
    PWM_MODULE_C = 2,
    PWM_MODULE_D = 3,
    PWM_MODULE_E = 4,
    PWM_MODULE_F = 5,
    PWM_MODULE_G = 6,
    PWM_MODULE_H = 7,
    PWM_MODULE_LAST = 8
} PWM_Module_t;



/* Heartbeat mode max duty cycle value  */
typedef enum
{
    NORMAL = 0,
    MAX_25_PERCENT = 1,
    MAX_50_PERCENT = 2,
    MAX_100_PERCENT = 3
} PWM_Heart_Beat_Mode_t;


/* Heartbeat mode Duty Cycle Control*/
typedef enum
{
    NORMAL_MODE = 0,
    VISVAL_MODE = 1,
    STANDARD_MODE = 2
} PWM_Heart_Beat_Ctrl_t;

/* Heartbeat mode rate divider  */
typedef enum
{
    DIVIDE_BY_1 = 0,
    DIVIDE_BY_4 = 1,
    DIVIDE_BY_8 = 2,
    DIVIDE_BY_16 = 3
} PWM_Heart_Beat_Rate_Div_t;

/* Heartbeat mode Extension off  */
typedef enum
{
    NO_EXTEN_OFF = 0,
    EXTEN_BY_8 = 1,
    EXTEN_BY_16 = 2,
    EXTEN_BY_CTR = 3
} PWM_Heart_Beat_Ext_Off_t;

/* Heartbeat mode Extension on */
typedef enum
{
    NO_EXTEN_ON = 0,
    EXTEN_ON_BY_32 = 1,
    EXTEN_ON_BY_48 = 2,
    EXTEN_ON_BY_64 = 3,
    EXTEN_ON_BY_96 = 4,
    EXTEN_ON_BY_128 = 5
} PWM_Heart_Beat_Ext_On_t;

/* Heartbeat mode Extension off 2  */
typedef enum
{
    EXTEN_OFF2_BY_32 = 0,
    EXTEN_OFF2_BY_64 = 1,
    EXTEN_OFF2_BY_96 = 2,
    EXTEN_OFF2_BY_128 = 3,
    EXTEN_OFF2_BY_192 = 4,
    EXTEN_OFF2_BY_256 = 5
} PWM_Heart_Beat_Ext_Off2_t;


/* Force active source  */
typedef enum
{
    DISABLED = 0,
    GPIO42 = 1,
    GPIO43 = 2
} PWM_Force_Src_t;


/* Clock source  */
typedef enum
{
    PWM_CORE_CLOCK = 0,
    PWM_32K_CLOCK  = 1,
    PWM_FIX_CLOCK = 2,
    PWM_FR_CLOCK = 3
} PWM_Clock_Select_t;



/*---------------------------------------------------------------------------------------------------------*/
/* PWM macros                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_INVERSE_OUTPUT                                                                               */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Inverse PWM output.                                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    pwm_module  The module num of the module that will use the pattern.                                  */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_INVERSE_OUTPUT(pwm_module) (SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_INVP))


/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_NORMAL_OUTPUT                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set PWM out to normal.                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    pwm_module  The module num of the module that will use the pattern.                                  */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_NORMAL_OUTPUT(pwm_module) (CLEAR_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_INVP))


/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_ENABLE                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable PWM module.                                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    pwm_module  The module num of the module that will use the pattern.                                  */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_ENABLE(pwm_module)         (SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_PWR))


/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_DISABLE                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable ENC module.                                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    pwm_module  The module num of the module that will use the pattern.                                  */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_DISABLE(pwm_module)        (CLEAR_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_PWR))


/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_SEL_BIT                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Convert force active enum source (GPIO42 or GPIO43) to the corresponding bit number in PWM_SEL       */
/*    register.                                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Force_Src_t source    Force active source.                                                       */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    The source's corresponding bit number in PWM_SEL register.                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_SEL_BIT(source)         (1 << (3 + source))


/*---------------------------------------------------------------------------------------------------------*/
/* Macro: PWM_SET_DUTYCYCLE                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set duty cycle                                                                                       */
/*                                                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    pwm_module                                                                                           */
/*    duty_cycle                                                                                           */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_SET_DUTYCYCLE(pwm_module, duty_cycle)   (WORD_PWM_REG(PWM_DCR, pwm_module) = duty_cycle)


/*---------------------------------------------------------------------------------------------------------*/
/* PWM functions                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_config(
    PWM_Module_t pwm_module,
    PWM_Clock_Select_t clock_select,
    WORD prescaler_divider,
    WORD cycle_time,
    WORD duty_cycle);

void PWM_config_heart_beat_mode(
    PWM_Module_t pwm_module,
    PWM_Heart_Beat_Mode_t heart_bit,
    PWM_Heart_Beat_Rate_Div_t rate_div,
    PWM_Heart_Beat_Ext_On_t Ext_On,
    PWM_Heart_Beat_Ext_Off_t Ext_Off,
    PWM_Heart_Beat_Ext_Off2_t Ext_Off2
    );

void PWM_ConfigHeartBeat(
    PWM_Module_t pwm_module,
    PWM_Heart_Beat_Ctrl_t hb_mode,
    DWORD rise_time,
    DWORD fall_time,
    DWORD on_time,
    DWORD off_time);

void PWM_custom_pattern(
    PWM_Module_t pwm_module,
    DWORD pattern);

void PWM_config_force_active(
    PWM_Module_t pwm_module,
    PWM_Force_Src_t source,
    bool source_active_high);

void PWM_clock_select(
    PWM_Module_t pwm_module,
    PWM_Clock_Select_t clock_select);



#endif // PWM_H


