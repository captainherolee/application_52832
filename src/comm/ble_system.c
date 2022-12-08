#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
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
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "app_error.h"
#include "app_uart.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include <nrf_timer.h>
#include <nrf_drv_timer.h>

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "common.h"
#include "ble_system.h"
#include "uart.h"
#include "systimer.h"
#include "chip.h"

comm_ble_rx_data_t g_op_data;

// BLE Global Variables
int32_t g_ble_evt = 0;
int32_t g_device_connected = 0;
int32_t g_advertising = 0;
int32_t g_ble_immediate_response = 0;
// BLE Instances
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_PERIPHERAL_LINK_COUNT);
NRF_BLE_GATT_DEF(m_gatt);
NRF_BLE_QWR_DEF(m_qwr);
BLE_ADVERTISING_DEF(m_advertising);

uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;
uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;
ble_uuid_t           m_adv_uuids;
ble_nus_t            *p_nus;

uint32_t             m_tx_count = 0;
uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;	
uint16_t             m_nus_data_len = (BLE_GATT_ATT_MTU_DEFAULT - 3);
uint8_t              m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
ble_gap_addr_t       m_bd_address;
ble_gap_adv_params_t m_adv_params; 
ble_gap_adv_data_t   m_adv_data;


void _gap_params_init(void)
{
    uint8_t res[DEVICE_ID_LIMIT+1] = {0,};
    uint8_t device_name[DEVICE_NAME_LIMIT+1] = {0,};

    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    config_get_chip_id(res);

    snprintf((char*)device_name, DEVICE_NAME_LIMIT+1, "%s (%s)", (char*)DEVICE_NAME, (char*)res);
    PRINTF("Device Name[%s]\n", device_name);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t*) device_name,
                                          strlen((char*)device_name));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void _nrf_qwr_error_handler(ret_code_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void _set_rx_data_to_buffer(comm_ble_rx_data_t* p_buff, int32_t buffer_idx, int32_t buffer_val)
{
    if (p_buff != NULL) {
        if (buffer_idx == 0) {
            p_buff->method = buffer_val;
        } else if (buffer_idx == 1) {
            p_buff->opcode = buffer_val;
        } else if (buffer_idx == 2) {
            p_buff->data[0] = buffer_val;
        } else if (buffer_idx == 3) {
            p_buff->data[1] = buffer_val;
        } else if (buffer_idx == 4) {
            p_buff->data[2] = buffer_val;
        } else if (buffer_idx == 5) {
            p_buff->data[3] = buffer_val;
        } else if (buffer_idx == 6) {
            p_buff->data[4] = buffer_val;
        }
    }
}
/*
void _nus_data_handler(ble_nus_evt_t * p_evt)
{
    int32_t i = 0;
    int32_t ble_buffer_val = 0;
    int32_t ble_data_idx = 0;
    comm_ble_rx_data_t * p_data = NULL;

    if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
        p_data = &g_op_data;
        memset(p_data, 0x00, sizeof(comm_ble_rx_data_t));
        for (i = 0; i < p_evt->params.rx_data.length; i++) {
            // Op Code process
            if (p_evt->params.rx_data.p_data[i] != BLE_DATA_SEPERATOR) {
                // Handle Error
                if (!isdigit(p_evt->params.rx_data.p_data[i])) {
                    PRINTF("Invalid BLE Input. Exit decoding process.\n");
                    memset(p_data, 0x00, sizeof(comm_ble_rx_data_t));
                    comm_ble_notify((uint8_t*)("BLE Bad Request"));
                    return;
                }
                ble_buffer_val *= 10;
                ble_buffer_val += p_evt->params.rx_data.p_data[i] - CONVERT_STRING_TO_NUMBER;
            } else {
                _set_rx_data_to_buffer(p_data, ble_data_idx, ble_buffer_val);
                
                ble_buffer_val = 0;
                ble_data_idx++;
            }
        }

        _set_rx_data_to_buffer(p_data, ble_data_idx, ble_buffer_val);
        comm_ble_op_handle(p_data);
    }
}*/
static void nus_data_handler(ble_nus_evt_t * p_evt)
{

    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;

        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }
        if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length - 1] == '\r')
        {
            while (app_uart_put('\n') == NRF_ERROR_BUSY);
        }
    }

}

void _services_init(void)
{
    int32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0,};

    qwr_init.error_handler = _nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    p_nus = &m_nus;
}

static void _on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

static void _conn_params_error_handler(ret_code_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
    return;
}

void _conn_params_app_timer_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void _conn_params_init(void)
{
    ret_code_t	err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = _on_conn_params_evt;
    cp_init.error_handler                  = _conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

static void _on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt) {
        case BLE_ADV_EVT_FAST:
            break;
        case BLE_ADV_EVT_IDLE:
        {
            PRINTF("Advertising Stop\n");
            comm_ble_advertising_start();
            break;
        }
        default:
            break;
    }
}

static void _ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;
    ble_gap_phys_t const phys = {
        .rx_phys = BLE_GAP_PHY_AUTO,
        .tx_phys = BLE_GAP_PHY_AUTO,
    };

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
        {
            PRINTF("BLE Connected\n");
            g_device_connected = TRUE;
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, m_conn_handle, 0);
            APP_ERROR_CHECK(err_code);

            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);


            break;
        }
        case BLE_GAP_EVT_DISCONNECTED:
        {
            PRINTF("BLE Disconnected\n");
            g_device_connected = FALSE;
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            

            break;
        }
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);

            break;
        } 
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        {
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);

            break;
        }
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        {
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);

            break;
        }
        case BLE_GATTC_EVT_TIMEOUT:
        {
            PRINTF("BLE_GATTC_EVT_TIMEOUT\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);

            break;
        }
        case BLE_GATTS_EVT_TIMEOUT:
        {
            PRINTF("BLE_GATTS_EVT_TIMEOUT\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);

            break;
        }
        default:
            break;
    }
}


void _ble_stack_init(void)
{
    ret_code_t err_code;

    _conn_params_app_timer_init();

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, _ble_evt_handler, NULL);
}

void _gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        PRINTF("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    PRINTF("ATT MTU exchange completed. central 0x%x peripheral 0x%x\r\n",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}

void _gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, _gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}

void _advertising_init(void)
{
    ret_code_t	err_code;
    ble_advertising_init_t init;

    m_adv_uuids.uuid = BLE_UUID_NUS_SERVICE;
    m_adv_uuids.type = NUS_SERVICE_UUID_TYPE;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = 1;
    init.srdata.uuids_complete.p_uuids  = &m_adv_uuids;

    init.config.ble_adv_fast_enabled  = TRUE;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = _on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, 0, 0);
    APP_ERROR_CHECK(err_code);
}

int32_t comm_ble_get_advertising(void)
{
    return g_advertising;
}

void comm_ble_advertising_start(void)
{
    PRINTF("Advertising Start\n");
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    g_advertising = TRUE;
}

void _advertising_stop(void)
{
    sd_ble_gap_adv_stop(m_advertising.adv_handle);
}

int32_t comm_ble_get_device_connected(void)
{
    return g_device_connected;
}

void comm_ble_set_device_connected(int32_t val)
{
    if (g_device_connected != val) {
        g_device_connected = val;
    }
}
int32_t comm_ble_get_evt(void)
{
    return g_ble_evt;
}

void comm_ble_set_evt(int32_t evt)
{
    if (g_ble_evt != evt) {
        g_ble_evt = evt;
    }
}

int32_t comm_ble_get_immediate_response(void)
{
    return g_ble_immediate_response;
}

void comm_ble_set_immediate_response(int32_t val)
{
    if (g_ble_immediate_response != val) {
        g_ble_immediate_response = val;
    }
}


comm_ble_rx_data_t* comm_ble_get_rxdata(void)
{
    return &g_op_data;
} 

void comm_ble_notify(uint8_t *data)
{
    ret_code_t err_code;
    uint16_t len = 0;

    if (data != NULL && g_device_connected == TRUE)  {
        len = strlen((char*)data);
        PRINTF("[%s]\n", data);

        err_code = ble_nus_data_send(p_nus, data, &len, m_conn_handle);

        if (err_code != NRF_SUCCESS) {
        	PRINTF("Error");
        }
    }
}

void comm_ble_init(void)
{
    _ble_stack_init();
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    _gap_params_init();
    _gatt_init();
    _services_init();
    _advertising_init();
    _conn_params_init();
    comm_ble_advertising_start();
    PRINTF("BLE init success\n");
}
