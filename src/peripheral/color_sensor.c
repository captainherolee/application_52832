

#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "app_gpiote.h"

#include "common.h"
#include "i2c.h"
#include "color_sensor.h"
#include "ble_system.h"


#define TCS34725_I2C_ADDRESS       (0x29 << 1)
#define TCS34725_I2C_PART_NUMBER    0x44 // TCS34721 and TCS34725

#define ctr_read(a)			i2c2_read_byte(TCS34725_I2C_ADDRESS, a)
#define ctr_send(a, b)		i2c2_send_byte(TCS34725_I2C_ADDRESS, a, b)
#define ctr_enable()		i2c2_enable(true)
#define ctr_disable()		i2c2_enable(false)


#define TCS34725_COMMAND_BIT            0x80

#define TCS34725_REG_ENABLE             0x00        /**< enable register */
#define TCS34725_REG_ATIME              0x01
#define TCS34725_REG_CONTROL            0x0F        /**< control register */

#define TCS34725_REG_ID                 0x12
#define TCS34725_REG_CDATAL             0x14
#define TCS34725_REG_CDATAH             0x15
#define TCS34725_REG_RDATAL             0x16
#define TCS34725_REG_RDATAH             0x17
#define TCS34725_REG_GDATAL             0x18
#define TCS34725_REG_GDATAH             0x19
#define TCS34725_REG_BDATAL             0x1A
#define TCS34725_REG_BDATAH             0x1B

#define TCS34725_REG_STATUS             0x13
#define TCS34725_REG_CLEAR              0xE6

#define TCS34725_REG_ENABLE_PON         0x01
#define TCS34725_REG_ENABLE_AEN         0x02
#define CTR_GAIN_1X                     0x00
#define CTR_GAIN_4X                     0x01
#define CTR_GAIN_16X                    0x02
#define CTR_GAIN_60X                    0x03

#define TCS34725_INTEGRATIONTIME_2_4MS   0xFF
#define TCS34725_INTEGRATIONTIME_614MS   0x00

color_sensor_data_t g_cct_data;

uint16_t _calculate_cct(uint16_t r, uint16_t g, uint16_t b)
{
    float X, Y, Z, xc, yc, n, cct;

    if (r == 0 && g == 0 && b == 0) {
        return 0;
    }

    X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
    Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);


    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);


    n = (xc - 0.3320F) / (0.1858F - yc);
    cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

    return (uint16_t)cct;
}

uint16_t _calculate_cct_dn40(uint16_t r, uint16_t g, uint16_t b, uint16_t c)
{
    uint16_t r2, b2;
    uint16_t ir, cct;

    if (c == 0) {
        return 0;
    }

    ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
    r2 = r - ir;
    b2 = b - ir;
    if( r2 == 0) {
        return 0;
    }

    cct = (3810 * (uint32_t)b2) / (uint32_t)r2 + 1391;

    return cct;
}

uint16_t _calculate_lux(uint16_t r, uint16_t g, uint16_t b)
{
    return (uint16_t)((-0.32466F * r) + (1.57837F * g) + (-0.73191F * b));
}

color_sensor_data_t* periph_color_sensor_cct_read(void)
{
	uint8_t data[8];
    uint16_t r, g, b, c;

	ctr_enable();

    data[0] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_RDATAL);
    data[1] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_RDATAH);
    data[2] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_GDATAL);
    data[3] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_GDATAH);
    data[4] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_BDATAL);
    data[5] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_BDATAH);
    data[6] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_CDATAL);
    data[7] = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_CDATAH);

	ctr_disable();
    nrf_delay_ms((256 - TCS34725_INTEGRATIONTIME_614MS) * 12 / 5 + 1);

    r = (((uint16_t) data[1] << 8) + data[0]);
    g = (((uint16_t) data[3] << 8) + data[2]);
    b = (((uint16_t) data[5] << 8) + data[4]);
    c = (((uint16_t) data[7] << 8) + data[6]);


    g_cct_data.cct = _calculate_cct_dn40(r, g, b, c);
    g_cct_data.lux = _calculate_lux(r, g, b);

    PRINTF("cct[%ld]lux[%ld]\n", g_cct_data.cct, g_cct_data.lux);

    return &g_cct_data;
}

void periph_color_sensor_init(void)
{
	uint8_t data;

	ctr_enable();
	/* Read Chip ID */
	data = ctr_read(TCS34725_COMMAND_BIT | TCS34725_REG_ID);
	nrf_delay_ms(10);

	PRINTF("TCS34725 Chip ID: 0x%02X\n", data);
    if (data == TCS34725_I2C_PART_NUMBER) {
        PRINTF("Device Found Success\n");
        ctr_send(TCS34725_COMMAND_BIT | TCS34725_REG_ATIME, TCS34725_INTEGRATIONTIME_614MS);
        ctr_send(TCS34725_COMMAND_BIT | TCS34725_REG_CONTROL, CTR_GAIN_1X);

        ctr_send(TCS34725_COMMAND_BIT | TCS34725_REG_ENABLE, TCS34725_REG_ENABLE_PON);
        nrf_delay_ms(3);
        ctr_send(TCS34725_COMMAND_BIT | TCS34725_REG_ENABLE, TCS34725_REG_ENABLE_PON | TCS34725_REG_ENABLE_AEN);
        nrf_delay_ms((256 - TCS34725_INTEGRATIONTIME_614MS) * 12 / 5 + 1);
    } else {
        PRINTF("TCS34725 Chip ID doesn't match. TCS34725 init failed.\n");
    }
	ctr_disable();
}


