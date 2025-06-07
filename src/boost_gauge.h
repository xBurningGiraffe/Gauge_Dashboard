#ifndef BOOST_GAUGE_H
#define BOOST_GAUGE_H

#include <lvgl.h>
#include "eurostile_fonts.h"   /* brings in the DIN fonts */

#ifdef __cplusplus
extern "C" {
#endif

    void create_boost_gauge(lv_obj_t* parent);
    /* psi ? [-10 … 40]; values outside are clipped                */
    void boost_set_value(int16_t psi);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
