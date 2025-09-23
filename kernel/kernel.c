#include "terminal.h"

// Kernel entry point - called from bootloader
void kernel_main(void) {
  // Initialize terminal
  terminal_initialize();

  // Print welcome message
  terminal_setcolor(VGA_COLOR_LIGHT_GRAY);
  terminal_writestring(
      "32-bit Protected Mode C Kernel loaded successfully!\n\n");

  terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
  terminal_writestring("\nKernel initialization complete!\n");
  terminal_setcolor(VGA_COLOR_LIGHT_GRAY);
  terminal_writestring("Kernel is now running in infinite loop...\n");

  // Kernel main loop
  while (1) {
  }
}
