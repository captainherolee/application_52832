#include <stdbool.h>

#include <nrf_timer.h>
#include <nrf_drv_timer.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "common.h"
#include "systimer.h"

volatile int32_t g_tick;
volatile int32_t g_clock_time_tick;
volatile uint32_t g_sys_timer_counter;

clock_time_t g_clock;


void clock_init(void)
{
    sd_clock_hfclk_request();

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}


void sys_timer_init(uint16_t period_ms)
{
    NRF_TIMER2->TASKS_STOP   = 1;
    NRF_TIMER2->TASKS_CLEAR  = 1;
    NRF_TIMER2->MODE         = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE      = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER2->PRESCALER    = 9;
    NRF_TIMER2->CC[0]        = (float)31250 * ((float)period_ms / 1000.0f);
    NRF_TIMER2->INTENSET     = 65536;
    NRF_TIMER2->SHORTS       = 0x01;

}

clock_time_t* sys_timer_clock_get(void)
{
    int32_t read_time = 0;

    read_time = g_clock_time_tick;

    g_clock.sec = read_time % 60;
    g_clock.min = (read_time / 60) % 60;
    g_clock.hour = (read_time / (60*60)) % 24;

    return &g_clock;
}

int32_t sys_timer_clock_get_hour(void)
{
    int32_t read_time = 0;

    read_time = g_clock_time_tick;

    return (read_time / (60*60)) % 24;
}

void sys_timer_start(void)
{
    NVIC_EnableIRQ(TIMER2_IRQn);
    NRF_TIMER2->TASKS_START = 1;
}

void sys_timer_stop(void)
{
    NVIC_DisableIRQ(TIMER2_IRQn);
    NRF_TIMER2->TASKS_START = 0;
    g_sys_timer_counter = 0;
}

uint32_t sys_timer_get(void)
{
    return g_sys_timer_counter;
}


void TIMER2_IRQHandler(void)
{
    if (NRF_TIMER2->EVENTS_COMPARE[0] != 0) {

        PRINTF("g_clock_time_tick: %ld\n", g_clock_time_tick);
        
        g_sys_timer_counter++;

        NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    }
}
