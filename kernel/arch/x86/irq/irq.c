#include "irq.h"
#include <arch/x86/pic/pic.h>
#include <core/timer/timer.h>
#include <drivers/keyboard/keyboard.h>

void irq_install(void (*set_gate)(int, void (*)(void), uint16_t, uint8_t)) {
  extern void isr32(), isr33(), isr34(), isr35(), isr36(), isr37(), isr38(),
      isr39();
  extern void isr40(), isr41(), isr42(), isr43(), isr44(), isr45(), isr46(),
      isr47();

  void (*handlers[16])() = {isr32, isr33, isr34, isr35, isr36, isr37,
                            isr38, isr39, isr40, isr41, isr42, isr43,
                            isr44, isr45, isr46, isr47};

  for (int i = 0; i < 16; i++)
    set_gate(32 + i, handlers[i], 0x08, 0x8E);
}

void irq_handler(struct regs r) {
  if (r.int_no == 32)
    timer_tick();
  else if (r.int_no == 33)
    keyboard_irq();

  pic_send_eoi(r.int_no - 32);
}
