#include "keyboard.h"

#include "lib/data_structures/ringbuffer.h"
#include "arch/i386/io.h"

static const unsigned char kbd_us[128] = {
    [0x01] = 27,   [0x02] = '1',  [0x03] = '2',  [0x04] = '3',  [0x05] = '4',
    [0x06] = '5',  [0x07] = '6',  [0x08] = '7',  [0x09] = '8',  [0x0A] = '9',
    [0x0B] = '0',  [0x0C] = '-',  [0x0D] = '=',  [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'q',  [0x11] = 'w',  [0x12] = 'e',  [0x13] = 'r',  [0x14] = 't',
    [0x15] = 'y',  [0x16] = 'u',  [0x17] = 'i',  [0x18] = 'o',  [0x19] = 'p',
    [0x1A] = '[',  [0x1B] = ']',  [0x1C] = '\n', [0x1E] = 'a',  [0x1F] = 's',
    [0x20] = 'd',  [0x21] = 'f',  [0x22] = 'g',  [0x23] = 'h',  [0x24] = 'j',
    [0x25] = 'k',  [0x26] = 'l',  [0x27] = ';',  [0x28] = '\'', [0x29] = '`',
    [0x2B] = '\\', [0x2C] = 'z',  [0x2D] = 'x',  [0x2E] = 'c',  [0x2F] = 'v',
    [0x30] = 'b',  [0x31] = 'n',  [0x32] = 'm',  [0x33] = ',',  [0x34] = '.',
    [0x35] = '/',  [0x37] = '*',  [0x39] = ' ',  [0x4A] = '-',  [0x4E] = '+'
};

static const unsigned char kbd_us_upper[128] = {
    [0x01] = 27,   [0x02] = '!',  [0x03] = '@',  [0x04] = '#',  [0x05] = '$',
    [0x06] = '%',  [0x07] = '^',  [0x08] = '&',  [0x09] = '*',  [0x0A] = '(',
    [0x0B] = ')',  [0x0C] = '_',  [0x0D] = '+',  [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'Q',  [0x11] = 'W',  [0x12] = 'E',  [0x13] = 'R',  [0x14] = 'T',
    [0x15] = 'Y',  [0x16] = 'U',  [0x17] = 'I',  [0x18] = 'O',  [0x19] = 'P',
    [0x1A] = '{',  [0x1B] = '}',  [0x1C] = '\n', [0x1E] = 'A',  [0x1F] = 'S',
    [0x20] = 'D',  [0x21] = 'F',  [0x22] = 'G',  [0x23] = 'H',  [0x24] = 'J',
    [0x25] = 'K',  [0x26] = 'L',  [0x27] = ':',  [0x28] = '\"', [0x29] = '~',
    [0x2B] = '|',  [0x2C] = 'Z',  [0x2D] = 'X',  [0x2E] = 'C',  [0x2F] = 'V',
    [0x30] = 'B',  [0x31] = 'N',  [0x32] = 'M',  [0x33] = '<',  [0x34] = '>',
    [0x35] = '?',  [0x37] = '*',  [0x39] = ' ',  [0x4A] = '-',  [0x4E] = '+'
};

#define KBD_SHIFT_L 0x2A
#define KBD_SHIFT_R 0x36
#define KBD_CAPS 0x3A
#define KBD_LET_GO 0x80
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

typedef enum{
    MOD_NONE = 0,
    MOD_SHIFT = 1 << 0,
    MOD_CAPSLOCK = 1 << 1 
} key_modifiers_t;

static uint8_t kbd_mem[KEYBOARD_BUFFER_SIZE];
static ring_buffer_t kbd_rb;
static volatile key_modifiers_t current_key_state = MOD_NONE;

void init_keyboard(void){
    init_buffer(&kbd_rb, kbd_mem, KEYBOARD_BUFFER_SIZE);
    while(inb(KBD_STATUS_PORT) & 0x01){inb(KBD_DATA_PORT);}
}

void handler_keyboard(void){
    int scancode = read_keyboard();
    if(-1 == scancode){outb(0x20, 0x20); return;}

    if(KBD_SHIFT_L == scancode || KBD_SHIFT_R == scancode){
        current_key_state |= MOD_SHIFT;
    }
    else if(KBD_SHIFT_L + KBD_LET_GO == scancode || KBD_SHIFT_R + KBD_LET_GO == scancode){
        current_key_state &= ~MOD_SHIFT;
    }
    else if(KBD_CAPS == scancode){
        current_key_state ^= MOD_CAPSLOCK;
    }
    else if(scancode < KBD_LET_GO){
        pushrb(&kbd_rb, (uint8_t)scancode);
    }
    
    outb(0x20, 0x20);
}

int read_keyboard(void){
    uint8_t status = inb(KBD_STATUS_PORT);
    if(!(status & 0x01)){return -1;}
    return (int)inb(KBD_DATA_PORT);
}

char get_char_keyboard(void){
    int scancode = get_scancode_keyboard();
    if(-1 == scancode){return 0;}
    if(scancode >= 128){return 0;}

    unsigned char lower = kbd_us[scancode];
    unsigned char upper = kbd_us_upper[scancode];

    bool shift = (current_key_state & MOD_SHIFT);
    bool caps = (current_key_state & MOD_CAPSLOCK);

    if(lower >= 'a' && lower <= 'z'){
        return (shift ^ caps) ? upper : lower;
    }
        
    return shift ? upper : lower;
}

int get_scancode_keyboard(void){
    return poprb(&kbd_rb);
}