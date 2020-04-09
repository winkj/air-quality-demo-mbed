#ifndef __AQDEMO_SEND_MESSAGE_H
#define __AQDEMO_SEND_MESSAGE_H

#include <cstdint>

class NetworkInterface;

int8_t aqdemo_send_message(NetworkInterface* network,
                    int32_t& temperature, int32_t& humidity,
                    uint16_t& tvoc_ppb, uint16_t& co2_eq_ppm,
                    struct sps30_measurement& m);

#endif /* __AQDEMO_SEND_MESSAGE_H */