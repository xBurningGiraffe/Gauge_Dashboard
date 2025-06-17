#ifndef ESPNOW_BOOST_H
#define ESPNOW_BOOST_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** Latest boost psi (rounded to int) **/
    extern volatile int espnow_boost_value;
    /** Flag: new boost reading available **/
    extern volatile bool espnow_boost_newData;

    /** Init ESP-NOW and hook boost callback **/
    void espnow_boost_init(void);

    /** Dispatcher will call this **/
    void onBoostRecvRaw(const uint8_t *mac, const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_BOOST_H
