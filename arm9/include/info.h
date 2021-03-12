#ifndef FSPDS_INFO_H_INCLUDED
#define FSPDS_INFO_H_INCLUDED

#define INFO_COUNT 11

void noprint(u8 r) {}

void InfoPrintSeparatorLine()
{
    for(u8 i=0;i<10;i++) iprintf("---");
}

void InfoPrintFileNameLabel()
{
    iprintf("File name:");
}

void InfoPrintFileNameValue()
{
    iprintf(" ");
    iprintf(files[7*CurrentPage+PageSelection]);
}

void InfoPrintFramesCount()
{
    iprintf("Frames count: %u",ppmHead_FrameCount);
}

void InfoPrintLockState()
{
    iprintf("Locked : ");
    iprintf(ppmMeta_Lock?"YES":"NO ");
}

void InfoPrintFramePlaybackSpeed()
{
    iprintf("Playback Speed : %d",ppm_FramePlaybackSpeed);
}

void InfoPrintRecordingFramePlaybackSpeed()
{
    iprintf("Recording Playback Speed : %d",ppm_RecordedPlaybackSpeed);
}

void (*InfoLine[INFO_COUNT])()=
{
    InfoPrintFileNameLabel,
    InfoPrintFileNameValue,
    InfoPrintSeparatorLine,
    InfoPrintFramesCount,
    InfoPrintSeparatorLine,
    InfoPrintLockState,
    InfoPrintSeparatorLine,
    InfoPrintFramePlaybackSpeed,
    InfoPrintSeparatorLine,
    InfoPrintRecordingFramePlaybackSpeed,
    InfoPrintSeparatorLine,
};

u8 InfoScrollPos=0;

void InfoDisplay()
{
    consoleSetWindow(&consoleFG,1,1,30,22);
    consoleClear();
    c_goto(0,0);
    for(u8 i=1,p=InfoScrollPos;i<31;i++)
    {
        if(p>=INFO_COUNT)
            break;
        InfoLine[p++](i);
        iprintf("\n");
    }
    consoleSetWindow(&consoleFG,0,0,32,24);
}



#endif // INFO_H_INCLUDED
