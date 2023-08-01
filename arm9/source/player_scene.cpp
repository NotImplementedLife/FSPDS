#include <nds.h>

#include "globals.hpp"

#include "scenes.hpp"

#include "frame_decoder.hpp"
#include "sound_decoder.hpp"

#include "0B33C4_0BC1601FB8421_000_bin.h"

#include "player_bg.h"
#include "bar_fragments.h"
#include "player_icons.h"
#include "loading_text.h"
#include "strings.hpp"

#include <stdlib.h>
#include <malloc.h>

// https://stackoverflow.com/questions/1046714/what-is-a-good-random-number-generator-for-a-game
static unsigned long x=123456789, y=362436069, z=521288629;
unsigned long qrand() {          //period 2^96-1
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;
	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;
	return z;
}

void* operator new(std::size_t n)
{
	if(n>256)
		Debug::log("[new] Allocating %i bytes", n);	
	void* ptr = malloc(n);
	Debug::log("[new] P= %X", ptr);	
	nds_assert(((unsigned int)ptr&0xFF000000) == 0x02000000 , "Allocation failed (corrupted)");	
	nds_assert(ptr!=nullptr, "Allocation failed");		
	if(n>256)
		Debug::log("[new] Reserved address %X", ptr);
	return ptr;  
}

void operator delete(void* p)
{		
	//Debug::log("free %i", malloc_usable_size(p));
	Debug::log("[new] Freeing %X", p);
	free(p);
}

void operator delete( void* p, std::size_t sz) 
{ 
	Debug::log("[new] Freeing_sz %X", p);
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
	Sprite* loading_text;
	Sprite* bar_fragments[14];	
	ObjFrame* bar_frames[17];		
	
	ObjFrame* play_frame;
	ObjFrame* resume_frame;
	ObjFrame* replay_off_frame;
	ObjFrame* replay_on_frame;	
	
	ObjFrame* shuffle_on_frame;	
	ObjFrame* shuffle_off_frame;	
	
	Sprite* play_resume_button;
	Sprite* replay_button;
	Sprite* shuffle_button;
	 
	PPMReader* ppm_reader = new PPMReader();
	bool autoplay = false;
	
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	
	void launch_other_flipnote(int index)
	{		
		Debug::log("C1 - Closing");
		auto* scenecom = close();
		Debug::log("C2 - Create loc prov");
		LocationsProvider* locations_provider = new LocationsProvider();		
		Debug::log("C3 - Getting");
		Location* loc = locations_provider->get_at(selected_location_index);
		
		Debug::log("Launching other flipnote (%i/%i)", index, loc->filenames.size());
		
		char* path = new char[strlen(loc->path)+29];
		path[0]=0;
		strcpy(path, loc->path);		
		char* filename = path + strlen(loc->path);		
		
		int j=0;
		for(char* ptr = loc->filenames[index].chars;j<24 && *ptr;j++)
		{
			filename[j]=*(ptr++);				
		}		
		strcpy(filename+j, ".ppm");		
		selected_flipnote_path = path;
		
		selected_thumbnail_page = index/9;
		selected_thumbnail_index = index%9;
		
		Debug::log("Filename = %s", selected_flipnote_path);
		
		delete locations_provider;
		//LocationsProvider::operator delete(locations_provider);		
		scenecom->next(get_player_scene());
		
	}
public:
	void init() override
	{
		GenericScene256::init();
		soundEnable();
		
		require_tiledmap_4bpp(MAIN_BG1, 256, 256, 32*24);
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
				
		require_bitmap(SUB_BG2, &ROA_player_bg8);

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
		
		shuffle_on_frame = new ObjFrame(&ROA_player_icons8, 4, 0);
		get_obj_allocator_sub()->allocate(shuffle_on_frame);
		
		shuffle_off_frame = new ObjFrame(&ROA_player_icons8, 5, 0);
		get_obj_allocator_sub()->allocate(shuffle_off_frame);
		
		play_resume_button = create_sprite(new Sprite(SIZE_16x16, Engine::Sub));
		play_resume_button->set_default_allocator(nullptr);
		play_resume_button->add_frame(play_frame);
		play_resume_button->set_position(122,150);
		
	    replay_button = create_sprite(new Sprite(SIZE_16x16, Engine::Sub));
		replay_button->set_default_allocator(nullptr);
		replay_button->add_frame(replay_off_frame);
		replay_button->set_position(89,147);				
		
		shuffle_button = create_sprite(new Sprite(SIZE_16x16, Engine::Sub));
		shuffle_button->set_default_allocator(nullptr);
		shuffle_button->add_frame(shuffle_off_frame);
		shuffle_button->set_position(153,146);		
		
		loading_text = create_sprite(new Sprite(SIZE_64x32, Engine::Main));
		loading_text->add_frame(new ObjFrame(&ROA_loading_text8, 0, 0));
		loading_text->set_position(96, 80);		
		
		end_sprites_init();
		
		set_autoplay(is_player_autoplay());
		set_shuffle(is_player_shuffle());
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
		
	void show_player_screen(bool value)
	{		
		fifoSendValue32(FIFO_USER_01, (player_screen_on = value) ? 0x0002 : 0x0001);
	}
		
	void on_key_down(void*, void* _k)
	{
		int keys = (int)_k;				
		if(load_error)
		{
			if(keys & KEY_B)
			{
				back_to_location_viewer();
			}				
			else if(keys & KEY_TOUCH)
			{
				touchRead(&touch);								
				if(touch_in_rect(0,0,32,32))
				{					
					back_to_location_viewer();
				}
			}
			return;
		}
		
		if(keys & KEY_A)
		{
			paused ? resume() : pause();
		}		
		else if(keys & KEY_TOUCH)
		{
			if(!player_screen_on)
			{
				show_player_screen(true);
				return;
			}
			
			touchRead(&touch);
			if(touch_in_rect(89,147,16,16))
				toggle_autoplay();
			else if(touch_in_rect(153,146, 16, 16))
				toggle_shuffle();
			else if(touch_in_range(128,156,16))
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
			else if(touch_in_rect(57,145,16,16))
			{
				prev_flipnote();
			}
			else if(touch_in_rect(183, 145, 16, 16))
			{
				next_flipnote();
			}
			else if(touch_in_rect(224,0,32,32))
			{
				show_player_screen(false);
			}
		}
		else if(keys & KEY_Y)
		{
			toggle_autoplay();
		}
		else if(keys & KEY_X)
		{
			toggle_shuffle();
		}
		else if(keys & KEY_L)
		{
			prev_flipnote();
		}
		else if(keys & KEY_R)
		{
			next_flipnote();
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
	short* sound_buffer = nullptr;
	
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
		if(load_error)
		{			
			for(int i=0;i<60;i++)
			{
				scanKeys();
				int k = keysDown();
				if(k & KEY_B)
					back_to_location_viewer();				
				GenericScene256::frame();
			}
			auto_next_flipnote();			
			return;
		}
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
				{		
					working=false;																									
					auto_next_flipnote();
					pause();	
				}
			}
			frame_index%=frames_count;			
		}
		else if(frame_countdown==1)
		{			
			for(int ty=0;ty<24;ty++)
			{
				for(int i=0;i<32;i++)
					for(int j=0;j<8;j++)
						layer1[256*ty+8*i+j] = buffer2[256*ty+32*j+i] | (buffer1[256*ty+32*j+i]<<1);
			}
	
			set_player_bar(frame_index*256/frames_count);						
		}
		
		frame_countdown++;
		if(frame_countdown==framePbSpeed) frame_countdown=0;
				
		GenericScene256::frame();
		working=false;
	}	
	
	void auto_next_flipnote()
	{		
		if(shuffle && location_flipnotes_count<2) return;
		if(!shuffle && location_flipnotes_count<1) return;
		
		int index = selected_thumbnail_page*9+selected_thumbnail_index;
		if(shuffle)
		{			
			int new_index = qrand()%(location_flipnotes_count/2) - location_flipnotes_count;
			if(new_index==0) new_index=1;														
			new_index = (index + new_index)%location_flipnotes_count;
			if(new_index<0) 
				new_index = location_flipnotes_count+new_index;	
			
			Debug::log("new_index = %i", new_index);
			
			launch_other_flipnote(new_index);
		}
		else
		{			
			int index = selected_thumbnail_page*9+selected_thumbnail_index;
			index++;
			if(index>=location_flipnotes_count) index=0;			
			launch_other_flipnote(index);
		}
	}
	
	void next_flipnote()
	{
		int index = selected_thumbnail_page*9+selected_thumbnail_index;
		index++;
		if(index>=location_flipnotes_count) return;
		launch_other_flipnote(index);
	}
	
	void prev_flipnote()
	{
		int index = selected_thumbnail_page*9+selected_thumbnail_index;
		if(index<=0) return;
		index--;
		launch_other_flipnote(index);				
	}
		
		
	__attribute__((noinline))
	void run() override
	{
		solve_map_requirements();
		load_assets();
		
		Hardware::MainEngine::objEnable(64, true);
		Hardware::SubEngine::objEnable(128, true);
		
		swiWaitForVBlank();
		GenericScene256::frame();
		swiWaitForVBlank();
		
		BG_PALETTE[0x91]=BG_PALETTE_SUB[0x91]=Colors::Black;
		BG_PALETTE[0x92]=BG_PALETTE_SUB[0x92]=Colors::Blue;
		BG_PALETTE[0x93]=BG_PALETTE_SUB[0x93]=Colors::Red;
		BG_PALETTE[0x94]=BG_PALETTE_SUB[0x94]=Colors::DarkGray;
		
		vwf->set_render_space(bgGetGfxPtr(1),24,32);
		VwfEngine::prepare_map(*vwf, MAIN_BG1, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);					
		
		unsigned short* bg0map = bgGetMapPtr(2);		
		
		layer1 = (int*)bgGetGfxPtr(2);
		
		Debug::log("Layer1 VRAM = %X", layer1);		
		
		for(int i=0;i<24*32;i++)
		{			
			bg0map[i]=0xE000 | i;
		}
		
		volatile short zero = 0;
		for(int i=0;i<24*32*16;i++)
			layer1[i]=zero;
		
		BG_PALETTE[0x00]=Colors::White;
		BG_PALETTE[0xE1]=Colors::Red;	
		BG_PALETTE[0xE2]=Colors::Blue;
		BG_PALETTE[0xE3]=Colors::Blue;		
		
		swiWaitForVBlank();
		GenericScene256::frame();
		swiWaitForVBlank();
		load();		
		
		loading_text->hide();
		
		swiWaitForVBlank();
		GenericScene256::frame();
		swiWaitForVBlank();
		
		key_down.add_event(&PlayerScene::on_key_down, this);
		

		irqEnable(IRQ_VBLANK);
		irqSet(IRQ_VBLANK, &vblank_handler);		
		
		Scene::run();
	}
	
	bool load_error = false;
	void load()
	{
		if(selected_flipnote_path!=nullptr)
		{
			ppm_reader->clear();
			int res = ppm_reader->read_file(selected_flipnote_path);							
			
			if(res<0)
			{
				load_error=true;
				vwf->set_cursor(6, Strings::pcx_error);
				vwf->put_text(Strings::str_error, Pal4bit, SolidColorBrush(0x3));						
				switch(res)
				{
					case PPMReader::ERR_NULL_ARGUMENT:
						vwf->set_cursor(7, Strings::pcx_err_null_argument); 
						vwf->put_text(Strings::str_err_null_argument, Pal4bit, SolidColorBrush(0x3)); 
						break;
					case PPMReader::ERR_FOPEN:									
						vwf->set_cursor(7, Strings::pcx_err_fopen); 
						vwf->put_text(Strings::str_err_fopen, Pal4bit, SolidColorBrush(0x3)); 
						break;					
					case PPMReader::ERR_SIZE_EXCEEDED:
						vwf->set_cursor(7, Strings::pcx_err_size_exceeded); 
						vwf->put_text(Strings::str_err_size_exceeded, Pal4bit, SolidColorBrush(0x3)); 
						break;
					case PPMReader::ERR_READ_COUNT:
						vwf->set_cursor(7, Strings::pcx_err_read_count); 
						vwf->put_text(Strings::str_err_read_count, Pal4bit, SolidColorBrush(0x3)); 
						break;					
				}
			}
			
			Debug::log("Player read finished %i", res);
			Debug::log("Animation data size = %i", ppm_reader->getAnimationDataSize());
			Debug::log("Sound data size = %i", ppm_reader->getSoundDataSize());
			Debug::log("Frames count = %i", ppm_reader->getFrameCount());		
			delete[] selected_flipnote_path;
		}
		else
		{
			int* d = (int*)ppm_reader;
			int* s = (int*)_0B33C4_0BC1601FB8421_000_bin;
			
			Debug::log("START = %X", d);
			
			for(int i=0;i<253196/4;i++)			
				*(d++)=*(s++);
		}					
		
		soundFreq = ppm_reader->getSoundFreq();		
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
			load_error=true;
			vwf->set_cursor(6, Strings::pcx_error);
			vwf->put_text(Strings::str_error, Pal4bit, SolidColorBrush(0x3));		
			vwf->set_cursor(7, Strings::pcx_sound_size_too_large);
			vwf->put_text(Strings::str_sound_size_too_large, Pal4bit, SolidColorBrush(0x3));		
			return;			
		}		
				
		sound_buffer = (short*)malloc(bgmSize*4);				
		Debug::log("Here? %X", sound_buffer);
		if(sound_buffer==nullptr)
		{
			load_error=true;
			vwf->set_cursor(6, Strings::pcx_error);
			vwf->put_text(Strings::str_error, Pal4bit, SolidColorBrush(0x3));		
			vwf->set_cursor(7, Strings::pcx_sound_buffer_not_allocated);
			vwf->put_text(Strings::str_sound_buffer_not_allocated, Pal4bit, SolidColorBrush(0x3));
			return;			
		}
		
		for(int i=0;i<bgmSize*2;i++)
			sound_buffer[i]=0;			
		
		Debug::log("Here2?");
		SoundDecoder::ADPCM2PCM16(ppm_reader->getBgmTrack(), sound_buffer, bgmSize);
		bgmSize*=4;
		
		unsigned short* sfx[3] = {nullptr, nullptr, nullptr};
		int sfxLen[3] = {0,0,0};
		for(int i=0;i<3;i++)
		{
			Debug::log("ii=%i", i);
			int size = ppm_reader->getSfxTrackSize(i);
			if(size==0) continue;
			sfx[i] = (unsigned short*)malloc(size*4);
			Debug::log("alloced=%X", sfx[i]);
			
			if(sound_buffer==nullptr)
			{
				load_error=true;
				vwf->set_cursor(6, Strings::pcx_error);
				vwf->put_text(Strings::str_error, Pal4bit, SolidColorBrush(0x3));		
				vwf->set_cursor(7, Strings::pcx_sfx_buffer_not_allocated);
				vwf->put_text(Strings::str_sfx_buffer_not_allocated, Pal4bit, SolidColorBrush(0x3));				
				return;			
			}
			Debug::log("size=%i", size);
			for(int j=0;j<size*2;j++) sfx[i][j]=0;					
			Debug::log("Decoding");
			SoundDecoder::ADPCM2PCM16(ppm_reader->getSfxTrack(i), (short*)sfx[i], size);			
			Debug::log("Done");
			sfxLen[i]=2*size;
		}
		
		Debug::log("SPF");
		int spf = ppm_reader->getSamplesPerFrame();
		Debug::log("SFX");
		char* sfx_map = ppm_reader->getSfxMapping();		
		unsigned short* sbuff = (unsigned short*)sound_buffer;
		for(int i=0;i<frames_count;i++)
		{						
			for(int k=0;k<3;k++)
			{				
				if(!(sfx_map[i]&(1<<k))) continue;				
				if(sfxLen[k]==0) continue;				
				int d = spf*i;				
				unsigned short* src=sfx[k];
				for(int q=0;q<sfxLen[k] && d+q<bgmSize/2;q++)
				{																
					int sample = sbuff[d+q] + src[q];				
					sbuff[d+q] = clamp(sample, 0, 65535);
				}
			}			
		}
		
		Debug::log("Freeing");
		for(int i=0;i<3;i++) free(sfx[i]);
		
		Debug::log("Here5?");
	}
	
	static int clamp(int x, int a, int b) { return x<=a ? a : x>=b ? b : x; }
	
	~PlayerScene()
	{
		soundPause(bgmId);
		soundKill(bgmId);
		soundDisable();
		swiWaitForVBlank();
		
		free(sound_buffer);
		key_down.remove_event(&PlayerScene::on_key_down, this);
		irqSet(IRQ_VBLANK, nullptr);		
		delete[] buffer1;
		delete[] buffer2;
		delete ppm_reader;		
		delete vwf;
		
		for(int i=0;i<14;i++)
		{
			bar_fragments[i]->set_frame(0, nullptr);
			delete bar_fragments[i];
		}
		
		for(int i=0;i<17;i++)
			delete bar_frames[i];
		
		play_resume_button->set_frame(0, nullptr);
		replay_button->set_frame(0, nullptr);
		shuffle_button->set_frame(0, nullptr);
		
		
		delete play_frame;
		delete resume_frame;
		delete replay_off_frame;
		delete replay_on_frame;		
		delete shuffle_on_frame;
		delete shuffle_off_frame;		
		delete loading_text;
		
		delete shuffle_button;
		delete play_resume_button;
		delete replay_button;

		set_player_autoplay(autoplay);
		set_player_shuffle(shuffle);	
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
		Debug::log("ssss = %i",4*bgmSize-2*sound_frame_counter);
		if(bgmSize>0)
		{			
			bgmId = soundPlaySample(sound_buffer+sound_frame_counter,SoundFormat_16Bit,4*bgmSize-2*sound_frame_counter, soundFreq, 100, 64, false, 0);
		}
	}
	
	void set_autoplay(bool value)
	{
		autoplay = value;
		replay_button->set_frame(0, autoplay ? replay_on_frame : replay_off_frame);
	}
	
	bool shuffle = false;
	
	void set_shuffle(bool value)
	{
		shuffle = value;
		shuffle_button->set_frame(0, shuffle ? shuffle_on_frame : shuffle_off_frame);
	}
		
	void toggle_autoplay() { set_autoplay(!autoplay); }
	void toggle_shuffle() { set_shuffle(!shuffle); }
};

int PlayerScene::sound_frame_counter = 0;

Scene* get_player_scene() { return new PlayerScene(); }