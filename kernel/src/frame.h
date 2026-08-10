#pragma once

#include <stdint.h>

uintptr_t frame_alloc();
void frame_free(uintptr_t frame);
int frame_init();