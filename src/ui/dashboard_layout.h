// DASHBOARD_LAYOUT.h

#ifndef DASHBOARD_LAYOUT_H
#define DASHBOARD_LAYOUT_H

#include <lvgl.h>
#include <fonts/fonts.h>
#include <images/images.h>
#include <gauges/gauges.h>

/**
 * @brief Create and display a full-screen boot splash with a dark gradient
 *        and a fading-in image. The splash will automatically remove itself
 *        after the timeout (handled internally).
 */
void create_dashboard_layout(lv_obj_t *parent_screen);

void update_time_label(uint8_t hour, uint8_t minute);

#endif /* DASHBOARD_LAYOUT_H */
