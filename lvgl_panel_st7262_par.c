#ifdef DISPLAY_ST7262_PAR

#include <assert.h>
#include <esp_log.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_heap_caps.h>
#include <lvgl.h>

static const char *TAG = "lvgl_fullbuf_sync";
static esp_lcd_panel_handle_t s_panel_handle = NULL;

/**
 * @brief Flush callback for LVGL full-refresh using synchronous draw.
 * Draws the entire frame and signals completion.
 */
static void full_refresh_lv_flush(lv_display_t *display,
                                  const lv_area_t *area,
                                  lv_color_t *px_map)
{
    LV_UNUSED(area);
    // Draw full frame synchronously
    esp_err_t err = esp_lcd_panel_draw_bitmap(
        s_panel_handle,
        0, 0,
        DISPLAY_WIDTH, DISPLAY_HEIGHT,
        (uint8_t *)px_map);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_lcd_panel_draw_bitmap failed: %d", err);
    }
    // Notify LVGL that the flush is done
    lv_display_flush_ready(display);
}

/**
 * @brief Initialize LVGL display with two full-frame PSRAM buffers and sync refresh.
 */
lv_display_t *lvgl_lcd_init(void)
{
    // 1) Create LVGL display
    lv_display_t *display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    assert(display);

    // 2) Allocate two full-screen buffers in PSRAM
    size_t buf_size = (size_t)DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(lv_color_t);
    void *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    void *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(buf1 && buf2);
    ESP_LOGI(TAG, "Allocated two full buffers: %u bytes each", (unsigned)buf_size);

    // 3) Register buffers for full-refresh mode
    lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_FULL);

    // 4) Configure and init the RGB panel driver
    esp_lcd_rgb_panel_config_t rgb_cfg = {
        .clk_src = ST7262_PANEL_CONFIG_CLK_SRC,
        .timings.pclk_hz = ST7262_PANEL_CONFIG_TIMINGS_PCLK_HZ,
        .timings.h_res = ST7262_PANEL_CONFIG_TIMINGS_H_RES,
        .timings.v_res = ST7262_PANEL_CONFIG_TIMINGS_V_RES,
        .timings.hsync_pulse_width = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_PULSE_WIDTH,
        .timings.hsync_back_porch = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_BACK_PORCH,
        .timings.hsync_front_porch = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_FRONT_PORCH,
        .timings.vsync_pulse_width = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_PULSE_WIDTH,
        .timings.vsync_back_porch = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_BACK_PORCH,
        .timings.vsync_front_porch = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_FRONT_PORCH,
        .timings.flags.hsync_idle_low = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_HSYNC_IDLE_LOW,
        .timings.flags.vsync_idle_low = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_VSYNC_IDLE_LOW,
        .timings.flags.de_idle_high = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_DE_IDLE_HIGH,
        .timings.flags.pclk_active_neg = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_PCLK_ACTIVE_NEG,
        .timings.flags.pclk_idle_high = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_PCLK_IDLE_HIGH,
        .data_width = ST7262_PANEL_CONFIG_DATA_WIDTH,
        .sram_trans_align = ST7262_PANEL_CONFIG_SRAM_TRANS_ALIGN,
        .psram_trans_align = ST7262_PANEL_CONFIG_PSRAM_TRANS_ALIGN,
        .hsync_gpio_num = ST7262_PANEL_CONFIG_HSYNC_GPIO_NUM,
        .vsync_gpio_num = ST7262_PANEL_CONFIG_VSYNC_GPIO_NUM,
        .de_gpio_num = ST7262_PANEL_CONFIG_DE_GPIO_NUM,
        .pclk_gpio_num = ST7262_PANEL_CONFIG_PCLK_GPIO_NUM,
        .data_gpio_nums = {
            ST7262_PANEL_CONFIG_DATA_GPIO_R0, ST7262_PANEL_CONFIG_DATA_GPIO_R1,
            ST7262_PANEL_CONFIG_DATA_GPIO_R2, ST7262_PANEL_CONFIG_DATA_GPIO_R3,
            ST7262_PANEL_CONFIG_DATA_GPIO_R4, ST7262_PANEL_CONFIG_DATA_GPIO_G0,
            ST7262_PANEL_CONFIG_DATA_GPIO_G1, ST7262_PANEL_CONFIG_DATA_GPIO_G2,
            ST7262_PANEL_CONFIG_DATA_GPIO_G3, ST7262_PANEL_CONFIG_DATA_GPIO_G4,
            ST7262_PANEL_CONFIG_DATA_GPIO_G5, ST7262_PANEL_CONFIG_DATA_GPIO_B0,
            ST7262_PANEL_CONFIG_DATA_GPIO_B1, ST7262_PANEL_CONFIG_DATA_GPIO_B2,
            ST7262_PANEL_CONFIG_DATA_GPIO_B3, ST7262_PANEL_CONFIG_DATA_GPIO_B4},
        .disp_gpio_num = -1,
        .flags.disp_active_low = ST7262_PANEL_CONFIG_FLAGS_DISP_ACTIVE_LOW,
        .flags.relax_on_idle = ST7262_PANEL_CONFIG_FLAGS_RELAX_ON_IDLE,
        .flags.fb_in_psram = ST7262_PANEL_CONFIG_FLAGS_FB_IN_PSRAM};
    esp_lcd_panel_handle_t panel_handle;
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&rgb_cfg, &panel_handle));
    s_panel_handle = panel_handle;
    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel_handle));
#ifdef DISPLAY_IPS
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(s_panel_handle, true));
#endif
#if defined(DISPLAY_GAP_X) || defined(DISPLAY_GAP_Y)
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(s_panel_handle, DISPLAY_GAP_X, DISPLAY_GAP_Y));
#endif

    // 5) Register flush callback
    lv_display_set_flush_cb(display, full_refresh_lv_flush);

    ESP_LOGI(TAG, "LVGL sync-full-refresh display initialized");
    return display;
}

#endif // DISPLAY_ST7262_PAR
