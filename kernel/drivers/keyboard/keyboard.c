#include "keyboard.h"
#include "shell/shell.h"
#include <arch/x86/portio.h>
#include <drivers/vga_console/vga_console.h>
#include <lib/io/io.h>

static char scancode_to_ascii[] = {
    0,   27,  '&', '2', '"',  '\'', '(',  '-',  // 0x00-0x07 (é->2 for now)
    '8', '_', '9', '0', ')',  '=',  '\b', '\t', // 0x08-0x0F (è->8, ç->9, à->0)
    'a', 'z', 'e', 'r', 't',  'y',  'u',  'i',  // 0x10-0x17
    'o', 'p', '^', '$', '\n', 0,    'q',  's',  // 0x18-0x1F (0x1D = Ctrl)
    'd', 'f', 'g', 'h', 'j',  'k',  'l',  'm',  // 0x20-0x27
    'u', '2', 0,   '*', 'w',  'x',  'c',  'v',  // 0x28-0x2F (ù->u, ²->2)
    'b', 'n', ',', ';', ':',  '!',  0,    '*',  // 0x30-0x37 (0x36 = RShift)
    0,   ' ', 0                                 // 0x38-0x3A (Alt, Space, Caps)
};

static char scancode_to_ascii_shift[] = {
    0,   27,  '1', '2', '3',  '4', '5',  '6',
    '7', '8', '9', '0', 'd',  '+', '\b', '\t', // °->d for degree
    'A', 'Z', 'E', 'R', 'T',  'Y', 'U',  'I',
    'O', 'P', 'u', 'p', '\n', 0,   'Q',  'S', // ¨->u, £->p
    'D', 'F', 'G', 'H', 'J',  'K', 'L',  'M',
    '%', '~', 0,   'u', 'W',  'X', 'C',  'V', // µ->u
    'B', 'N', '?', '.', '/',  's', 0,    '*', // §->s
    0,   ' ', 0};

static int shift_pressed = 0;

void keyboard_irq(void) {
  uint8_t sc = inb(0x60);

  // Check for key release (bit 7 set)
  if (sc & 0x80) {
    sc &= 0x7F; // Remove release bit

    // Handle shift release
    if (sc == 0x2A || sc == 0x36) { // Left/Right Shift
      shift_pressed = 0;
    }
    return;
  }

  // Handle key press
  switch (sc) {
  case 0x2A: // Left Shift
  case 0x36: // Right Shift
    shift_pressed = 1;
    break;

  default:
    // Convert to ASCII if valid scancode
    if (sc < sizeof(scancode_to_ascii)) {
      char c =
          shift_pressed ? scancode_to_ascii_shift[sc] : scancode_to_ascii[sc];

      if (c != 0) {
        shell_handle_char(c);
      }
    }
    break;
  }
}
