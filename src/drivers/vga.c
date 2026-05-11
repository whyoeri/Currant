#include "vga.h"

#define START_ADDRESS_VGA 0xB8000
#define SIZE_ROW 80
#define BYTES_ONE_SYMBOL 2

static volatile uint8_t* const vga_buffer = (uint8_t*)START_ADDRESS_VGA;

void put_vga(const char c, uint8_t color, int x, int y){
    uint32_t offset = (y * SIZE_ROW + x) * BYTES_ONE_SYMBOL;
    vga_buffer[offset] = c;
    vga_buffer[offset + 1] = color;
}