#include "commands.h"

#include "arch/i386/io.h"
#include "src/drivers/vga.h"
#include "src/terminal/terminal.h"
#include "lib/string/string.h"
#include "lib/other/types.h"

typedef struct{
    const char* name;
    void(*func)(void);
} terminalcommand_t;

terminalcommand_t tcommand[] = {
    {"kitty", kitty},
    {"help", help},
    {"panic", panic},
    {"clear", clear_screen},
    {"black", black_theme},
    {"white", white_theme},
    {"red", red_theme},
};

#define COMMAND_COUNT (sizeof(tcommand) / sizeof(terminalcommand_t))

void execute_command(char* command){
    if(NULL == command || '\0' == command[0]){return;}
    
    while(' ' == *command){command++;}

    char* name = command;
    int i = 0;
    while(' ' != command[i] && '\0' != command[i]){i++;}
    
    char separator = command[i];
    command[i] = '\0';

    if('\0' == name[0]){command[i] = separator; return;}

    for(int j = 0; j < COMMAND_COUNT; j++){
        if(0 == strcom(name, tcommand[j].name)){
            tcommand[j].func();
            command[i] = separator; 
            return;
        }
    }

    print_str(name);
    print_str(": command not found\n");

    command[i] = separator;
}

// output ascii kitty
void kitty(void){
    print_str("\n"
            "       |\\      _,,,---,,_\n"
            " Zzz   /,`.-'`'    -.  ;-;;,_\n"
            "      |,4-  ) )-,_. ,\\ (  `'-'\n"
            "     '---''(_/--'  `-'\\_)\n\n");
}

// snow help information
void help(void){
    print_str("Commands:\n");
    print_str("black  white  red  green\n");
    print_str("blue  panic  help  kitty\n");
    print_str("Creator: whyoeri\n");
    print_str("More information: https://github.com/whyoeri/Currant \n");
}

// trigger kernel panic
void panic(void){
    __asm__ volatile("ud2");
}

// installing the theme
void black_theme(void){
    set_theme_terminal(WHITE_COLOR, BLACK_COLOR);
    clear_screen();
}

void white_theme(void){
    set_theme_terminal(BLACK_COLOR, WHITE_COLOR);
    clear_screen();
}

void red_theme(void){
    set_theme_terminal(BLACK_COLOR, RED_COLOR);
    clear_screen();
}