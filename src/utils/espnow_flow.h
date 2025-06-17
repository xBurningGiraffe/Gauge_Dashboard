#ifndef ESPNOW_FLOW_H
#define ESPNOW_FLOW_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** Latest spray‐flow psi (rounded to int) **/
    extern volatile int espnow_flow_value;
    /** Flag: new flow reading available **/
    extern volatile bool espnow_flow_newData;

    /** Init ESP-NOW and hook flow callback **/
    void espnow_flow_init(void);

    /** Dispatcher should call this on every frame **/
    void onFlowRecvRaw(const uint8_t *mac, const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_FLOW_H
