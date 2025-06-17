// boot_screen.h

#ifndef BOOT_SCREEN_H
#define BOOT_SCREEN_H

#include <lvgl.h>
#include "fonts/porsche_fonts.h"
#include <images/wasp_small_red.h>

/**
 * @brief Create and display a full-screen boot splash with a dark gradient
 *        and a fading-in image. The splash will automatically remove itself
 *        after the timeout (handled internally).
 */
void create_boot_screen(void);

#endif /* BOOT_SCREEN_H */
