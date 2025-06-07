#ifndef BOOST_SAFE_H
#define BOOST_SAFE_H

#include <lvgl.h>
#include "eurostile_fonts.h"

/**
 * Call this once (after LVGL is initialized) to start the simulated
 * Boost-Safe monitor. Whenever it “fails,” a pop-up message box appears.
 */
void create_boost_safe_indicator(lv_obj_t* parent);

#endif /* BOOST_SAFE_H */
