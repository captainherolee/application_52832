#ifndef __SYSTIMER_H__
#define __SYSTIMER_H__

typedef struct {
    int32_t hour;
    int32_t min;
    int32_t sec;
} clock_time_t;

// For Accuracy
void clock_init(void);


// Clock Timer
void sys_timer_waiting_time_start(void);
void sys_timer_waiting_time_stop(void);
clock_time_t* sys_timer_clock_get(void);
int32_t sys_timer_clock_get_hour(void);
void sys_timer_clock_set(clock_time_t *time);
void sys_timer_init(uint16_t period_ms);
void sys_timer_start(void);
void sys_timer_stop(void);
uint32_t sys_timer_get(void);


#endif /* __SYSTIMER_H__ */
