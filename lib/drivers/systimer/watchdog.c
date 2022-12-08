#include "nrf.h"
#include "nrf_delay.h"

#include "common.h"
#include "watchdog.h"

void watchdog_init(uint32_t time_sec)
{
    NRF_WDT->TASKS_START = 0;
    NRF_WDT->CONFIG = ((WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos));
    NRF_WDT->CRV = (time_sec * 32768);
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;      // Enable Reload Register 0
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;      // Reload The Watchdog Counter  
    NRF_WDT->TASKS_START = 1;

    PRINTF("[%s] CRV = %ld\r\n", _FN_, NRF_WDT->CRV);
}

void watchdog_update(void)
{
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;  // Reload The Watchdog Counter
}
