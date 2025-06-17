#include <Arduino.h>
#include <Ticker.h>
#include <esp32_smartdisplay.h>
#include <lvgl.h>
#include <time.h>
// #include "utils/espnow_utils.h"
#include "gauges/fluid_seg_gauge.h"
#include "utils/espnow_fluid.h"
#include "utils/espnow_flow.h"
#include "utils/espnow_boost.h"
#include "utils/espnow_boostsafe.h"
#include "utils/espnow_dispatcher.h"
#include "utils/espnow_time.h"

extern "C"
{
#include "ui/ui.h"         // boot_screen.h + dashboard_layout.h
#include "gauges/gauges.h" // other C‐based gauge headers
#include "fonts/fonts.h"   // your aggregated font headers
}

// LVGL tick period in ms
static constexpr uint32_t LVGL_TICK_PERIOD_MS = 2;
// Delay in loop to yield to RTOS (ms)
static constexpr uint32_t LOOP_DELAY_MS = 5;
// Boot splash duration (ms)
static constexpr uint32_t SPLASH_DURATION_MS = 3000;

static Ticker lvgl_tick_ticker;
static lv_timer_t *splash_end_timer = nullptr;

// ← New globals to manage screens & one-shot handoff
static lv_obj_t *splash_scr = nullptr;
static lv_obj_t *dash_scr = nullptr;
static bool dash_already_up = false;

// Called once splash duration elapses
static void splash_end_cb(lv_timer_t *timer)
{
    // Only transition once
    if (dash_already_up)
        return;
    dash_already_up = true;

    // Stop the timer so it never fires again
    lv_timer_del(timer);

    // 1) Build the dashboard on a fresh screen
    dash_scr = lv_obj_create(NULL);
    create_dashboard_layout(dash_scr);

    // 2) Load that dashboard screen
    lv_scr_load(dash_scr);

    // 3) Completely delete the old splash screen
    lv_obj_del(splash_scr);

    // 4) Now kick off your ESP-NOW dispatcher
    espnow_dispatcher_init();
}

void setup()
{
    // 1) Serial for debug
    Serial.begin(115200);
    delay(100);

    // 2) Init display, touch, LVGL, PSRAM
    smartdisplay_init();

    // 3) LVGL tick via hw timer
    lvgl_tick_ticker.attach_ms(LVGL_TICK_PERIOD_MS, []()
                               { lv_tick_inc(LVGL_TICK_PERIOD_MS); });

    // 4) Show boot splash on its *own* screen
    splash_scr = lv_obj_create(NULL);
    lv_scr_load(splash_scr);
    create_boot_screen();

    // 5) Schedule the one-shot splash→dashboard transition
    splash_end_timer = lv_timer_create(splash_end_cb, SPLASH_DURATION_MS, nullptr);
    lv_timer_set_repeat_count(splash_end_timer, 1);
}

void loop()
{
    // 1) Pump LVGL (draw, timers, animations)
    lv_timer_handler();

    // 2) Consume any incoming ESP-NOW data flags

    // Fluid gauge (methanol)
    if (espnow_fluid_newData)
    {
        espnow_fluid_newData = false;
        fluid_seg_set_value(espnow_fluid_pct);
    }
    
    /*
    // Time update
    if (espnow_time_newData)
    {
        espnow_time_newData = false;
        update_time_label(espnow_hour, espnow_minute);
    }
    */
   
    // WMI flow gauge
    if (espnow_flow_newData)
    {
        espnow_flow_newData = false;
        wmi_flow_set_value(espnow_flow_value);
    }

    // Boost gauge
    if (espnow_boost_newData)
    {
        espnow_boost_newData = false;
        boost_set_value(espnow_boost_value);
    }

    // Boost-Safe indicator
    if (espnow_boostSafe_newData)
    {
        espnow_boostSafe_newData = false;
        handle_boost_safe_flag(espnow_boostSafe);
    }

    // 3) Yield to RTOS
    delay(LOOP_DELAY_MS);
}
