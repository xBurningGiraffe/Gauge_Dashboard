#ifndef FLUID_SEG_GAUGE_H
#define FLUID_SEG_GAUGE_H

#include <lvgl.h>
#include <fonts/fonts.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void create_fluid_seg_gauge(lv_obj_t *parent);
    void fluid_seg_set_value(uint8_t pct);

#ifdef __cplusplus
}
#endif
#endif
