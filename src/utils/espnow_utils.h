// espnow_utils.h
#ifndef ESPNOW_UTILS_H
#define ESPNOW_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Fluid
    extern volatile uint8_t espnow_fluid_pct;
    extern volatile bool espnow_fluid_newData;
    void espnow_fluid_init(void);

    // Flow
    extern volatile int espnow_flow_value;
    extern volatile bool espnow_flow_newData;
    void espnow_flow_init(void);

    // Boost
    extern volatile int espnow_boost_value;
    extern volatile bool espnow_boost_newData;
    void espnow_boost_init(void);

    // Boost-Safe
    extern volatile bool espnow_boostSafe;
    extern volatile bool espnow_boostSafe_newData;
    void espnow_boostsafe_init(void);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_UTILS_H
