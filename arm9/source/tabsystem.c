#include "tabsystem.h"

#include "console.h"
#include "filemanager.h"
#include "ppm.h"
#include "info.h"
#include "sound.h"
#include "player.h"
#include "vars.h"
#include "ppm_list.h"
#include "flipnote_provider.h"
#include "dialog.h"
#include "convert.h"

#include "play_tab.h"

void nextPage()
{
	lis_select(&ppm_source, (ppm_source.selected_index/7+1)*7);
	displayThumbnail();
    uilist_write_page(&ppm_list);      
}

void prevPage()
{    
	lis_select(&ppm_source, (ppm_source.selected_index/7-1)*7);
	displayThumbnail();
    uilist_write_page(&ppm_list);   
}

void nextEntry()
{
	lis_select(&ppm_source, ppm_source.selected_index+1);
	displayThumbnail();	
    uilist_write_page(&ppm_list);   
}

void prevEntry()
{
	lis_select(&ppm_source, ppm_source.selected_index-1);
	displayThumbnail();
    uilist_write_page(&ppm_list);   
}


ConsoleTab* CurrentTab;

ConsoleTab FilesTab;
ConsoleTab InfoTab;
ConsoleTab PlayTab;
ConsoleTab PathSelectorTab;
ConsoleTab AboutTab;

void TabNoAction() { }

int files_tab_empty = 0;

void FilesTabLoading()
{   
	displayThumbnail(); 
}

void FilesTabDrawing()
{		
	consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
	iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,13);
    iprintf("Files");
	uilist_write_page(&ppm_list);    
	files_tab_empty = 0;
	if(ppm_loadMetadata()!=0) // try to load the first ppm file (pos 0 in ppm list), if exists
	{
		files_tab_empty = 1;
		char* msg=malloc(256);
		strcpy(msg,"Folder is empty\nThere are no flipnotes here\n\n\n\n");
		strcat(msg, ppm_current_path);
		c_displayError(msg,false);
		free(msg);
		/*c_goto(8,9);
		iprintf("Folder is empty");
		c_goto(13,5);
		iprintf("There are no flipnotes");
		c_goto(14,14);
		iprintf("here");*/
		return;
	}
}

void FilesTabKeyDown(u32 input)
{	
    if(input & KEY_RIGHT)
	{
        if(!files_tab_empty) nextPage();
	}
    else if(input & KEY_LEFT)
	{
        if(!files_tab_empty) prevPage();
	}
    else if(input & KEY_DOWN)
	{
        if(!files_tab_empty) nextEntry();
	}
    else if(input & KEY_UP)
	{
        if(!files_tab_empty) prevEntry();
	}
    else if(input & KEY_A)
    {
		if(files_tab_empty) return;
        PlayerState=PLAYING;
        PlayerFrameIndex=0;
        PlayerForceAnimationReload=true;
        PlayerThumbnailNeedsRedraw=true;
        CurrentTab=&PlayTab;
        CurrentTab->loadingProc();
        CurrentTab->drawingProc();
    }
	else if(input & KEY_B)
	{
		CurrentTab->leavingProc();
		CurrentTab=&PathSelectorTab;
		CurrentTab->loadingProc();
		CurrentTab->drawingProc();
	}
	else if(input & KEY_SELECT)
	{
		const int ACTION_CHK_INTEGRITY = 0;
		const int ACTION_EXPORT = 1;
		int action = show_dialog(15,18,15,4, "Options", "\n*Chk.Integrity\n*Export As...\n");
		switch(action)
		{
			case ACTION_CHK_INTEGRITY:
			{
				show_dialog(6,10,20,3,"Error","\nWork in progress...");
				break;
			}
			case ACTION_EXPORT:
			{
				int format=show_dialog(23,16,7,6, "", "* AMV\n* AVI\n* DPG4\n* GIF\n* MKV\n* MP4");
				if(format>=0)
				{
					char* fn = get_selected_file_name();
					convert(fn, (ConversionFormat)format);
					free(fn);
					show_dialog(6,10,20,3,"Error","\nWork in progress...");
				}
				break;
			}
		}
	}
}

void InfoTabLoading()
{
    ppm_loadMetadata();
}

void InfoTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,14);
    iprintf("Info");
    InfoDisplay();
}

void InfoTabKeyDown(u32 input)
{
    if(input & KEY_DOWN)
    {
        if(InfoScrollPos<INFO_COUNT-1)
        {
            InfoScrollPos++;
            InfoDisplay();
        }
    }
    else if(input & KEY_UP)
    {
        if(InfoScrollPos>0)
        {
            InfoScrollPos--;
            InfoDisplay();
        }
    }
}


void PathSelectorDrawing()
{
	consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();	    
	c_goto(0,14);
    iprintf("Path");
	uilist_write_page(&path_selector_list);
}

void PathSelectorTabKeyDown(u32 input)
{
	if(input & KEY_DOWN)
	{
		lis_select(&path_selector_source, path_selector_source.selected_index+1);
		uilist_write_page(&path_selector_list);   
	}
	else if(input & KEY_UP)
	{
		lis_select(&path_selector_source, path_selector_source.selected_index-1);
		uilist_write_page(&path_selector_list); 
	}
	else if(input & KEY_A)
	{		
		int index = (int)lis_get_selected_item(&path_selector_source);
		index--;		
		if(strcmp(ppm_locations[index].description,"About FSPDS")==0)
		{
			CurrentTab->leavingProc();
			CurrentTab=&AboutTab;
			CurrentTab->loadingProc();
			CurrentTab->drawingProc();					
			return;
		}
		provider_init(ppm_locations[index].path);
		set_current_path(ppm_locations[index].path);
		CurrentTab->leavingProc();
		CurrentTab=&FilesTab;
		CurrentTab->loadingProc();
		CurrentTab->drawingProc();		
	}
}

#include "version.h"

void AboutTabDrawing()
{
	consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
	iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,13);
    iprintf("About");
	c_goto(2,2);
	iprintf("Build V%u.%u.%lu.%c",MAJOR, MINOR, BUILD, BUILD_TYPE);	
	c_goto(3,2);
	iprintf("Created by:");
	c_goto(4,2);
	iprintf("    NotImplementedLife");
}

void AboutTabKeyDown(u32 input)
{
	CurrentTab->leavingProc();
	CurrentTab=&PathSelectorTab;
	CurrentTab->loadingProc();
	CurrentTab->drawingProc();	
}

void initTabs()
{	
	initPPMLists();	
	
    FilesTab.loadingProc=FilesTabLoading;
    FilesTab.drawingProc=FilesTabDrawing;
    FilesTab.keyDownProc=FilesTabKeyDown;
    FilesTab.touchRdProc=TabNoAction;
    FilesTab.leavingProc=TabNoAction;
    FilesTab.left=&PlayTab;
    FilesTab.right=&InfoTab;

    InfoTab.loadingProc=InfoTabLoading;
    InfoTab.drawingProc=InfoTabDrawing;
    InfoTab.keyDownProc=InfoTabKeyDown;
    InfoTab.touchRdProc=TabNoAction;
    InfoTab.leavingProc=TabNoAction;
    InfoTab.left=&FilesTab;
    InfoTab.right=&PlayTab;

    PlayTab.loadingProc=PlayTabLoading;
    PlayTab.drawingProc=PlayTabDrawing;
    PlayTab.keyDownProc=PlayTabKeyDown;
    PlayTab.touchRdProc=PlayTabRdTouch;
    PlayTab.leavingProc=PlayTabLeaving;
    PlayTab.left=&InfoTab;
    PlayTab.right=&FilesTab;
		
	PathSelectorTab.loadingProc = TabNoAction;
	PathSelectorTab.drawingProc = PathSelectorDrawing;
	PathSelectorTab.keyDownProc = PathSelectorTabKeyDown;
	PathSelectorTab.leavingProc = TabNoAction;

	AboutTab.loadingProc=TabNoAction;
    AboutTab.drawingProc=AboutTabDrawing;
    AboutTab.keyDownProc=AboutTabKeyDown;
    AboutTab.touchRdProc=TabNoAction;
    AboutTab.leavingProc=TabNoAction;

    CurrentTab=&PathSelectorTab;
    //CurrentTab=&FilesTab;
}
