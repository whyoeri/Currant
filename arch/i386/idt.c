#include "idt.h"

#include "arch/i386/isr.h"
#include "arch/i386/io.h"

#define ICW1_INIT 0x11
#define ICW4_8086 0x01
#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28
#define PIC_MASTER_SLAVE_BIT 0x04
#define PIC_SLAVE_ID 0x02
#define PIC_MASK_ALL 0xFF
#define PIC_MASK_TIMER_KBD 0xF8
#define SELECTOR_KERNEL_CODE 0x08
#define ACCESS_BYTE_KERNEL 0x8E

extern void idt_load(uint32_t);
static idt_entry_t idt[256];
static idt_ptr_t iptr;

void remap_pic(void){
    outb(PIC1_COMMAND_PORT, ICW1_INIT);
    outb(PIC2_COMMAND_PORT, ICW1_INIT);
    outb(PIC1_DATA_PORT, PIC1_OFFSET);
    outb(PIC2_DATA_PORT, PIC2_OFFSET);
    outb(PIC1_DATA_PORT, PIC_MASTER_SLAVE_BIT);
    outb(PIC2_DATA_PORT, PIC_SLAVE_ID);
    outb(PIC1_DATA_PORT, ICW4_8086);
    outb(PIC2_DATA_PORT, ICW4_8086);
    outb(PIC1_DATA_PORT, PIC_MASK_TIMER_KBD);
    outb(PIC2_DATA_PORT, PIC_MASK_ALL);
}

void idt_set_gate(int num, uint32_t base, uint16_t sel_code, uint8_t type_attr){
    idt[num].base_lo = base & 0xFFFF;
    idt[num].sel_code = sel_code;
    idt[num].zero_bit = 0;
    idt[num].type_attr = type_attr;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
}

void init_idt(void){
    iptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    iptr.base = (uint32_t)&idt;

    for(uint16_t i = 0; i < 256; i++){
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(0, (uint32_t)isr0, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(1, (uint32_t)isr1, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(2, (uint32_t)isr2, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(3, (uint32_t)isr3, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(4, (uint32_t)isr4, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(5, (uint32_t)isr5, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(6, (uint32_t)isr6, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(7, (uint32_t)isr7, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(8, (uint32_t)isr8, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(9, (uint32_t)isr9, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(10, (uint32_t)isr10, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(11, (uint32_t)isr11, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(12, (uint32_t)isr12, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(13, (uint32_t)isr13, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(14, (uint32_t)isr14, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(15, (uint32_t)isr15, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(16, (uint32_t)isr16, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(17, (uint32_t)isr17, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(18, (uint32_t)isr18, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(19, (uint32_t)isr19, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(20, (uint32_t)isr20, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(21, (uint32_t)isr21, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(22, (uint32_t)isr22, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(23, (uint32_t)isr23, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(24, (uint32_t)isr24, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(25, (uint32_t)isr25, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(26, (uint32_t)isr26, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(27, (uint32_t)isr27, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(28, (uint32_t)isr28, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(29, (uint32_t)isr29, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(30, (uint32_t)isr30, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(31, (uint32_t)isr31, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(32, (uint32_t)isr32, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);
    idt_set_gate(33, (uint32_t)isr33, SELECTOR_KERNEL_CODE, ACCESS_BYTE_KERNEL);

    idt_load((uint32_t)&iptr);
}