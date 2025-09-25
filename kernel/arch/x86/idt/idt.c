#include "idt.h"
#include <arch/x86/irq/irq.h>
#include <arch/x86/isr/isr.h>
#include <arch/x86/pic/pic.h>
#include <arch/x86/portio.h>

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

static void idt_set_gate(int n, void (*handler)(void), uint16_t selector,
                         uint8_t type_attr) {
  uintptr_t addr = (uintptr_t)handler;
  idt[n].offset_low = addr & 0xFFFF;
  idt[n].selector = selector;
  idt[n].zero = 0;
  idt[n].type_attr = type_attr;
  idt[n].offset_high = (addr >> 16) & 0xFFFF;
}

void idt_init(void) {
  idtp.limit = sizeof(idt) - 1;
  idtp.base = (uint32_t)&idt;

  for (int i = 0; i < IDT_SIZE; i++)
    idt[i] = (struct idt_entry){0};

  isr_install(idt_set_gate);
  irq_install(idt_set_gate);

  pic_remap(0x20, 0x28);

  __asm__ volatile("lidt %0" : : "m"(idtp));
  __asm__ volatile("sti");
}
