
#include "nrf_gpio.h"

#include "common.h"
#include "pwm.h"


#define MAX_PWM_CH	2

#define PWM_CH_1	0
#define PWM_CH_2	1

uint16_t pwm_value[MAX_PWM_CH] = { PWM0_PERIOD_MAX, PWM0_PERIOD_MAX };

void pwm_set(uint16_t ch1, uint16_t ch2)
{
    pwm_value[PWM_CH_1] = (PWM0_PERIOD_MAX - ch1);
    pwm_value[PWM_CH_2] = (PWM0_PERIOD_MAX - ch2);

    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
}

void pwm_init(int8_t pin_day, int8_t pin_night)
{
    // PORT
    nrf_gpio_cfg_output(PWM_CH_1);
    nrf_gpio_pin_clear(PWM_CH_1);
    
    nrf_gpio_cfg_output(PWM_CH_2);
    nrf_gpio_pin_clear(PWM_CH_2);
    
    NRF_PWM0->PSEL.OUT[PWM_CH_1] = ((int)pin_day << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[PWM_CH_2] = ((int)pin_night << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

    NRF_PWM0->ENABLE     = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM0->MODE       = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER  = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM0->COUNTERTOP = (PWM0_PERIOD_MAX << PWM_COUNTERTOP_COUNTERTOP_Pos);
    NRF_PWM0->LOOP       = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    NRF_PWM0->DECODER    = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);

    NRF_PWM0->SEQ[0].PTR = (uint32_t) pwm_value;
    NRF_PWM0->SEQ[0].CNT = (4 << PWM_SEQ_CNT_CNT_Pos);
    NRF_PWM0->SEQ[0].REFRESH  = 0;
    NRF_PWM0->SEQ[0].ENDDELAY = 0;

    NRF_PWM0->TASKS_SEQSTART[0] = 0;
}
