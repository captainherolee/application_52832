#ifndef __ACCEL_MC3416_H__
#define	__ACCEL_MC3416_H__

#define XOUT_EX_L          0x0D
#define XOUT_EX_H          0x0E
#define YOUT_EX_L          0x0F
#define YOUT_EX_H          0x10
#define ZOUT_EX_L          0x11
#define ZOUT_EX_H          0x12
#define MC3416_CHIP_ID_REG 0x18
#define MC3416_VENDOR_ID   0xA0

typedef struct {
    uint8_t addr;
    uint8_t data;
} acc_reg_st;

typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
} acc_data_st;

void periph_accel_interrupt_init(void);
void periph_accel_mc3416_read(acc_data_st* acc);
void periph_accel_init(void);

#endif /* __ACCEL_MC3416_H__ */

