#include "tabsystem.h"

#include "console.h"
#include "filesystem.h"
#include "ppm.h"
#include "info.h"
#include "sound.h"
#include "player.h"
#include "vars.h"
#include "ppm_list.h"

void ppm_write_entry(void* item, int listpos, int is_highlighted)
{			
	if(item==NULL) 
	{
		consoleSelect(&consoleFG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }
        consoleSelect(&consoleBG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }		
        return;
	}
		
	file_data* fd = (file_data*)item;	
	
	consoleSelect(&consoleBG);
    iprintf(is_highlighted?"\x1b[39m":"\x1b[30m");
    for(int i=0;i<3;i++)
    {
        c_goto(1+3*listpos+i,1);
        for(int j=0;j<30;j++) iprintf("\x02");
    }
    consoleSelect(&consoleFG);
    c_goto(1+3*listpos,2);
    iprintf(is_highlighted?"\x1b[30m":"\x1b[39m");
    iprintf(fd->name);	
    if(listpos==7) return;
    c_goto(3+3*listpos,1);
	iprintf("                              ");
	c_goto(2+3*listpos,1);
	iprintf("                              ");
    c_goto(2+3*listpos,20);	
    iprintf(fd->size_str);
}

/*void ppmwr(void* item, int listpos, int is_highlighted)
{				
	c_goto(1+listpos,1);
	if(is_highlighted) iprintf("*");
	else iprintf(" ");
	if(item==NULL) 
	{				
		iprintf("[NULL]");
        return;
	}
	file_data* fd = (file_data*)item;			    
	iprintf(fd->name);	
	
	if(is_highlighted)
	{
		c_goto(0,0);
		iprintf("%p", (void*)fd->name);
	}
	return;	
}*/

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

void TabNoAction() { }

void FilesTabDrawing()
{
    consoleSelect(&consoleBG); c_cls();
    consoleSelect(&consoleFG); c_cls();
    c_writeFrame();
    c_goto(0,13);
    iprintf("Files");
	uilist_write_page(&ppm_list);    
}

void FilesTabKeyDown(u32 input)
{
    if(input & KEY_RIGHT)
        nextPage();
    else if(input & KEY_LEFT)
        prevPage();
    else if(input & KEY_DOWN)
        nextEntry();
    else if(input & KEY_UP)
        prevEntry();
    else if(input & KEY_A)
    {
        PlayerState=PLAYING;
        PlayerFrameIndex=0;
        PlayerForceAnimationReload=true;
        PlayerThumbnailNeedsRedraw=true;
        CurrentTab=&PlayTab;
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

void loadFlipnote()
{
    soundKill(BGMId);
    ppm_loadMetadata();
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
        soundSetFreq(BGMId,0); //pause the song
        playerClear();
        playerSwapBuffers();
        playerClear();
        displayThumbnail();
        PlayerThumbnailNeedsRedraw=false;
        counter=0;
    }
}

void initTabs()
{	
	initPPMLists();
	uilist_set_write_entry_proc(&ppm_list, ppm_write_entry);
	//uilist_set_write_entry_proc(&ppm_list, ppmwr);
	
    FilesTab.loadingProc=TabNoAction;
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


    CurrentTab=&FilesTab;
}
