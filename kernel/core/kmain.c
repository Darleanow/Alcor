#include <arch/x86/idt/idt.h>
#include <arch/x86/irq/irq.h>
#include <arch/x86/pic/pic.h>
#include <core/timer/timer.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/vga_console/vga_console.h>
#include <shell/shell.h>

void kernel_main(void) {
  vga_console_init();
  vga_console_write("Alcor kernel starting...\n");

  // gdt_init();
  idt_init();
  pic_remap(0x20, 0x28);
  timer_init(); // On IRQ0

  shell_init();

  asm volatile("sti");

  for (;;)
    asm volatile("hlt");
}
