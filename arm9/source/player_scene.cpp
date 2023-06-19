#include <nds.h>

#include "globals.hpp"

#include "scenes.hpp"

#include "frame_decoder.hpp"
#include "sound_decoder.hpp"

#include "0B33C4_0BC1601FB8421_000_bin.h"

#include "player_bg.h"
#include "bar_fragments.h"
#include "player_icons.h"

#include <stdlib.h>
#include <malloc.h>

void* operator new(std::size_t n)
{
	if(n>256)
		Debug::log("[new] Allocating %i bytes", n);	
	void* ptr = malloc(n);
	nds_assert(ptr!=nullptr, "Allocation failed");	
	if(n>256)
		Debug::log("[new] Reserved address %X", ptr);
	return ptr;  
}

void operator delete(void* p)
{		
	//Debug::log("free %i", malloc_usable_size(p));
	//Debug::log("[new] Freeing %X", p);
	free(p);
}

const char* selected_flipnote_path = nullptr; // "fat://flipnotes/0BC769_0A978C458A07A_002.ppm";

bool working=false;
int vbl=0;
void vblank_handler()
{
	vbl++;
	if(working)
	{
		Debug::log("WORKING----------------------------------");
		while(1) swiWaitForVBlank();
	}
}

class PlayerScene : public GenericScene256
{
private:
	Sprite* bar_fragments[14];	
	ObjFrame* bar_frames[17];
	
	ObjFrame* play_frame;
	ObjFrame* resume_frame;
	ObjFrame* replay_off_frame;
	ObjFrame* replay_on_frame;	
	
	Sprite* play_resume_button;
	Sprite* replay_button;
	 
	PPMReader* ppm_reader = new PPMReader();
	bool autoplay = false;
public:
	void init() override
	{
		GenericScene256::init();
		soundEnable();
		
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
		
		require_bitmap(SUB_BG2, &ROA_player_bg8);		
		
		/*int* d = (int*)ppm_reader;
		int* s = (int*)_0B33C4_0BC1601FB8421_000_bin;
		
		Debug::log("START = %X", d);
		
		for(int i=0;i<253196/4;i++)			
			*(d++)=*(s++);*/
		
		
		if(selected_flipnote_path!=nullptr)
		{
			int res = ppm_reader->read_file(selected_flipnote_path);							
			Debug::log("Player read finished %i", res);
			Debug::log("Animation data size = %i", ppm_reader->getAnimationDataSize());
			Debug::log("Sound data size = %i", ppm_reader->getSoundDataSize());
			Debug::log("Frames count = %i", ppm_reader->getFrameCount());		
			delete[] selected_flipnote_path;
		}
		
		
		soundFreq = ppm_reader->getSoundFreq();
		Debug::log("FREQ = %i",ppm_reader->getSoundFreq());
			
		for(int i=0;i<10;i++)
		{
			Debug::log("Frame %i", i);						
			char* f=ppm_reader->getFrame(i);
			Debug::log("Offset %X",(int)f-(int)ppm_reader);
			Debug::log("%X %X %X %X %X %X %X %X %X %X", f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9]);
		}						
		frames_count = ppm_reader->getFrameCount();		
		framePbSpeed = ppm_reader->getFramePlaybackSpeed();		
		
		Debug::log("FPS = %X", ((0x6A0+ppm_reader->getAnimationDataSize()+ppm_reader->getFrameCount()+3)/4)*4);
		Debug::log("FPS = %i", framePbSpeed);
		
		Debug::log("allocating buffers");
		buffer1 = new int[32*192]();
		buffer2 = new int[32*192]();
		
		bgmSize = ppm_reader->getBgmTrackSize();
		Debug::log("bgmSize = %i", bgmSize);
		if(bgmSize>=491520/2+100)
		{
			Debug::log("Sound TOO BIG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}		
				
		sound_buffer = (short*)malloc(bgmSize*4);
		Debug::log("Here? %X", sound_buffer);
		if(sound_buffer==nullptr)
		{
			Debug::log("Sound buffer not allocated!!!");
		}
		Debug::log("Here2?");
		SoundDecoder::ADPCM2PCM16(ppm_reader->getBgmTrack(), sound_buffer, bgmSize);
		bgmSize*=4;
		
		key_down.add_event(&PlayerScene::on_key_down, this);
		Debug::log("Here5?");
		
		begin_sprites_init();				
		
		for(int i=0;i<17;i++)
		{
			bar_frames[i] = new ObjFrame(&ROA_bar_fragments8, i,0);
			get_obj_allocator_sub()->allocate(bar_frames[i]);
		}

		for(int i=0;i<14;i++)
		{	
			bar_fragments[i] = create_sprite(new Sprite(SIZE_16x8, Engine::Sub));
			bar_fragments[i]->set_default_allocator(nullptr);
			bar_fragments[i]->add_frame(bar_frames[1]);
			bar_fragments[i]->set_position(16+16*i, 136);
		}				

		
		play_frame = new ObjFrame(&ROA_player_icons8, 0, 0);
		get_obj_allocator_sub()->allocate(play_frame);
		resume_frame= new ObjFrame(&ROA_player_icons8, 1, 0);
		get_obj_allocator_sub()->allocate(resume_frame);
		replay_off_frame = new ObjFrame(&ROA_player_icons8, 2, 0);
		get_obj_allocator_sub()->allocate(replay_off_frame);
		replay_on_frame = new ObjFrame(&ROA_player_icons8, 3, 0);
		get_obj_allocator_sub()->allocate(replay_on_frame);
		
		play_resume_button = create_sprite(new Sprite(SIZE_16x16, Engine::Sub));
		play_resume_button->set_default_allocator(nullptr);
		play_resume_button->add_frame(play_frame);
		play_resume_button->set_position(122,150);
		
	    replay_button = create_sprite(new Sprite(SIZE_16x16, Engine::Sub));
		replay_button->set_default_allocator(nullptr);
		replay_button->add_frame(replay_off_frame);
		replay_button->set_position(89,147);
		
		end_sprites_init();
		
		set_autoplay(true);
	}
	
	void set_player_bar(int value)
	{
		value = (value*224)>>8;
		int p = value/16;
		for(int i=0;i<p;i++)
			bar_fragments[i]->set_frame(0,bar_frames[16]);
		bar_fragments[p]->set_frame(0,bar_frames[value&0xF]);
		for(int i=p+1;i<14;i++)
			bar_fragments[i]->set_frame(0,bar_frames[0]);
	}
	
	int bgmId;
	int bgmSize;
	int soundFreq;	
	
	touchPosition touch;
	
	bool touch_in_rect(int x, int y, int w, int h)
	{
		if(touch.px<x || touch.px>=x+w) return false;
		if(touch.py<y || touch.py>=y+h) return false;
		return true;
	}
	
	bool touch_in_range(int cx, int cy, int r)
	{
		cx-=touch.px;
		cy-=touch.py;
		return cx*cx+cy*cy<=r*r;
	}
		
	void on_key_down(void*, void* _k)
	{
		int keys = (int)_k;				
		
		if(keys & KEY_A)
		{
			paused ? resume() : pause();
		}		
		else if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
			if(touch_in_rect(89,147,16,16))
				toggle_autoplay();
			if(touch_in_range(128,156,16))
			{
				paused ? resume() : pause();
			}
			if(touch_in_rect(0,0,32,32))
			{
				if(!paused) 
				{
					pause();				
					frame();
				}				
				back_to_location_viewer();				
			}
		}
		else if(keys & KEY_Y)
		{
			toggle_autoplay();
		}
	}
	
	void back_to_location_viewer()
	{		
		close()->next(get_location_viewer_scene());
	}
	
	int frames_count;
	int frame_index=0;
	
	int* layer1;	
	
	int* buffer1;
	int* buffer2;	
	short* sound_buffer;
	
	int framePbSpeed;
	int bgmPbSpeed;	
	
	static short get_layer_color(int id, int paperColor)
	{
		switch(id)
		{
			case 2: return Colors::Red;
			case 3: return Colors::Blue;
			default: return 0x7fff-paperColor;
		}
	}
	
	int frame_countdown = 0;
	
	int k=0;
	
	void frame() override
	{						
		working=true;
		
		/*k++;
		if(k==300)
		{
			working=false;
			close()->next(get_player_scene());
		}*/

		if(frame_countdown==0 && !paused)
		{					
			char header = ppm_reader->getFrame(frame_index)[0];		
			BG_PALETTE[0x00] = (header&1) ? Colors::White : Colors::Black;
			BG_PALETTE[0xE1] = get_layer_color((header>>3)&3, BG_PALETTE[0x00]);
			BG_PALETTE[0xE2] = BG_PALETTE[0xE3] = get_layer_color((header>>1)&3, BG_PALETTE[0x00]);
			
			for(int ty=0;ty<24;ty++)
			{
				for(int i=0;i<32;i++)
					for(int j=0;j<8;j++)
						layer1[256*ty+8*i+j] = buffer2[256*ty+32*j+i] | (buffer1[256*ty+32*j+i]<<1);
			}
			
			if(frame_index==0)
			{
				sound_frame_counter=0;
				play_resume_button->set_frame(0, resume_frame);
				soundKill(bgmId);				
				play_sound();				
				timerStart(0, ClockDivider_1, TIMER_FREQ(soundFreq/4), &PlayerScene::timerCallback);		
			}
			
			Debug::log("FID = %i", frame_index);			
			FrameDecoder::decode(buffer1, buffer2, ppm_reader->getFrame(frame_index++));			
			if(frame_index==frames_count)
			{
				soundKill(bgmId);
				if(!autoplay)					
					pause();
			}
			frame_index%=frames_count;
		}
		else if(frame_countdown==1)
		{					
			set_player_bar(frame_index*256/frames_count);			
		}
		
		frame_countdown++;
		if(frame_countdown==framePbSpeed) frame_countdown=0;
				
		GenericScene256::frame();
		
		working=false;
	}
		
	__attribute__((noinline))
	void run() override
	{
		solve_map_requirements();
		load_assets();
		
		unsigned short* bg0map = bgGetMapPtr(2);		
		
		layer1 = (int*)bgGetGfxPtr(2);
		
		Debug::log("Layer1 VRAM = %X", layer1);		
		
		for(int i=0;i<24*32;i++)
		{			
			bg0map[i]=0xE000 | i;
		}
		
		BG_PALETTE[0x00]=Colors::White;
		BG_PALETTE[0xE1]=Colors::Red;	
		BG_PALETTE[0xE2]=Colors::Blue;
		BG_PALETTE[0xE3]=Colors::Blue;
				
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128			

		irqEnable(IRQ_VBLANK);
		irqSet(IRQ_VBLANK, &vblank_handler);
		Scene::run();	
	}
	
	~PlayerScene()
	{
		soundDisable();
		swiWaitForVBlank();
		
		free(sound_buffer);
		key_down.remove_event(&PlayerScene::on_key_down, this);
		irqSet(IRQ_VBLANK, nullptr);		
		delete[] buffer1;
		delete[] buffer2;
		delete ppm_reader;		
		
		for(int i=0;i<14;i++)
		{
			bar_fragments[i]->set_frame(0, nullptr);
			delete bar_fragments[i];
		}
		
		for(int i=0;i<17;i++)
			delete bar_frames[i];
		
		play_resume_button->set_frame(0, nullptr);
		replay_button->set_frame(0, nullptr);
		
		delete play_frame;
		delete resume_frame;
		delete replay_off_frame;
		delete replay_on_frame;			
	}
		
	static int sound_frame_counter;	
	static void timerCallback()
	{
		sound_frame_counter += 4;
	}
	
	bool paused=false;
	
	void pause()
	{
		play_resume_button->set_frame(0, play_frame);		
		paused=true;
		timerPause(0);
		soundKill(bgmId);						
	}	
	
	void resume()
	{
		play_resume_button->set_frame(0, resume_frame);
		paused=false;
        play_sound();
        timerUnpause(0);		
	}
	
	void play_sound()
	{				
		soundKill(bgmId);
		bgmId = soundPlaySample(sound_buffer+sound_frame_counter,SoundFormat_16Bit,4*bgmSize-2*sound_frame_counter, soundFreq, 100, 64, false, 0);
	}
	
	void set_autoplay(bool value)
	{
		autoplay = value;
		replay_button->set_frame(0, autoplay ? replay_on_frame : replay_off_frame);		
	}
	
	void toggle_autoplay() { set_autoplay(!autoplay); }
};

int PlayerScene::sound_frame_counter = 0;

Scene* get_player_scene() { return new PlayerScene(); }