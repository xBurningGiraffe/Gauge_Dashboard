// src/boot_screen.c

#include "boot_screen.h"

static lv_obj_t *boot_cont = NULL;
static lv_obj_t *logo_img = NULL;
static lv_obj_t *label_stinger = NULL;

/* Forward declarations */
static void boot_img_fade_cb(void *target, int32_t v);
static void boot_label_fade_cb(void *target, int32_t v);
static void start_fade_out_cb(lv_timer_t *t);
static void fade_out_exec_cb(void *target, int32_t v);
static void delete_boot_cont_cb(lv_timer_t *t);

/**
 * Create and display a full?screen boot splash with a dark gradient,
 * a fading?in logo, and a “STINGER” label underneath. After a brief pause,
 * the entire container fades out smoothly.
 */
void create_boot_screen(void)
{
    /* 1) Create a full?screen container for the boot splash */
    boot_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(boot_cont);
    lv_obj_set_size(boot_cont, 800, 480);
    lv_obj_align(boot_cont, LV_ALIGN_CENTER, 0, 0);

    /* 2) Apply a dark gradient background to boot_cont */
    static lv_style_t cont_grad;
    lv_style_init(&cont_grad);
    lv_style_set_bg_color(&cont_grad, lv_color_hex(0x333333));
    lv_style_set_bg_grad_color(&cont_grad, lv_color_hex(0x141414));
    lv_style_set_bg_grad_dir(&cont_grad, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&cont_grad, LV_OPA_COVER);
    lv_obj_add_style(boot_cont, &cont_grad, 0);

    /* 3) Create the logo image, positioned slightly below vertical center */
    logo_img = lv_img_create(boot_cont);
    lv_img_set_src(logo_img, &wasp_small_red); // your converted C?array
    lv_obj_align(logo_img, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_img_opa(logo_img, LV_OPA_TRANSP, 0);

    /* 4) Create the “STINGER” label under the logo */
    label_stinger = lv_label_create(boot_cont);
    lv_label_set_text(label_stinger, "STINGER");
    /* Use Porsche 48?px font, color = 0xB10000 */
    lv_obj_set_style_text_font(label_stinger, &porsche_48, 0);
    lv_obj_set_style_text_color(label_stinger, lv_color_hex(0xB10000), 0);
    /* Start fully transparent */
    lv_obj_set_style_text_opa(label_stinger, LV_OPA_TRANSP, LV_PART_MAIN);
    /* Position label 10px below the logo */
    lv_obj_align_to(label_stinger, logo_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /* 5) Animate the logo fade?in: 0 ? 255 over 1000 ms */
    {
        static lv_anim_t fade_in_anim_logo;
        lv_anim_init(&fade_in_anim_logo);
        lv_anim_set_var(&fade_in_anim_logo, logo_img);
        lv_anim_set_exec_cb(&fade_in_anim_logo, boot_img_fade_cb);
        lv_anim_set_values(&fade_in_anim_logo, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_set_time(&fade_in_anim_logo, 1000);
        lv_anim_start(&fade_in_anim_logo);
    }

    /* 6) Animate the label fade?in in parallel: 0 ? 255 over 1000 ms */
    {
        static lv_anim_t fade_in_anim_label;
        lv_anim_init(&fade_in_anim_label);
        lv_anim_set_var(&fade_in_anim_label, label_stinger);
        lv_anim_set_exec_cb(&fade_in_anim_label, boot_label_fade_cb);
        lv_anim_set_values(&fade_in_anim_label, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_set_time(&fade_in_anim_label, 1000);
        lv_anim_start(&fade_in_anim_label);
    }

    /* 7) After fade?in + 500 ms pause, begin fading out the entire boot_cont */
    lv_timer_create(start_fade_out_cb, 1500, NULL);
}

static void boot_img_fade_cb(void *target, int32_t v)
{
    lv_obj_set_style_img_opa((lv_obj_t *)target, v, 0);
}

static void boot_label_fade_cb(void *target, int32_t v)
{
    lv_obj_set_style_text_opa((lv_obj_t *)target, v, LV_PART_MAIN);
}

static void start_fade_out_cb(lv_timer_t *t)
{
    lv_timer_del(t);

    /* Animate boot_cont from opaque ? transparent over 500 ms */
    static lv_anim_t fade_out_anim;
    lv_anim_init(&fade_out_anim);
    lv_anim_set_var(&fade_out_anim, boot_cont);
    lv_anim_set_exec_cb(&fade_out_anim, fade_out_exec_cb);
    lv_anim_set_values(&fade_out_anim, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&fade_out_anim, 500);
    lv_anim_start(&fade_out_anim);

    /* After the 500 ms fade, delete boot_cont (add 50 ms buffer) */
    lv_timer_create(delete_boot_cont_cb, 550, NULL);
}

static void fade_out_exec_cb(void *target, int32_t v)
{
    lv_obj_set_style_bg_opa((lv_obj_t *)target, v, 0);
}

static void delete_boot_cont_cb(lv_timer_t *t)
{
    lv_timer_del(t);
    if (boot_cont)
    {
        lv_obj_del(boot_cont);
        boot_cont = NULL;
    }
}
