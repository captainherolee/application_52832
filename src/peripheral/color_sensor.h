#ifndef __COLOR_SENSOR_H__
#define __COLOR_SENSOR_H__

typedef struct {
    int16_t cct;
    int16_t lux;
} color_sensor_data_t;

color_sensor_data_t* periph_color_sensor_cct_read(void);
void periph_color_sensor_init(void);
#endif /* __COLOR_SENSOR_H__ */
