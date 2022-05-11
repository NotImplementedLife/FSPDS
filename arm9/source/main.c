#include <nds.h>

#include "console.h"
#include "filesystem.h"
#include "player.h"
#include "ppm.h"
#include "tabsystem.h"

#include "ui_list.h"
#include "ppm_list.h"

void ppm_write_entry(void* item, int listpos, int is_highlighted)
{	
	c_goto(2*listpos+1, 0);
	if(is_highlighted) iprintf("*");
	else iprintf(" ");
	if(item==NULL) iprintf("[NULL]\n");
	else
	{		
		file_data* fd = (file_data*)item;
		iprintf("%s\n             %s\n",fd->name, fd->size_str);
	}
}


void callback(file_data* fd, void* arg)
{
	iprintf("%s\n",fd->name);
	int *y=(int*)arg;
	(*y)++;	
}

int main(int argc, char ** argv)
{
	consoleDemoInit();
	fsInit();
	initPPMLists();
	
	uilist_set_write_entry_proc(&ppm_list, ppm_write_entry);
	iprintf("123\n");		
	
	UiList* list = &ppm_list;
	ListItemsSource* lis = &ppm_source;
	
	c_cls();
	uilist_write_page(list);

	while(1)
    {
		swiWaitForVBlank();
		
		
		scanKeys();
        uint32 input=keysDown();		
		if(input & KEY_DOWN) {
			lis_select(lis, lis->selected_index+1);
			c_cls();
			c_goto(0,0); 			
			iprintf("% 3i",lis->selected_index);
			uilist_write_page(list);
		}
		else if(input & KEY_UP) {
			lis_select(lis, lis->selected_index-1);
			c_cls();
			c_goto(0,0); 
			iprintf("% 3i",lis->selected_index);
			uilist_write_page(list);
		}
	}
	/*UiList list;
	uilist_init(&list);
	uilist_set_write_entry_proc(&list, write_entry);
	
	ListItemsSource lis;
	lis_init(&lis);
	lis_set_chunk_procs(&lis, ld_chunk, rl_chunk);
	lis_select(&lis, 1);
	
	uilist_attach_lis(&list, &lis);
	
	uilist_write_page(&list);
	
	while(1)
    {
		swiWaitForVBlank();
		scanKeys();
        uint32 input=keysDown();		
		if(input & KEY_DOWN) {
			lis_select(&lis, lis.selected_index+1);
			c_goto(0,0); 
			iprintf("% 3i",lis.selected_index);
			uilist_write_page(&list);
		}
		else if(input & KEY_UP) {
			lis_select(&lis, lis.selected_index-1);
			c_goto(0,0); 
			iprintf("% 3i",lis.selected_index);
			uilist_write_page(&list);
		}
	}*/
	
	return 0;
	
	
	//fsInit();
	
    powerOn(POWER_ALL_2D);
	
	// Reserve VRAM 0x06040000 - 0x0607FFFF for flipnote work data 
	// 0x40000 = 256KB
	//vramSetBankC(VRAM_C_MAIN_BG_0x06040000);
	//vramSetBankD(VRAM_D_MAIN_BG_0x06060000);
	
	//vramSetBankE(VRAM_E_MAIN_SPRITE);
	//vramSetBankF(VRAM_F_MAIN_SPRITE_0x06410000);
	//vramSetBankG(VRAM_G_MAIN_SPRITE_0x06414000);
	
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