#pragma once

struct _ConsoleTab
{
    struct _ConsoleTab* left;
    struct _ConsoleTab* right;
    void (*loadingProc)();
    void (*drawingProc)();
    void (*keyDownProc)(uint32);
    void (*touchRdProc)();
    void (*leavingProc)();
};

typedef struct _ConsoleTab ConsoleTab;

extern ConsoleTab* CurrentTab;

extern ConsoleTab FilesTab;
extern ConsoleTab InfoTab;
extern ConsoleTab PlayTab;

void TabNoAction();

void FilesTabDrawing();

void FilesTabKeyDown(u32 input);

void InfoTabLoading();

void InfoTabDrawing();

void InfoTabKeyDown(u32 input);

void timerCallBack();

void loadFlipnote();

void PlayTabLoading();

extern const char* PlayButton;
extern const char* PauseButton;

void PlayTabDrawing();

void PlayTabPlayButtonPressed();

void PlayTabKeyDown(uint32 input);

void PlayTabRdTouch();

void PlayTabLeaving();

void initTabs();


// void ppmwr(void* item, int listpos, int is_highlighted);
