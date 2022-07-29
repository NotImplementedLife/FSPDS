#include "console.h"

PrintConsole consoleBG,consoleFG;

void c_writeFrame()
{
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
    c_goto(23,3);    
    //for(i=32;i--;) iprintf("\x02");
}

void c_drawBorder(u8 l1,u8 c1,u8 l2,u8 c2)
{
    c_goto(l1,c1);
    iprintf("\024");
    for(int i=c1+1;i<c2;i++) iprintf("\004");
    iprintf("\022");
    for(int i=l1+1;i<l2;i++)
    {
        c_goto(i,c1); iprintf("\003");
        c_goto(i,c2); iprintf("\003");
    }
    c_goto(l2,c1);
    iprintf("\025");
    for(int i=c1+1;i<c2;i++) iprintf("\004");
    iprintf("\023");
}

void initConsole()
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

    // half weight vertical bars
    dmaFillWords(0xFFFF0000, (void*)0x62000A0,32);
    dmaFillWords(0x0000FFFF, (void*)0x62000C0,32);


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

    // Char #26 - Play Triangle Up 1
    void* address=(void*)0x62002C0;
    u32 word=0x000000FF;
    for(u8 i=3;i--;)
    {
        dmaFillWords(word,address,4);
        word=(word<<8)|0xFF;
        address+=0x4;
    }
    dmaFillWords(word, address,20);

    // Char #27 - Play Triangle Up 2
    address=(void*)0x62002E0;
    word=0x00000000;
    dmaFillWords(word,address,16);
    word=0x000000FF;
    address+=16;
    for(u8 i=4;i--;)
    {
        dmaFillWords(word,address,4);
        word=(word<<8)|0xFF;
        address+=0x4;
    }

    // Char #28 - Play Triangle Bottom 1
    address=(void*)0x6200300;
    word=0xFFFFFFFF;

    dmaFillWords(word, address,20);
    address+=0x14;
    word>>=8;
    for(int i=3;i--;)
    {
        dmaFillWords(word,address,4);
        word>>=8;
        address+=0x4;
    }

    // Char #29 - Play Triangle Bottom 2
    address=(void*)0x6200320;
    word=0xFFFFFFFF;
    for(int i=4;i--;)
    {
        dmaFillWords(word,address,4);
        word>>=8;
        address+=0x4;
    }
    dmaFillWords(word,address,16);

    // Char #30 - Bullet
    dmaFillWords(0x00000000, (void*)0x6200340, 8);
    dmaFillWords(0x000FF000, (void*)0x6200348, 4);
    dmaFillWords(0x00FFFF00, (void*)0x620034C, 8);
    dmaFillWords(0x000FF000, (void*)0x6200354, 4);
    dmaFillWords(0x00000000, (void*)0x6200358, 8);
	
	// Char #31 - Reverse Triangle Up 1
	
	address=(void*)0x6200360;
    word=0x00000000;
    dmaFillWords(word,address,16);
    word=0xFF000000;
    address+=16;
    for(u8 i=4;i--;)
    {
        dmaFillWords(word,address,4);
        word=(word>>8)|0xFF000000;
        address+=0x4;
    }
	
	
	// Char #32 - Reverse Triangle Up 2
	address=(void*)0x6200380;
    word=0xFF000000;
    for(u8 i=3;i--;)
    {
        dmaFillWords(word,address,4);
        word=(word>>8)|0xFF000000;
        address+=0x4;
    }
    dmaFillWords(word, address,20);
	
	// Char #33 - Reverse Triangle Bottom 1
	
	 address=(void*)0x62003A0;
    word=0xFFFFFFFF;
    for(int i=4;i--;)
    {
        dmaFillWords(word,address,4);
        word<<=8;
        address+=0x4;
    }
    dmaFillWords(word,address,16);   

    // Char #34 - Reverse Triangle Bottom 2
    address=(void*)0x62003C0;
    word=0xFFFFFFFF;

    dmaFillWords(word, address,20);
    address+=0x14;
    word<<=8;
    for(int i=3;i--;)
    {
        dmaFillWords(word,address,4);
        word<<=8;
        address+=0x4;
    }
	
	// Char #128 - Mini Triangle Left Up
		
	dmaFillWords(0x00000000, (void*)0x6201000, 16);
	dmaFillWords(0xFF000000, (void*)0x6201010, 4);
    dmaFillWords(0xFFFF0000, (void*)0x6201014, 4);
    dmaFillWords(0xFFFFFF00, (void*)0x6201018, 4);
    dmaFillWords(0xFFFFFFFF, (void*)0x620101C, 4);
	
	// Char #129 - Mini Triangle Left Bottom
			
	dmaFillWords(0xFFFFFFFF, (void*)0x6201020, 4);
    dmaFillWords(0xFFFFFF00, (void*)0x6201024, 4);
    dmaFillWords(0xFFFF0000, (void*)0x6201028, 4);
    dmaFillWords(0xFF000000, (void*)0x620102C, 4);
	dmaFillWords(0x00000000, (void*)0x6201030, 16);
	
	// Char #130 - Mini Triangle Right Up
		
	dmaFillWords(0x00000000, (void*)0x6201040, 16);
	dmaFillWords(0x000000FF, (void*)0x6201050, 4);
    dmaFillWords(0x0000FFFF, (void*)0x6201054, 4);
    dmaFillWords(0x00FFFFFF, (void*)0x6201058, 4);
    dmaFillWords(0xFFFFFFFF, (void*)0x620105C, 4);
	
	// Char #131 - Mini Triangle Right Bottom
			
	dmaFillWords(0xFFFFFFFF, (void*)0x6201060, 4);
    dmaFillWords(0x00FFFFFF, (void*)0x6201064, 4);
    dmaFillWords(0x0000FFFF, (void*)0x6201068, 4);
    dmaFillWords(0x000000FF, (void*)0x620106C, 4);
	dmaFillWords(0x00000000, (void*)0x6201070, 16);
	
	// Char #132 - Slim bar left top half		
	dmaFillWords(0x00000000, (void*)0x6201080, 16);
	dmaFillWords(0x00000FF0, (void*)0x6201090, 16);	
	
	// Char #133 - Slim bar left bottom half		
	dmaFillWords(0x00000FF0, (void*)0x62010A0, 16);
	dmaFillWords(0x00000000, (void*)0x62010B0, 16);
	
	// Char #134 - Slim bar right top half		
	dmaFillWords(0x00000000, (void*)0x62010C0, 16);
	dmaFillWords(0x0FF00000, (void*)0x62010D0, 16);	
	
	// Char #135 - Slim bar right bottom half		
	dmaFillWords(0x0FF00000, (void*)0x62010E0, 16);
	dmaFillWords(0x00000000, (void*)0x62010F0, 16);
	
	
	
   
	
	

	BG_PALETTE[  0]=BG_PALETTE_SUB[  0]=0x7FFF; // used when clear screen
    BG_PALETTE[ 15]=BG_PALETTE_SUB[ 15]=0x7FFF; // used by \x1b[30m
    BG_PALETTE[255]=BG_PALETTE_SUB[255]=0x01DF; // used by \x1b[39m
}

// screen to display error messages
bool c_displayError(const char* message,bool isfatal)
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();

	iprintf("\x1b[39m");
    c_writeFrame();

    c_goto(0,13);
    iprintf("ERROR!");

    consoleSetWindow(&consoleFG,2,2,28,20);
    c_goto(0,0);
    iprintf(message);

    if(isfatal)
    {
		while(1) swiWaitForVBlank();        
    }
    consoleSetWindow(&consoleFG,0,0,32,24);
    return true;
}

void c_loadingBox()
{
    consoleSelect(&consoleBG);
    iprintf("\x1b[30m");
    for(int i=10;i<13;i++)
    {
        c_goto(i,10);
        for(int j=0;j<12;j++) iprintf(" ");
    }
    consoleSelect(&consoleFG);
    iprintf("\x1b[39m");
    c_goto(10,10);
    iprintf("\024");
    for(int i=0;i<10;i++) iprintf("\004");
    iprintf("\022");
    c_goto(11,10);
    iprintf("\003Loading...\003");
    c_goto(12,10);
    iprintf("\025");
    for(int i=0;i<10;i++) iprintf("\004");
    iprintf("\023");
}
