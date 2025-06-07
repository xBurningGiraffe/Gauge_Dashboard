// boost_safe.c

#include "boost_safe.h"
#include <string.h>

static lv_obj_t *boost_safe_msgbox = NULL;
static lv_timer_t *boost_safe_timer = NULL;

/* Called when a footer button is clicked */
static void boost_safe_msgbox_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target_obj(e);
    const char *txt = lv_label_get_text(lv_obj_get_child(btn, 0));
    if (txt && strcmp(txt, "OK") == 0)
    {
        lv_msgbox_close(boost_safe_msgbox);
        boost_safe_msgbox = NULL;
    }
}

/* Toggles the simulated Boost-Safe flag every 5s; when it fails, pop up msgbox */
static void boost_safe_toggle_cb(lv_timer_t *t)
{
    static bool boost_is_ok = true;
    LV_UNUSED(t);

    boost_is_ok = !boost_is_ok;
    if (!boost_is_ok && boost_safe_msgbox == NULL)
    {
        boost_safe_msgbox = lv_msgbox_create(lv_layer_top());
        lv_msgbox_add_title(boost_safe_msgbox, "Boost Safe Error");
        lv_msgbox_add_text(boost_safe_msgbox, "Boost Safe Signal\nFAIL");
        lv_obj_set_style_text_font(boost_safe_msgbox, &eurostile_extended_black_14, 0);

        /* Create the “OK” button and immediately recolor it to 0xB10000: */
        lv_obj_t *btn = lv_msgbox_add_footer_button(boost_safe_msgbox, "OK");
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xB10000), LV_PART_MAIN);
        lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_text_font(btn, &eurostile_extended_black_12, LV_PART_ITEMS);

        lv_obj_add_event_cb(btn, boost_safe_msgbox_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_center(boost_safe_msgbox);
    }
}

void create_boost_safe_indicator(lv_obj_t *parent)
{
    LV_UNUSED(parent);
    if (boost_safe_timer == NULL)
    {
        boost_safe_timer = lv_timer_create(boost_safe_toggle_cb, 5000, NULL);
    }
}
