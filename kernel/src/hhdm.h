#pragma once

#include <stdint.h>

extern uint64_t hhdm_offset;

static inline void *to_virt(uint64_t phys) {
    return (void *)(phys + hhdm_offset);
}

static inline uint64_t to_phys(void *virt) {
    return (uint64_t)((uint8_t *)virt - hhdm_offset);
}