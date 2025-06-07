#include <Arduino.h>
#include <Ticker.h>
#include <esp32_smartdisplay.h>
#include <lvgl.h>
#include <time.h>

extern "C"
{
#include "boot_screen.h"
#include "dashboard_layout.h"
#include "wmi_flow_gauge.h"
#include "boost_gauge.h"
#include "fluid_seg_gauge.h"
}

// LVGL tick period in ms
static constexpr uint32_t LVGL_TICK_PERIOD_MS = 2;
// Delay in loop to yield to RTOS (ms)
static constexpr uint32_t LOOP_DELAY_MS = 5;
// Boot splash duration (ms)
static constexpr uint32_t SPLASH_DURATION_MS = 3000;

static Ticker lvgl_tick_ticker;
static lv_timer_t *splash_end_timer = nullptr;

// Animation state
static int g_flow_value = 0;    // 0…25 psi
static int g_boost_value = -10; // –10…30 psi
static uint8_t g_metho_pct = 0; // 0…100%

// Gauge animation callbacks
static void flow_anim_cb(lv_timer_t *t)
{
    (void)t;
    wmi_flow_set_value(g_flow_value);
    // Smooth wrap: use lv_anim for easing if desired
    g_flow_value = (g_flow_value + 1) % 26;
}

static void boost_anim_cb(lv_timer_t *t)
{
    (void)t;
    boost_set_value(g_boost_value);
    g_boost_value++;
    if (g_boost_value > 30)
        g_boost_value = -10;
}

static void fluid_anim_cb(lv_timer_t *t)
{
    (void)t;
    fluid_seg_set_value(g_metho_pct);
    g_metho_pct = (g_metho_pct + 1) % 101;
}

// Called once splash duration elapses
static void splash_end_cb(lv_timer_t *timer)
{
    lv_timer_del(timer);
    // Create dashboard UI
    create_dashboard_layout(lv_scr_act());

    // Start gauge animations at 120 ms intervals
    lv_timer_create(flow_anim_cb, 120, nullptr);
    lv_timer_create(boost_anim_cb, 120, nullptr);
    lv_timer_create(fluid_anim_cb, 120, nullptr);
}

void setup()
{
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(100);

    // Initialize LVGL + display + touch + PSRAM
    smartdisplay_init();

    // Set up LVGL tick on a regular hardware timer
    lvgl_tick_ticker.attach_ms(LVGL_TICK_PERIOD_MS, []()
                               { lv_tick_inc(LVGL_TICK_PERIOD_MS); });

    // Show boot splash
    create_boot_screen();

    // Schedule transition from splash to dashboard
    splash_end_timer = lv_timer_create(
        splash_end_cb,
        SPLASH_DURATION_MS,
        nullptr);
    lv_timer_set_repeat_count(splash_end_timer, 1);
}

void loop()
{
    // Service LVGL tasks (rendering, animations, timers)
    lv_timer_handler();

    // Yield to allow RTOS maintenance and PSRAM cache tasks
    delay(LOOP_DELAY_MS);
}
