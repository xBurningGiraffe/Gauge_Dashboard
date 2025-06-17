/* wmi_flow_gauge.c
 *
 * 0–25 psi WMI Flow Gauge using an arc indicator (no needle) on a half?arc scale.
 * Matches the bar?style fluid gauge and circular boost gauge for a cohesive dashboard.
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <lvgl.h>       /* Unified LVGL header */
#include "wmi_flow_gauge.h"

 /*???????????????????????????????????????????????????????????????????????????*/
 /* Geometry constants                                                      */
 /*???????????????????????????????????????????????????????????????????????????*/
 /* Outer bezel diameter (same as boost gauge)                              */
#define DIAL_D       320
/* Inner scale diameter: leave room for bezel & ticks                       */
#define SCALE_D      (DIAL_D - 80)
/* Total minor ticks (0…25 inclusive ? 26 total)                            */
#define MINOR_TICKS  26
/* Major tick at every 5 psi (0, 5, 10, 15, 20, 25)                          */
#define MAJOR_EVERY  5
/* Radial padding to pull labels inward                                      */
#define PAD_RADIAL -70
/* Vertical offset for “psi” label                                           */
#define PSI_Y_OFF    26
/* Bezel border thickness                                                    */
#define BORDER_W     2
// Overall scale range
#define SCALE_RANGE 250
// Overall scale rotation
#define SCALE_ROTATION 145
// Arc width
#define ARC_WIDTH 40

/*???????????????????????????????????????????????????????????????????????????*/
/* Static handle for the arc indicator                                       */
/*???????????????????????????????????????????????????????????????????????????*/
static lv_obj_t* flow_arc;

/*???????????????????????????????????????????????????????????????????????????*/
/* Draggable helper: makes the entire gauge container movable                 */
/*???????????????????????????????????????????????????????????????????????????*/
static void drag_cb(lv_event_t* e) {
    lv_obj_t* cont = lv_event_get_target_obj(e);
    lv_indev_t* id = lv_indev_active();
    if (!id) return;
    lv_point_t v;
    lv_indev_get_vect(id, &v);
    lv_obj_set_pos(cont,
        lv_obj_get_x(cont) + v.x,
        lv_obj_get_y(cont) + v.y);
}

/*???????????????????????????????????????????????????????????????????????????*/
/* Public API: set the WMI flow value (0…25 psi)                              */
/*???????????????????????????????????????????????????????????????????????????*/
void wmi_flow_set_value(int16_t psi) {
    if (psi < 0)  psi = 0;
    if (psi > 25) psi = 25;
    /* Map 0?25 ? arc value 0?25 */
    lv_arc_set_value(flow_arc, psi);
}

/*???????????????????????????????????????????????????????????????????????????*/
/* Constructor: build the WMI flow gauge UI                                   */
/*   parent: any lvgl container (e.g. screen or dashboard panel)              */
/*???????????????????????????????????????????????????????????????????????????*/
void create_wmi_flow_gauge(lv_obj_t* parent) {


    /*
       1) Create a slightly larger “frame” behind the bezel that uses a vertical gradient.
          This frame is 260×260, so it extends 10px beyond the 240×240 bezel on each side.
    */
    lv_obj_t* frame = lv_obj_create(parent);
    lv_obj_remove_style_all(frame);
    lv_obj_set_size(frame, 350, 350);
    lv_obj_center(frame);

    static lv_style_t frame_grad;
    lv_style_init(&frame_grad);
    /* Top half = mid?gray, bottom half = nearly black */
    lv_style_set_bg_color(&frame_grad, lv_color_hex(0x333333));
    lv_style_set_bg_grad_color(&frame_grad, lv_color_hex(0x141414));
    lv_style_set_bg_grad_dir(&frame_grad, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&frame_grad, LV_OPA_COVER);
    lv_style_set_radius(&frame_grad, LV_RADIUS_CIRCLE);
    lv_obj_add_style(frame, &frame_grad, 0);

    /*
       2) Create the 240×240 bezel container on top of that 260×260 frame.
          Its background (inside the border) will be solid black.
          Its own 5px border sits “inside” the 260×260 gradient frame.
    */
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 320, 320);
    lv_obj_center(cont);

    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);

    /* Inside?bezel fill */
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    /* Bezel border (drawn on top of the black face, yet beneath the inner ticks) */
    lv_obj_set_style_border_width(cont, 5, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(0x3B3B3B), 0);
    lv_obj_set_style_border_opa(cont, LV_OPA_COVER, 0);

    /* Keep overflow visible so we can place ticks / arcs inside */
    lv_obj_add_flag(cont, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(cont, drag_cb, LV_EVENT_PRESSING, NULL);

    /* 2) Inner half?arc scale: ticks + numeric labels */
    lv_obj_t* scale = lv_scale_create(cont);
    lv_obj_center(scale);
    lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);
    lv_obj_set_size(scale, SCALE_D, SCALE_D);
    lv_obj_add_flag(scale, LV_OBJ_FLAG_CLICKABLE);

    /* Configure as a half?circle gauge */
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_range(scale, 0, 25);
    lv_scale_set_total_tick_count(scale, MINOR_TICKS);
    lv_scale_set_major_tick_every(scale, MAJOR_EVERY);
    lv_scale_set_angle_range(scale, SCALE_RANGE);
    lv_scale_set_rotation(scale, SCALE_ROTATION);
    lv_scale_set_label_show(scale, true);

    /* Style minor ticks: violet (#A040FF), semi-transparent (80%) */
    lv_obj_set_style_line_width(scale, 1, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 14, LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_opa(scale, LV_OPA_90, LV_PART_ITEMS);

    /* Style major ticks: lighter violet (#C080FF), fully opaque */
    lv_obj_set_style_line_width(scale, 7, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, 24, LV_PART_INDICATOR);
    lv_obj_set_style_line_color(scale, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_opa(scale, LV_OPA_COVER, LV_PART_INDICATOR);

    /* Numeric labels (0, 5, 10, 15, 20, 25): Eurostile 14, white, pulled inward */
    lv_obj_set_style_text_font(scale, &eurostile_extended_bold_24, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(scale, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_pad_radial(scale, PAD_RADIAL, LV_PART_INDICATOR);

    /* 3) Violet arc indicator behind the ticks */
    flow_arc = lv_arc_create(cont);
    /* Size the arc widget to match the scale diameter */
    lv_obj_set_size(flow_arc, DIAL_D, DIAL_D);
    // Center it
    lv_obj_center(flow_arc);
    /* Remove knob and disable user interaction */
    lv_obj_remove_style(flow_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(flow_arc, LV_OBJ_FLAG_CLICKABLE);

    /* Indicator arc: violet, semi-transparent (80%), squared ends */
    lv_obj_set_style_arc_color(flow_arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(flow_arc, LV_OPA_90, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(flow_arc, ARC_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(flow_arc, false, LV_PART_INDICATOR);

    /* Hide the background arc (main part) entirely */
    lv_obj_set_style_arc_color(flow_arc, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_arc_width(flow_arc, 0, LV_PART_MAIN);

    /* Configure sweep: 0…25 over 180° starting at 180° */
    lv_arc_set_range(flow_arc, 0, 25);
    lv_arc_set_bg_angles(flow_arc, 0, SCALE_RANGE);
    lv_arc_set_rotation(flow_arc, SCALE_ROTATION);
    lv_arc_set_value(flow_arc, 0);  /* start at 0 psi */

    /* Send the arc behind the scale so ticks sit on top */
    lv_obj_move_background(flow_arc);

    /* 4) “psi” label under 25 psi tick */
    {
        lv_obj_t* psi_lbl = lv_label_create(cont);
        lv_label_set_text(psi_lbl, "psi");
        lv_obj_set_style_text_font(psi_lbl, &eurostile_extended_bold_14, 0);
        lv_obj_set_style_text_color(psi_lbl, lv_palette_main(LV_PALETTE_BLUE), 0);

        /* Position: 25 psi is at angle 0°, radius just outside ticks */
        float angle_rad = 0.0f * ((float)M_PI / 180.0f);
        int cx = DIAL_D / 2, cy = DIAL_D / 2;
        float r = (SCALE_D / 2.0f) + 12.0f;
        int x = (int)(cx + r * cosf(angle_rad)) - (lv_obj_get_width(psi_lbl) / 2 + 75);
        int y = (int)(cy + r * sinf(angle_rad)) - (lv_obj_get_height(psi_lbl) / 2) + PSI_Y_OFF + 40;
        lv_obj_set_pos(psi_lbl, x, y);
    }

    /* 5) “WMI FLOW” caption at the bottom */
    {
        lv_obj_t* cap = lv_label_create(cont);
        lv_label_set_text(cap, "WMI Flow");
        lv_obj_set_style_text_font(cap, &eurostile_extended_bold_20, 0);
        lv_obj_set_style_text_color(cap, lv_color_white(), 0);
        lv_obj_align(cap, LV_ALIGN_BOTTOM_MID, 0, -15);
    }

    /* 6) Initialize the arc at 0 psi */
    wmi_flow_set_value(0);
}
