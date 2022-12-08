#include "nrf.h"

#include "common.h"
#include "chip.h"

uint8_t* config_get_chip_id(uint8_t* res)
{
    int32_t chip_id = (int32_t)(NRF_FICR->DEVICEADDR[0] >> 16);

    res[0] = TO_HEX((((int)chip_id & 0xF000) >> 12));
    res[1] = TO_HEX((((int)chip_id & 0x0F00) >> 8));
    res[2] = TO_HEX((((int)chip_id & 0x00F0) >> 4));
    res[3] = TO_HEX(((int)chip_id & 0x000F));

    PRINTF("Chip ID: [%s]\n", (char*)res);

    return res;
}
