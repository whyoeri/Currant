#ifndef GDT_H
#define GDT_H

#include "lib/other/types.h"

typedef struct __attribute__((packed)){
    uint16_t limit_low; // lower 16 bits segment limit
    uint16_t base_low; // lower 16 bits base address
    uint8_t base_middle; // next 8 bits base address (bits 16-23)
    uint8_t access; // access flags
    uint8_t granularity; // limit (bits 16-19) + flags (G, D/B, L, AVL)
    uint8_t base_high; // last 8 bits base address (bits 24-31)
} gdt_entry_t;

typedef struct __attribute__((packed)){
    uint16_t limit; // size gdt (in bytes)
    uint32_t base; // linear address
} gdt_ptr_t;

typedef struct __attribute__((packed)){
    uint32_t prev_tss; // previous TSS
    uint32_t esp0; // kernel stack 
    uint32_t ss0; // kernel stack segment 
    uint32_t esp1; // stack ring 1 (not used)
    uint32_t ss1; // stack segment ring 1 (not used)
    uint32_t esp2; // stack ring 2 (not used)
    uint32_t ss2; // stack segment ring 2 (not used)
    uint32_t cr3; // Page Directory Base Register (PDBR)
    uint32_t eip; // instruction pointer
    uint32_t eflags; // processor flags
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi; // 32 bit registers
    uint32_t es, cs, ss, ds, fs, gs; // segment selectors
    uint32_t ldt; // local descriptor table (not used)
    uint16_t trap; // debug trap flag
    uint16_t iomap_base; // I/O permission bitmap offset
} tss_entry_t;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void init_gdt(void);

#endif