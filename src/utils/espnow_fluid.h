#ifndef ESPNOW_FLUID_H
#define ESPNOW_FLUID_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** Latest methanol % (0–100) **/
    extern volatile uint8_t espnow_fluid_pct;
    /** Flag: new methanol reading available **/
    extern volatile bool espnow_fluid_newData;

    /** Call once at startup to hook into ESP-NOW **/
    void espnow_fluid_init(void);

    /** Internal raw callback (dispatcher will call this) **/
    void onFluidRecvRaw(const uint8_t *mac, const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_FLUID_H
