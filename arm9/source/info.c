#include "info.h"

#include "console.h"
#include "vars.h"
#include "filesystem.h"

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
	char* fn = get_selected_file_name();    
    iprintf(fn);
	free(fn);
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

void infoPrintSoundFrequency()
{
	iprintf("Sound Frequency (Hz) : %d",soundFreq);
}

void infoPrintSoundSize()
{
	char szstr[7];
	long_to_size_string(szstr,ppm_BGMSize);
	iprintf("BGM Size : %s",szstr);
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
	infoPrintSoundSize,
    InfoPrintSeparatorLine,
	infoPrintSoundFrequency,
    InfoPrintSeparatorLine
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
