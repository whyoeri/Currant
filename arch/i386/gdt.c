#include "gdt.h"

#define MASK_GIVE_LOW_16_BYTE 0xFFFF
#define MASK_GIVE_1_BYTE 0xFF
#define MASK_GIVE_LOW_4_BIT 0x0F
#define MASK_GIVE_HIGH_4_BIT 0xF0
#define OFFSET_MIDDLE_BYTE 16
#define OFFSET_HIGH_BYTE 24
#define ACCESS_CODE_RING0 0x9A
#define ACCESS_DATA_RING0 0x92
#define ACCESS_CODE_RING3 0xFA
#define ACCESS_DATA_RING3 0xF2
#define LIMIT_MEMORY 0xFFFFFFFF
#define GRAN 0xCF

extern void gdt_flush(uint32_t);
static gdt_entry_t gdt[5];
static gdt_ptr_t gptr;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    gdt[num].limit_low = (limit & MASK_GIVE_LOW_16_BYTE);
    gdt[num].base_low = (base & MASK_GIVE_LOW_16_BYTE);
    gdt[num].base_middle = (base >> OFFSET_MIDDLE_BYTE) & MASK_GIVE_1_BYTE;
    gdt[num].access = access;
    gdt[num].granularity = ((limit >> OFFSET_MIDDLE_BYTE) & MASK_GIVE_LOW_4_BIT) | (gran & MASK_GIVE_HIGH_4_BIT); 
    gdt[num].base_high = (base >> OFFSET_HIGH_BYTE) & MASK_GIVE_1_BYTE;
}

void init_gdt(void){
    gptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gptr.base = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0); // NULL
    gdt_set_gate(1, 0, LIMIT_MEMORY, ACCESS_CODE_RING0, GRAN); // Kernel code
    gdt_set_gate(2, 0, LIMIT_MEMORY, ACCESS_DATA_RING0, GRAN); // Kernel data
    gdt_set_gate(3, 0, LIMIT_MEMORY, ACCESS_CODE_RING3, GRAN); // User code
    gdt_set_gate(4, 0, LIMIT_MEMORY, ACCESS_DATA_RING3, GRAN); // User data

    gdt_flush((uint32_t)&gptr);
}