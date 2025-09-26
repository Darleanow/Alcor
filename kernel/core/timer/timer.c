#include "timer.h"
#include <drivers/vga_console/vga_console.h>

static unsigned ticks = 0;

void timer_init(void) {}

void timer_tick(void) {
  //    if (++ticks % 100 == 0) vga_console_write(".");
}
