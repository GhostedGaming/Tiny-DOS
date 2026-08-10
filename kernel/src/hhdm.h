#pragma once

#include <stdint.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

static inline void *to_virt(uint64_t phys) {
    return (void *)(phys + hhdm_request.response->offset);
}

static inline uint64_t to_phys(void *virt) {
    return (uint64_t)(virt - hhdm_request.response->offset);
}