#include "mbed.h"
#include "platform/mbed_thread.h"
#include "NetworkInterface.h"

#include "sensors/sps30.h"
#include "aqdemo_sensors.h"
#include "aqdemo_send_message.h"

// Blinking rate in milliseconds
#define SENSOR_UPDATE_RATE                                                    1000


static WiFiInterface *wifi = 0;

int aqdemo_connect_to_wifi()
{
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -2;
    }

    printf("Success\n\n");

    SocketAddress a;
    wifi->get_ip_address(&a);
    printf("IP: %s\n", a.get_ip_address());

    return 0;
}

void aqdemo_disconnect_wifi()
{
    wifi->disconnect();
}

void aqdemo_demo_broken_loop_forever()
{
    DigitalOut essLedRed(D9);
    DigitalOut essLedOrange(D10);
    DigitalOut essLedGreen(D11);

    while (true) {
        essLedGreen  = !essLedGreen;
        essLedRed    = !essLedRed;
        essLedOrange = !essLedOrange;
        thread_sleep_for(1000);
    }   
}

int main()
{
    DigitalOut led(LED1);

    // Configuration for Sensirion Environmental Sensor Shield
    DigitalOut essLedRed(D9);
    DigitalOut essLedOrange(D10);
    DigitalOut essLedGreen(D11);

    essLedGreen  = 0;
    essLedRed    = 0;

    essLedOrange = 1;
    if (aqdemo_sensors_probe() != 0) {
        printf("Sensor initialization failed. Aborting\n");
        aqdemo_demo_broken_loop_forever();
    }

    if (aqdemo_connect_to_wifi() != 0) {
        printf("Wifi setup failed. Aborting\n");
        aqdemo_demo_broken_loop_forever();
    }
    essLedOrange = 0;

    int rateLimitLoopCount = 0;
    while (true) {
        led = !led;
        int32_t temperature, humidity;
        uint16_t tvoc_ppb, co2_eq_ppm;
        struct sps30_measurement m;

        if (aqdemo_sensors_update(temperature, humidity, tvoc_ppb, co2_eq_ppm, m) == 0) {
            printf("T:     %.2f\nRH:    %.2f\n", temperature  / 1000.0f, humidity / 1000.0f);
            printf("TVOC:  %d\nCO2eq: %d\n", tvoc_ppb, co2_eq_ppm);
            printf("PM2.5: %.2f\n", m.mc_2p5);
            printf("-----\n");

            // we can only submit one sample every 5 second
            if (rateLimitLoopCount % 6 == 0) {
                aqdemo_send_message(wifi, temperature, humidity, tvoc_ppb, co2_eq_ppm, m);
            }
        } else {
            printf("Failed to update sensors\n");
        }

        ++rateLimitLoopCount;
        thread_sleep_for(SENSOR_UPDATE_RATE);
    }
}
