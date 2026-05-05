#ifndef IDT_H
#define IDT_H

#include "lib/other/types.h"

typedef struct __attribute__((packed)){
    uint16_t base_lo;
    uint16_t sel_code;
    uint8_t zero_bit;
    uint8_t type_attr;
    uint16_t base_hi;
} idt_entry_t;

typedef struct __attribute__((packed)){
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

void remap_pic(void);
void idt_set_gate(int num, uint32_t base, uint16_t sel_code, uint8_t type_attr);
void init_idt(void);

#endif