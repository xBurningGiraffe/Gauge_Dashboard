// dashboard_layout.c

#include "wasp_small_red.h"
#include <time.h>
#include "eurostile_fonts.h"
#include "porsche_fonts.h"

static lv_obj_t *time_lbl;
static lv_obj_t *label_stinger;

static void update_time_cb(lv_timer_t *t)
{
    LV_UNUSED(t);
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    if (!lt)
        return;
    char buf[16];
    strftime(buf, sizeof(buf), "%I:%M %p", lt);
    lv_label_set_text(time_lbl, buf);
}

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
    const lv_coord_t screen_w = 800;
    const lv_coord_t screen_h = 480;

    //
    // 1) Fluid gauge wrapper: 320×150, centered at (400, 75)
    //
    const lv_coord_t fluid_w = 420;
    const lv_coord_t fluid_h = 150;

    /*  Initialize gradient style background
    static lv_style_t scr_grad;
    lv_style_init(&scr_grad);

    // Gradient settings
    lv_style_set_bg_color(&scr_grad, lv_color_hex(0x333333));
    lv_style_set_bg_grad_color(&scr_grad, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&scr_grad, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&scr_grad, LV_OPA_COVER);

    lv_obj_add_style(lv_scr_act(), &scr_grad, 0);
    */

    lv_obj_t *bg_logo = lv_img_create(parent_screen);
    lv_img_set_src(bg_logo, &wasp_small_red);
    lv_obj_center(bg_logo);
    lv_obj_set_style_img_opa(bg_logo, LV_OPA_20, 0);
    // lv_obj_move_background(bg_logo);

    // 1) Create the time label in top?left corner
    time_lbl = lv_label_create(parent_screen);
    lv_obj_set_style_text_font(time_lbl, &eurostile_extended_black_18, 0);
    lv_obj_set_style_text_color(time_lbl, lv_color_hex(0xCCCCCC), 0);
    lv_label_set_text(time_lbl, "00:00"); // initial placeholder
    lv_obj_align(time_lbl, LV_ALIGN_TOP_LEFT, 8, 8);
    lv_timer_create(update_time_cb, 60000, NULL);
    // Do one immediate update so it doesn't stay at 00:00:00 until 1 s elapses
    update_time_cb(NULL);

    lv_obj_t *wrap_fluid = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_fluid);
    lv_obj_set_size(wrap_fluid, fluid_w, fluid_h);
    /* Center X =  (screen_w - fluid_w)/2 = (800 - 320)/2 = 240
       Center Y = 75  (half of 150) */
    lv_obj_set_pos(wrap_fluid,
                   (screen_w - fluid_w) / 2,
                   0); /* top?aligned; we’ll push it down by a few px if desired below */

    /* If you want a small vertical gap from the screen top, use lv_obj_align:
       lv_obj_align(wrap_fluid, LV_ALIGN_TOP_MID, 0, 8);
       (That would place its top at y=8 and center it horizontally.)
    */
    lv_obj_align(wrap_fluid, LV_ALIGN_TOP_MID, 0, 8);

    create_fluid_seg_gauge(wrap_fluid);

    //
    // 2) Three 260×260 gauges along the bottom
    //    – Boost   at bottom?left
    //    – WMI Flow at bottom?right
    //
    const lv_coord_t square_w = 260;
    const lv_coord_t square_h = 260;
    const lv_coord_t bottom_y = screen_h - square_h - 10;
    // 10px bottom margin; so y = 480 - 260 - 10 = 210

    // ??? Boost gauge wrapper (bottom?left corner of screen) ???
    lv_obj_t *wrap_boost = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_boost);
    lv_obj_set_size(wrap_boost, 300, 300);
    // Align to bottom?left with a 10px margin:
    lv_obj_align(wrap_boost, LV_ALIGN_BOTTOM_LEFT, 10, -2);
    create_boost_gauge(wrap_boost);

    // ??? WMI Flow gauge wrapper (bottom?center of screen) ???
    lv_obj_t *wrap_wmi = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(wrap_wmi);
    lv_obj_set_size(wrap_wmi, 300, 300);
    // Align to bottom?center with a 0px x?offset, 10px up from bottom:
    lv_obj_align(wrap_wmi, LV_ALIGN_BOTTOM_RIGHT, -10, -2);
    create_wmi_flow_gauge(wrap_wmi);
}
