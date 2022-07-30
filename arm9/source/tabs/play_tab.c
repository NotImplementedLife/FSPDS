#include "play_tab.h"
#include "tabsystem.h"

#include <nds.h>
#include "console.h"
#include "player.h"
#include "ppm.h"
#include "ppm_list.h"

#include "bottom_screen_power.h"

#include "bulb.h"
#include "bar-fragments.h"


int power_off_timer = 0;

void timerCallBack()
{
    sound_frame_counter += 4;
	power_off_timer += 4;
	if(power_off_timer>=5*soundFreq)
	{
		bottom_screen_power_off();
	}
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
	dmaCopy(bulbTiles, (int*)0x06600000, bulbTilesLen);
	dmaCopy(bulbPal, SPRITE_PALETTE_SUB, bulbPalLen);
	oamSet(&oamSub, 0, 224, 16, 0, 0,
			SpriteSize_16x16, SpriteColorFormat_256Color, (int*)0x06600000, -1, 0, 0, 0, 0, 0);
	
	dmaCopy(bar_fragmentsTiles, (int*)(0x06600000+bulbTilesLen),bar_fragmentsTilesLen);
	
	// manually align bar fragments palette offset because grit doesn't want to
	u16* buffer = (u16*)(0x06600000+bulbTilesLen);
	int len=bar_fragmentsTilesLen/2;
	volatile u16 val; // prevent compiler optimization, possibly leading to 8-bit VRAM writes
	int offset=bulbPalLen/2;
	for(int i=0;i<len;i++)
	{
		val = buffer[i];
		if(val & 0x00FF) val += offset-1;
		if(val & 0xFF00) val += (offset-1)<<8;
		buffer[i]=val;
	}	
	
	SPRITE_PALETTE_SUB[offset]=bar_fragmentsPal[1];
	
	for(int i=1;i<=14;i++)
	{
		int tileno=0x26;
		if(i==1) tileno=0x4;
		else if(i==14) tileno=0x48;
		oamSet(&oamSub, i, 16*i, 144, 0, 0,
			SpriteSize_16x8, SpriteColorFormat_256Color, (int*)(0x06600000+64*tileno), -1, 0, 0, 0, 0, 0);
	}
	
	
    if(PlayerForceAnimationReload)
    {
        c_loadingBox();
        loadFlipnote();		
		//c_displayError("CHECKPOINT!\nYou need custom papers\n\n",false);
    }
    else
    {

    }
}

void setFragValue(int id /* = 1..14*/, int val /* = 0..16 */)
{
	int tileno = (id==1 ? 0x4 : id==14 ? 0x48 : 0x26) + 2 * val;
	oamSetGfx(&oamSub, id, SpriteSize_16x8, SpriteColorFormat_256Color, (int*)(0x06600000+64*tileno));
}

void setPlaybarValue(int val /* = 0..256 */)
{
	val = (val*224)>>8;
	int full_frags = val>>4;
	int v_crt_frag = val & 0xF;
	for(int i=1;i<=full_frags;i++) 
		setFragValue(i, 16);
	setFragValue(full_frags+1, v_crt_frag);
	for(int i=full_frags+2;i<=14;i++)
		setFragValue(i, 0);
}

const char* PlayButton="\033[10;14H\026\027  \033[11;14H\002\002\026\027\033[12;14H\002\002\x18\x19\033[13;14H\x18\x19  ";
const char* PauseButton="\033[10;14H\x05\x06\x05\x06\033[11;14H\x05\x06\x05\x06\033[12;14H\x05\x06\x05\x06\033[13;14H\x05\x06\x05\x06";
const char* BeginButton="\033[11;9H\x1B\x1C\033[12;9H\x1D\x1E";
const char* EndButton="\033[11;21H\x16\x17\033[12;21H\x18\x19";
const char* BackButton="\033[11;5H\x80\x84\033[12;5H\x81\x85";
const char* ForwardButton="\033[11;25H\x86\x82\033[12;25H\x87\x83";

u8 x=0;

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
	
	c_drawBorder(10,3,13,7);
	printf(BackButton);

	c_drawBorder(9,8,14,11);
	printf(BeginButton);
	
	c_drawBorder(9,20,14,23);
	printf(EndButton);
	
	c_drawBorder(10,24,13,28);
	printf(ForwardButton);
	
	setPlaybarValue(x++);
	
	oamUpdate(&oamSub);	
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
	if(!is_bottom_screen_on)
	{
		bottom_screen_power_on();
		power_off_timer=0;
		return;
	}
	
    if(input & KEY_A)
    {
        PlayTabPlayButtonPressed();
    }
}

void PlayTabRdTouch()
{
	if(!is_bottom_screen_on)
	{
		bottom_screen_power_on();
		power_off_timer=0;
		return;
	}
    if(104<=touch.px && touch.px<152 && 72<=touch.py && touch.py<120)
	{
        PlayTabPlayButtonPressed();
	}
	if((touch.px>>4)==14 && (touch.py>>4)==1)
	{			
		bottom_screen_power_off();
	}
}

void PlayTabLeaving()
{
    PlayerState=PAUSED;	
	oamClear(&oamSub,0,128);
	oamUpdate(&oamSub);	
	
	bottom_screen_power_on();				
	power_off_timer=0;	
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