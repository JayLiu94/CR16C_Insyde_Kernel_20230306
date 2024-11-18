/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     PWM.C
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "types.h"
#include "com_defs.h"
#include "swtchs.h"
#include "pwm.h"
#include "regs.h"
#include <math.h>
#if 1 //PWM_SUPPORTED

/*---------------------------------------------------------------------------------------------------------*/
/* Function: PWM_config                                                                                    */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Configure prescaler divider, cycle time and duty cycle value in the module defined by pwm_module.    */
/*    Note: The function disables the requested module so it will be possible to configure it.Therefore,   */
/*    The macro PWM_ENABLE() must be used after it to enable the module.                                   */
/*    Note 2: When using heart beat mode, it is most reccomended to use a cycle time of 0x007F. Lower      */
/*    values can be used, but it might result in a "coarse" appearance of heart beat effect.               */
/*    The full heart beat cycle time for the current configured module is:                                 */
/*             Cycle_Time == (prescaler_divider + 1) * (cycle_time + 1) * RATE_DIV * 64 * Tclk             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Module_t          pwm_module          The module to be configured.                               */
/*    PWM_Clock_Select_t    clock_select        FXCLK, FRCLK, Core clock or 32K clock.                     */
/*    WORD                  prescaler_divider   Prescaler divider value.                                   */
/*    WORD                  cycle_time          Cycle time value.                                          */
/*    WORD                  duty_cycle          Duty cycle value.                                          */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_config(
    PWM_Module_t pwm_module,
    PWM_Clock_Select_t clock_select,
    WORD prescaler_divider,
    WORD cycle_time,
    WORD duty_cycle)
{
    ASSERT(pwm_module < PWM_MODULE_LAST);

    SET_BIT(DEVALT5, 1 << pwm_module);
    PWM_DISABLE(pwm_module);

    PWM_clock_select(pwm_module, clock_select);
    WORD_PWM_REG(PWM_PRSC, pwm_module) = prescaler_divider;
    WORD_PWM_REG(PWM_CTR, pwm_module)  = cycle_time;
    WORD_PWM_REG(PWM_DCR, pwm_module)  = duty_cycle;
    PWM_ENABLE(pwm_module);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: PWM_config_heart_beat_mode                                                                    */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Configure the heart beat mode.                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Module_t              pwm_module      The module to be configured.                               */
/*    PWM_Heart_Bit_Mode_t      heart_beat      Heart beat mode, can be configured as:                     */
/*                                              00 - Off.                                                  */
/*                                              01 - 25% maximum duty cycle (when CTR == 0x007F)           */
/*                                              10 - 50% maximum duty cycle (when CTR == 0x007F)           */
/*                                              11 - 100% maximum duty cycle (when CTR == 0x007F)          */
/*    PWM_Heart_Beat_Rate_Div_t rate_div        Heart beat divider.                                        */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_config_heart_beat_mode(
    PWM_Module_t pwm_module,
    PWM_Heart_Beat_Mode_t heart_beat,
    PWM_Heart_Beat_Rate_Div_t rate_div,
    PWM_Heart_Beat_Ext_On_t Ext_On,
    PWM_Heart_Beat_Ext_Off_t Ext_Off,
    PWM_Heart_Beat_Ext_Off2_t Ext_Off2
    )
{
    WORD ctr;
    ASSERT(pwm_module < PWM_MODULE_LAST);

    SET_FIELD(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_HBSEL, heart_beat);
    SET_FIELD(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), PWMCTLEX_RATE_DIV, rate_div);
    SET_FIELD(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_EXT_OFF, Ext_Off);
    SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_INVP);
    ctr = ((WORD)Ext_On << 10) + ((WORD)Ext_Off2 << 7) + PWM_HB_CTR;
    PWM_config(pwm_module, PWM_32K_CLOCK, 0, ctr, 0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PWM_ConfigHeartBeat                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  pwm_module - The module to be configured.                                              */
/*                  rise_time  - Rise time length in ms (up to ~3.8sec)                                    */
/*                  on_time    - On time length in ms (0 to ~12.8)                                         */
/*                  fall_time  - Fall time length in ms (up to ~3.8sec)                                    */
/*                  off_time   - Off time length in ms (0 to ~12.8)                                        */
/*                  hb_mode    - standard or visual                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the heart beat module.                                         */
/*                  The HW can be configured in many ways, but currently this                              */
/*                  function support the follwoing timings:                                                */
/*                  On/Off time can be 0 or between N*101.5625 ms up to ~12.8 seconds                      */
/*                  Rise/Fall time can be of up ~to 3.8sec                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_ConfigHeartBeat(
    PWM_Module_t pwm_module,
    PWM_Heart_Beat_Ctrl_t hb_mode,
    DWORD rise_time,
    DWORD fall_time,
    DWORD on_time,
    DWORD off_time)
{
    WORD ctr;
    BYTE ext_off, ext_on, steps;
    DWORD clock;


    ASSERT(pwm_module < PWM_MODULE_LAST);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set prescaler according to the requested timing                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((off_time >= 6500)  || (on_time >= 6500) ||
        (rise_time >= 2000) || (fall_time > 2000))
    {
        clock = 32768 / 2;
        WORD_PWM_REG(PWM_PRSC, pwm_module) = 1;
    }
    else
    {
        clock = 32768;
        WORD_PWM_REG(PWM_PRSC, pwm_module) = 0;
    }
    if (hb_mode == STANDARD_MODE)
    {
        ctr = sqrt((DWORD)rise_time*clock/1000UL);     //calc rise time ctr
        steps = ctr/4;
    }
    else
    {
        ctr = sqrt((DWORD)rise_time*clock*23/4000UL);     //calc rise time ctr
        steps = ctr/23;
    }

    SET_BIT(DEVALT5, 1 << pwm_module);
    PWM_DISABLE(pwm_module);
    SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_INVP);
    CLEAR_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_HBNK_SEL);
    SET_FIELD(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWM_HB_DC_CTL, hb_mode);
    /* Clock source select. */
    PWM_clock_select(pwm_module, PWM_32K_CLOCK);
    SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_HBNK_SEL);

    WORD_PWM_REG(PWM_CTR_RS, pwm_module) = ctr - 1;
    BYTE_PWM_REG(PWM_N_STEP_RS, pwm_module)  = steps;
    WORD_PWM_REG(PWM_MAX_DC_RS, pwm_module)  = ctr - 1;

    /* Fall time configured. */
    if (hb_mode == STANDARD_MODE)
    {
        ctr = sqrt((DWORD)fall_time*clock/1000UL);     //calc rise time ctr
        steps = ctr/4;
    }
    else
    {
        ctr = sqrt((DWORD)fall_time*clock*23/4000UL);     //calc rise time ctr
        steps = ctr/23;
    }

    WORD_PWM_REG(PWM_CTR_FL, pwm_module)  = ctr - 1;
    BYTE_PWM_REG(PWM_N_STEP_FL, pwm_module)  = steps;
    WORD_PWM_REG(PWM_MAX_DC_FL, pwm_module)  = ctr - 1;
    /* on/off time configured. */
    BYTE_PWM_REG(PWM_EXT_ON, pwm_module)  = (BYTE) (((DWORD)on_time * clock) / (PWM_HB_ONOFF_PRESCALAR * 1000UL));
    BYTE_PWM_REG(PWM_EXT_OFF, pwm_module)  = (BYTE) (((DWORD)off_time * clock) / (PWM_HB_ONOFF_PRESCALAR * 1000UL));

    PWM_ENABLE(pwm_module);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: PWM_config_force_active                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Configure force active.                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Module_t    pwm_module       The module to be forced active.                                     */
/*    PWM_Force_Src_t source           The source of the force active action (GPIO42, GPIO43 or DISABLED). */
/*    bool            source_active_high  TRUE  - source active high                                       */
/*                                        FALSE - source active low                                        */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_config_force_active(
    PWM_Module_t pwm_module,
    PWM_Force_Src_t source,
    bool source_active_high)
{
    SET_FIELD(PWM_SEL, PWM_SEL_FPWM_SEL, source);

    if (source) /* (source == GPIO42) || (source == GPIO43) */
    {
        if (source_active_high)
        {
            SET_BIT(PWM_SEL, PWM_SEL_BIT(source));
        }
        else
        {
            CLEAR_BIT(PWM_SEL, PWM_SEL_BIT(source));
        }

        SET_BIT(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), FORCE_EN);
    }
    else /* source == DISABLED */
    {
        CLEAR_BIT(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), FORCE_EN);
    }
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: PWM_clock_select                                                                              */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Configure force active.                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Module_t       pwm_module        The module to be forced active.                                 */
/*    PWM_Clock_Select_t clock_select      FXCLK, FRCLK, Core clock or 32K clock.                          */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_clock_select(PWM_Module_t pwm_module, PWM_Clock_Select_t clock_select)
{
    if ((clock_select == PWM_32K_CLOCK) || (clock_select == PWM_CORE_CLOCK))
    {
        SET_FIELD(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), PWM_FCK_SEL, FCK_CORE_32K);
    if (clock_select == PWM_32K_CLOCK)
    {
        SET_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_CKSEL);
    }
    else /* PWM_CORE_CLOCK */
    {
        CLEAR_BIT(BYTE_PWM_REG(PWM_PWMCTL, pwm_module), PWMCTL_CKSEL);
    }
}
    else if (clock_select == PWM_FIX_CLOCK)
    {   /* PWM FMCLK/2 clock */
        SET_FIELD(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), PWM_FCK_SEL, FCK_FXCLK);
    }
    else
    {   /* PWM FRCLK clock */
        SET_FIELD(BYTE_PWM_REG(PWM_PWMCTLEX, pwm_module), PWM_FCK_SEL, PWM_FR_CLOCK);
    }
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: PWM_custom_pattern                                                                            */
/*                                                                                                         */
/* Description:                                                                                            */
/*    This function control custom pattern for led blinking. The pattern is defined by 2 16-bits registers,*/
/*    SEQH and SEQL, which construct together a 32-bit shift register. Every 128 PWM cycles, the register  */
/*    is shifted by one bit (that period of time is considered as one slot). If the current bit of the     */
/*    pattern is 1, the PWM generates a normal PWM cycle. If the current bit is 0, the PWM is inactive.    */
/*    The duration of one slot of the 32-bit pattern is:                                                   */
/*       (PRSCn + 1) * (CTRn + 1) * 128 * Tclk.                                                            */
/*    Note: By default, pattern value is 0xFFFFFFFF. To bring a PWM back to normal operation from a pattern*/
/*    operation, the default value should be assigned to SEQH and SEQL.                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    PWM_Module_t  pwm_module  The module num of the module that will use the pattern.                    */
/*    DWORD        pattern     The pattern                                                                */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None.                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_custom_pattern(PWM_Module_t pwm_module, DWORD pattern)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Assign the pattern low and high register with the requested pattern.                                */
    /*-----------------------------------------------------------------------------------------------------*/
    WORD_PWM_REG(PWM_SEQL, pwm_module) = pattern&0xFFFF;
    WORD_PWM_REG(PWM_SEQH, pwm_module) = (pattern&0xFFFF0000)>>16;
}





#endif // PWM_SUPPORTED

