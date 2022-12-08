#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>


// Utility macro
//#define MIN(a, b) (((a) < (b)) ? (a) : (b))
//#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(a) (((a) > 0 ? (a) : (-a)))

#define DEFAULT_WATCHDOG_TIME_SEC 3
#define POWERSAVE_TIME			  1200 //120 Sec (100 millisecond criteria)

// BLE & Timer define
//====================================================================
#define MANUFACTURER_NAME               "Captain Tech"
#define APP_BLE_CONN_CFG_TAG            1
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN
#define APP_BLE_OBSERVER_PRIO           3
#define APP_ADV_INTERVAL                300
#define APP_ADV_DURATION                12000   //120 Sec (10 millisecond criteria
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)
#define SLAVE_LATENCY                   0
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)
#define MAX_CONN_PARAMS_UPDATE_COUNT    3

#define SEC_PARAM_BOND                      1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                      0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS                  0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                      /**< Maximum encryption key size. */

#define APP_TIMER_PRESCALER			0
#define APP_TIMER_OP_QUEUE_SIZE		5
#define APP_TIMER_INTERVAL   APP_TIMER_TICKS(100)

#define TIME_MIN			(60)
#define TIME_HOUR			(60 * TIME_MIN)
#define TIME_DAY			(24 * TIME_HOUR)
#define TIMER_PERIOD        1000
#define TIMER_UNIT_SEC      1
#define TIMER_UNIT_MIN      60
#define ISR_INTERVAL_MS		100

#define TIME_200MS_ISR		(200  / ISR_INTERVAL_MS)
#define TIME_500MS_ISR		(500  / ISR_INTERVAL_MS)
#define TIME_1SEC_ISR		(1000 / ISR_INTERVAL_MS)
#define TIME_2SEC_ISR		(2000 / ISR_INTERVAL_MS)
#define TIME_4SEC_ISR		(4000 / ISR_INTERVAL_MS)
#define TIME_MIN_ISR		(60 * TIME_1SEC_ISR)

#define MAIN_TIMER_INTERVAL 	1000
#define SYS_TIMER_INTERVAL 	    1000
#define CLOCK_TIMER_MAXSEC      86399
#define GENERAL_TIMER_INTERVAL	1000
#define GENERAL_TIMER_MAXSEC    86399

// GPIO
//====================================================================
#define I2C1_CHANNEL        1
#define I2C2_CHANNEL        2
#define GPIO_UART_CTS		0xFF			// UART CTS
#define GPIO_UART_RTS		0xFF			// UART RTS
#define GPIO_I2C1_SDA		GPIO_ACC_SDA
#define GPIO_I2C1_SCL		GPIO_ACC_SCL

#define GPIO_I2C2_SDA		GPIO_LIGHT_SDA
#define GPIO_I2C2_SCL		GPIO_LIGHT_SCL

// Define
//====================================================================
#define FALSE   			0
#define TRUE    			1
#define ERRORED             2

#define LOW					0
#define HIGH				1

#define SLOW				0
#define FAST				1

#define OFF					0
#define ON					1
#define NO_CTL				2
#define NULL ((void *)0)

#define _LN_				__LINE__
#define _FN_				(char *) __FUNCTION__

#define LOG_ENABLED         TRUE


#if (LOG_ENABLED == true)
#define PRINTF                      printf
#else
#define PRINTF(...)
#endif

// System Settings
// ================================================================================
// DFU
#define START_DFU 0xB1

// FW Version
#define DEVICE_FW_VERSION   "1.0"
// SDK Version
#define SDK_VERSION         "v17.1"

// Device Name
#define DEVICE_NAME_LIMIT   11
#define DEVICE_ID_LIMIT     4
#define DEVICE_NAME         "Hero"



// Timer Limit
#define TIMER_LIMIT_HIGH_LIMIT        240     // 4 hours
#define TIMER_LIMIT_LOW_LIMIT         1       // 1 minute

#define BLE_BUFFER_MAXSIZE          20

#define DB_START_ADDRESS            0x69000
#define DB_END_ADDRESS              0x69FFF

#define DB_READ_MAXSIZE             4

#define DB_BRIGHTNESS               0
#define DB_CCT                      1
#define DB_TIMERLIMIT               2
#define DB_DIMFACTOR                3

#define DB_CHAR_LEN                 11
#define DB_STABILIZE_WAIT           20

typedef enum {
    GPIO_XL1,		// P0.0
    GPIO_XL2,
    GPIO_ADC_BAT,
	GPIO_CHARGE_EN,
	GPIO_CHARGE_STAT,	// P0.4
    GPIO_NC_05,
    GPIO_NC_06,
	GPIO_HALL_SENSOR,
    GPIO_NC_08,		// P0.8
	GPIO_NC_09,
	GPIO_NC_10,
    GPIO_NC_11,
    GPIO_PWM_LED1,	// P0.12
    GPIO_PWM_LED2,
    GPIO_NC_14,
    GPIO_CHARGE_LED,
    GPIO_HALL_SENSOR_EN,	// P0.16
    GPIO_NC_17,
    GPIO_NC_18,
    GPIO_UART_TXD,
    GPIO_UART_RXD,	// P0.20
    GPIO_NU_21,
    GPIO_NC_22,
    GPIO_NC_23,
    GPIO_NC_24,		// P0.24
	GPIO_USB_DETECT,
    GPIO_LIGHT_INT,
    GPIO_LIGHT_SCL,
    GPIO_LIGHT_SDA,	// P0.28
    GPIO_ACC_INT,
    GPIO_ACC_SDA,
    GPIO_ACC_SCL,
} gpio0_map_en;

typedef enum {
    RESET_MODE_POWER = 0,
    RESET_MODE_SLEEP,
} RESET_MODE;

#endif /* __COMMON_H__ */
