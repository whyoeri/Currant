#include "vga.h"

#define START_ADDRESS_VGA 0xB8000

static volatile uint8_t* const vga_buffer = (uint8_t*)START_ADDRESS_VGA;

void put_vga(const char c, uint8_t color, int x, int y){
    uint32_t offset = (y * 80 + x) * 2;
    vga_buffer[offset] = c;
    vga_buffer[offset + 1] = color;
}