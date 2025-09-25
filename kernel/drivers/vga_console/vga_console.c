#include "vga_console.h"
#include <arch/x86/portio.h>
#include <stddef.h>
#include <stdint.h>

/* Mémoire text VGA */
static volatile uint16_t *const VGA = (uint16_t *)0xB8000;

/* Current State */
static uint8_t cur_row = 0;
static uint8_t cur_col = 0;
static uint8_t cur_attr = (VGA_LIGHT_GREY | (VGA_BLACK << 4));

/* Intern */
static inline uint16_t vga_entry(char c, uint8_t attr) {
  return (uint16_t)c | ((uint16_t)attr << 8);
}

static void hw_update_cursor(void) {
  uint16_t pos = (uint16_t)cur_row * VGA_COLS + cur_col;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void scroll_if_needed(void) {
  if (cur_row < VGA_ROWS)
    return;

  /* Moves line upwards */
  for (uint16_t r = 1; r < VGA_ROWS; ++r) {
    for (uint16_t c = 0; c < VGA_COLS; ++c) {
      VGA[(r - 1) * VGA_COLS + c] = VGA[r * VGA_COLS + c];
    }
  }
  /* Erases last line */
  for (uint16_t c = 0; c < VGA_COLS; ++c) {
    VGA[(VGA_ROWS - 1) * VGA_COLS + c] = vga_entry(' ', cur_attr);
  }
  cur_row = VGA_ROWS - 1;
}

void vga_console_init(void) {
  vga_console_clear();
  vga_console_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

void vga_console_clear(void) {
  for (uint16_t i = 0; i < VGA_ROWS * VGA_COLS; ++i)
    VGA[i] = vga_entry(' ', cur_attr);
  cur_row = 0;
  cur_col = 0;
  hw_update_cursor();
}

void vga_console_set_color(uint8_t fg, uint8_t bg) {
  cur_attr = (uint8_t)(fg | (bg << 4));
}

void vga_console_setpos(uint8_t row, uint8_t col) {
  if (row >= VGA_ROWS)
    row = VGA_ROWS - 1;
  if (col >= VGA_COLS)
    col = VGA_COLS - 1;
  cur_row = row;
  cur_col = col;
  hw_update_cursor();
}

void vga_console_putchar(char c) {
  switch (c) {
  case '\n':
    cur_col = 0;
    ++cur_row;
    scroll_if_needed();
    break;
  case '\r':
    cur_col = 0;
    break;
  case '\t': {
    /* Tab on multiple of 4 */
    uint8_t next = (uint8_t)((cur_col + 4) & ~3);
    if (next >= VGA_COLS) {
      cur_col = 0;
      ++cur_row;
      scroll_if_needed();
    } else {
      while (cur_col < next) {
        VGA[(uint16_t)cur_row * VGA_COLS + cur_col] = vga_entry(' ', cur_attr);
        ++cur_col;
      }
    }
    break;
  }
  default:
    VGA[(uint16_t)cur_row * VGA_COLS + cur_col] = vga_entry(c, cur_attr);
    ++cur_col;
    if (cur_col >= VGA_COLS) {
      cur_col = 0;
      ++cur_row;
      scroll_if_needed();
    }
    break;
  }
  hw_update_cursor();
}

void vga_console_write(const char *s) {
  if (!s)
    return;
  while (*s)
    vga_console_putchar(*s++);
}
