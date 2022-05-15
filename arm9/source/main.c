#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "ppm.h"
#include "tabsystem.h"

#include "ui_list.h"
#include "ppm_list.h"

void run_dbg()
{
	consoleDemoInit();
	fsInit();
	initPPMLists();
	
	uilist_set_write_entry_proc(&ppm_list, ppmwr);
	
	c_cls();
	uilist_write_page(&ppm_list);

	while(1)
    {
		swiWaitForVBlank();
		scanKeys();
        uint32 input=keysDown();		
		if(input & KEY_DOWN) {
			lis_select(&ppm_source, ppm_source.selected_index+1);
			c_cls();
			c_goto(0,0); 			
			iprintf("% 3i",ppm_source.selected_index);
			uilist_write_page(&ppm_list);
		}
		else if(input & KEY_UP) {
			lis_select(&ppm_source, ppm_source.selected_index-1);
			c_cls();
			c_goto(0,0); 
			iprintf("% 3i",ppm_source.selected_index);
			uilist_write_page(&ppm_list);
		}
	}
	
	exit(0);
}

int main(int argc, char ** argv)
{
	//run_dbg();
	
    powerOn(POWER_ALL_2D);
	
	// Reserve VRAM 0x06040000 - 0x0607FFFF for flipnote work data 
	// 0x40000 = 256KB
	//vramSetBankC(VRAM_C_MAIN_BG_0x06040000);
	//vramSetBankD(VRAM_D_MAIN_BG_0x06060000);
	
	//vramSetBankE(VRAM_E_MAIN_SPRITE);
	//vramSetBankF(VRAM_F_MAIN_SPRITE_0x06410000);
	//vramSetBankG(VRAM_G_MAIN_SPRITE_0x06414000);
	
    soundEnable();

	fsInit();
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
    //displayThumbnail();

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