#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_power.h"
#include "nrf_gpio.h"
#include "nrf_sdh.h"

#include "common.h"
#include "power_mgmt.h"
#include "systimer.h"
#include "ble_system.h"


static RESET_MODE g_reset_mode = RESET_MODE_POWER;

void device_power_mgmt_init_reset_mode_status(void)
{
	uint32_t mode = 0;

	if (nrf_sdh_is_enabled() == TRUE) {
		sd_power_gpregret_get(0, &mode);
		g_reset_mode = mode;
		sd_power_gpregret_clr(0, 0xffffffff);
	} else {
		g_reset_mode = NRF_POWER->GPREGRET;
		NRF_POWER->GPREGRET = 0;
	}
}

RESET_MODE device_power_mgmt_get_static_reset_mode_status(void)
{
	return g_reset_mode;
}

void device_power_mgmt_set_reset_mode_status(uint32_t status)
{
    if (nrf_sdh_is_enabled() == TRUE) {
        sd_power_gpregret_set(0, status);
        g_reset_mode = status;
    } else {
        NRF_POWER->GPREGRET = status;
        g_reset_mode = status;
    }
}
