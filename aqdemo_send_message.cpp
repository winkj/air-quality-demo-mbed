/*
 *  Copyright (c) 2020, Sensirion AG
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Sensirion AG nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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