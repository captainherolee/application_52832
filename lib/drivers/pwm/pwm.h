#ifndef __PWM_H__
#define __PWM_H__

#define PWM0_PERIOD_MAX 500

void pwm_set(uint16_t ch1, uint16_t ch2);
void pwm_init(int8_t pin_day, int8_t pin_night);

#endif /* __PWM_H__ */