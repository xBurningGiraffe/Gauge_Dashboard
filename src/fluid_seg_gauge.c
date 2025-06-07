// fluid_seg_gauge.c – landscape gauge with left?side bars taller, tapering to base height

#include "fluid_seg_gauge.h"

/* ---------- configurable constants --------------------------------- */
/* Number of segments (bars) total */
#define SEG_COUNT 20 /* or any N ? 1 */

/* How many of the leftmost bars get extra height that then tapers away */
#define TAPER_COUNT 16 /* first 5 bars are “taller,” then taper */

/* Maximum extra pixels added to the very first bar. Bars 0..TAPER_COUNT?1
 * will each get ( TAPER_COUNT – i ) * (MAX_EXTRA_HEIGHT/TAPER_COUNT ) added
 * to the base height. */
#define MAX_EXTRA_HEIGHT 18

/* Container size: make it wider if you want a longer row of bars */
#define CONT_W 420 /* e.g. 320 px wide instead of 260 */
#define CONT_H 150 /* keep 130 px tall */

/* Rounded?corner and border thickness */
#define CORNER_RADIUS 16
#define BORDER_W 2

/* Base bar dimensions */
#define BASE_SEG_WIDTH 16  /* thickness (x) of each bar */
#define BASE_SEG_HEIGHT 50 /* base height (y) of each bar */
#define SEG_GAP 4          /* horizontal gap between bars */

/* How many rightmost bars are “low?zone” (turn red instead of blue) */
#define LOW_ZONE_COUNT 5

static lv_obj_t *segs[SEG_COUNT];
static lv_obj_t *label_pct;

/*???????????????????????????????????????????????????????????????????????????*/
/* Drag callback so you can move the entire gauge around                  */
/*???????????????????????????????????????????????????????????????????????????*/
static void drag_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target_obj(e);
    lv_indev_t *indev = lv_indev_active();
    if (!indev)
        return;
    lv_point_t v;
    lv_indev_get_vect(indev, &v);
    lv_obj_set_pos(obj,
                   lv_obj_get_x(obj) + v.x,
                   lv_obj_get_y(obj) + v.y);
}

/*???????????????????????????????????????????????????????????????????????????*/
/* Turn a single segment “on” or “off,” picking color based on low?zone      */
/*???????????????????????????????????????????????????????????????????????????*/
static void seg_set_on(lv_obj_t *seg, bool on, bool low_zone)
{
    lv_color_t c_on = low_zone ? lv_palette_main(LV_PALETTE_RED)
                               : lv_palette_main(LV_PALETTE_BLUE);
    lv_color_t c_off = lv_color_hex3(0x222);

    lv_obj_set_style_bg_opa(seg, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(seg, on ? c_on : c_off, 0);
    lv_obj_set_style_border_width(seg, 1, 0);
    lv_obj_set_style_border_color(seg, lv_color_hex3(0x111), 0);
}

/*???????????????????????????????????????????????????????????????????????????*/
/* Public setter: light up bars left?to?right based on pct (0–100)          */
/*???????????????????????????????????????????????????????????????????????????*/
void fluid_seg_set_value(uint8_t pct) /* 0–100% */
{
    if (pct > 100)
        pct = 100;
    /* Determine how many bars to light (rounded) */
    uint8_t lit = (pct * SEG_COUNT + 50) / 100;

    for (int i = 0; i < SEG_COUNT; i++)
    {
        bool on = (i < lit);
        bool low = (i >= SEG_COUNT - LOW_ZONE_COUNT);
        seg_set_on(segs[i], on, low);
    }

    /* Update the text */
    lv_label_set_text_fmt(label_pct, "%u%%", pct);

    /* Recolor the label based on pct: */
    if (pct < 30)
    {
        // 0–29% ? red
        lv_obj_set_style_text_color(label_pct, lv_color_hex(0xB10000), 0);
    }
    else if (pct < 70)
    {
        // 30–69% ? yellow
        lv_obj_set_style_text_color(label_pct, lv_color_hex(0xFFFF00), 0);
    }
    else
    {
        // 70–100% ? green
        lv_obj_set_style_text_color(label_pct, lv_color_hex(0x00FF00), 0);
    }
}

/*???????????????????????????????????????????????????????????????????????????*/
/* Constructor: build the gauge                                            */
/*???????????????????????????????????????????????????????????????????????????*/
void create_fluid_seg_gauge(lv_obj_t *parent)

{

    lv_obj_t *frame = lv_obj_create(parent);
    lv_obj_remove_style_all(frame);
    /* Make it, for example, 340×170 so it extends 10 px on all sides */
    lv_obj_set_size(frame, 440, 170);
    lv_obj_center(frame);

    // Apply vertical dark gradient style to frame
    static lv_style_t frame_grad;
    lv_style_init(&frame_grad);

    /* Top = dark gray 0x333333, bottom = near-black 0x141414 */
    lv_style_set_bg_color(&frame_grad, lv_color_hex(0x333333));
    lv_style_set_bg_grad_color(&frame_grad, lv_color_hex(0x141414));
    lv_style_set_bg_grad_dir(&frame_grad, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&frame_grad, LV_OPA_COVER);

    /* Make the frame circular?cornered to match the contour of “cont” */
    lv_style_set_radius(&frame_grad, CORNER_RADIUS);

    /* 3) Attach the gradient style to the frame */
    lv_obj_add_style(frame, &frame_grad, 0);

    // Create solid black bezel container on top
    lv_obj_t *bezel = lv_obj_create(parent);
    lv_obj_remove_style_all(bezel);
    /* Bezel size exactly matches your gauge area (here 320×150): */
    lv_obj_set_size(bezel, CONT_W, CONT_H);
    lv_obj_center(bezel);

    /* Fill bezel interior with fully opaque black so no gradient shows inside */
    lv_obj_set_style_bg_color(bezel, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(bezel, LV_OPA_COVER, 0);

    /* Draw a white border “inside” that black area */
    lv_obj_set_style_border_width(bezel, BORDER_W, 0);
    lv_obj_set_style_border_color(bezel, lv_color_white(), 0);
    lv_obj_set_style_radius(bezel, CORNER_RADIUS, 0);

    /* Ensure the bezel sits above the gradient frame */
    lv_obj_move_foreground(bezel);

    /* 1) Create the rounded?rectangle container */
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, CONT_W, CONT_H);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_radius(cont, CORNER_RADIUS, 0);
    lv_obj_set_style_border_width(cont, BORDER_W, 0);
    lv_obj_set_style_border_color(cont, lv_color_white(), 0);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(cont, drag_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_center(cont);
    lv_obj_move_foreground(cont);

    /* 2) “METHANOL” caption at top-center */
    lv_obj_t *lbl_title = lv_label_create(cont);
    lv_label_set_text(lbl_title, "METHANOL");
    lv_obj_set_style_text_color(lbl_title, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_title, &eurostile_extended_black_18, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 6);

    /* 3) Compute each bar’s height, then place them bottom-aligned */
    /*    – Bars 0..TAPER_COUNT–1 get extra height: */
    /*        extra = (TAPER_COUNT – i) * (MAX_EXTRA_HEIGHT / TAPER_COUNT) */
    /*    – Bars i ? TAPER_COUNT use BASE_SEG_HEIGHT exactly.            */
    static lv_coord_t seg_h[SEG_COUNT];
    for (int i = 0; i < SEG_COUNT; i++)
    {
        if (i < TAPER_COUNT)
        {
            /* linearly taper from MAX_EXTRA_HEIGHT ? 0 */
            seg_h[i] = BASE_SEG_HEIGHT +
                       ((TAPER_COUNT - i) * MAX_EXTRA_HEIGHT) / TAPER_COUNT;
        }
        else
        {
            seg_h[i] = BASE_SEG_HEIGHT;
        }
    }
    /* All bars share SEG_WIDTH (thickness) but vary in height now */
    const lv_coord_t seg_w = BASE_SEG_WIDTH;

    /* 4) Determine horizontal centering for entire row */
    int total_row_width = (SEG_COUNT * seg_w) + ((SEG_COUNT - 1) * SEG_GAP);
    int start_x = (CONT_W - total_row_width) / 2;

    /* 5) Determine the baseline Y (all bars bottom align here) */
    /*    We want some space at the very bottom so the percent label can fit.
     *    For example, leave 30 pixels at bottom. Adjust as needed.            */
    const int BOTTOM_PADDING = 30;
    int bar_baseline_y = CONT_H - BOTTOM_PADDING;

    for (int i = 0; i < SEG_COUNT; i++)
    {
        segs[i] = lv_obj_create(cont);
        lv_obj_remove_style_all(segs[i]);
        lv_obj_set_size(segs[i], seg_w, seg_h[i]);
        lv_obj_set_style_radius(segs[i], 2, 0);

        /* Initial “off” state, color based on low?zone */
        bool is_low_zone = (i >= SEG_COUNT - LOW_ZONE_COUNT);
        seg_set_on(segs[i], false, is_low_zone);

        /* X = start_x + i*(seg_w + SEG_GAP) */
        int x_pos = start_x + i * (seg_w + SEG_GAP);
        /* Y = baseline – seg_h[i] (so bottom of each bar sits on baseline) */
        int y_pos = bar_baseline_y - seg_h[i];
        lv_obj_set_pos(segs[i], x_pos, y_pos);
    }

    /* 6) “F” and “E” labels, sitting just above each end bar */
    const int LABEL_OFF_Y = 12;  /* vertical gap between bar top and label */
    const int LABEL_INSET_X = 8; /* push inward horizontally so they’re not cut off */
    const int LABEL_RAISE_Y = 8; /* lift them up a bit more */

    /* “F” above the leftmost bar */
    lv_obj_t *lbl_f = lv_label_create(cont);
    lv_label_set_text(lbl_f, "F");
    lv_obj_set_style_text_color(lbl_f, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_f, &eurostile_extended_black_20, 0);

    /* Leftmost bar’s X center = start_x + seg_w/2 */
    int f_x = start_x + (seg_w / 2) + LABEL_INSET_X - (lv_obj_get_width(lbl_f) / 2 + 15);
    /* Bar 0’s top Y = (bar_baseline_y - seg_h[0]) ; place label above that by LABEL_OFF_Y */
    int f_y = (bar_baseline_y - seg_h[0]) - LABEL_OFF_Y - (lv_obj_get_height(lbl_f) / 2) - LABEL_RAISE_Y;
    lv_obj_set_pos(lbl_f, f_x, f_y);

    /* “E” above the rightmost bar */
    lv_obj_t *lbl_e = lv_label_create(cont);
    lv_label_set_text(lbl_e, "E");
    lv_obj_set_style_text_color(lbl_e, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_text_font(lbl_e, &eurostile_extended_black_20, 0);

    /* Rightmost bar’s X center = start_x + (SEG_COUNT–1)*(seg_w+SEG_GAP) + seg_w/2 */
    int right_bar_center_x = start_x + (SEG_COUNT - 1) * (seg_w + SEG_GAP) + (seg_w / 2);
    int e_x = right_bar_center_x - LABEL_INSET_X - (lv_obj_get_width(lbl_e) / 2);
    int e_y = (bar_baseline_y - seg_h[SEG_COUNT - 1]) - LABEL_OFF_Y - (lv_obj_get_height(lbl_e) / 2 + 10) - LABEL_RAISE_Y;
    lv_obj_set_pos(lbl_e, e_x, e_y);

    /* 7) Numeric read?out (% value) at bottom?center */
    label_pct = lv_label_create(cont);
    lv_obj_set_style_text_color(label_pct, lv_color_white(), 0);
    lv_obj_set_style_text_font(label_pct, &eurostile_extended_black_20, 0);
    lv_label_set_text(label_pct, "0%");
    /* Align to bottom middle, with an upward offset so it doesn’t overlap the container’s border */
    lv_obj_align(label_pct, LV_ALIGN_BOTTOM_MID, 0, -2);
    fluid_seg_set_value(0);
}
