#ifndef __UART_H__
#define __UART_H__

#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 256

void uart_init(uint8_t pin_rx, uint8_t pin_tx);

#endif
