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

void timerCallBack()
{
    sound_frame_counter+=4;
}

int loadFlipnote()
{	
    soundKill(BGMId);
    if(ppm_loadMetadata()!=0)
		return -1;
    ppm_loadAnimationData();
    ppm_loadSoundData();
    PlayerForceAnimationReload=false;
    //PlayerLoadedFileIndex=7*CurrentPage+PageSelection;	
    PlayerLoadedFileIndex = get_selected_file_index();
    BGMId=soundPlaySample(ppm_BGMData,SoundFormat_16Bit,4*ppm_BGMSize,soundFreq,100,64,false,0);
	/// timer: make it tick at a lower frequnece to avoid letting the image and sound out of sync
	/// due to many timer interrupts inside vBlank
	timerStart(0, ClockDivider_1, TIMER_FREQ(soundFreq/4), timerCallBack);
	sound_frame_counter = 0;
	return 0;
}

void PlayTabLoading()
{
    if(PlayerForceAnimationReload)
    {
        c_loadingBox();
        loadFlipnote();
		//c_goto(2,2);
		//c_displayError("CHECKPOINT!\nYou need custom papers\n\n",false);
		//iprintf("%i %i", ppm_FramePlaybackSpeed, frameTime[ppm_FramePlaybackSpeed]);
		//while(1) { swiWaitForVBlank();}
    }
    else
    {

    }
}

const char* PlayButton="\033[10;14H\026\027  \033[11;14H\002\002\026\027\033[12;14H\002\002\x18\x19\033[13;14H\x18\x19  ";
const char* PauseButton="\033[10;14H\x05\x06\x05\x06\033[11;14H\x05\x06\x05\x06\033[12;14H\x05\x06\x05\x06\033[13;14H\x05\x06\x05\x06";

void PlayTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    iprintf("\x1b[39m");
    c_writeFrame();
    c_goto(0,13);
    iprintf("Player");
    c_drawBorder(8,12,15,19);
    iprintf(PlayerState ? PlayButton : PauseButton);
}

void PlayTabPlayButtonPressed()
{	    
    s16 index = get_selected_file_index();
    if(PlayerLoadedFileIndex!=index)
    {
        c_goto(18,10);
        iprintf("Loading...");
        PlayerFrameIndex=0;
        loadFlipnote();
        c_goto(18,10);
        iprintf("          ");
    }
    PlayerState=1-PlayerState;
    if(PlayerState==PLAYING)
    {
        // to resume the song, reset the play offset to sound_frame_counter        	
		soundKill(BGMId);		
        BGMId = soundPlaySample(ppm_BGMData+2*sound_frame_counter,SoundFormat_16Bit,4*ppm_BGMSize-2*sound_frame_counter,soundFreq,100,64,false,0);
        timerUnpause(0);
		//iprintf("\033[%d;%dH %i %i", 5, 2, 2*sound_frame_counter,4*ppm_BGMSize);
    }
    else
    {
        // pause the song
        timerPause(0);
        soundPause(BGMId);
    }
    iprintf(PlayerState ? PlayButton : PauseButton);
    PlayerThumbnailNeedsRedraw = true;
}

void PlayTabKeyDown(uint32 input)
{
    if(input & KEY_A)
    {
        PlayTabPlayButtonPressed();
    }
}

void PlayTabRdTouch()
{
    if(104<=touch.px && touch.px<152 && 72<=touch.py && touch.py<120)
        PlayTabPlayButtonPressed();

}

void PlayTabLeaving()
{
    PlayerState=PAUSED;	
    if(PlayerThumbnailNeedsRedraw)
    {
        // pause the song
		timerPause(0);
		soundPause(BGMId);
        playerClear();
        playerSwapBuffers();
        playerClear();
        displayThumbnail();
        PlayerThumbnailNeedsRedraw=false;
        counter=0;
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
