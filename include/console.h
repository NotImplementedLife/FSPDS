#ifndef FSPDS_CONSOLE_H_INCLUDED
#define FSPDS_CONSOLE_H_INCLUDED

#include <nds.h>
#include <stdio.h>

static PrintConsole consoleBG,consoleFG;

static inline void c_cls()
{
    iprintf("\x1b[2J");
}

static inline void c_goto(int row, int column)
{
    printf("\033[%d;%dH", row, column);
}

void c_writeFrame()
{
    iprintf("\x1b[33m");
    c_goto(0,0);
    iprintf("\024");
    for(int i=1;i<31;i++) iprintf("\004");
    iprintf("\022");
    for(int i=1;i<23;i++)
    {
        c_goto(i,0); iprintf("\003");
        c_goto(i,31); iprintf("\003");
    }
    c_goto(23,0);
    iprintf("\025");
    for(int i=1;i<31;i++) iprintf("\004");
    iprintf("\023");
    c_goto(23,0);
    //for(i=32;i--;) iprintf("\x02");
}

static inline void initConsole()
{
    videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(&consoleBG, 1, BgType_Text4bpp, BgSize_T_256x256, 7, 0, false, true);
	consoleInit(&consoleFG, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

    // overwrite some character fonts
    // thanks: https://github.com/DS-Homebrew/GodMode9i/blob/master/arm9/source/main.cpp
	dmaFillWords(0xFFFFFFFF, (void*)0x6200040, 32);
    dmaFillWords(0x00F00F00, (void*)0x6200060, 32);

    dmaFillWords(0x00000000, (void*)0x6200080, 8);
    dmaFillWords(0xFFFFFFFF, (void*)0x6200088, 4);
    dmaFillWords(0x00000000, (void*)0x620008C, 8);
    dmaFillWords(0xFFFFFFFF, (void*)0x6200094, 4);
    dmaFillWords(0x00000000, (void*)0x6200098, 8);

    dmaFillWords(0x00000000, (void*)0x6200240, 8);
    dmaFillWords(0x000FFFFF, (void*)0x6200248, 4);
    dmaFillWords(0x00F00000, (void*)0x620024C, 8);
    dmaFillWords(0x00F00FFF, (void*)0x6200254, 4);
    dmaFillWords(0x00F00F00, (void*)0x6200258, 8);

    dmaFillWords(0x00F00F00, (void*)0x6200260, 8);
    dmaFillWords(0x00F00FFF, (void*)0x6200268, 4);
    dmaFillWords(0x00F00000, (void*)0x620026C, 8);
    dmaFillWords(0x000FFFFF, (void*)0x6200274, 4);
    dmaFillWords(0x00000000, (void*)0x6200278, 8);

    dmaFillWords(0x00000000, (void*)0x6200280, 8);
    dmaFillWords(0xFFFFF000, (void*)0x6200288, 4);
    dmaFillWords(0x00000F00, (void*)0x620028C, 8);
    dmaFillWords(0xFFF00F00, (void*)0x6200294, 4);
    dmaFillWords(0x00F00F00, (void*)0x6200298, 8);

    dmaFillWords(0x00F00F00, (void*)0x62002A0, 8);
    dmaFillWords(0xFFF00F00, (void*)0x62002A8, 4);
    dmaFillWords(0x00000F00, (void*)0x62002AC, 8);
    dmaFillWords(0xFFFFF000, (void*)0x62002B4, 4);
    dmaFillWords(0x00000000, (void*)0x62002B8, 8);

	BG_PALETTE[ 0]=BG_PALETTE_SUB[ 0]=0x7FFF;
    BG_PALETTE[63]=BG_PALETTE_SUB[63]=0x01DF;
}

// screen to display error messages
bool c_displayError(const char* message,bool isfatal)
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();

    c_writeFrame();

    c_goto(0,13);
    iprintf("ERROR!");

    consoleSetWindow(&consoleFG,2,2,28,20);
    c_goto(0,0);
    iprintf(message);

    if(isfatal)
    {
        exit(-1);
    }
    consoleSetWindow(&consoleFG,0,0,32,24);
    return true;
}


#endif // CONSOLE_H_INCLUDED
