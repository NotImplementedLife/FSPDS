#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "tabsystem.h"

int main(int argc, char ** argv)
{
    powerOn(POWER_ALL_2D);
    soundEnable();

    initConsole();

    initTabs();
    initPlayer();

    c_loadingBox();
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
                /// counter=0; // <-- force 30fps [DEBUG]
            }
            if(counter==frameTime[ppm_FramePlaybackSpeed])
            {
                counter=0;
            }
        }
	}

	return 0;
}
/*#include <stdio.h>
#include <nds.h>
#include <nds/ndstypes.h>

int sound = 0;
int sndStatus = 0;
touchPosition TouchStructure;
u16 Pressed;
u16 Held;
u16 Released;
const int FREQ_MUL = 100;
const int VOLUME = 64;
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	int xPos = 120;
	int yPos = 40;
	int span = 64;
	int duty = 50;
	consoleDemoInit();
	soundEnable();
	sound = soundPlayPSG(duty,yPos * FREQ_MUL,VOLUME,span);
	soundPause(sound);
	while(1)
	{
		swiWaitForVBlank();

		scanKeys();
		Pressed = keysDown();
		Held = keysHeld();
		Released = keysUp();

		touchRead(&TouchStructure);
		if(TouchStructure.px > 0)
		{
			xPos = TouchStructure.px;
			span = div32(xPos, 2);
			soundSetPan(sound, span);
			iprintf("xPos: %d - pan: %d\n", xPos, span);
		}
		if(TouchStructure.py > 0)
		{
			yPos = TouchStructure.py;
			iprintf("yPos: %d\n", yPos);
		}

		if(KEY_A & Pressed && sndStatus == 0)
		{
			soundResume(sound);
			sndStatus = 1;
			iprintf("Sound On\n");
		}
		else if(KEY_A & Pressed && sndStatus == 1)
		{
			soundPause(sound);
			sndStatus = 0;
			iprintf("Sound OFF\n");
		}

		soundSetFreq(sound, yPos * FREQ_MUL);

		if(KEY_L & Pressed)
			if(duty >= 5)
				duty -= 5;
		if(KEY_R & Pressed)
			if(duty <= 95)
				duty += 5;

		soundSetWaveDuty(sound, duty);

		if(KEY_B & Pressed)
			iprintf("Freq is %d, pan is %d, duty is%d\n", yPos, span, duty);
	}
}*/

