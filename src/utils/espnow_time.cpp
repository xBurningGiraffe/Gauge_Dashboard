// src/utils/espnow_time.cpp
#include "utils/espnow_time.h"

uint8_t espnow_hour = 0;
uint8_t espnow_minute = 0;
bool espnow_time_newData = false;

void espnow_time_init()
{
    espnow_hour = 0;
    espnow_minute = 0;
    espnow_time_newData = false;
}
