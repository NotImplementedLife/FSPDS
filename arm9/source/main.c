#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "tabsystem.h"

int main(int argc, char ** argv)
{
    powerOn(POWER_ALL_2D);

    initConsole();
    initTabs();
    initPlayer();

    c_loadingBox();
    c_goto(0,0);
    iprintf("%i",bgGetMapBase(bgMain));

    loadFiles();

    PagesCount=filescount/7;
    if(filescount%7>0)
        PagesCount++;
    if(filescount>0)
    {
        ppm_loadMetadata();
    }

    CurrentTab->drawingProc();
    playerClear();
    playerSwapBuffers();
    playerClear();
    displayThumbnail();

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
            counter++;
            if(counter==1)
            {
                playerNextFrameVBlank0(&dt);
            }
            else if(counter==2)
            {
                playerNextFrameVBlank1(&dt);
            }
            if(counter==frameTime[ppm_FramePlaybackSpeed])
            {
                //c_goto(1,1);
                //iprintf("%d",ppm_FramePlaybackSpeed);
                counter=0;
            }
        }
        /**if(PlayerFrameIndex==252)
        {
            counter=100;
            c_goto(1,1);
            iprintf("%d %d %d   ",dt.diffing,dt.tX,dt.tY);
        }*/
	}

	return 0;
}

