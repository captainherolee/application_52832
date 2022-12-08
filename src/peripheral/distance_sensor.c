#include <stdint.h>
#include "common.h"
#include "i2c.h"
#include "distance_sensor.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"

// *****************************************************************************
// Define
// *****************************************************************************
#define VL530l0X_SLAVE_ADDR		 0x52

#define vl_read(a)			i2c2_read_byte(VL530l0X_SLAVE_ADDR, a)
#define vl_send(a, b)		i2c2_send_byte(VL530l0X_SLAVE_ADDR, a, b)
#define vl_send_word(a, b)  i2c2_send_byte2(VL530l0X_SLAVE_ADDR, a, b)
#define vl_read_word(a)		i2c2_read_byte2(VL530l0X_SLAVE_ADDR, a)

#define vl_enable()			i2c2_enable(true)
#define vl_disable()		i2c2_enable(false)

uint8_t stop_variable = 0;

uint16_t periph_distance_sensor_read(void)
{
	uint8_t data[2];
    uint16_t distance_data = 0;

	vl_enable();

	data[1] = vl_read(0x1E);
	data[0] = vl_read(0x1F);


	distance_data = (((uint16_t) data[1] << 8) + data[0]);
	vl_send(0x00, 0x01);

	vl_disable();

	return distance_data;
}

void periph_distance_sensor_init(void)
{

	vl_enable();

	vl_disable();
}
