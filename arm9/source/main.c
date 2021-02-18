#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "tabsystem.h"

int main(int argc, char ** argv)
{
    initConsole();
    initTabs();
    initPlayer();

    c_loadingBox();

    loadFiles();

    PagesCount=filescount/7;
    if(filescount%7>0)
        PagesCount++;

    CurrentTab->drawingProc();
    playerClear();
    playerSwapBuffers();
    playerClear();
    displayThumbnail();

    u16 counter=0;
	while(1)
    {
		swiWaitForVBlank();
		scanKeys();
        uint32 input=keysDown();
        if(input == 0)
        {
            goto __LBL__SKIP_KEY_CHK__;
        }
        if(input & KEY_L)
        {
            CurrentTab->leavingProc();
            CurrentTab=CurrentTab->left;
            CurrentTab->loadingProc();
            CurrentTab->drawingProc();
        }
        else if (input & KEY_R)
        {
            CurrentTab->leavingProc();
            CurrentTab=CurrentTab->right;
            CurrentTab->loadingProc();
            CurrentTab->drawingProc();
        }
        else if (input & KEY_TOUCH)
        {
            touchRead(&touch);
            CurrentTab->touchRdProc();
        }
        else
        {
            CurrentTab->keyDownProc(input);
        }
        __LBL__SKIP_KEY_CHK__:

        if(PlayerState==PLAYING)
        {
            playerNextFrame();
            /*counter++;
            if(counter==3)
            {
                playerNextFrame();
                counter=0;
            }*/
        }
	}

	return 0;
}

