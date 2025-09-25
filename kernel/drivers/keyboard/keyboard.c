#include "keyboard.h"
#include <arch/x86/portio.h>
#include <drivers/vga_console/vga_console.h>

void keyboard_irq(void) {
  uint8_t sc = inb(0x60);
  if (sc < 0x80)
    vga_console_write("[KEY]");
}
