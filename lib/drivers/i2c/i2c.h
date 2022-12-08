#ifndef __I2C_H__
#define __I2C_H__

#define	I2C_READ	1

static __inline void i2c_delay(void)
{
    __asm volatile(
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "nop                    \n"
        "                       \n"
        "bx lr                  \n"
    );
}

uint8_t i2c1_read_byte(uint8_t devAddr, uint8_t regAddr);
uint8_t i2c1_read_byte2(uint8_t devAddr, uint16_t regAddr);
void 	i2c1_read_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c1_read_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c1_send_byte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void 	i2c1_send_byte2(uint8_t devAddr, uint16_t regAddr, uint8_t data);
void 	i2c1_send_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c1_send_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c1_enable(uint8_t enable);

uint8_t i2c2_read_byte(uint8_t devAddr, uint8_t regAddr);
uint8_t i2c2_read_byte2(uint8_t devAddr, uint16_t regAddr);
void 	i2c2_read_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c2_read_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c2_send_byte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void 	i2c2_send_byte2(uint8_t devAddr, uint16_t regAddr, uint8_t data);
void 	i2c2_send_buff(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c2_send_buff2(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *buffer);
void 	i2c2_enable(uint8_t enable);

void i2c_init(uint32_t channel);

#endif /* __I2C_H__ */
