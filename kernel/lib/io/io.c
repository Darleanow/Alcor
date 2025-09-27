#include "io.h"

#include <drivers/vga_console/vga_console.h>
#include <lib/standard/standard.h>
#include <stdarg.h>

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  int count = 0;
  const char *p = format;

  while (*p) {
    if (*p == '%' && *(p + 1)) {
      p++;

      switch (*p) {
      case 's': {
        char *str = va_arg(args, char *);
        if (str) {
          while (*str) {
            vga_console_putchar(*str);
            str++;
            count++;
          }
        } else {
          char *null_str = "(null)";
          while (*null_str) {
            vga_console_putchar(*null_str);
            null_str++;
            count++;
          }
        }
        break;
      }
      case 'c': {
        char c = (char)va_arg(args, int);
        vga_console_putchar(c);
        count++;
        break;
      }
      case 'd': {
        int num = va_arg(args, int);
        char buffer[32];
        itoa(num, buffer);

        char *p = buffer;
        while (*p) {
          vga_console_putchar(*p);
          p++;
          count++;
        }
        break;
      }
      case 'p': {
        void *ptr = va_arg(args, void *);
        uintptr_t addr = (uintptr_t)ptr;

        vga_console_write("0x");
        count += 2;

        for (int i = 7; i >= 0; i--) {
          uint8_t nibble = (addr >> (i * 4)) & 0xF;
          char hex_char = (nibble < 10) ? ('0' + nibble) : ('a' + nibble - 10);
          vga_console_putchar(hex_char);
          count++;
        }
        break;
      }
      case '%':
        vga_console_putchar('%');
        count++;
        break;
      default:
        vga_console_putchar('%');
        vga_console_putchar(*p);
        count += 2;
        break;
      }
    } else {
      vga_console_putchar(*p);
      count++;
    }
    p++;
  }

  va_end(args);
  return count;
}
