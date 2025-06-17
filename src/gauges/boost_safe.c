// boost_safe.c

#include "boost_safe.h"
#include <string.h>

static lv_obj_t *boost_safe_msgbox = NULL;

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


void handle_boost_safe_flag(bool safe)
{
    if (!safe && boost_safe_msgbox == NULL)
    {
        boost_safe_msgbox = lv_msgbox_create(lv_layer_top());
        lv_msgbox_add_title(boost_safe_msgbox, "Boost Safe Error");
        lv_msgbox_add_text(boost_safe_msgbox, "Boost Safe Signal \nFail");
        lv_obj_set_style_text_font(boost_safe_msgbox, &eurostile_extended_bold_14, 0);

        lv_obj_t *btn = lv_msgbox_add_footer_button(boost_safe_msgbox, "OK");
        lv_obj_set_style_bg_color(btn, lv_color_hex(0XB100000), LV_PART_MAIN);
        lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_text_font(btn, &eurostile_extended_bold_14, LV_PART_ITEMS);
        lv_obj_add_event_cb(btn, boost_safe_msgbox_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_center(boost_safe_msgbox);
    }
}
