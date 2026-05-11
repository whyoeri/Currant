#ifndef IDT_H
#define IDT_H

#include "lib/other/types.h"

typedef struct __attribute__((packed)){
    uint16_t base_lo; // low 16 bits address handler
    uint16_t sel_code; // kernel code segment
    uint8_t zero_bit; // reserved 
    uint8_t type_attr; // access flags
    uint16_t base_hi; // high 16 bits address handler
} idt_entry_t;

typedef struct __attribute__((packed)){
    uint16_t limit; // size idt (maximum 65535 bytes)
    uint32_t base; // linear address 
} idt_ptr_t;

void remap_pic(void);
void idt_set_gate(int num, uint32_t base, uint16_t sel_code, uint8_t type_attr);
void init_idt(void);

#endif