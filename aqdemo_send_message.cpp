#include "mbed.h"

#include "http_request.h"
#include "mbed_mem_trace.h"
#include "NetworkInterface.h"

#include "sensors/sps30.h"


int8_t aqdemo_send_message(NetworkInterface* network, int32_t& temperature, int32_t& humidity,
                    uint16_t& tvoc_ppb, uint16_t& co2_eq_ppm,
                    struct sps30_measurement& m)
{
    const char* url = "http://demo.thingsboard.io/api/v1/" MBED_CONF_APP_THINGSBOARD_TOKEN "/telemetry/";
    
    char body[255];        // Enough to hold the message body
     snprintf(body, 
              255, 
              "{temp:%f,rh:%f,tvoc:%d,co2eq:%d,pm1_0:%f,pm2_5:%f,pm4_0:%f,pm10_0:%f}",
              temperature/1000.0f, humidity/1000.0f, tvoc_ppb, co2_eq_ppm, 
              m.mc_1p0,  m.mc_2p5,  m.mc_4p0,  m.mc_10p0);
    
    
    HttpRequest* request = new HttpRequest(network, HTTP_POST, url);
    request->set_header("Content-Type", "application/json");
    HttpResponse* response = request->send(body, strlen(body));
    // if response is NULL, check response->get_error()
    if (response == NULL) {
        printf("NSAPI error: %d\n", request->get_error());
    } else {
        printf("status is %d - %s\n", response->get_status_code(), response->get_status_message().c_str());
        printf("body is:\n%s\n", response->get_body_as_string().c_str());
    }
    
    delete request; // also clears out the response

    return response != NULL;
}