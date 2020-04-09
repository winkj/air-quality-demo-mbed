#include "aqdemo_sensors.h"

#include "sensors/shtc1.h"
#include "sensors/sgp30.h"
#include "sensors/sps30.h"

int8_t probe_sgp()
{
    const char *driver_version = sgp30_get_driver_version();
    if (driver_version) {
        printf("SGP30 driver version %s\n", driver_version);
    } else {
        printf("fatal: Getting driver version failed\n");
        return -1;
    }

    if (sgp30_probe() != STATUS_OK) {
        printf("SGP sensor probing failed\n");
        return -2;
    }

    printf("SGP sensor probing successful\n");

    int16_t err = sgp30_iaq_init();
    if (err == STATUS_OK) {
        printf("sgp30_iaq_init done\n");
    } else {
        printf("sgp30_iaq_init failed!\n");
        return -2;
    }

    return 0;
}

int16_t probe_sps()
{
    uint8_t auto_clean_days = 4;
    uint32_t auto_clean;
    int16_t err;

    while (sps30_probe() != 0) {
        printf("SPS30: SPS sensor probing failed\n");
        thread_sleep_for(500);
    }
    printf("SPS30: SPS sensor probing successful\n");

    int16_t ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
    if (ret)
        printf("SPS30: error %d setting the auto-clean interval\n", ret);

    ret = sps30_start_measurement();
    if (ret < 0) {
        printf("SPS30: error starting measurement\n");
        return 1;
    }
    printf("SPS30: measurements started\n");

    return 0;
}

int8_t aqdemo_sensors_probe()
{
    if (probe_sgp() != 0) {
        printf("Probing SGP failed\n");
        return 1;
    }

    // TODO: error check
    if (probe_sps()) {
        printf("Probing SPS failed\n");
        return 2;
    }
    

    if (shtc1_probe() != STATUS_OK) {
        printf("Probing SHT failed\n");
        return 3;
    }

    printf("Sensor probing successful\n");
    return 0;
}

int8_t aqdemo_sensors_update(int32_t& temperature, int32_t& humidity,
                             uint16_t& tvoc_ppb, uint16_t& co2_eq_ppm,
                             struct sps30_measurement& m)
{
    uint16_t data_ready;

    int8_t ret = shtc1_measure_blocking_read(&temperature, &humidity);
    if (ret != STATUS_OK) {
        printf("error reading SHT measurement\n");
        return 1;
    }


    int16_t err = sgp30_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err != STATUS_OK) {
        printf("error reading SGP measurement\n");
        return 2;
    }

    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0) {
        printf("error %d reading data-ready flag\n", ret);
        return 3;
    } else if (data_ready) {
        ret = sps30_read_measurement(&m);
        if (ret < 0) {
            printf("error reading measurement\n");
            return 4;
        }
    }

    return 0;
}