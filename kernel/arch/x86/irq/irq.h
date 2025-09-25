#pragma once
#include <arch/x86/isr/isr.h>

void irq_install(void (*set_gate)(int, void (*)(void), uint16_t, uint8_t));
void irq_handler(struct regs);
