#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "tabsystem.h"

int main(int argc, char **argv)
{
    initConsole();
    initTabs();
    initPlayer();

    c_loadingBox();
    swiDelay(10000);

    /*srand(time(NULL));
    layer=layer1;
    for(int y=0;y<192;y++)
        for(int x=0;x<256;x++)
            layerSet(256*y+x,(x%8==0) || (y%3==0));

    for(int y=0;y<192;y++)
        for(int x=0;x<256;x++)
        {
            u16 p=256*y+x;
            mainGFX[p]=layerGet(p)?ARGB16(1,0,0,0):ARGB16(1,31,31,31);
        }
    playerSwapBuffers();*/

    loadFiles();

    PagesCount=filescount/7;
    if(filescount%7>0)
        PagesCount++;

    CurrentTab->drawingProc();
    clearPlayer();
    playerSwapBuffers();
    clearPlayer();
    displayThumbnail();

    u16 counter=0;
	while(1)
    {
		swiWaitForVBlank();
		scanKeys();
        uint32 input=keysDown();
        if(input & KEY_L)
        {
            CurrentTab=CurrentTab->left;
            CurrentTab->loadingProc();
            CurrentTab->drawingProc();
        }
        else if (input & KEY_R)
        {
            CurrentTab=CurrentTab->right;
            CurrentTab->loadingProc();
            CurrentTab->drawingProc();
        }
        else
        {
            CurrentTab->keyDownProc(input);
        }

        if(PlayerState==PLAYING)
        {
            counter++;
            if(counter==3)
            {
                playerNextFrame();
                counter=0;
            }
        }
        //playerSwapBuffers();
	}

	return 0;
}

