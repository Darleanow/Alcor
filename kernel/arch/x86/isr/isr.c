#include "isr.h"
#include <drivers/vga_console/vga_console.h>

void isr_install(void (*set_gate)(int, void (*)(void), uint16_t, uint8_t)) {
  extern void isr0(), isr1(), isr2(), isr3(), isr4(), isr5(), isr6(), isr7(),
      isr8(), isr9(), isr10(), isr11(), isr12(), isr13(), isr14(), isr15(),
      isr16(), isr17(), isr18(), isr19(), isr20(), isr21(), isr22(), isr23(),
      isr24(), isr25(), isr26(), isr27(), isr28(), isr29(), isr30(), isr31();

  void (*handlers[32])() = {isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,
                            isr7,  isr8,  isr9,  isr10, isr11, isr12, isr13,
                            isr14, isr15, isr16, isr17, isr18, isr19, isr20,
                            isr21, isr22, isr23, isr24, isr25, isr26, isr27,
                            isr28, isr29, isr30, isr31};

  for (int i = 0; i < 32; i++)
    set_gate(i, handlers[i], 0x08, 0x8E);
}

void isr_handler(struct regs r) {
  (void)r;
  vga_console_write("EXCEPTION\n");
  for (;;)
    __asm__ volatile("cli; hlt");
}
