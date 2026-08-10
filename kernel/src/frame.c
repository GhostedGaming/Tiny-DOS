#include "frame.h"
#include "hhdm.h"
#include <stdint.h>
#include <stddef.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

static uintptr_t page_list;

uintptr_t frame_alloc() {
    if (!page_list) {
        return 0;
    }

    uint64_t *virt = (uint64_t *)to_virt(page_list);
    uintptr_t frame = page_list;
    page_list = *virt;

    return frame;
}

void frame_free(uintptr_t frame) {
    if (!frame) {
        return;
    }
    uint64_t *virt = (uint64_t *)to_virt(frame);

    *virt = page_list;
    page_list = frame;
}

int frame_init() {
    struct limine_memmap_response *memmap = memmap_request.response;
    if (!memmap) {
        return -1;
    }

    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uintptr_t start = entry->base;
            uintptr_t end = start + entry->length;

            for (uintptr_t phys = start; phys < end; phys += 4096) {
                frame_free(phys);
            }
        }
    }

    return 0;
}