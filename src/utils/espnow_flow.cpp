#include "espnow_flow.h"
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

volatile int espnow_flow_value = 0;
volatile bool espnow_flow_newData = false;

extern "C" void onFlowRecvRaw(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    if (len != sizeof(float))
        return;
    float psi;
    memcpy(&psi, data, sizeof(psi));
    espnow_flow_value = (int)roundf(psi);
    espnow_flow_newData = true;
}

void espnow_flow_init(void)
{
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onFlowRecvRaw);
}
