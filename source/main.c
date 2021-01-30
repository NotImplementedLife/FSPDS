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

    loadFiles();


    /**filescount=25;
    char* fname=(char*)malloc(29*sizeof(char));
    char* fsize=(char*)malloc(7*sizeof(char));
    for(int i=0;i<filescount;i++)
    {
        sprintf(fname,"000000_0000000000000_%03d.ppm",i);
        long_to_size_string(fsize,rand()%(1<<20));
        strcpy(files[i],fname);
        strcpy(sizes[i],fsize);
    }
    */


    PagesCount=filescount/7;
    if(filescount%7>0)
        PagesCount++;

    CurrentTab->drawingProc();

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
	}

	return 0;
}

