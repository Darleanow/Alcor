#pragma once
#include <stddef.h>
#include <stdint.h>

#define MIN_ORDER 4  // 16 bytes minimum allocation
#define MAX_ORDER 25 // Will be calculated dynamically

void buddy_init(void *start, size_t size);

void *kmalloc(size_t size);

void kfree(void *ptr);

// Debug/stats
void buddy_stats(void);
