#pragma once
#include <stdint.h>

struct regs {
  uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

void isr_install(void (*set_gate)(int, void (*)(void), uint16_t, uint8_t));
void isr_handler(struct regs);
