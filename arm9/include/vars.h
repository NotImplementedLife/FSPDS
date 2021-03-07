#ifndef FSPDS_VARS_H_INCLUDED
#define FSPDS_VARS_H_INCLUDED

#include <nds.h>
/// Filesystem variables

touchPosition touch;

#define MAXFILESCOUNT 1024
int filescount=0;
static char files[MAXFILESCOUNT][29];
static char sizes[MAXFILESCOUNT][7];

int PagesCount, PageSelection=0, CurrentPage=0;

/// PPM file data

static char ppmHead_Magic[5]="NULL";
u32 ppmHead_AnimationDataSize;
u32 ppmHead_SoundDataSize;
u16 ppmHead_FrameCount;
u16 ppmHead_FormatVersion;
u16 ppmMeta_Lock;
u16 ppmMeta_ThumbnailFrameIndex;
u8 ppmMeta_ParentAuthorId[8];
u8 ppmMeta_CurrentAuthorId[8];
u8 ppmMeta_RootAuthorId[8];
u32 ppmMeta_Timestamp;

u8 ppm_FramePlaybackSpeed;
const u8 frameTime[9]= { 2, 120, 60, 30, 15, 10, 5, 3, 2 };

FILE* PPM_Current;

u16 ppmADat_0x6A6; // Flags
static u32 ppm_OffsetTable[999];
static u8  ppm_AnimationData[1<<20];

enum
{
    PLAYING,
    PAUSED
} PlayerState = PAUSED;

s16 PlayerLoadedFileIndex=-1;
bool PlayerForceAnimationReload = false;
bool PlayerThumbnailNeedsRedraw = false;

u16 PlayerFrameIndex=0;

static u16 counter=0;
static struct decodeType dt;

#endif // FSPDS_VARS_H_INCLUDED
