#ifndef FSPDS_PLAYER_H_INCLUDED
#define FSPDS_PLAYER_H_INCLUDED

#include <nds.h>

int bgMain;
u16* mainGFX;


void initPlayer()
{
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_BG);
    videoSetMode(MODE_5_2D);
    bgMain=bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
    mainGFX = bgGetGfxPtr(bgMain);
}

u16 THUMBNAIL_PALETTE[16]=
{
    0xFFFF,0xA94A,0xFFFF,0xCE73,
    0xA13F,0xA559,0xD6BF,0x83E0,
    0xFD09,0xDD2A,0xFEB5,0x83E0,
    0xD956,0x83E0,0x83E0,0x83E0
};

void setPixelThumbnail(u8 x,u8 y,u16 c)
{
    int xx=64+2*x;
    int yy=48+2*y;
    mainGFX[256*yy+xx]=mainGFX[256*yy+xx+1]=mainGFX[256*(yy+1)+xx]=mainGFX[256*(yy+1)+(xx+1)]=c;
}

void clearPlayer()
{
    for(int x=0;x<256;x++)
        for(int y=0;y<192;y++)
            mainGFX[256*y+x]=ARGB16(1,12,12,12);
}

void displayThumbnail()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* fp=fopen(fn,"rb");
    fseek(fp,0xA0,SEEK_SET);
    u8 bytes[1536];
    fread(bytes,1536,1,fp);

    int offset=0,x,y;
    for(u8 ty=0;ty<48;ty+=8)
    {
        for(u8 tx=0;tx<64;tx+=8)
        {
            for(int l=0;l<8;l++)
                for(int p=0;p<8;p+=2)
                {
                    x=tx+p, y=ty+l;
                    setPixelThumbnail(x,y,THUMBNAIL_PALETTE[bytes[offset]&0xF]);
                    setPixelThumbnail(x+1,y,THUMBNAIL_PALETTE[(bytes[offset]>>4)&0xF]);
                    offset++;
                }
        }
    }
}

#endif // FSPDS_PLAYER_H_INCLUDED
