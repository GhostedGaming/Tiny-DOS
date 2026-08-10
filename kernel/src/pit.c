#include "pit.h"
#include "portio.h"
#include <stdint.h>

#define PIT_CHANNEL0_DATA   0x40
#define PIT_CHANNEL1_DATA   0x41
#define PIT_CHANNEL2_DATA   0x42
#define PIT_COMMAND         0x43

#define PIT_BASE_FREQUENCY  1193182u

#define PIT_CH0_SELECT      0x00
#define PIT_ACCESS_LOHI     0x30
#define PIT_MODE_SQUARE     0x06
#define PIT_BINARY          0x00

static volatile uint64_t pit_ticks = 0;
static uint32_t pit_frequency = 0;

void pit_handler() {
    pit_ticks++;
}

uint64_t pit_get_ticks() {
    return pit_ticks;
}

void pit_init() {
    uint32_t divisor = PIT_BASE_FREQUENCY / 1000;

    if (divisor > 0xFFFF) {
        divisor = 0xFFFF;
    }

    pit_frequency = PIT_BASE_FREQUENCY / divisor;
    pit_ticks = 0;

    uint8_t command = PIT_CH0_SELECT | PIT_ACCESS_LOHI | PIT_MODE_SQUARE | PIT_BINARY;
    outb(PIT_COMMAND, command);

    outb(PIT_CHANNEL0_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t pit_get_frequency() {
    return pit_frequency;
}