#ifndef ESPNOW_BOOSTSAFE_H
#define ESPNOW_BOOSTSAFE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** Latest safe flag (true=OK, false=FAIL) **/
    extern volatile bool espnow_boostSafe;
    /** Flag: new safe‐flag available **/
    extern volatile bool espnow_boostSafe_newData;

    /** Init ESP-NOW and hook safe callback **/
    void espnow_boostsafe_init(void);

    /** Dispatcher will call this **/
    void onBoostSafeRecvRaw(const uint8_t *mac, const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_BOOSTSAFE_H
