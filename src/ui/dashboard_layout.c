// dashboard_layout.c

#include <time.h>
#include <ui/dashboard_layout.h>

#define SCREEN_W 800
#define SCREEN_H 480

// static lv_obj_t *time_lbl;

/* Forward declarations of your existing gauge?creation functions */
void create_fluid_seg_gauge(lv_obj_t *parent);
void create_boost_gauge(lv_obj_t *parent);
void create_wmi_flow_gauge(lv_obj_t *parent);
void create_boost_safe_indicator(lv_obj_t *parent);

/**
 * @brief Arrange four gauges on an 800×480 screen:
 *   • Fluid (320×150) at top?center
 *   • Boost  (260×260) at bottom?left
 *   • WMI Flow (260×260) at bottom?center
 *   • Boost?Safe (260×260) at bottom?right (pop?up appears inside this container)
 */
void create_dashboard_layout(lv_obj_t *parent_screen)
{

    //
    // 1) Fluid gauge wrapper: 320×150, centered at (400, 75)
    //
    const lv_coord_t fluid_w = 440;
    const lv_coord_t fluid_h = 120;

    // Dithered gradient background
    lv_obj_set_style_bg_img_src(parent_screen, &dithered_gradient, 0);

    // Red wasp background logo
    lv_obj_t *bg_logo = lv_img_create(parent_screen);
    lv_img_set_src(bg_logo, &wasp_small_red);
    lv_obj_center(bg_logo);
    lv_obj_set_style_img_opa(bg_logo, LV_OPA_20, 0);
    // lv_obj_move_background(bg_logo);
    
    // Time label in top left
    /*
    time_lbl = lv_label_create(parent_screen);
    lv_obj_set_style_text_font(time_lbl, &eurostile_extended_bold_22, 0);
    lv_obj_set_style_text_color(time_lbl, lv_color_hex(0xCCCCCC), 0);
    lv_label_set_text(time_lbl, "00:00");  // initial placeholder
    lv_obj_align(time_lbl, LV_ALIGN_TOP_LEFT, 8, 8);
    */

    // Fluid gauge wrapper
    lv_obj_t *wrap_fluid = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_fluid);
    lv_obj_set_size(wrap_fluid, fluid_w, fluid_h);
    lv_obj_set_pos(wrap_fluid,
                   (SCREEN_W - fluid_w) / 2,
                   0); /* top?aligned; we’ll push it down by a few px if desired below */
    lv_obj_align(wrap_fluid, LV_ALIGN_TOP_MID, 0, 8);
    create_fluid_seg_gauge(wrap_fluid);

    // Boost gauge wrapper
    lv_obj_t *wrap_boost = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_boost);
    lv_obj_set_size(wrap_boost, 350, 350);
    // Align to bottom?left with a 10px margin:
    lv_obj_align(wrap_boost, LV_ALIGN_BOTTOM_LEFT, 8, 0);
    create_boost_gauge(wrap_boost);

    // WMI flow gauge wrapper
    lv_obj_t *wrap_wmi = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_wmi);
    lv_obj_set_size(wrap_wmi, 350, 350);
    // Align to bottom?center with a 0px x?offset, 10px up from bottom:
    lv_obj_align(wrap_wmi, LV_ALIGN_BOTTOM_RIGHT, -8, 0);
    create_wmi_flow_gauge(wrap_wmi);
}

/*
void update_time_label(uint8_t hour, uint8_t minute)
{
    char buf[6];
    snprintf(buf, sizeof(buf), "%02u:%02u", hour, minute);
    lv_label_set_text(time_lbl, buf);
}
*/