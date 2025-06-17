#include "espnow_boostsafe.h"
#include <WiFi.h>
#include <esp_now.h>

volatile bool espnow_boostSafe = true;
volatile bool espnow_boostSafe_newData = false;

extern "C" void onBoostSafeRecvRaw(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    if (len < 1)
        return;
    // assume safe flag is first byte
    espnow_boostSafe = (data[0] != 0);
    espnow_boostSafe_newData = true;
}

void espnow_boostsafe_init(void)
{
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onBoostSafeRecvRaw);
}
