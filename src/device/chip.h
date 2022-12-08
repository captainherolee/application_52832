#ifndef __CHIP_H__
#define __CHIP_H__

#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

uint8_t* config_get_chip_id(uint8_t* res);

#endif /* __CHIP_H__ */