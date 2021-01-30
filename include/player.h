#ifndef FSPDS_PLAYER_H_INCLUDED
#define FSPDS_PLAYER_H_INCLUDED

#include <nds.h>

u16* mainGFX;

void initPlayer()
{
    vramSetBankA(VRAM_A_LCD);
    videoSetMode(MODE_FB0);
    mainGFX = bgGetGfxPtr(bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0));
}



#endif // FSPDS_PLAYER_H_INCLUDED
