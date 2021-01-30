#ifndef FSPDS_PPM_H_INCLUDED
#define FSPDS_PPM_H_INCLUDED

#include "filesystem.h"

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

void ppm_loadMetadata()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* fp=fopen(fn,"rb");
    // Parse head
    fread(ppmHead_Magic,4,1,fp);  ppmHead_Magic[4]=0;
    fread(&ppmHead_AnimationDataSize,sizeof(u32),1,fp);
    fread(&ppmHead_SoundDataSize,sizeof(u32),1,fp);
    fread(&ppmHead_FrameCount,sizeof(u16),1,fp); ppmHead_FrameCount++;
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
    fclose(fp);
}

u16 ppmADat_0x6A0; // Size of frame offset table
u16 ppmADat_0x6A6; // Flags


FILE* ppmLoadAnimationData()
{
    char fn[40]="/flipnotes/";
    strcat(fn,files[7*CurrentPage+PageSelection]);
    FILE* fp=fopen(fn,"rb");
    fseek(fp,0x6A0,SEEK_SET);
    fread(&ppmADat_0x6A0,2,1,fp);
    fseek(fp,4,SEEK_CUR);
    fread(&ppmADat_0x6A8,2,1,fp);
}

#endif // FSPDS_PPM_H_INCLUDED
