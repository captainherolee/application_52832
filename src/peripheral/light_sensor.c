#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "app_gpiote.h"

#include "common.h"
#include "i2c.h"
#include "light_sensor.h"

#include "ble_system.h"



#define LTR303_SLAVE_ADDR	0x52

#define ltr_read(a)			i2c2_read_byte(LTR303_SLAVE_ADDR, a)
#define ltr_send(a, b)		i2c2_send_byte(LTR303_SLAVE_ADDR, a, b)
#define ltr_enable()		i2c2_enable(true)
#define ltr_disable()		i2c2_enable(false)

#define LTR_ALS_CONTR			0x80
#define LTR_ALS_MEAS_RATE		0x85
#define LTR_PART_ID				0x86
#define LTR_MANUFAC_ID			0x87
#define LTR_ALS_DATA_CH1_L		0x88
#define LTR_ALS_DATA_CH1_H		0x89
#define LTR_ALS_DATA_CH0_L		0x8A
#define LTR_ALS_DATA_CH0_H		0x8B
#define LTR_ALS_STATUS			0x8C

#define LTR_GAIN                6
#define MEAS_RATE               200


#if (MODULE_LIGHT_SENSOR_MODEL == LIGHT_LTR303)
    light_sensor_data_t g_light_data = {
        .ch0 = 0,
        .ch1 = 1,
    };
#endif

light_sensor_data_t* periph_light_sensor_read(void)
{
	uint8_t data[4];
    uint16_t light_ch0_data;
    uint16_t light_ch1_data;

	ltr_enable();

	data[2] = ltr_read(LTR_ALS_DATA_CH1_L);
	data[3] = ltr_read(LTR_ALS_DATA_CH1_H);
    data[0] = ltr_read(LTR_ALS_DATA_CH0_L);
	data[1] = ltr_read(LTR_ALS_DATA_CH0_H);

	ltr_disable();

	light_ch0_data = (((uint16_t) data[1] << 8) + data[0]);
	light_ch1_data = ((uint16_t) (data[3] << 8) + data[2]);

    g_light_data.ch0 = (int32_t)(light_ch0_data);
    g_light_data.ch1 = (int32_t)(light_ch1_data);

    g_light_data.lux = periph_light_sensor_calc_lux(g_light_data.ch0, g_light_data.ch1);

    return &g_light_data;
}

double periph_light_sensor_calc_lux(int32_t ch0, int32_t ch1)
{
    double ratio;
    double d0, d1;
    double lux;

    d0 = (double)(ch0);
    d1 = (double)(ch1);

    if (ch0 == 65535 || ch1 == 65535) {
        return 0.0;
    }

    ratio = d1 / d0;

    d0 *= (402.0 / MEAS_RATE);
    d1 *= (402.0 / MEAS_RATE);

    if (ratio < 0.5) {
        lux = 0.0304 * d0 - 0.062 * d0 * pow(ratio, 1.4);
    } else if (0.5 <= ratio && ratio < 0.61) {
        lux = 0.0224 * d0 - 0.031 * d1;
    } else if (0.61 <= ratio && ratio < 0.80) {
        lux = 0.0128 * d0 - 0.0153 * d1;
    } else if (0.80 <= ratio && ratio < 1.30) {
        lux = 0.00146 * d0 - 0.00112 * d1;
    } else {
        lux = 0.0;
    }

    return lux;
}

void periph_light_sensor_init(void)
{
    uint8_t gain = LTR_GAIN;
	uint8_t data[2] = { 0, 0 };

	ltr_enable();

	/* Read Chip ID */
	data[0] = ltr_read(LTR_MANUFAC_ID);
	nrf_delay_ms(10);
		
	data[1] = ltr_read(LTR_PART_ID);
	nrf_delay_ms(10);
		
	if((data[0] == 0x05) && (data[1] == 0xA0)) {
		PRINTF("[%s] Success. ID = %02X, %02X\r\n", _FN_, data[0], data[1]);
	
        ltr_send(LTR_ALS_CONTR, (gain << 2) | 0x01);
        ltr_send(LTR_ALS_MEAS_RATE, 0x12);
	} else {
		PRINTF("[%s] Error. ID = %02X, %02X", _FN_, data[0], data[1]);
	}

	ltr_disable();
}
