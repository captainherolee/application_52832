#ifndef __LIGHT_SENSOR_H__
#define __LIGHT_SENSOR_H__

typedef struct {
    int32_t ch0;
    int32_t ch1;
    double lux;
} light_sensor_data_t;

light_sensor_data_t* periph_light_sensor_read(void);
double periph_light_sensor_calc_lux(int32_t ch0, int32_t ch1);
void periph_light_sensor_init(void);

#endif /* __LIGHT_SENSOR_H__ */