#pragma once
#include <stdint.h>

/* VGA text 80x25, memory at 0xB8000 */
#define VGA_COLS 80
#define VGA_ROWS 25

/* 4 bits VGA Colors */
enum vga_color {
  VGA_BLACK = 0,
  VGA_BLUE,
  VGA_GREEN,
  VGA_CYAN,
  VGA_RED,
  VGA_MAGENTA,
  VGA_BROWN,
  VGA_LIGHT_GREY,
  VGA_DARK_GREY,
  VGA_LIGHT_BLUE,
  VGA_LIGHT_GREEN,
  VGA_LIGHT_CYAN,
  VGA_LIGHT_RED,
  VGA_LIGHT_MAGENTA,
  VGA_YELLOW,
  VGA_WHITE
};

void vga_console_init(void);
void vga_console_clear(void);
void vga_console_set_color(uint8_t fg, uint8_t bg);
void vga_console_setpos(uint8_t row, uint8_t col);
void vga_console_putchar(char c);
void vga_console_write(const char *s);
