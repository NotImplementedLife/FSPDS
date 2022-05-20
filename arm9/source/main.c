#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "ppm.h"
#include "tabsystem.h"
#include "flipnote_provider.h"

#include "ui_list.h"
#include "ppm_list.h"

void run_dbg()
{
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
	
	provider_init("/flipnotes");	

    initTabs();
    initPlayer();

    c_loadingBox();
	swiWaitForVBlank();
		

	// error source if no flipnotes have been loaded:       
    ppm_loadMetadata();
	
    playerClear();
    playerSwapBuffers();
    playerClear();	
		
	CurrentTab = &FilesTab;
	CurrentTab->loadingProc();
    CurrentTab->drawingProc();    
	
	while(1)
    {				
		swiWaitForVBlank();
		scanKeys();
		uint32 input=keysDown();
		CurrentTab->keyDownProc(input);
		
		u8 trigger = 0;
		provider_background(&trigger);
		if(trigger && CurrentTab==&FilesTab)
		{
			uilist_write_page(&ppm_list);    
		}
		/*if(PlayerState==PLAYING)
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
        }*/
					
	}
}

int main(int argc, char ** argv)
{
	run_dbg();
	
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

	// error source if no flipnotes have been loaded:       
    ppm_loadMetadata();
		
	
    playerClear();
    playerSwapBuffers();
    playerClear();
	
	CurrentTab->loadingProc();
    CurrentTab->drawingProc();    
	
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
			if(CurrentTab->left !=NULL)
			{
				CurrentTab->leavingProc();
				CurrentTab=CurrentTab->left;
				CurrentTab->loadingProc();
				CurrentTab->drawingProc();
			}
        }
        else if (input & KEY_R)
        {
			if(CurrentTab->right !=NULL)
			{
				CurrentTab->leavingProc();
				CurrentTab=CurrentTab->right;
				CurrentTab->loadingProc();
				CurrentTab->drawingProc();
			}
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