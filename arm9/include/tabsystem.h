#ifndef FSPDS_TABSYSTEM_H_INCLUDED
#define FSPDS_TABSYSTEM_H_INCLUDED

#include "console.h"
#include "ppm.h"
#include "info.h"


struct ConsoleTab
{
    struct ConsoleTab* left;
    struct ConsoleTab* right;
    void (*loadingProc)();
    void (*drawingProc)();
    void (*keyDownProc)(uint32);
    void (*touchRdProc)();
    void (*leavingProc)();
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

void FilesTabKeyDown(u32 input)
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
        PlayerThumbnailNeedsRedraw=true;
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
    InfoDisplay();
}

void InfoTabKeyDown(u32 input)
{
    if(input & KEY_DOWN)
    {
        if(InfoScrollPos<INFO_COUNT-1)
        {
            InfoScrollPos++;
            InfoDisplay();
        }
    }
    else if(input & KEY_UP)
    {
        if(InfoScrollPos>0)
        {
            InfoScrollPos--;
            InfoDisplay();
        }
    }
}


void PlayTabLoading()
{
    if(PlayerForceAnimationReload)
    {
        c_loadingBox();
        ppm_loadMetadata();
        ppm_loadAnimationData();
        PlayerForceAnimationReload=false;
        PlayerLoadedFileIndex=7*CurrentPage+PageSelection;
    }
    else
    {

    }
}

const char* PlayButton="\033[10;14H\026\027  \033[11;14H\002\002\026\027\033[12;14H\002\002\x18\x19\033[13;14H\x18\x19  ";
const char* PauseButton="\033[10;14H\x05\x06\x05\x06\033[11;14H\x05\x06\x05\x06\033[12;14H\x05\x06\x05\x06\033[13;14H\x05\x06\x05\x06";

void PlayTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,13);
    iprintf("Player");
    c_drawBorder(8,12,15,19);
    iprintf(PlayerState ? PlayButton : PauseButton);
}

void PlayTabPlayButtonPressed()
{
    s16 index=7*CurrentPage+PageSelection;
    if(PlayerLoadedFileIndex!=index)
    {
        c_goto(18,10);
        iprintf("Loading...");
        PlayerFrameIndex=0;
        ppm_loadAnimationData();
        PlayerLoadedFileIndex=7*CurrentPage+PageSelection;
        c_goto(18,10);
        iprintf("          ");
        PlayerLoadedFileIndex=index;
    }
    PlayerState=1-PlayerState;
    iprintf(PlayerState ? PlayButton : PauseButton);
    PlayerThumbnailNeedsRedraw=true;
}

void PlayTabKeyDown(uint32 input)
{
    if(input & KEY_A)
    {
        PlayTabPlayButtonPressed();
    }
}

void PlayTabRdTouch()
{
    if(104<=touch.px && touch.px<152 && 72<=touch.py && touch.py<120)
        PlayTabPlayButtonPressed();

}

void PlayTabLeaving()
{
    PlayerState=PAUSED;
    if(PlayerThumbnailNeedsRedraw)
    {
        playerClear();
        playerSwapBuffers();
        playerClear();
        displayThumbnail();
        PlayerThumbnailNeedsRedraw=false;
        counter=0;
    }
}

void initTabs()
{
    FilesTab.loadingProc=TabNoAction;
    FilesTab.drawingProc=FilesTabDrawing;
    FilesTab.keyDownProc=FilesTabKeyDown;
    FilesTab.touchRdProc=TabNoAction;
    FilesTab.leavingProc=TabNoAction;
    FilesTab.left=&PlayTab;
    FilesTab.right=&InfoTab;

    InfoTab.loadingProc=InfoTabLoading;
    InfoTab.drawingProc=InfoTabDrawing;
    InfoTab.keyDownProc=InfoTabKeyDown;
    InfoTab.touchRdProc=TabNoAction;
    InfoTab.leavingProc=TabNoAction;
    InfoTab.left=&FilesTab;
    InfoTab.right=&PlayTab;

    PlayTab.loadingProc=PlayTabLoading;
    PlayTab.drawingProc=PlayTabDrawing;
    PlayTab.keyDownProc=PlayTabKeyDown;
    PlayTab.touchRdProc=PlayTabRdTouch;
    PlayTab.leavingProc=PlayTabLeaving;
    PlayTab.left=&InfoTab;
    PlayTab.right=&FilesTab;


    CurrentTab=&FilesTab;
}

#endif // FSPDS_TABSYSTEM_H_INCLUDED
