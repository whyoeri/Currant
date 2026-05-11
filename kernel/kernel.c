#include "arch/i386/multiboot.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "src/memory/paging.h"
#include "src/memory/pmm.h"
#include "src/drivers/keyboard.h"
#include "src/drivers/timer.h"
#include "src/scheduler/scheduler.h"
#include "src/terminal/terminal.h"
#include "lib/other/types.h"

#define GRUB_MAGIC 0x2BADB002 

void kmain(multiboot_info_t* mbi, uint32_t signature_grub){
    if(GRUB_MAGIC != signature_grub){__asm__ volatile("hlt"); return;}
    init_gdt();
    remap_pic();
    init_idt();
    init_pmm(mbi);
    init_paging();
    init_timer(DEFAULT_TIMER_FREQUENCY);
    init_schedule();
    init_terminal();
    init_keyboard();
    __asm__ volatile("sti");

    while(true) {
        input_processing_terminal();
        __asm__ volatile("hlt");
    }
}