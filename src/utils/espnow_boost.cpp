#include "espnow_boost.h"
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

volatile int espnow_boost_value = 0;
volatile bool espnow_boost_newData = false;

extern "C" void onBoostRecvRaw(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    if (len != sizeof(float))
        return;
    float psi;
    memcpy(&psi, data, sizeof(psi));
    espnow_boost_value = (int)roundf(psi);
    espnow_boost_newData = true;
}

void espnow_boost_init(void)
{
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onBoostRecvRaw);
}
