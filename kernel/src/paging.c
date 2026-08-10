#include "paging.h"
#include "hhdm.h"
#include "frame.h"
#include "memory.h"
#include <stdint.h>
#include <stddef.h>

uint64_t *k_space = NULL;

static uint64_t get_pml(void *v, uint8_t i) {
    switch(i) {
        case 1:
            return ((uint64_t)v >> 12) & 0x1FF;
        case 2:
            return ((uint64_t)v >> 21) & 0x1FF;
        case 3:
            return ((uint64_t)v >> 30) & 0x1FF;
        case 4:
            return ((uint64_t)v >> 39) & 0x1FF;
    }
    return 0;
}

static inline void invalidate_page(void *addr, uint64_t len) {
    for (uint64_t i = 0; i < len; i += 0x1000) {
        asm volatile("invlpg (%0)" : : "r"(addr + i) : "memory");
    }
}

uint8_t paging_map(void *v, uintptr_t p, uint64_t flags) {
    if (!k_space) {
        return 1;
    }

    uint64_t i4 = get_pml(v, 4);
    if (!(k_space[i4] & PAGE_PRESENT)) {
        uintptr_t new_table = frame_alloc();
        if (!new_table) return 1;
        memset(to_virt(new_table), 0, PAGE_SIZE);
        k_space[i4] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t *pml3 = (uint64_t *)to_virt(k_space[i4] & PAGE_ADDR_MASK);
    uint64_t i3 = get_pml(v, 3);
    if (!(pml3[i3] & PAGE_PRESENT)) {
        uintptr_t new_table = frame_alloc();
        if (!new_table)return 1;
        memset(to_virt(new_table), 0, PAGE_SIZE);
        pml3[i3] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t *pml2 = (uint64_t *)to_virt(pml3[i3] & PAGE_ADDR_MASK);
    uint64_t i2 = get_pml(v, 2);
    if (!(pml2[i2] & PAGE_PRESENT)) {
        uintptr_t new_table = frame_alloc();
        if (!new_table) return 1;
        memset(to_virt(new_table), 0, PAGE_SIZE);
        pml2[i2] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t *pml1 = (uint64_t *)to_virt(pml2[i2] & PAGE_ADDR_MASK);
    uint64_t i1 = get_pml(v, 1);
    pml1[i1] = p | flags | PAGE_PRESENT;
    invalidate_page(v, 0x1000);
    return 0;
}

void paging_free(void *v) {
    if (!k_space) {
        return;
    }

    uint64_t i4 = get_pml(v, 4);
    if (!(k_space[i4] & PAGE_PRESENT)) return;

    uint64_t *pml3 = (uint64_t *)to_virt(k_space[i4] & PAGE_ADDR_MASK);
    uint64_t i3 = get_pml(v, 3);
    if (!(pml3[i3] & PAGE_PRESENT)) return;

    uint64_t *pml2 = (uint64_t *)to_virt(pml3[i3] & PAGE_ADDR_MASK);
    uint64_t i2 = get_pml(v, 2);
    if (!(pml2[i2] & PAGE_PRESENT)) return;

    uint64_t *pml1 = (uint64_t *)to_virt(pml2[i2] & PAGE_ADDR_MASK);
    uint64_t i1 = get_pml(v, 1);
    pml1[i1] = 0;
    invalidate_page(v, 0x1000);
    frame_free((pml1[i1] & PAGE_ADDR_MASK));
}