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

void initTabs();