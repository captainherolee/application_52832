#ifndef __BLE_SYSTEM_H__
#define __BLE_SYSTEM_H__

#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_nus.h"

typedef struct {
    int32_t method;
    int32_t opcode;
    int32_t data[5];
} comm_ble_rx_data_t;

extern uint16_t m_conn_handle;
extern uint16_t m_nus_data_len;

void comm_ble_set_immediate_response(int32_t val);
int32_t comm_ble_get_immediate_response(void);
int32_t comm_ble_get_device_connected(void);
void comm_ble_set_device_connected(int32_t val);
comm_ble_rx_data_t* comm_ble_get_rxdata(void);
int32_t comm_ble_get_evt(void);
void comm_ble_set_evt(int32_t evt);
void comm_ble_notify(uint8_t *data);
void advertising_start(void);
void comm_ble_init(void);
int32_t comm_ble_get_advertising(void);
void comm_ble_advertising_start(void);

#endif /* __BLE_SYSTEM_H__ */
