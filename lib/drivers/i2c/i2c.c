
#include "common.h"
#include "nrf_gpio.h"
#include "i2c.h"
#include "nrf_delay.h"


void i2c1_start(void)
{
    nrf_gpio_pin_set(GPIO_I2C1_SDA);
    nrf_gpio_pin_set(GPIO_I2C1_SCL);
    i2c_delay();
    nrf_gpio_pin_clear(GPIO_I2C1_SDA);
    i2c_delay();
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
}

void i2c1_restart(void)
{
    nrf_gpio_cfg_output(GPIO_I2C1_SDA);
    nrf_gpio_pin_clear(GPIO_I2C1_SDA);
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    
    nrf_gpio_pin_set(GPIO_I2C1_SDA);
    nrf_gpio_pin_set(GPIO_I2C1_SCL);

    i2c1_start();
}

void i2c1_stop(void)
{
    nrf_gpio_pin_clear(GPIO_I2C1_SDA);
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    i2c_delay();
    nrf_gpio_pin_set(GPIO_I2C1_SCL);
    i2c_delay();
    nrf_gpio_pin_set(GPIO_I2C1_SDA);
    i2c_delay();
}

void i2c1_acks(void)
{
    uint8_t wait = 10;
    
    nrf_gpio_cfg_input(GPIO_I2C1_SDA, NRF_GPIO_PIN_PULLUP);
    i2c_delay();
    
    nrf_gpio_pin_set(GPIO_I2C1_SCL);

    while (wait--) {
        if (nrf_gpio_pin_read(GPIO_I2C1_SDA) == LOW) {
            break;
        }

        i2c_delay();
    }


    if (wait == 0xFF) {
        PRINTF("==== I2C1 ack ====\n");
    }

    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    nrf_gpio_cfg_output(GPIO_I2C1_SDA);
}

void i2c1_ackm(void)
{
    nrf_gpio_cfg_output(GPIO_I2C1_SCL);
    nrf_gpio_cfg_output(GPIO_I2C1_SDA);
    
    nrf_gpio_pin_clear(GPIO_I2C1_SDA);
    i2c_delay();

    nrf_gpio_pin_set(GPIO_I2C1_SCL);
    i2c_delay();
    
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    i2c_delay();
}

void i2c1_nacks(void)
{
    uint8_t wait = 10;
    
    nrf_gpio_cfg_input(GPIO_I2C1_SDA, NRF_GPIO_PIN_PULLUP);
    i2c_delay();
    
    nrf_gpio_pin_set(GPIO_I2C1_SCL);

    while (wait--) {
        if (nrf_gpio_pin_read(GPIO_I2C1_SDA) == HIGH) {
            break;
        }

        i2c_delay();
    }

    if (wait == 0xFF) {
        PRINTF("==== I2C1 NAK ====\n");
    }

    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    nrf_gpio_cfg_output(GPIO_I2C1_SDA);
}

void i2c1_nackm(void)
{
    nrf_gpio_cfg_output(GPIO_I2C1_SCL);
    nrf_gpio_cfg_output(GPIO_I2C1_SDA);
    
    nrf_gpio_pin_set(GPIO_I2C1_SDA);
    i2c_delay();

    nrf_gpio_pin_set(GPIO_I2C1_SCL);
    i2c_delay();
    
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
    i2c_delay();
}

void i2c1_send_data(uint8_t data)
{
    uint8_t index = 8;
        
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);
        
    while (index--) {
        if ((data & 0x80) == 0) {
            nrf_gpio_pin_clear(GPIO_I2C1_SDA);
        } else {
            nrf_gpio_pin_set(GPIO_I2C1_SDA);
        }
        
        i2c_delay();
        nrf_gpio_pin_set(GPIO_I2C1_SCL);

        i2c_delay();
        nrf_gpio_pin_clear(GPIO_I2C1_SCL);
        
        data <<= 1;
    }
}

uint8_t i2c1_read_data(void)
{
    uint8_t index = 8;
    uint8_t data = 0;

    nrf_gpio_cfg_input(GPIO_I2C1_SDA, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_pin_clear(GPIO_I2C1_SCL);	  	
    
    while (index--) {
        data <<= 1;
        
        nrf_gpio_pin_set(GPIO_I2C1_SCL);
        i2c_delay();
        
        if (nrf_gpio_pin_read(GPIO_I2C1_SDA) == HIGH) {
            data |= HIGH;
        }
        
        nrf_gpio_pin_clear(GPIO_I2C1_SCL);
        i2c_delay();
    }

    nrf_gpio_cfg_output(GPIO_I2C1_SDA);

    return data;
}

uint8_t i2c1_read_byte(uint8_t devAddr, uint8_t regAddr)
{
    uint8_t data;

    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data(regAddr);
    i2c1_acks();
    
    i2c1_restart();

    i2c1_send_data(devAddr + I2C_READ);
    i2c1_acks();
    
    data = i2c1_read_data();
    i2c1_nackm();
    
    i2c1_stop();

    return data;
}


uint8_t i2c1_read_byte2(uint8_t devAddr, uint16_t regAddr)
{
    uint8_t data;

    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 8) & 0xFF);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 0) & 0xFF);
    i2c1_acks();
    
    i2c1_start();
    
    i2c1_send_data(devAddr + I2C_READ);
    i2c1_acks();
    
    data = i2c1_read_data();
    i2c1_acks();
    
    i2c1_stop();

    return data;
}

void i2c1_read_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data(regAddr);
    i2c1_acks();
    
    i2c1_stop();
    
    i2c1_start();
    
    i2c1_send_data(devAddr + I2C_READ);
    i2c1_acks();

    while (length) {
        *buffer++ = i2c1_read_data();
        
        if (length == 1) {
            i2c1_nackm();
        } else {
            i2c1_ackm();
        }

        length--;
    }
    
    i2c1_stop();
}

void i2c1_read_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 8) & 0xFF);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 0) & 0xFF);
    i2c1_acks();
    
    i2c1_start();
    
    i2c1_send_data(devAddr + I2C_READ);
    i2c1_acks();

    while (length) {
        *buffer++ = i2c1_read_data();
            
        if (length == 1) {
            i2c1_nackm();
        } else {
            i2c1_ackm();
        }

        length--;
    }
    
    i2c1_stop();
}

void i2c1_send_byte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data(regAddr);
    i2c1_acks();
    
    i2c1_send_data(data);
    i2c1_acks();
    
    i2c1_stop();
}

void i2c1_send_byte2(uint8_t devAddr, uint16_t regAddr, uint8_t data)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 8) & 0xFF);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 0) & 0xFF);
    i2c1_acks();
    
    i2c1_send_data(data);
    i2c1_acks();
    
    i2c1_stop();
}

void i2c1_send_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data(regAddr);
    i2c1_acks();
    
    while (length) {
        i2c1_send_data(*buffer++);

        if (length == 1) {
            i2c1_nackm();
        } else {
            i2c1_ackm();
        }

        length--;
    }
    
    i2c1_stop();
}

void i2c1_send_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c1_start();
    
    i2c1_send_data(devAddr);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 8) & 0xFF);
    i2c1_acks();
    
    i2c1_send_data((regAddr >> 0) & 0xFF);
    i2c1_acks();
    
    while (length--) {
        i2c1_send_data(*buffer++);
        i2c1_acks();
    }
    
    i2c1_stop();
}

void i2c1_enable(uint8_t enable)
{
    if (enable == TRUE) {
        // Output Port
        nrf_gpio_cfg_output(GPIO_I2C1_SCL);
        nrf_gpio_cfg_output(GPIO_I2C1_SDA);

        // Port Initilize
        nrf_gpio_pin_set(GPIO_I2C1_SCL);
        nrf_gpio_pin_set(GPIO_I2C1_SDA);
    } else {
        // Input Port
        nrf_gpio_cfg_input(GPIO_I2C1_SCL, NRF_GPIO_PIN_NOPULL);
        nrf_gpio_cfg_input(GPIO_I2C1_SDA, NRF_GPIO_PIN_NOPULL);
    }
}

void i2c2_start(void)
{
    nrf_gpio_pin_set(GPIO_I2C2_SDA);
    nrf_gpio_pin_set(GPIO_I2C2_SCL);
    i2c_delay();
    nrf_gpio_pin_clear(GPIO_I2C2_SDA);
    i2c_delay();
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
}

void i2c2_restart(void)
{
    nrf_gpio_cfg_output(GPIO_I2C2_SDA);
    nrf_gpio_pin_clear(GPIO_I2C2_SDA);
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    
    nrf_gpio_pin_set(GPIO_I2C2_SDA);
    nrf_gpio_pin_set(GPIO_I2C2_SCL);

    i2c2_start();
}

void i2c2_stop(void)
{
    nrf_gpio_pin_clear(GPIO_I2C2_SDA);
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    i2c_delay();
    nrf_gpio_pin_set(GPIO_I2C2_SCL);
    i2c_delay();
    nrf_gpio_pin_set(GPIO_I2C2_SDA);
    i2c_delay();
}

void i2c2_acks(void)
{
    uint8_t wait = 10;
    
    nrf_gpio_cfg_input(GPIO_I2C2_SDA, NRF_GPIO_PIN_PULLUP);
    i2c_delay();
    
    nrf_gpio_pin_set(GPIO_I2C2_SCL);

    while (wait--) {
        if (nrf_gpio_pin_read(GPIO_I2C2_SDA) == LOW) {
            break;
        }

        i2c_delay();
    }


    if (wait == 0xFF) {
        PRINTF("==== I2C2 ack ====\n");
    }

    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    nrf_gpio_cfg_output(GPIO_I2C2_SDA);
}

void i2c2_ackm(void)
{
    nrf_gpio_cfg_output(GPIO_I2C2_SCL);
    nrf_gpio_cfg_output(GPIO_I2C2_SDA);
    
    nrf_gpio_pin_clear(GPIO_I2C2_SDA);
    i2c_delay();

    nrf_gpio_pin_set(GPIO_I2C2_SCL);
    i2c_delay();
    
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    i2c_delay();
}

void i2c2_nacks(void)
{
    uint8_t wait = 10;
    
    nrf_gpio_cfg_input(GPIO_I2C2_SDA, NRF_GPIO_PIN_PULLUP);
    i2c_delay();
    
    nrf_gpio_pin_set(GPIO_I2C2_SCL);

    while (wait--) {
        if (nrf_gpio_pin_read(GPIO_I2C2_SDA) == HIGH) {
            break;
        }

        i2c_delay();
    }

    if (wait == 0xFF) {
    	PRINTF("==== I2C2 NAK ====\n");
    }

    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    nrf_gpio_cfg_output(GPIO_I2C2_SDA);
}

void i2c2_nackm(void)
{
    nrf_gpio_cfg_output(GPIO_I2C2_SCL);
    nrf_gpio_cfg_output(GPIO_I2C2_SDA);
    
    nrf_gpio_pin_set(GPIO_I2C2_SDA);
    i2c_delay();

    nrf_gpio_pin_set(GPIO_I2C2_SCL);
    i2c_delay();
    
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
    i2c_delay();
}

void i2c2_send_data(uint8_t data)
{
    uint8_t index = 8;
        
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);
        
    while (index--) {
        if ((data & 0x80) == 0) {
            nrf_gpio_pin_clear(GPIO_I2C2_SDA);
        } else {
            nrf_gpio_pin_set(GPIO_I2C2_SDA);
        }
        
        i2c_delay();
        nrf_gpio_pin_set(GPIO_I2C2_SCL);

        i2c_delay();
        nrf_gpio_pin_clear(GPIO_I2C2_SCL);
        
        data <<= 1;
    }
}

uint8_t i2c2_read_data(void)
{
    uint8_t index = 8;
    uint8_t data = 0;

    nrf_gpio_cfg_input(GPIO_I2C2_SDA, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_pin_clear(GPIO_I2C2_SCL);	  	
    
    while (index--) {
        data <<= 1;
        
        nrf_gpio_pin_set(GPIO_I2C2_SCL);
        i2c_delay();
        
        if (nrf_gpio_pin_read(GPIO_I2C2_SDA) == HIGH) {
            data |= HIGH;
        }
        
        nrf_gpio_pin_clear(GPIO_I2C2_SCL);
        i2c_delay();
    }

    nrf_gpio_cfg_output(GPIO_I2C2_SDA);

    return data;
}

uint8_t i2c2_read_byte(uint8_t devAddr, uint8_t regAddr)
{
    uint8_t data;

    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data(regAddr);
    i2c2_acks();
    
    i2c2_restart();

    i2c2_send_data(devAddr + I2C_READ);
    i2c2_acks();
    
    data = i2c2_read_data();
    i2c2_nackm();
    
    i2c2_stop();

    return data;
}


uint8_t i2c2_read_byte2(uint8_t devAddr, uint16_t regAddr)
{
    uint8_t data;

    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 8) & 0xFF);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 0) & 0xFF);
    i2c2_acks();
    
    i2c2_start();
    
    i2c2_send_data(devAddr + I2C_READ);
    i2c2_acks();
    
    data = i2c2_read_data();
    i2c2_acks();
    
    i2c2_stop();

    return data;
}

void i2c2_read_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data(regAddr);
    i2c2_acks();
    
    i2c2_stop();
    
    i2c2_start();
    
    i2c2_send_data(devAddr + I2C_READ);
    i2c2_acks();

    while (length) {
        *buffer++ = i2c2_read_data();
        
        if (length == 1) {
            i2c2_nackm();
        } else {
            i2c2_ackm();
        }

        length--;
    }
    
    i2c2_stop();
}

void i2c2_read_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 8) & 0xFF);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 0) & 0xFF);
    i2c2_acks();
    
    i2c2_start();
    
    i2c2_send_data(devAddr + I2C_READ);
    i2c2_acks();

    while (length) {
        *buffer++ = i2c2_read_data();
            
        if (length == 1) {
            i2c2_nackm();
        } else {
            i2c2_ackm();
        }

        length--;
    }
    
    i2c2_stop();
}

void i2c2_send_byte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data(regAddr);
    i2c2_acks();
    
    i2c2_send_data(data);
    i2c2_acks();
    
    i2c2_stop();
}

void i2c2_send_byte2(uint8_t devAddr, uint16_t regAddr, uint8_t data)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 8) & 0xFF);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 0) & 0xFF);
    i2c2_acks();
    
    i2c2_send_data(data);
    i2c2_acks();
    
    i2c2_stop();
}

void i2c2_send_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data(regAddr);
    i2c2_acks();
    
    while (length) {
        i2c2_send_data(*buffer++);

        if (length == 1) {
            i2c2_nackm();
        } else {
            i2c2_ackm();
        }

        length--;
    }
    
    i2c2_stop();
}

void i2c2_send_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer)
{
    i2c2_start();
    
    i2c2_send_data(devAddr);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 8) & 0xFF);
    i2c2_acks();
    
    i2c2_send_data((regAddr >> 0) & 0xFF);
    i2c2_acks();
    
    while (length--) {
        i2c2_send_data(*buffer++);
        i2c2_acks();
    }
    
    i2c2_stop();
}

void i2c2_enable(uint8_t enable)
{
    if (enable == TRUE) {
        // Output Port
        nrf_gpio_cfg_output(GPIO_I2C2_SCL);
        nrf_gpio_cfg_output(GPIO_I2C2_SDA);

        // Port Initilize
        nrf_gpio_pin_set(GPIO_I2C2_SCL);
        nrf_gpio_pin_set(GPIO_I2C2_SDA);
    } else {
        // Input Port
        nrf_gpio_cfg_input(GPIO_I2C2_SCL, NRF_GPIO_PIN_NOPULL);
        nrf_gpio_cfg_input(GPIO_I2C2_SDA, NRF_GPIO_PIN_NOPULL);
    }
}


void i2c_init(uint32_t channel)
{
    if (channel == I2C1_CHANNEL) {
        i2c1_enable(TRUE);
    } else if (channel == I2C2_CHANNEL) {
        i2c2_enable(TRUE);
    }
}
