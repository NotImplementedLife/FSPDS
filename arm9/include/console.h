#pragma once

#include <nds.h>
#include <stdio.h>

extern PrintConsole consoleBG,consoleFG;

inline void c_cls()
{
    iprintf("\x1b[2J");
}

inline void c_goto(int row, int column)
{
    printf("\033[%d;%dH", row, column);
}

void c_writeFrame();

void c_drawBorder(u8 l1,u8 c1,u8 l2,u8 c2);

void initConsole();

// screen to display error messages
bool c_displayError(const char* message,bool isfatal);

void c_loadingBox();

