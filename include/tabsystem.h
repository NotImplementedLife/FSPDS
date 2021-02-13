#ifndef FSPDS_TABSYSTEM_H_INCLUDED
#define FSPDS_TABSYSTEM_H_INCLUDED

#include "console.h"
#include "ppm.h"


struct ConsoleTab
{
    struct ConsoleTab* left;
    struct ConsoleTab* right;
    void (*loadingProc)();
    void (*drawingProc)();
    void (*keyDownProc)(uint32);
};

struct ConsoleTab* CurrentTab;

struct ConsoleTab FilesTab;
struct ConsoleTab InfoTab;
struct ConsoleTab PlayTab;

void TabNoAction() { }

void FilesTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    c_writeFrame();
    c_goto(0,13);
    iprintf("Files");
    writePage();
}

void FilesTabKeyDown(uint32 input)
{
    if(input & KEY_RIGHT)
        nextPage();
    else if(input & KEY_LEFT)
        prevPage();
    else if(input & KEY_DOWN)
        nextEntry();
    else if(input & KEY_UP)
        prevEntry();
    else if(input & KEY_A)
    {
        PlayerState=PLAYING;
        PlayerFrameIndex=0;
        PlayerForceAnimationReload=true;
        CurrentTab=&PlayTab;
        CurrentTab->loadingProc();
        CurrentTab->drawingProc();
    }
}

void InfoTabLoading()
{
    ppm_loadMetadata();
}

void InfoTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,14);
    iprintf("Info");
    c_goto(1,2);
    iprintf(files[7*CurrentPage+PageSelection]);
    c_goto(2,1);
    iprintf("Frames count: %i",ppmHead_FrameCount);
    c_goto(3,1);
    iprintf("Locked      : ");
    iprintf(ppmMeta_Lock==1?"YES":"NO");
}

void PlayTabLoading()
{
    if(PlayerForceAnimationReload)
    {
        c_loadingBox();
        ppm_loadAnimationData();
        PlayerForceAnimationReload=false;
    }
}

void PlayTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,13);
    iprintf("Player");
    // To do : Add interface

}

void PlayTabKeyDown(uint32 input)
{
    if(input & KEY_A)
    {
        clearPlayer();
        playerNextFrame();
        //c_goto(1,1);
        //iprintf("%u\n%u\n%u",PlayerFrameIndex,ppmHead_AnimationDataSize,ppmHead_FrameCount);
    }
}

void initTabs()
{
    FilesTab.loadingProc=TabNoAction;
    FilesTab.drawingProc=FilesTabDrawing;
    FilesTab.keyDownProc=FilesTabKeyDown;
    FilesTab.left=&PlayTab;
    FilesTab.right=&InfoTab;

    InfoTab.loadingProc=InfoTabLoading;
    InfoTab.drawingProc=InfoTabDrawing;
    InfoTab.keyDownProc=TabNoAction;
    InfoTab.left=&FilesTab;
    InfoTab.right=&PlayTab;

    PlayTab.loadingProc=PlayTabLoading;
    PlayTab.drawingProc=PlayTabDrawing;
    PlayTab.keyDownProc=PlayTabKeyDown;
    PlayTab.left=&InfoTab;
    PlayTab.right=&FilesTab;


    CurrentTab=&FilesTab;
}

#endif // FSPDS_TABSYSTEM_H_INCLUDED
