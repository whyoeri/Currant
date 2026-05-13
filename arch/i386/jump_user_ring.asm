[global jump_user_ring]
jump_user_ring:
    mov eax, [esp + 4] ;EIP for Ring 3
    mov ecx, [esp + 8] ;ESP for Ring 3

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23 ; SS
    push ecx ; ESP

    pushfd
    pop edx
    or edx, 0x200 ; turn on interrupts
    push edx

    push 0x1B
    push eax

    iret