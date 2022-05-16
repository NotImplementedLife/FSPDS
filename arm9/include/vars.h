#pragma once

#include <nds.h>
#include <stdio.h>
#include <maxmod9.h>

#include "types.h"

/// Filesystem variables

extern touchPosition touch;

extern int PagesCount, PageSelection, CurrentPage;

/// PPM file data

extern char ppmHead_Magic[5];
extern u32 ppmHead_AnimationDataSize;
extern u32 ppmHead_SoundDataSize;
extern u16 ppmHead_FrameCount;
extern u16 ppmHead_FormatVersion;
extern u16 ppmMeta_Lock;
extern u16 ppmMeta_ThumbnailFrameIndex;
extern u8 ppmMeta_ParentAuthorId[8];
extern u8 ppmMeta_CurrentAuthorId[8];
extern u8 ppmMeta_RootAuthorId[8];
extern u32 ppmMeta_Timestamp;

extern u8 ppm_FramePlaybackSpeed;
extern const u8 frameTime[9];

extern u16 ppmADat_0x6A6; // Flags
extern u32 ppm_OffsetTable[999];
extern u8  ppm_AnimationData[1<<20];

// sound data
extern u8 ppm_SfxUsage[999];

extern u32 ppm_BGMSize;
/**
	! Memory limitation-proof measure
	ppm_BGMData -  normally <1MB sized (1<<20), but its size grows 4 times after
	converting ADPCM to PCM. The conversion is done in-place:
	- the original ADPCM data is stored in the second half of the array 
	  (ppm_BGMData + (1<<20))
	- the toPCM destination array starts from offset 0 of ppm_BGMData
	- the original ADPCM raw data is therefore overwritten by the converted PCM 
	- FSPDS assumes BGM data is no bigger than 512KB !!! (The tested flipnotes had <240KB BGM size data)
*/
extern u8 ppm_BGMData[1<<21];

extern u32 ppm_SE1Size;
extern u8 ppm_SE1Data[0x2000];

extern u32 ppm_SE2Size;
extern u8 ppm_SE2Data[0x2000];

extern u32 ppm_SE3Size;
extern u8 ppm_SE3Data[0x2000];

extern u8 ppm_RecordedPlaybackSpeed;

extern u16 soundFreq;
extern int BGMId,SE1Id,SE2Id,SE3Id;

typedef enum 
{
    PLAYING,
    PAUSED
} _PlayerState;

extern _PlayerState PlayerState;

extern s16 PlayerLoadedFileIndex;
extern bool PlayerForceAnimationReload;
extern bool PlayerThumbnailNeedsRedraw;

extern u16 PlayerFrameIndex;

extern u8 counter;
extern decodeType dt;

extern int sound_frame_counter; // counter for BGM
