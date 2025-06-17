/* boost_gauge.c
 *
 * –10…30 psi gauge with a sweeping red arc that fills the
 * bezel-to-scale gap, built-in labels, inner ticks, instant updates.
 */

#define _USE_MATH_DEFINES

#include <math.h>
#include "boost_gauge.h"


 /* Geometry constants */
#define DIAL_D       320                   /* full bezel diameter       */
#define SCALE_D      (DIAL_D - 80)         /* your chosen inner scale   */
#define MINOR_CNT    41                    /* –10…30 inclusive ticks    */
#define MAJOR_EVERY  10                    /* major every 10 psi       */
#define PAD_RADIAL  -70                 /* label inward offset      */
#define PSI_Y_OFF    80            /* you already tweaked this */
// #define ARC_WIDTH    ((DIAL_D/2) - (SCALE_D/2))
#define ARC_WIDTH 40
#define SCALE_SIZE  260
#define SCALE_ROTATION 140

/* Static handles */
static lv_obj_t* scale_obj;
static lv_obj_t* arc_indicator;


/* draggable helper */
static void drag_cb(lv_event_t* e) {
    lv_obj_t* o = lv_event_get_target_obj(e);
    lv_indev_t* id = lv_indev_active();
    if (!id) return;
    lv_point_t   v; lv_indev_get_vect(id, &v);
    lv_obj_set_pos(o,
        lv_obj_get_x(o) + v.x,
        lv_obj_get_y(o) + v.y);
}

/* Public setter: instant red-arc sweep */
void boost_set_value(int16_t psi) {
    if (psi < -10) psi = -10;
    if (psi > 40) psi = 40;
    lv_arc_set_value(arc_indicator, psi + 10);


}



/* Constructor */
void create_boost_gauge(lv_obj_t* parent) {

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
    /* Top half = mid‐gray, bottom half = nearly black */
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

    /* Inside‐bezel fill */
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    /* Bezel border (drawn on top of the black face, yet beneath the inner ticks) */
    lv_obj_set_style_border_width(cont, 5, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(0x3B3B3B), 0);
    lv_obj_set_style_border_opa(cont, LV_OPA_COVER, 0);

    /* Keep overflow visible so we can place ticks / arcs inside */
    lv_obj_add_flag(cont, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(cont, drag_cb, LV_EVENT_PRESSING, NULL);


    /* 2) Inner scale: ticks + built-in labels */
    scale_obj = lv_scale_create(cont);
    lv_obj_center(scale_obj);
    lv_obj_set_style_arc_width(scale_obj, 0, LV_PART_MAIN);
    lv_obj_set_size(scale_obj, SCALE_D, SCALE_D);
    lv_obj_add_flag(scale_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

    lv_scale_set_mode(scale_obj, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_range(scale_obj, 0, 40);
    lv_scale_set_total_tick_count(scale_obj, MINOR_CNT);
    lv_scale_set_major_tick_every(scale_obj, MAJOR_EVERY);
    lv_scale_set_angle_range(scale_obj, SCALE_SIZE);
    lv_scale_set_rotation(scale_obj, SCALE_ROTATION);
    lv_scale_set_label_show(scale_obj, true);

    /* minor ticks */
    lv_obj_set_style_line_width(scale_obj, 1, LV_PART_ITEMS);
    lv_obj_set_style_length(scale_obj, 14, LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale_obj, lv_color_white(), LV_PART_ITEMS);

    /* major ticks */
    lv_obj_set_style_line_width(scale_obj, 7, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale_obj, 24, LV_PART_INDICATOR);
    lv_obj_set_style_line_color(scale_obj, lv_color_white(), LV_PART_INDICATOR);

    /* labels */
    lv_obj_set_style_text_font(scale_obj, &eurostile_extended_bold_24, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(scale_obj, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_pad_radial(scale_obj, PAD_RADIAL, LV_PART_INDICATOR);

    /* 3) Red arc indicator (fills bezel-to-scale gap) */
    arc_indicator = lv_arc_create(cont);
    /* make widget full-bezel size, so stroke reaches inward to SCALE_D and outward to border */
    lv_obj_set_size(arc_indicator, DIAL_D, DIAL_D);
    lv_obj_center(arc_indicator);
    lv_obj_remove_style(arc_indicator, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_indicator, LV_OBJ_FLAG_CLICKABLE);

    /* Foreground = red, semi-transparent or opaque as you like */
    lv_obj_set_style_arc_color(arc_indicator, lv_color_hex(0xB10000), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc_indicator, LV_OPA_90, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_indicator, ARC_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc_indicator, false, LV_PART_INDICATOR);

    /* Hide the background arc completely */
    lv_obj_set_style_arc_color(arc_indicator, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_indicator, 0, LV_PART_MAIN);

    /* Configure sweep: 0…40 over 225° at 135° start */
    lv_arc_set_range(arc_indicator, 0, 40);
    lv_arc_set_bg_angles(arc_indicator, 0, SCALE_SIZE);
    lv_arc_set_rotation(arc_indicator, SCALE_ROTATION);

    /* start at –10 psi */
    lv_arc_set_value(arc_indicator, 0);
    lv_obj_move_background(arc_indicator);

    // Inner bar scale
    lv_obj_t* inner = lv_scale_create(cont);
    lv_obj_center(inner);
    // Smaller than main scale
    lv_obj_set_size(inner, SCALE_D - 115, SCALE_D - 115 );
    // Configuration
    lv_scale_set_mode(inner, LV_SCALE_MODE_ROUND_INNER);
    // Range from 0 to 3
    lv_scale_set_range(inner, 0, 3);
    // 31 minor ticks → one tick per 0.1 unit
    lv_scale_set_total_tick_count(inner, 31);
    // Label every 10 ticks → at 0, 10, 20, 30 → values 0,1,2,3
    lv_scale_set_major_tick_every(inner, 10);
    // Full 225° sweep, same rotation as main
    lv_scale_set_angle_range(inner, 220);
    lv_scale_set_rotation(inner, 140);
    lv_scale_set_label_show(inner, true);
    // Minor ticks: thin and short
    lv_obj_set_style_line_width(inner, 1, LV_PART_ITEMS);
    lv_obj_set_style_length(inner, 6, LV_PART_ITEMS);
    lv_obj_set_style_line_color(inner, lv_color_hex(0xB10000), LV_PART_ITEMS);
    // Major ticks: thicker
    lv_obj_set_style_line_width(inner, 3, LV_PART_INDICATOR);
    lv_obj_set_style_length(inner, 12, LV_PART_INDICATOR);
    lv_obj_set_style_line_color(inner, lv_color_hex(0xB10000), LV_PART_INDICATOR);
    // Labels: white, small font
    lv_obj_set_style_text_font(inner, &eurostile_extended_bold_14, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(inner, lv_color_hex(0xB10000), LV_PART_INDICATOR);
    // Pull them in slightly so they don’t overlap ticks
    lv_obj_set_style_pad_radial(inner, -8, LV_PART_ITEMS);
    lv_obj_set_style_arc_color(inner, lv_color_hex(0xB10000), LV_PART_MAIN);
    lv_obj_move_foreground(inner);

    // bar label
    lv_obj_t* l = lv_label_create(cont);
    lv_label_set_text(l, "bar");
    lv_obj_set_style_text_font(l, &eurostile_extended_bold_12, 0);
    lv_obj_set_style_text_color(l, lv_color_hex(0xB10000), 0);
    float ang = (225.0f - ((30 + 10) * (270.0f / 40.0f)))
        * (float)M_PI / 180.0f;
    int cx = DIAL_D / 2, cy = DIAL_D / 2;
    float r = (SCALE_D / 2.0f) + 8.0f;
    int x = (int)(cx + r * cosf(ang)) - lv_obj_get_width(l) / 2 - 50;
    int y = (int)(cy + r * sinf(ang)) - lv_obj_get_height(l) / 2 + PSI_Y_OFF + 10;
    lv_obj_set_pos(l, x, y);





    /* 4) “psi” label under 30 psi */
    {
        lv_obj_t* u = lv_label_create(cont);
        lv_label_set_text(u, "psi");
        lv_obj_set_style_text_font(u, &eurostile_extended_bold_12, 0);
        lv_obj_set_style_text_color(u, lv_color_hex(0xB10000), 0);
        float ang = (225.0f - ((30 + 10) * (270.0f / 40.0f)))
            * (float)M_PI / 180.0f;
        int cx = DIAL_D / 2, cy = DIAL_D / 2;
        float r = (SCALE_D / 2.0f) + 8.0f;
        int x = (int)(cx + r * cosf(ang)) - lv_obj_get_width(u) / 2 - 35;
        int y = (int)(cy + r * sinf(ang)) - lv_obj_get_height(u) / 2 + PSI_Y_OFF + 87;
        lv_obj_set_pos(u, x, y);
    }

    // 5) “BOOST” caption
    
     {
        lv_obj_t* b = lv_label_create(cont);
        lv_label_set_text(b, "Boost");
        lv_obj_set_style_text_font(b, &eurostile_extended_bold_22, 0);
        lv_obj_set_style_text_color(b, lv_color_hex(0xB10000), 0);
        lv_obj_align(b, LV_ALIGN_BOTTOM_MID, 0, -20);
    }
    
    
    /* initialize at –10 psi */
    boost_set_value(-10);


}
