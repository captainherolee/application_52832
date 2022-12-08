#include <stdint.h>
#include "common.h"
#include "i2c.h"
#include "accel_mc3416.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "app_gpiote.h"

// *****************************************************************************
// Define
// *****************************************************************************
#define MC3416_SLAVE_ADDR		(0x98)

#define acc_read_byte(a)		i2c1_read_byte(MC3416_SLAVE_ADDR, a)
#define acc_send_byte(a, b)		i2c1_send_byte(MC3416_SLAVE_ADDR, a, b)
#define acc_enable()			i2c1_enable(true)
#define acc_disable()			i2c1_enable(false)

#define GPIOTE_MAX_USERS    1
app_gpiote_user_id_t m_user_id;

acc_reg_st	g_add_reg[] = {
	{ 0x07, 0x00 },	// Standby Mode
	{ 0x20, 0x09 },	// Range Control, 2g
	{ 0x08, 0x05 },	// Sample Rate, 512
	{ 0x06, 0x44 },	// Interrupt Control   ACQ_INT_EN
	{ 0x09, 0x04 },	// Motion Control
	{ 0x43, 0x14 },	// AnyMotion Threshold LSB 
	{ 0x44, 0x00 },	// AnyMotion Threshold MSB 
	{ 0x45, 0x88 },	// AnyMotion Debounce 
	{ 0x07, 0x01 },	// Wake Mode
	{ 0xFF, 0xFF },	// End
};

void _event_handler(uint32_t const *event_pins_low_to_high, uint32_t const *event_pins_high_to_low)
{
    if (*event_pins_low_to_high & (1 << GPIO_ACC_INT)) {

    	PRINTF("Acc Interrupt\r\n");
    }
}

void periph_accel_interrupt_init(void)
{
    uint32_t    gpio_pin_state;
    uint32_t    err_code = 0;
    app_gpiote_user_pin_config_t config[1];
    
    APP_GPIOTE_INIT(GPIOTE_MAX_USERS);

    config[0].pin_number = GPIO_ACC_INT;
    config[0].sense      = NRF_GPIOTE_POLARITY_LOTOHI;
    
    err_code = app_gpiote_user_register_ex(&m_user_id, config, 1, _event_handler);
    APP_ERROR_CHECK(err_code); 
    
    err_code = app_gpiote_user_enable(m_user_id);
    APP_ERROR_CHECK(err_code); 
    

    nrf_gpio_cfg_sense_input(GPIO_ACC_INT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    err_code = app_gpiote_pins_state_get(m_user_id, &gpio_pin_state);
    APP_ERROR_CHECK(err_code); 

}

void periph_accel_mc3416_read(acc_data_st *acc)
{
	uint8_t data[6];

	acc_enable();

	data[0] = acc_read_byte(XOUT_EX_L);
	data[1] = acc_read_byte(XOUT_EX_H);
	data[2] = acc_read_byte(YOUT_EX_L);
	data[3] = acc_read_byte(YOUT_EX_H);
	data[4] = acc_read_byte(ZOUT_EX_L);
	data[5] = acc_read_byte(ZOUT_EX_H);
	
	acc_disable();
	
	acc->ax = (int16_t) (data[0] | ((uint16_t) data[1] << 8));
	acc->ay = (int16_t) (data[2] | ((uint16_t) data[3] << 8));
	acc->az = (int16_t) (data[4] | ((uint16_t) data[5] << 8));

}

void _accel_mc3416_setup(void)
{
	uint8_t index;

	for (index = 0; ; index++) {
		if (g_add_reg[index].addr == 0xFF) {
			break;
		}

		acc_send_byte(g_add_reg[index].addr, g_add_reg[index].data);
		nrf_delay_ms(5);

		if (acc_read_byte(g_add_reg[index].addr) != g_add_reg[index].data) {
			PRINTF("E A[%02X]D[%02X]\r\n", g_add_reg[index].addr, g_add_reg[index].data);
			return;
		}
	}

	PRINTF("Accel MC3416 Setup Success\n");
}

void periph_accel_init(void)
{
	uint8_t data;

	acc_enable();

	data = acc_read_byte(MC3416_CHIP_ID_REG);
	PRINTF("Accel Chip ID: 0x%02X\n", data);
	if (data == MC3416_VENDOR_ID) {
		PRINTF("Device Found Success\n");
		_accel_mc3416_setup();
	} else {
		PRINTF("Accel Chip ID doesn't match. Accel init failed.\n");
	}
	
	acc_disable();
}
