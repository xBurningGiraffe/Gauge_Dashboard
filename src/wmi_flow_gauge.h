#ifndef WMI_FLOW_GAUGE_H
#define WMI_FLOW_GAUGE_H

#include <lvgl.h>
#include "eurostile_fonts.h"

/*============================================================================*/
/* Public API for the WMI Flow Gauge                                           */
/*============================================================================*/

/**
 * Create a 0–25 psi semi-circular WMI flow gauge.
 *
 * @param parent  Parent LVGL object (e.g., the screen or a containing panel).
 */
void create_wmi_flow_gauge(lv_obj_t* parent);

/**
 * Update the WMI flow gauge needle to the specified pressure.
 *
 * @param psi  Flow pressure in psi (0 … 25). Values outside this range will be clamped.
 */
void wmi_flow_set_value(int16_t psi);

#endif /* WMI_FLOW_GAUGE_H */
