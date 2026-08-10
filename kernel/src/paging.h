#pragma once

#include <stdint.h>

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE (0x1 << 1)
#define PAGE_USER (0x1 << 2)
#define PAGE_WRITE_THROUGH (0x1 << 3)
#define PAGE_DISABLE_CACHE (0x1 << 4)
#define PAGE_ACCESSED (0x1 << 5)
#define PAGE_DIRTY (0x1 << 6)
#define PAGE_NXE (1ULL << 63)

#define GET_PAGE_OFFSET(addr) ((addr) & 0xFFF)
#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL

#define PAGE_SIZE 0x1000

uint8_t paging_map(void *v, uintptr_t p, uint64_t flags);
void paging_free(void *v);