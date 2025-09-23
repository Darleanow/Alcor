#include "terminal.h"

// Terminal state
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)0xB8000;

  // Clear screen
  terminal_clear();
}

void terminal_clear(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
  terminal_row = 0;
  terminal_column = 0;
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(void) {
  // Move all lines up by one
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      size_t src_index = y * VGA_WIDTH + x;
      size_t dst_index = (y - 1) * VGA_WIDTH + x;
      terminal_buffer[dst_index] = terminal_buffer[src_index];
    }
  }

  // Clear last line
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(' ', terminal_color);
  }

  terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      terminal_scroll();
    }
    return;
  }

  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      terminal_scroll();
    }
  }
}

void terminal_writestring(const char *data) {
  for (size_t i = 0; data[i] != '\0'; i++) {
    terminal_putchar(data[i]);
  }
}
