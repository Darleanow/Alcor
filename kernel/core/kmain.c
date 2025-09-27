#include <arch/x86/idt/idt.h>
#include <arch/x86/irq/irq.h>
#include <arch/x86/isr/isr.h>
#include <arch/x86/pic/pic.h>
#include <core/timer/timer.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/vga_console/vga_console.h>
#include <lib/io/io.h>
#include <mm/buddy.h>
#include <shell/shell.h>

typedef struct {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t acpi_extended;
} __attribute__((packed)) e820_entry_t;

#define E820_RAM 1
#define E820_RESERVED 2
#define E820_ACPI 3
#define E820_NVS 4
#define E820_UNUSABLE 5

void kernel_main(uint32_t *memory_map, uint32_t entry_count) {
  vga_console_init();
  printf("Alcor kernel starting...\n");

  // Process memory map
  printf("Memory map (%d entries):\n", entry_count);
  e820_entry_t *mmap = (e820_entry_t *)memory_map;

  size_t total_ram = 0;
  size_t usable_regions = 0;

  for (uint32_t i = 0; i < entry_count; i++) {
    if (mmap[i].type == E820_RAM) {
      uint32_t base_mb = (uint32_t)(mmap[i].base / (1024 * 1024));
      uint32_t size_mb = (uint32_t)(mmap[i].length / (1024 * 1024));

      printf("  RAM: %d MB at %d MB\n", size_mb, base_mb);
      total_ram += mmap[i].length;
      usable_regions++;
    }
  }

  printf("Total RAM: %d MB (%d usable regions)\n",
         (uint32_t)(total_ram / (1024 * 1024)), usable_regions);

  // Initialize buddy allocator with largest RAM region
  void *best_start = NULL;
  size_t best_size = 0;

  for (uint32_t i = 0; i < entry_count; i++) {
    if (mmap[i].type == E820_RAM && mmap[i].length > best_size) {
      best_start = (void *)(uintptr_t)mmap[i].base;
      best_size = mmap[i].length;
    }
  }

  if (best_start && best_size > 1024 * 1024) {
    buddy_init(best_start, best_size);
  }

  // Initialize interrupt handling
  idt_init();
  pic_remap(0x20, 0x28);
  timer_init();

  // Initialize shell
  shell_init();

  // Enable interrupts and enter main loop
  asm volatile("sti");

  for (;;) {
    asm volatile("hlt");
  }
}
