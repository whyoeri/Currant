#include "tss.h"

#include "arch/i386/gdt.h"

static tss_entry_t tss_entry;

void tss_set_stack(uint32_t stack_ptr){
    tss_entry.esp0 = stack_ptr;
}