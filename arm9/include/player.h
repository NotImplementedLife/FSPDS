#ifndef FSPDS_PLAYER_H_INCLUDED
#define FSPDS_PLAYER_H_INCLUDED

#include <nds.h>

int bgMain;
u16* mainGFX;

#include "vars.h"

#define LAYER_SIZE 32*192

u8 *layer1, *layer2, *layerA;
void initPlayer()
{
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
    videoSetMode(MODE_5_2D);
    bgMain=bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
    mainGFX = bgGetGfxPtr(bgMain)+256*256;

    layer1=malloc(LAYER_SIZE);
    layer2=malloc(LAYER_SIZE);
    layerA=malloc(LAYER_SIZE); // auxiliary
}

void playerSwapBuffers()
{
    mainGFX=(u16*)bgGetGfxPtr(bgMain);
    bgSetMapBase(bgMain,8-bgGetMapBase(bgMain));
}


const u16 THUMBNAIL_PALETTE[16]=
{
    0xFFFF,0xA94A,0xFFFF,0xCE73,
    0xA13F,0xA559,0xD6BF,0x83E0,
    0xFD09,0xDD2A,0xFEB5,0x83E0,
    0xD956,0x83E0,0x83E0,0x83E0
};

void setPixelThumbnail(u8 x,u8 y,u16 c)
{
    int xx=64+2*x, yy=48+2*y;
    mainGFX[256*yy+(xx++)]=c;
    mainGFX[256*(yy++)+xx]=c;
    mainGFX[256*yy+(xx--)]=c;
    mainGFX[256*yy+xx]=c;
}

void playerClear()
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

    playerSwapBuffers();
}

u8 *layer;

const u16 FlipnoteColors[4] =
{
    0x8421, // Black
    0xFFFF, // White
    0x94BF, // Red
    0xFCE1  // Blue
};

void playerNextFrame()
{
    int i=ppm_OffsetTable[PlayerFrameIndex];
    u8 first_byte_header=ppm_AnimationData[i++];
    s8 tX=0,tY=0; // translate parameters
    bool diffing=!(first_byte_header & 0b10000000);
    if(first_byte_header & 0b01100000)
    {
        tX=ppm_AnimationData[i++];
        tY=ppm_AnimationData[i++];
    }

    layer=layer1;
    int k=i+0x60;

    __LBL__BUILD_LAYER__:
    if(diffing)
    {
        memcpy(layerA,layer,LAYER_SIZE);
    }
    for(u16 y=0;y<192;y++)
    {
        u16 yy=y<<5;
        switch((ppm_AnimationData[i+(y>>2)]>>((y&0x3)<<1)) & 0x3)
        {
            case 0:
                {
                    for(u8 _i=0;_i<32;_i++)
                        layer[yy++]=0;
                    break;
                }
            case 3:
                {
                    for(u16 c=0;c<32;c++)
                    {
                        u8 chunk=ppm_AnimationData[k++];
                        for(u8 b=0;b<8;b++)
                            layer[yy] ^= ((-(u8)((chunk>>b)&1) ^ layer[yy]) & (1<<b));
                        yy++;
                    }
                    break;
                }
            case 2:
                {
                    // Fill line
                    for(int _i=0; _i<32; layer[yy+(_i++)]=0xFF);
                    goto __DECODETYPE_1_2__;
                }

            case 1:
                {
                    // Clear line
                    for(int _i=0; _i<32; layer[yy+(_i++)]=0);

                    __DECODETYPE_1_2__: ;

                    u32 bytes=ppm_AnimationData[k++]<<24;
                    bytes|=ppm_AnimationData[k++]<<16;
                    bytes|=ppm_AnimationData[k++]<<8;
                    bytes|=ppm_AnimationData[k++];
                    for(;bytes;)
                    {
                        if(bytes & 0x80000000)
                        {
                            u8 chunk=ppm_AnimationData[k++];
                            for(int b=0;b<8;b++)
                                layer[yy] ^= (-(u8)((chunk>>b)&1) ^ layer[yy]) & (1<<b);
                        }
                        bytes<<=1;
                        yy++;
                    }
                    break;
                }
        }
    }
    if(diffing)
    {
        for(u16 y=0;y<192;y++)
        {
            if(y<tY) continue;
            if(y-tY>=192) break;
            u16 BL=y<<5,BA=(y-tY)<<5;
            for(u8 c=0;c<32;c++)
                for(u8 b=0;b<8;b++)
                {
                    u8 x=8*c+b;
                    if(x<tX) continue;
                    if(x-tX>=256) break;
                    u8 dx=(x-tX);
                    layer[BL+c]^= ((bool)(layerA[BA+(dx>>3)]&(1<<(dx&0x7))))<<b;
                }
        }
    }
    if(layer==layer1)
    {
        layer=layer2;
        i+=0x30;
        goto __LBL__BUILD_LAYER__;
    }

    u16 paperColor = (first_byte_header & 1);
    u16 layer1Color = (first_byte_header>>1) & 0x3;
    if(layer1Color<2)
        layer1Color = 1-paperColor;
    u16 layer2Color = (first_byte_header>>3) & 0x3;
    if(layer2Color<2)
        layer2Color = 1-paperColor;
    paperColor=FlipnoteColors[paperColor];
    layer1Color=FlipnoteColors[layer1Color];
    layer2Color=FlipnoteColors[layer2Color];

    for(u16 y=0,yy=0;y<192;y++)
    {
        //u16 yy=(y<<5);
        for(u8 c=32;c--;yy++)
        {
            for(u8 b=0;b<8;b++)
            {
                u16 p=(yy<<3)+b;
                if(layer1[yy]&(1<<b))
                    mainGFX[p]=layer1Color;
                else
                    mainGFX[p]=(layer2[yy]&(1<<b))?layer2Color:paperColor;
            }
        }
    }

    playerSwapBuffers();
    PlayerFrameIndex++;
    if(PlayerFrameIndex==ppmHead_FrameCount)
    {
        PlayerFrameIndex=0;
    }
}


#endif // FSPDS_PLAYER_H_INCLUDED
