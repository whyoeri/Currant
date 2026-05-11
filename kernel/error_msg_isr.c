#include "error_msg_isr.h"

const char* error_messages_isr[] = {
    "Divide by zero", // 0
    "Debug Exception", // 1
    "Non Maskable Interrupt", // 2
    "Breakpoint", // 3
    "Into Detected Overflow", // 4
    "BOUND Range Exceeded", // 5
    "Invalid Opcode (Undefined Opcode)", // 6
    "Device Not Available (No Math Coprocessor)", // 7
    "Double Fault", // 8
    "Coprocessor Segment Overrun", // 9
    "Invalid TSS", // 10
    "Segment Not Present", // 11
    "Stack-Segment Fault", // 12
    "General Protection Fault", // 13
    "Page Fault", // 14
    "Unknown Interrupt", // 15
    "Coprocessor Fault", // 16
    "Alignment Check", // 17
    "Machine Check", // 18
    "SIMD Floating-Point Exception", // 19
    "Virtualization Exception", // 20
    "Control Protection Exception", // 21
    "Reserved", // 22
    "Reserved", // 23
    "Reserved", // 24
    "Reserved", // 25
    "Reserved", // 26
    "Reserved", // 27
    "Reserved", // 28
    "Reserved", // 29
    "Reserved", // 30
    "Reserved", // 31
    "System timer", // 32
    "Keyboard Controller", // 33
};
