#include "gdt.h"

#include "lib/string/string.h"

#define GDT_LIM_MSK 0xFFFF
#define GDT_BYTE_MSK 0xFF
#define NIBBLE_MSK_LOW 0x0F
#define NIBBLE_MSK_HIGH 0xF0
#define OFF_MID 16
#define OFF_HIGH 24
#define K_CODE 0x9A
#define K_DATA 0x92
#define K_DATA_SEGMENT 0x10
#define U_CODE 0xFA
#define U_DATA 0xF2
#define GDT_ENTRIES_COUNT 6
#define GDT_LIMIT_ADJUST 1
#define LIMIT_MEMORY 0xFFFFFFFF
#define TSS_SETTINGS 0x89
#define TSS_SELECTOR 0x28
#define GRAN 0xCF
#define GRAN_OFF 0x00

extern void gdt_flush(uint32_t);
static gdt_entry_t gdt[GDT_ENTRIES_COUNT];
static gdt_ptr_t gptr;
static tss_entry_t tss_entry;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    gdt[num].limit_low = (limit & GDT_LIM_MSK);
    gdt[num].base_low = (base & GDT_LIM_MSK);
    gdt[num].base_middle = (base >> OFF_MID) & GDT_BYTE_MSK;
    gdt[num].access = access;
    gdt[num].granularity = ((limit >> OFF_MID) & NIBBLE_MSK_LOW) | (gran & NIBBLE_MSK_HIGH); 
    gdt[num].base_high = (base >> OFF_HIGH) & GDT_BYTE_MSK;
}

void init_gdt(void){
    gptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES_COUNT) - GDT_LIMIT_ADJUST;
    gptr.base = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0); // NULL
    gdt_set_gate(1, 0, LIMIT_MEMORY, K_CODE, GRAN); // kernel code
    gdt_set_gate(2, 0, LIMIT_MEMORY, K_DATA, GRAN); // kernel data
    gdt_set_gate(3, 0, LIMIT_MEMORY, U_CODE, GRAN); // user code
    gdt_set_gate(4, 0, LIMIT_MEMORY, U_DATA, GRAN); // user data

    memset(&tss_entry, 0, sizeof(tss_entry_t));
    tss_entry.ss0 = K_DATA_SEGMENT;
    tss_entry.iomap_base = sizeof(tss_entry_t);

    gdt_set_gate(5, (uint32_t)&tss_entry, sizeof(tss_entry_t) - 1, TSS_SETTINGS, GRAN_OFF); // Task State Segment (TSS)

    gdt_flush((uint32_t)&gptr);
    __asm__ volatile("ltr %%ax" : : "a"(TSS_SELECTOR));
}