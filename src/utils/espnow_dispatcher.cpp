// Pre-time_sync version

// src/utils/espnow_dispatcher.cpp

#include <WiFi.h>
#include <esp_now.h>
#include "espnow_dispatcher.h"
#include "utils/espnow_fluid.h"
#include "utils/espnow_flow.h"
#include "utils/espnow_boost.h"
#include "utils/espnow_boostsafe.h"
#include "utils/espnow_time.h"
#include "ui/dashboard_layout.h"

// --- YOUR TWO SENDER MAC ADDRESSES ---
static const uint8_t MAC_FLUID[6] = {0x14, 0x33, 0x5C, 0x0D, 0xD1, 0xE4};
static const uint8_t MAC_FSB[6] = {0x14, 0x33, 0x5C, 0x0F, 0x2D, 0x48};

// Matches your sender’s struct<float level; uint8_t hour; uint8_t minute;>
typedef struct __attribute__((packed))
{
    float level;
    uint8_t hour;
    uint8_t minute;
} FluidTimePacket;

// Must match the sender’s struct layout:
typedef struct
{
    float boostPsi;
    float sprayPsi;
    bool boostSafe;
} FSBPacket;

static void onRecv(const uint8_t *mac, const uint8_t *data, int len)
{
    Serial.printf("Packet from %02X:%02X:%02X:%02X:%02X:%02X  len=%d\n",
    mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], len);
    /* fluid-time sender ------------------------------------------------ */
    if (memcmp(mac, MAC_FLUID, 6) == 0 && len == sizeof(FluidTimePacket)) {

        onFluidRecvRaw(mac, data, sizeof(float));

        FluidTimePacket pkt;
        memcpy(&pkt, data, sizeof(pkt));
        espnow_hour        = pkt.hour;
        espnow_minute      = pkt.minute;
        espnow_time_newData = true;
    }
    
    /* boost / flow / safe sender -------------------------------------- */
    else if (memcmp(mac, MAC_FSB, 6) == 0 && len == sizeof(FSBPacket)) {

        FSBPacket pkt;
        memcpy(&pkt, data, sizeof(pkt));

        espnow_boost_value    = int(roundf(pkt.boostPsi));
        espnow_boost_newData  = true;

        espnow_flow_value     = int(roundf(pkt.sprayPsi));
        espnow_flow_newData   = true;

        espnow_boostSafe      = pkt.boostSafe;
        espnow_boostSafe_newData = true;
    }
    /* else: packet from an unknown sender – ignore */
}

void espnow_dispatcher_init(void)
{
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("❌ ESP-NOW init failed");
        while (true)
            delay(1000);
    }
    esp_now_register_recv_cb(onRecv);
    Serial.println("✅ ESP-NOW dispatcher ready");
}
