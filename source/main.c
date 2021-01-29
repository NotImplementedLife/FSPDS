#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAXFILESCOUNT 1024

int filescount=0;
char files[MAXFILESCOUNT][29];
char sizes[MAXFILESCOUNT][7];

PrintConsole consoleBG,console;

void long_to_size_string(char dest[7],long sz)
{
    dest[5]='B';
    dest[6]='\0';
    for(int i=0;i<5;i++) dest[i]=' ';
    if(sz==0)
    {
        dest[4]='0';
        return;
    }
    if(sz<1024)
    {
        for(int i=4;sz>0;i--)
        {
            dest[i]=(sz%10)+'0';
            sz/=10;
        }
        return;
    }
    if(sz<1024*1024)
    {
        sz>>=10;
        dest[4]='K';
        for(int i=3;sz>0;i--)
        {
            dest[i]=(sz%10)+'0';
            sz/=10;
        }
        return;
    }
    dest[4]='M';
    sz>>=10;
    int q=sz>>10;
    int r=((sz&0x3FF)*100)>>10;
    if(q>=10)
    {
        iprintf("Fatal error: size limit exceeded. \n");
        exit(-1);
    }
    if(r>=100) r/=10;
    dest[3]=(r%10)+'0';
    r/=10;
    dest[2]=(r%10)+'0';
    dest[1]='.';
    dest[0]=q+'0';
}

void loadFiles()
{
    if(!fatInitDefault())
    {
        iprintf("Fatal error: FAT init failed.\n");
        exit(-1);
    }
    DIR *root;
    struct dirent *entry;
    root=opendir("/flipnotes");
    if (root)
    {
        while((entry=readdir(root))!=NULL && filescount<MAXFILESCOUNT)
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            if(entry->d_type != DT_DIR)
            {
                if(strlen(entry->d_name)==28 && strcmp(".ppm",entry->d_name+24)==0)
                {
                    strcpy(files[filescount],entry->d_name);
                    char fn[40]="/flipnotes/";
                    strcat(fn,entry->d_name);
                    FILE* fp=fopen(fn,"rb");
                    fseek(fp,0L,SEEK_END);
                    long_to_size_string(sizes[filescount],ftell(fp));
                    fclose(fp);
                    filescount++;
                }
            }
        }
        closedir(root);
    }
    else
    {
        iprintf ("Fatal error: Open directory failed.\n");
    }
}

void c_cls()
{
    //iprintf("\e[1;1H\e[2J");
    iprintf("\x1b[2J");
}

void c_goto(int row, int column)
{
    printf("\033[%d;%dH", row, column);
}

void writeFrame()
{
    int i;
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

void writeEntry(int i,int listpos)
{
    consoleSelect(&consoleBG);
    for(int i=0;i<3;i++)
    {
        c_goto(1+3*listpos+i,1);
        for(int j=0;j<30;j++) iprintf("\x1b[33m\x02");
    }
    consoleSelect(&console);
    c_goto(1+3*listpos,2);
    iprintf("\x1b[39m%s",files[i]);
    c_goto(2+3*listpos,20);
    iprintf("\x1b[39m%s",sizes[i]);
    //iprintf("  %s\n\t\t\t\t\t\t\t%s\n",files[i],sizes[i]);
}

void init()
{
    //thanks: https://github.com/DS-Homebrew/GodMode9i/blob/master/arm9/source/main.cpp
    videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(&consoleBG, 1, BgType_Text4bpp, BgSize_T_256x256, 7, 0, false, true);
	consoleInit(&console, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

    // overwrite character fonts
	dmaFillWords(0xFFFFFFFF, (void*)0x6200040, 32);
    dmaFillWords(0x00F00F00, (void*)0x6200060, 32);

    dmaFillWords(0x00000000, (void*)0x6200080, 8);
    dmaFillWords(0xFFFFFFFF, (void*)0x6200088, 4);
    dmaFillWords(0x00000000, (void*)0x620008C, 8);
    dmaFillWords(0xFFFFFFFF, (void*)0x6200094, 4);
    dmaFillWords(0x00000000, (void*)0x6200098, 8);

    dmaFillWords(0x00000000, (void*)0x6200240, 8);
    dmaFillWords(0x00FFFFFF, (void*)0x6200248, 4);
    dmaFillWords(0x00F00000, (void*)0x620024C, 8);
    dmaFillWords(0x00F00FFF, (void*)0x6200254, 4);
    dmaFillWords(0x00F00F00, (void*)0x6200258, 8);

    dmaFillWords(0x00F00F00, (void*)0x6200260, 8);
    dmaFillWords(0x00F00FFF, (void*)0x6200268, 4);
    dmaFillWords(0x00F00000, (void*)0x620026C, 8);
    dmaFillWords(0x00FFFFFF, (void*)0x6200274, 4);
    dmaFillWords(0x00000000, (void*)0x6200278, 8);

    dmaFillWords(0x00000000, (void*)0x6200280, 8);
    dmaFillWords(0xFFFFFF00, (void*)0x6200288, 4);
    dmaFillWords(0x00000F00, (void*)0x620028C, 8);
    dmaFillWords(0xFFF00F00, (void*)0x6200294, 4);
    dmaFillWords(0x00F00F00, (void*)0x6200298, 8);

    dmaFillWords(0x00F00F00, (void*)0x62002A0, 8);
    dmaFillWords(0xFFF00F00, (void*)0x62002A8, 4);
    dmaFillWords(0x00000F00, (void*)0x62002AC, 8);
    dmaFillWords(0xFFFFFF00, (void*)0x62002B4, 4);
    dmaFillWords(0x00000000, (void*)0x62002B8, 8);

	BG_PALETTE[ 0]=BG_PALETTE_SUB[ 0]=0x7FFF;
    BG_PALETTE[63]=BG_PALETTE_SUB[63]=0x01DF;
}

int main(int argc, char **argv)
{
    init();
    //loadFiles();

    writeFrame();
    //c_cls();
    filescount=2;
    strcpy(files[0],"000000_0000000000000_000.ppm");
    strcpy(sizes[0]," 127KB");
    strcpy(files[1],"000001_0000000000000_001.ppm");
    strcpy(sizes[1],"   13B");
    for(int i=0;i<filescount;i++)
    {
        writeEntry(i,i);
    }

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysDown()&KEY_START) break;
	}

	return 0;
}

