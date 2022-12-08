#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_delay.h"


#include "common.h"
#include "i2c.h"
#include "accel_mc3416.h"
#include "light_sensor.h"
#include "systimer.h"
#include "watchdog.h"
#include "power_mgmt.h"
#include "ble_system.h"
#include "chip.h"
#include "db.h"
#include "uart.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


int main(void)
{
    clock_init();
    device_power_mgmt_init_reset_mode_status();
    nrf_pwr_mgmt_init();

#if (LOG_ENABLED == TRUE)
    uart_init(GPIO_UART_RXD, GPIO_UART_TXD);
#endif
    
    PRINTF("* ================================================ *\n");
    PRINTF("* ==================SYSTEM START================== *\n");
    PRINTF("* ================================================ *\n");

    sys_timer_init(SYS_TIMER_INTERVAL);
    sys_timer_start();

    watchdog_init(DEFAULT_WATCHDOG_TIME_SEC);

    i2c_init(I2C1_CHANNEL);
    i2c_init(I2C2_CHANNEL);

    periph_accel_interrupt_init();
    periph_accel_init();
    periph_light_sensor_init();
    

    for(;;)
    {
        nrf_pwr_mgmt_run();
    }
}
