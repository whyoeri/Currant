#include "kernel_panic.h"

#include "lib/statistics/statistics.h"
#include "src/terminal/terminal.h"
#include "src/drivers/vga.h"
#include "lib/other/types.h"

static volatile bool in_handler = false;

void kernel_panic(registers_t* rg){
    asm volatile("cli");

    if(in_handler){__asm__ volatile("hlt");}
    in_handler = true;

    set_theme_terminal(BLACK_COLOR, RED_COLOR);

    clear_screen();
    print_str("KERNEL PANIC!\n");
    print_str("A fatal exception has occurred\n");
    print_str("System halted to prevent damage\n");
    print_str("Please reboot!\n");
    print_str("Information:\n");

    if(rg->int_no < 34){
        output_error(rg, "EXCEPTION: ");
    }
    else{
        output_error(rg, "UNKNOWN EXCEPTION!");
    }

    output_registers_info(rg);
    output_flags(rg);
    output_context(rg);
}