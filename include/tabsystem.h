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
}

void InfoTabLoading()
{
    ppm_loadMetadata();
}

void InfoTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    c_writeFrame();
    c_goto(0,14);
    iprintf("Info");
    c_goto(1,2);
    iprintf(files[7*CurrentPage+PageSelection]);
    c_goto(2,1);
    iprintf("Frames count: %i",ppmHead_FrameCount);
    c_goto(3,1);
    iprintf("Lock        : ");
    iprintf(ppmMeta_Lock==1?"YES":"NO");
}

void initTabs()
{
    FilesTab.loadingProc=TabNoAction;
    FilesTab.drawingProc=FilesTabDrawing;
    FilesTab.keyDownProc=FilesTabKeyDown;
    FilesTab.left=FilesTab.right=&InfoTab;

    InfoTab.loadingProc=InfoTabLoading;
    InfoTab.drawingProc=InfoTabDrawing;
    InfoTab.keyDownProc=TabNoAction;
    InfoTab.left=InfoTab.right=&FilesTab;

    CurrentTab=&FilesTab;
}

#endif // FSPDS_TABSYSTEM_H_INCLUDED
