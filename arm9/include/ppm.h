#ifndef FSPDS_PPM_H_INCLUDED
#define FSPDS_PPM_H_INCLUDED

#include "filesystem.h"
#include "sound.h"

void ppm_loadMetadata()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* fp=fopen(fn,"rb");
    // Parse header
    fread(ppmHead_Magic,4,1,fp);  ppmHead_Magic[4]=0;
    fread(&ppmHead_AnimationDataSize,sizeof(u32),1,fp);
    fread(&ppmHead_SoundDataSize,sizeof(u32),1,fp);
    fread(&ppmHead_FrameCount,sizeof(u16),1,fp);
    // Actually there's one more frame than it is specified at 0xC
    ppmHead_FrameCount++;
    fread(&ppmHead_FormatVersion,sizeof(u16),1,fp);
    // Parse meta
    fread(&ppmMeta_Lock,sizeof(u16),1,fp);
    fread(&ppmMeta_ThumbnailFrameIndex,sizeof(u16),1,fp);
    // skip author names until i find out how to read wide chars
    fseek(fp,66,SEEK_CUR);

    fread(ppmMeta_ParentAuthorId,8,1,fp);
    fread(ppmMeta_CurrentAuthorId,8,1,fp);
    fseek(fp,36,SEEK_CUR); // skip filenames - redundant data
    fread(ppmMeta_RootAuthorId,8,1,fp);
    fread(&ppmMeta_Timestamp,sizeof(u32),1,fp);

    // Jump to frame playback speed
    fseek(fp,-ppmHead_SoundDataSize-0xA0,SEEK_END);
    fread(&ppm_FramePlaybackSpeed,1,1, fp);
    ppm_FramePlaybackSpeed=8-ppm_FramePlaybackSpeed;
    fclose(fp);
}

void ppm_loadAnimationData()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* PPM_Current=fopen(fn,"rb");
    fseek(PPM_Current,0x4,SEEK_SET);
    fread(&ppmHead_AnimationDataSize,4,1,PPM_Current);
    fread(&ppmHead_SoundDataSize,4,1,PPM_Current);
    fread(&ppmHead_FrameCount,2,1,PPM_Current);
    ppmHead_FrameCount++;
    /// Assume size of frame offset table is 4*FrameCount, we don't need that
    /// Jump directly to flags
    fseek(PPM_Current,0x6A6,SEEK_SET);
    fread(&ppmADat_0x6A6,2,1,PPM_Current);
    // read offset table
    fread(ppm_OffsetTable,ppmHead_FrameCount*sizeof(u32),1,PPM_Current);
    // read animation data
    fread(ppm_AnimationData,ppmHead_AnimationDataSize,1,PPM_Current);
	
	fclose(PPM_Current);
}

void ppm_loadSoundData()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* PPM_Current=fopen(fn,"rb");
    u32 offset=0x6A0+ppmHead_AnimationDataSize;
    fseek(PPM_Current,offset,SEEK_SET);
    fread(ppm_SfxUsage,ppmHead_FrameCount,1,PPM_Current);
    offset+=ppmHead_FrameCount;
    fseek(PPM_Current,(4-(offset&3))&3,SEEK_CUR);
    fread(&ppm_BGMSize,4,1,PPM_Current);
    fread(&ppm_SE1Size,4,1,PPM_Current);
    fread(&ppm_SE2Size,4,1,PPM_Current);
    fread(&ppm_SE3Size,4,1,PPM_Current);
    fread(&ppm_FramePlaybackSpeed,1,1, PPM_Current);
    ppm_FramePlaybackSpeed=8-ppm_FramePlaybackSpeed;
    fread(&ppm_RecordedPlaybackSpeed,1,1, PPM_Current);
    ppm_RecordedPlaybackSpeed=8-ppm_RecordedPlaybackSpeed;	
    soundFreq=8192*frameTime[ppm_RecordedPlaybackSpeed]/frameTime[ppm_FramePlaybackSpeed];
	// what happens when recordedPlaybackSpeed==30Hz && framePlaybackSpeed==0.5Hz ?
	// soundFreq = 8195 
    fseek(PPM_Current,14,SEEK_CUR);

    fread(ppm_BGMData + (1<<20), ppm_BGMSize,1,PPM_Current);
    fread(ppm_SE1Data          , ppm_SE1Size,1,PPM_Current);
    fread(ppm_SE2Data          , ppm_SE2Size,1,PPM_Current);
    fread(ppm_SE3Data          , ppm_SE3Size,1,PPM_Current);
	
	fclose(PPM_Current);
	
	// creade decoded BGM	
	toPCM(ppm_BGMData + (1<<20), ppm_BGMSize, ppm_BGMData);
}


#endif // FSPDS_PPM_H_INCLUDED
