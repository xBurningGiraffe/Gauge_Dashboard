/* eurostile_fonts.h
 *
 * Consolidated declarations for the Eurostile Extended bold LVGL fonts
 * generated as C arrays:
 *      eurostile_extended_bold_14.c
 *      eurostile_extended_bold_16.c
 *      eurostile_extended_bold_18.c
 *      eurostile_extended_bold_20.c
 *      eurostile_extended_bold_22.c
 *      eurostile_extended_bold_24.c
 *
 * --------------------------------------------------------------------
 * Usage:
 *      #include "eurostile_fonts.h"
 *      lv_obj_set_style_text_font(label, &eurostile_extended_bold_16, 0);
 *
 * If you want one of these to be the global LVGL default, add e.g.
 *      #define LV_FONT_DEFAULT    &eurostile_extended_bold_16
 * to your lv_conf.h after including lvgl headers.
 * --------------------------------------------------------------------
 */

#ifndef EUROSTILE_FONTS_H
#define EUROSTILE_FONTS_H


#include <lvgl.h>          /* pulls in <lv_font.h> / lv_font_t */

#ifdef __cplusplus
extern "C" {
#endif  

    extern lv_font_t eurostile_extended_bold_10;

    /* 12 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_12;

    /* 14 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_14;

    /* 16 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_16;

    /* 18 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_18;

    /* 20 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_20;

    /* 22 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_22;

    /* 24 px Eurostile Extended bold */
    extern lv_font_t eurostile_extended_bold_24;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EUROSTILE FONTS */
