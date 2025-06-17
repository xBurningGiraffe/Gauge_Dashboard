#include "espnow_fluid.h"
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

volatile uint8_t espnow_fluid_pct = 0;
volatile bool espnow_fluid_newData = false;

static const uint8_t SENDER_MAC[6] = {/* your sender MAC here if needed */};

extern "C" void onFluidRecvRaw(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    if (len != sizeof(float))
        return;
    float level;
    memcpy(&level, data, sizeof(level));
    int pct = (int)roundf(level);
    espnow_fluid_pct = (uint8_t)constrain(pct, 0, 100);
    espnow_fluid_newData = true;
}

void espnow_fluid_init(void)
{
    WiFi.mode(WIFI_STA);
    esp_now_init();
    // (optional) add peer if you want to limit source MAC
    esp_now_register_recv_cb(onFluidRecvRaw);
}
