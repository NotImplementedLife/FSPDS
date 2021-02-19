#ifndef FSPDS_INFO_H_INCLUDED
#define FSPDS_INFO_H_INCLUDED

#define INFO_COUNT 7

void noprint(u8 r) {}

void InfoPrintSeparatorLine(u8 r)
{
    c_goto(r,1);
    for(u8 i=0;i<10;i++) iprintf("---");
}

void InfoPrintFileNameLabel(u8 r)
{
    c_goto(r,1);
    iprintf("File name:");
}

void InfoPrintFileNameValue(u8 r)
{
    c_goto(r,2);
    iprintf(files[7*CurrentPage+PageSelection]);
}

void InfoPrintFramesCount(u8 r)
{
    c_goto(r,1);
    iprintf("Frames count: %u",ppmHead_FrameCount);
}

void InfoPrintLockState(u8 r)
{
    c_goto(r,1);
    iprintf("Locked : ");
    iprintf(ppmMeta_Lock?"YES":"NO ");
}

void (*InfoLine[INFO_COUNT])()=
{
    InfoPrintFileNameLabel,
    InfoPrintFileNameValue,
    InfoPrintSeparatorLine,
    InfoPrintFramesCount,
    InfoPrintSeparatorLine,
    InfoPrintLockState,
    InfoPrintSeparatorLine
};

u8 InfoScrollPos=0;

void InfoDisplay()
{
    for(u8 i=1,p=InfoScrollPos;i<31;i++)
    {
        if(p>=INFO_COUNT)
            break;
        InfoLine[p++](i);
    }
}



#endif // INFO_H_INCLUDED
