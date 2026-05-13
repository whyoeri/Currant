section .multiboot
    align 4
    dd 0x1BADB002
    dd 0x03
    dd -(0x1BADB002 + 0x03)
section .stack
    align 4096
    global stack_top
stack_bottom:
    resb 16384 ; 16 kb
stack_top:
section .text
    global _start
    extern kmain
_start:
    cli
    mov esp, stack_top
    push eax 
    push ebx 
    call kmain
.loop:
    hlt
    jmp .loop