#ifndef COMMANDS_H
#define COMMANDS_H

void execute_command(char* command);

// output ascii kitty
void kitty(void);

// snow help information
void help(void);

// trigger kernel panic
void panic(void);

// installing the theme
void black_theme(void);
void white_theme(void);
void red_theme(void);

#endif