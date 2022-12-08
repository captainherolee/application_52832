#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "fds.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_delay.h"

#include "common.h"
#include "db.h"
#include "ble_system.h"

static void _fds_evt_handler(fds_evt_t const* p_evt);

static bool volatile _m_fds_initialized;
db_save_t g_save_cfg;

static bool volatile g_flash_busy;

fds_record_t const save_data = {
    .file_id           = FILE_ID,
    .key               = REC_KEY,
    .data.p_data       = &g_save_cfg,
    .data.length_words = (sizeof(g_save_cfg) / sizeof(uint32_t)),
};

static struct {
    bool delete_next;
    bool pending;
} m_delete_all;

const char* _fds_err_str(ret_code_t ret)
{
    /* Array to map FDS return values to strings. */
    static char const* err_str[] = {
        "FDS_ERR_OPERATION_TIMEOUT",
        "FDS_ERR_NOT_INITIALIZED",
        "FDS_ERR_UNALIGNED_ADDR",
        "FDS_ERR_INVALID_ARG",
        "FDS_ERR_NULL_ARG",
        "FDS_ERR_NO_OPEN_RECORDS",
        "FDS_ERR_NO_SPACE_IN_FLASH",
        "FDS_ERR_NO_SPACE_IN_QUEUES",
        "FDS_ERR_RECORD_TOO_LARGE",
        "FDS_ERR_NOT_FOUND",
        "FDS_ERR_NO_PAGES",
        "FDS_ERR_USER_LIMIT_REACHED",
        "FDS_ERR_CRC_CHECK_FAILED",
        "FDS_ERR_BUSY",
        "FDS_ERR_INTERNAL",
    };

    return err_str[ret - NRF_ERROR_FDS_ERR_BASE];
}

static void _fds_evt_handler(fds_evt_t const* p_evt)
{
    if (p_evt->result != NRF_SUCCESS) {
        PRINTF("[%s] Error while executing an fds operation.\n");
        PRINTF("fds error: %s\n", _fds_err_str(p_evt->result));
        return;
    }

    switch (p_evt->id) {
        case FDS_EVT_INIT:
        {
            if (p_evt->result == NRF_SUCCESS) {
                _m_fds_initialized = true;
                g_flash_busy = false;
                PRINTF("fds init event success\n");
            }
            break;
        }
        case FDS_EVT_WRITE:
        {
            if (p_evt->result == NRF_SUCCESS) {
                g_flash_busy = false;
                PRINTF("fds write event success\n");
            }
            break;
        }
        case FDS_EVT_UPDATE:
        {
            if (p_evt->result == NRF_SUCCESS) {
                g_flash_busy = false;
                PRINTF("fds update event success\n");
            }
            break;
        }
        case FDS_EVT_DEL_RECORD:
        {
            if (p_evt->result == NRF_SUCCESS) {
                m_delete_all.pending = false;
                g_flash_busy = false;
                PRINTF("fds delete event success\n");
            }
            break;
        }
        case FDS_EVT_DEL_FILE:
        {
            if (p_evt->result == NRF_SUCCESS) {
                g_flash_busy = false;
                PRINTF("fds delete FILE event success\n");
            }
            break;
        }
        case FDS_EVT_GC:
        {
            if (p_evt->result == NRF_SUCCESS) {
                g_flash_busy = false;
                PRINTF("fds garbage collect event success\n");
            }
            break;
        }
        default:
            break;
    }
}

void _db_wait_for_init(void)
{
    uint32_t err_code;

    while (!_m_fds_initialized) {
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}

void _db_wait_for_ready(void)
{
    uint32_t err_code;

    while (db_get_flash_busy()) {
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}

void db_save(void)
{
    fds_record_desc_t record_desc={0,};

    _db_wait_for_ready();

    g_flash_busy = true;

    ret_code_t ret = fds_record_write(&record_desc, &save_data);
    if (ret != NRF_SUCCESS) {
    	PRINTF("fds save failed\n");
        return;
    }

    _db_wait_for_ready();
    
    PRINTF("fds write success, writing record ID = %d\n", (int)record_desc.record_id);
    return;
}

void db_clean(void)
{
    fds_record_desc_t record_desc;
    fds_find_token_t ftok;
    ftok.page = 0;
    ftok.p_addr = NULL;

    _db_wait_for_ready();

    g_flash_busy = true;

    while (fds_record_find(FILE_ID, REC_KEY, &record_desc, &ftok) == NRF_SUCCESS) {
        fds_record_delete(&record_desc);
        PRINTF("fds cleaning in progress... -> deleted record ID: %d\n", (int)record_desc.record_id);
    }

    ret_code_t ret = fds_gc();
    if (ret != NRF_SUCCESS) {
    	PRINTF("fds clean: garbage collection failed\n");
        return;
    }

    _db_wait_for_ready();
    
    return;
}

uint32_t db_load(uint32_t* p_buff)
{
    fds_flash_record_t  flash_record;
    fds_record_desc_t   record_desc;
    fds_find_token_t    ftok={0};
    uint32_t err_code;

    _db_wait_for_ready();

    g_flash_busy = true;

    while (fds_record_find(FILE_ID, REC_KEY, &record_desc, &ftok) == NRF_SUCCESS) {
    	PRINTF("fds read: record load start\n");
        err_code = fds_record_open(&record_desc, &flash_record);
        if (err_code != NRF_SUCCESS) {
        	PRINTF("fds read: record open error\n");
            return err_code;
        }

        memcpy(p_buff, (uint32_t*)flash_record.p_data, sizeof(uint32_t) * DB_READ_MAXSIZE);

        for (uint16_t i = 0; i < flash_record.p_header->length_words; i++) {
        	PRINTF("%2uth: %d\n", i, (int)p_buff[i]);
        }

        err_code = fds_record_close(&record_desc);
        if (err_code != NRF_SUCCESS) {
        	PRINTF("fds read: record close error\n");
            return err_code;
        }
        log_dPRINTFebug("fds read: record load end\n");
    }

    PRINTF("fds read: record load success\n");

    g_flash_busy = false;

    return err_code;
}

void db_init(void)
{
    _m_fds_initialized = false;
    g_flash_busy = true;

    fds_register(_fds_evt_handler);
    fds_init();

    _db_wait_for_init();
    _db_wait_for_ready();

    return;
}

void db_update(void)
{
    db_clean();
    _db_wait_for_ready();

    db_save();
    _db_wait_for_ready();
}

bool db_get_flash_busy(void)
{
    return g_flash_busy;
}

void db_set_config(db_save_t *config)
{
    PRINTF("Set DB\n");
}
