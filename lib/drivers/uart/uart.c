#include "app_uart.h"
#include "nrf_gpio.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "common.h"
#include "uart.h"

void uart_event_handle(app_uart_evt_t * p_event)
{
    uint8_t data;

    switch (p_event->evt_type) {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data));
            break;

        case APP_UART_COMMUNICATION_ERROR:
            //APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

void uart_init(uint8_t pin_rx, uint8_t pin_tx)
{
    ret_code_t err_code;
    app_uart_comm_params_t const comm_params = {
        .rx_pin_no    = (int)pin_rx,
        .tx_pin_no    = (int)pin_tx,
        .rts_pin_no   = GPIO_UART_RTS,
        .cts_pin_no   = GPIO_UART_CTS,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        #if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
        #else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
        #endif
    };

    APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, uart_event_handle, APP_IRQ_PRIORITY_LOWEST, err_code);
    APP_ERROR_CHECK(err_code);
}
