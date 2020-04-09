#ifndef __AQDEMO_SENSORS_H
#define __AQDEMO_SENSORS_H

#include "mbed.h"

int8_t aqdemo_sensors_probe();
int8_t aqdemo_sensors_update(int32_t& temperature, int32_t& humidity,
                             uint16_t& tvoc_ppb, uint16_t& co2_eq_ppm,
                             struct sps30_measurement& m);

#endif /* __AQDEMO_SENSORS_H */