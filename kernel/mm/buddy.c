#include "buddy.h"
#include <lib/io/io.h>

typedef struct free_block {
  struct free_block *next;
  struct free_block *prev;
} free_block_t;

typedef struct {
  uint8_t order;
  uint8_t is_free;
} page_info_t;

static void *heap_start;
static size_t heap_size;
static int max_order;
static page_info_t *page_info;
static free_block_t *free_lists[MAX_ORDER + 1];

static void *page_to_addr(size_t page) {
  return (char *)heap_start + (page << MIN_ORDER);
}

static size_t addr_to_page(void *addr) {
  return ((char *)addr - (char *)heap_start) >> MIN_ORDER;
}

static size_t buddy_page(size_t page, int order) {
  return page ^ (1 << (order - MIN_ORDER));
}

static int size_to_order(size_t size) {
  int order = MIN_ORDER;
  size_t block_size = 1 << MIN_ORDER;

  while (block_size < size && order <= max_order) {
    order++;
    block_size <<= 1;
  }

  return (order <= max_order) ? order : -1;
}

static void add_to_free_list(size_t page, int order) {
  free_block_t *block = (free_block_t *)page_to_addr(page);

  block->next = free_lists[order];
  block->prev = NULL;

  if (free_lists[order]) {
    free_lists[order]->prev = block;
  }

  free_lists[order] = block;
  page_info[page].is_free = 1;
  page_info[page].order = order;
}

static void remove_from_free_list(size_t page, int order) {
  free_block_t *block = (free_block_t *)page_to_addr(page);

  if (block->prev) {
    block->prev->next = block->next;
  } else {
    free_lists[order] = block->next;
  }

  if (block->next) {
    block->next->prev = block->prev;
  }

  page_info[page].is_free = 0;
}

void buddy_init(void *start, size_t size) {
  heap_start = start;
  heap_size = size;

  max_order = MIN_ORDER;
  while ((1UL << max_order) < heap_size && max_order < MAX_ORDER) {
    max_order++;
  }
  max_order--;

  size_t num_pages = heap_size >> MIN_ORDER;
  page_info = (page_info_t *)((char *)start + heap_size);

  for (int i = 0; i <= max_order; i++) {
    free_lists[i] = NULL;
  }

  for (size_t i = 0; i < num_pages; i++) {
    page_info[i].order = 0;
    page_info[i].is_free = 0;
  }

  size_t current_page = 0;
  size_t remaining_pages = num_pages;

  while (remaining_pages > 0) {
    int order = max_order;
    size_t pages_needed = 1 << (order - MIN_ORDER);

    while (pages_needed > remaining_pages ||
           (current_page & (pages_needed - 1)) != 0) {
      order--;
      pages_needed = 1 << (order - MIN_ORDER);
    }

    add_to_free_list(current_page, order);
    current_page += pages_needed;
    remaining_pages -= pages_needed;
  }
}

void *kmalloc(size_t size) {
  if (size == 0)
    return NULL;

  int order = size_to_order(size);
  if (order == -1)
    return NULL;

  int current_order = order;
  while (current_order <= max_order && !free_lists[current_order]) {
    current_order++;
  }

  if (current_order > max_order) {
    return NULL;
  }

  free_block_t *block = free_lists[current_order];
  size_t page = addr_to_page(block);
  remove_from_free_list(page, current_order);

  while (current_order > order) {
    current_order--;
    size_t buddy = page + (1 << (current_order - MIN_ORDER));
    add_to_free_list(buddy, current_order);
  }

  page_info[page].order = order;
  page_info[page].is_free = 0;

  return page_to_addr(page);
}

void kfree(void *ptr) {
  if (!ptr)
    return;

  size_t page = addr_to_page(ptr);
  int order = page_info[page].order;

  while (order < max_order) {
    size_t buddy = buddy_page(page, order);

    if (buddy >= (heap_size >> MIN_ORDER) || !page_info[buddy].is_free ||
        page_info[buddy].order != order) {
      break;
    }

    remove_from_free_list(buddy, order);

    if (buddy < page) {
      page = buddy;
    }
    order++;
  }

  add_to_free_list(page, order);
}

void buddy_stats(void) {
  printf("Buddy allocator statistics:\n");

  for (int order = MIN_ORDER; order <= max_order; order++) {
    int count = 0;
    free_block_t *block = free_lists[order];

    while (block) {
      count++;
      block = block->next;
    }

    if (count > 0) {
      printf("  Order %d (%d bytes): %d free blocks\n", order, 1 << order,
             count);
    }
  }
}
