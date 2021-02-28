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
    int iPos, kPos;
    u8 first_byte_header;
    s8 tX=0,tY=0; // translate parameters
    bool diffing;
	while(1)
    {
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
            if(counter==0) // vBlankOp
            {
                playerNextFrameVBlank0(&iPos,&kPos,&first_byte_header,&tX,&tY,&diffing);
            }
            else if(counter==1)
            {
                playerNextFrameVBlank1(&iPos,&kPos,&first_byte_header,&tX,&tY,&diffing);
            }
            counter++;
            if(counter==frameTime[ppm_FramePlaybackSpeed])
            {
                counter=0;
            }
        }
        else counter=0;
        swiWaitForVBlank();
	}

	return 0;
}

