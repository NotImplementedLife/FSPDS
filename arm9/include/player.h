#ifndef FSPDS_PLAYER_H_INCLUDED
#define FSPDS_PLAYER_H_INCLUDED

#include <nds.h>

int bgMain;
u16* mainGFX;

#include "vars.h"

#define LAYER_SIZE 32*192

const u16 THUMBNAIL_PALETTE[16]=
{
    0xFFFF,0xA94A,0xFFFF,0xCE73,
    0xA13F,0xA559,0xD6BF,0x83E0,
    0xFD09,0xDD2A,0xFEB5,0x83E0,
    0xD956,0x83E0,0x83E0,0x83E0
};

const u16 FlipnoteColors[4] =
{
    0x8421, // Black
    0xFFFF, // White
    0x94BF, // Red
    0xFCE1  // Blue
};

static u8 layer1[LAYER_SIZE], layer2[LAYER_SIZE];
static u8 layerA[LAYER_SIZE], layerB[LAYER_SIZE];
void initPlayer()
{
    for(int i=0;i<16;i++)
        BG_PALETTE[i]=THUMBNAIL_PALETTE[i];
    BG_PALETTE[16]=ARGB16(1,12,12,12);
    for(int i=0;i<4;i++)
        BG_PALETTE[17+i]=FlipnoteColors[i];
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
    videoSetMode(MODE_5_2D);
    bgMain=bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0,0);
    mainGFX = (u16*)bgGetGfxPtr(bgMain)+ 256*256;
}

void playerSwapBuffers()
{
    mainGFX=(u16*)bgGetGfxPtr(bgMain);
    bgSetMapBase(bgMain,8-bgGetMapBase(bgMain));
}

void setPixelThumbnail(u8 x,u8 y,u16 c)
{
    int xx=32+x, yy=48+2*y;
    mainGFX[128*(yy++)+xx]=c+(c<<8);
    mainGFX[128*yy+xx]=c+(c<<8);
}

void playerClear()
{
    memset(mainGFX,0x10,256*192);
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
                    setPixelThumbnail(x,y,bytes[offset]&0xF);
                    setPixelThumbnail(x+1,y,(bytes[offset]>>4)&0xF);
                    offset++;
                }
        }
    }
    playerSwapBuffers();
}

struct decodeType
{
    int i, k;
    u8 first_byte_header;
    s8 tX, tY;
    bool diffing;
};

/*****************************  [ vBlank Op ] *****************************/
/** > The maximum flipnote framerate is 30fps.                            */
/** >    1/30 = 1/60 + 1/60, hence, in 1/30 seconds two vBlanks happen    */
/** > Frame rendering is too expensive to be executed in < 1/60 = 16.6ms  */
/** > So, I'll split the frame rendering into two separate subtasks, each */
/** > to be executed in its own vBlank.                                   */
/** >                                                                     */
/** > The procedure is as follows:                                        */
/** > vBlank 0 - parse animation data and do frame diffing up to ~68% of  */
/** >            the lines                                                */
/** > vBlank 1 - finish frame diffing and write graphics into memory      */
/** >                                                                     */
/** > If the playback speed is smaller (e.g. 12fps = 5 vBlanks), then the */
/** > code does nothing during the additional vBlanks.                    */
/** > The real rendering speed is adjusted using the {counter} in main()  */


void playerNextFrameVBlank0(struct decodeType* dt)
{
    dt->i=ppm_OffsetTable[PlayerFrameIndex];
    dt->first_byte_header=ppm_AnimationData[dt->i++];
    dt->tX=0,dt->tY=0; // translate parameters
    dt->diffing=!(dt->first_byte_header & 0b10000000);
    if(dt->first_byte_header & 0b01100000)
    {
        dt->tX=ppm_AnimationData[dt->i++];
        dt->tY=ppm_AnimationData[dt->i++];
    }
    dt->k=dt->i+0x60;

    if(dt->diffing)
    {
        memcpy(layerA,layer1,LAYER_SIZE);
        memcpy(layerB,layer2,LAYER_SIZE);
    }
    for(u16 y=0;y<192;y++)
    {
        u16 yy=y<<5;
        switch((ppm_AnimationData[dt->i+(y>>2)]>>((y&0x3)<<1)) & 0x3)
        {
            case 0:
                {
                    memset(layer1+yy,0,32);
                    yy+=32;
                    break;
                }
            case 3:
                {
                    memcpy(layer1+yy,ppm_AnimationData+dt->k,32);
                    yy+=32;
                    dt->k+=32;
                    break;
                }
            case 2:
                {
                    // Fill line
                    memset(layer1+yy,0xFF,32);
                    goto __DECODETYPE_1_2__;
                }

            case 1:
                {
                    // Clear line
                    memset(layer1+yy,0x00,32);

                    __DECODETYPE_1_2__: ;

                    u32 bytes=ppm_AnimationData[dt->k++]<<24;
                    bytes|=ppm_AnimationData[dt->k++]<<16;
                    bytes|=ppm_AnimationData[dt->k++]<<8;
                    bytes|=ppm_AnimationData[dt->k++];
                    for(;bytes;)
                    {
                        if(bytes & 0x80000000)
                        {
                            layer1[yy] = ppm_AnimationData[dt->k++];
                        }
                        bytes<<=1;
                        yy++;
                    }
                    break;
                }
        }
    }
    dt->i+=0x30;
     for(u16 y=0;y<192;y++)
    {
        u16 yy=y<<5;
        switch((ppm_AnimationData[dt->i+(y>>2)]>>((y&0x3)<<1)) & 0x3)
        {
            case 0:
                {
                    memset(layer2+yy,0,32);
                    yy+=32;
                    break;
                }
            case 3:
                {
                    memcpy(layer2+yy,ppm_AnimationData+dt->k,32);
                    yy+=32;
                    dt->k+=32;
                    break;
                }
            case 2:
                {
                    // Fill line
                    memset(layer2+yy,0xFF,32);
                    goto __DECODETYPE_1_2__L2;
                }

            case 1:
                {
                    // Clear line
                    memset(layer2+yy,0x00,32);

                    __DECODETYPE_1_2__L2: ;

                    u32 bytes=ppm_AnimationData[dt->k++]<<24;
                    bytes|=ppm_AnimationData[dt->k++]<<16;
                    bytes|=ppm_AnimationData[dt->k++]<<8;
                    bytes|=ppm_AnimationData[dt->k++];
                    for(;bytes;)
                    {
                        if(bytes & 0x80000000)
                        {
                            layer2[yy] = ppm_AnimationData[dt->k++];
                        }
                        bytes<<=1;
                        yy++;
                    }
                    break;
                }
        }
    }
    if(dt->diffing)
    {
        u16 ld0 = (dt->tX>=0) ? (dt->tX>>3) : 0;
        u16 pi0 = (dt->tX>=0) ? 0 : (-dt->tX)>>3;
        u8 del = dt->tX>=0 ? (dt->tX & 7) : (((u8)(dt->tX))&7);
        u8 ndel=8-del;
        u8 alpha = (1<<(8-del))-1;
        u8 nalpha = ~alpha;
        u16 pi=0,ld=0;
        if(dt->tX>=0)
        {
            for(u16 y=0;y<130;y++)
            {
                if(y<dt->tY) continue;
                if(y-dt->tY>=192) break;
                ld=(y<<5)+ld0;
                pi=((y-dt->tY)<<5)+pi0;
                layer1[ld] ^= layerA[pi] & alpha;
                layer2[ld++] ^= layerB[pi] & alpha;
                while((ld&31)<31)
                {
                    layer1[ld]^=((layerA[pi]&nalpha)>>ndel) | ((layerA[pi+1]&alpha)<<del);
                    layer2[ld]^=((layerB[pi]&nalpha)>>ndel) | ((layerB[pi+1]&alpha)<<del);
                    ld++; pi++;
                }
                layer1[ld]^=(layerA[pi]&nalpha) | (layerA[pi+1]&alpha);
                layer2[ld]^=(layerB[pi]&nalpha) | (layerB[pi+1]&alpha);
            }
        }
        else
        {
            for(u16 y=0;y<130;y++)
            {
                if(y<dt->tY) continue;
                if(y-dt->tY>=192) break;
                ld=(y<<5)+ld0;
                pi=((y-dt->tY)<<5)+pi0;
                while((pi&31)<31)
                {
                    layer1[ld]^=((layerA[pi]&nalpha)>>ndel) | ((layerA[pi+1]&alpha)<<del);
                    layer2[ld]^=((layerB[pi]&nalpha)>>ndel) | ((layerB[pi+1]&alpha)<<del);
                    ld++; pi++;
                }
                layer1[ld] ^= layerA[pi] & nalpha;
                layer2[ld] ^= layerB[pi] & nalpha;
            }
        }
    }

}

void playerNextFrameVBlank1(struct decodeType* dt)
{
    if(dt->diffing)
    {
        u16 ld0 = (dt->tX>=0) ? (dt->tX>>3) : 0;
        u16 pi0 = (dt->tX>=0) ? 0 : (((u8)(-dt->tX))>>3);
        u8 del = (dt->tX>=0) ? (dt->tX & 7) : (((u8)(dt->tX))&7);
        u8 ndel=8-del;
        u8 alpha = (1<<(8-del))-1;
        u8 nalpha = ~alpha;
        u16 pi=0,ld=0;
        if(dt->tX>=0)
        {
            for(u16 y=130;y<192;y++)
            {
                if(y<dt->tY) continue;
                if(y-dt->tY>=192) break;
                ld=(y<<5)+ld0;
                pi=((y-dt->tY)<<5)+pi0;
                layer1[ld] ^= (layerA[pi] & alpha)<<del;
                layer2[ld++] ^= (layerB[pi] & alpha)<<del;
                while((ld&31)<31)
                {
                    layer1[ld]^=((layerA[pi]&nalpha)>>ndel) | ((layerA[pi+1]&alpha)<<del);
                    layer2[ld]^=((layerB[pi]&nalpha)>>ndel) | ((layerB[pi+1]&alpha)<<del);
                    ld++; pi++;
                }
                layer1[ld]^=((layerA[pi]&nalpha)>>ndel) | ((layerA[pi+1]&alpha)<<del);
                layer2[ld]^=((layerB[pi]&nalpha)>>ndel) | ((layerB[pi+1]&alpha)<<del);
            }
        }
        else
        {
            for(u16 y=130;y<192;y++)
            {
                if(y<dt->tY) continue;
                if(y-dt->tY>=192) break;
                ld=(y<<5)+ld0;
                pi=((y-dt->tY)<<5)+pi0;
                while((pi&31)<31)
                {
                    layer1[ld]^=((layerA[pi]&nalpha)>>ndel) | ((layerA[pi+1]&alpha)<<del);
                    layer2[ld]^=((layerB[pi]&nalpha)>>ndel) | ((layerB[pi+1]&alpha)<<del);
                    ld++; pi++;
                }
                layer1[ld] ^= (layerA[pi] & nalpha)>>ndel;
                layer2[ld] ^= (layerB[pi] & nalpha)>>ndel;
            }
        }
    }

    // prepare to draw frame

    u16 paperColor = (dt->first_byte_header & 1);
    u16 layer1Color = (dt->first_byte_header>>1) & 0x3;
    if(layer1Color<2)
        layer1Color = 1-paperColor;
    u16 layer2Color = (dt->first_byte_header>>3) & 0x3;
    if(layer2Color<2)
        layer2Color = 1-paperColor;

    paperColor+=17;
    layer1Color+=17;
    layer2Color+=17;

    u16 p=0,val=0;
    for(u16 y=0,yy=0;y<192;y++)
    {
        for(u8 c=32;c--;yy++)
        {
            for(u8 b=0;b<8;b++)
            {
                if(layer1[yy]&(1<<b))
                    val|=layer1Color<<((b&1)<<3);
                else
                    val|=((layer2[yy]&(1<<b))?layer2Color:paperColor)<<((b&1)<<3);
                if(b&1)
                {
                    mainGFX[p++]=val;
                    val=0;
                }
            }
        }
    }
    playerSwapBuffers();

    PlayerFrameIndex++; // go to next frame
    if(PlayerFrameIndex==ppmHead_FrameCount)
    {
        PlayerFrameIndex=0; // Loop
        soundPause(BGMId);
        soundResume(BGMId); // restart the track
    }
}

#endif // FSPDS_PLAYER_H_INCLUDED
