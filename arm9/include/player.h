#ifndef FSPDS_PLAYER_H_INCLUDED
#define FSPDS_PLAYER_H_INCLUDED

#include <nds.h>

extern int bgMain;
extern u16* mainGFX;

#include "vars.h"

#define LAYER_SIZE 32*192

extern const u16 THUMBNAIL_PALETTE[16];

extern const u16 FlipnoteColors[4];

extern u8 layer1[LAYER_SIZE], layer2[LAYER_SIZE];
extern u8 layerA[LAYER_SIZE], layerB[LAYER_SIZE];

void initPlayer();

void playerSwapBuffers();

void setPixelThumbnail(u8 x, u8 y,u16 c);

void playerClear();

void displayThumbnail();

void playerNextFrameVBlank0(decodeType* dt);

void playerNextFrameVBlank1(decodeType* dt);

#endif // FSPDS_PLAYER_H_INCLUDED
