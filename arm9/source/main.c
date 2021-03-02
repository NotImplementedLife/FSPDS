#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "tabsystem.h"

time_t rawTime;

void clockUpdater()
{
    rawTime++;
}

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

    time(&rawTime);
    timerStart(0,ClockDivider_1024,TIMER_FREQ_1024(1),clockUpdater);
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
            //playerNextFrame();
            //counter++;
            /*if(counter==1)
            {
                playerNextFrameVBlank0(&dt);
            }
            else*/ //if(counter==2)
            {
                //playerNextFrameVBlank1(&dt);
                //playerNextFrame();
                //counter=0;
            }
            //playerNextFrameVBlank0(&dt);
            //swiWaitForVBlank();
            //playerNextFrameVBlank1(&dt);
            playerNextFrame();
        }
	}

	return 0;
}

