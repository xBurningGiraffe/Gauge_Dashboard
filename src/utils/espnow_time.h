#ifndef ESPNOW_TIME_H
#define ESPNOW_TIME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // these get written by the dispatcher:
    extern uint8_t espnow_hour;
    extern uint8_t espnow_minute;
    extern bool espnow_time_newData;

#ifdef __cplusplus
}
#endif

#endif