#ifndef IO_H
#define IO_H

#include "lib/other/types.h"

// OCW2 - Operation Control Word 2
#define PIC_EOI 0x20
// master
#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
// slave
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

static inline void outb(uint16_t port, uint8_t val){
    asm volatile("outb %0, %w1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

#endif