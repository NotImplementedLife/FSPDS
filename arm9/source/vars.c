#include "vars.h"

touchPosition touch;

/// PPM file data

char ppmHead_Magic[5]="NULL";
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

u16 ppmADat_0x6A6; // Flags
u32 ppm_OffsetTable[999];
u8  ppm_AnimationData[1<<20];

// sound data
u8 ppm_SfxUsage[999];

u32 ppm_BGMSize;
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
u8 ppm_BGMData[1<<21];

u32 ppm_SE1Size;
u8 ppm_SE1Data[0x2000];

u32 ppm_SE2Size;
u8 ppm_SE2Data[0x2000];

u32 ppm_SE3Size;
u8 ppm_SE3Data[0x2000];

u8 ppm_RecordedPlaybackSpeed;

u16 soundFreq;
int BGMId,SE1Id,SE2Id,SE3Id;
int durationCount;

_PlayerState PlayerState = PAUSED;

s16 PlayerLoadedFileIndex=-1;
bool PlayerForceAnimationReload = false;
bool PlayerThumbnailNeedsRedraw = false;

u16 PlayerFrameIndex=0;

u8 counter=0;
decodeType dt;

int sound_frame_counter = 0; // counter for BGM
