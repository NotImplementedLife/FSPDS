#include <nds.h>

#include "scenes.hpp"

#include "frame_decoder.hpp"
#include "sound_decoder.hpp"

#include "0B33C4_0BC1601FB8421_000_bin.h"

#include "bg2.h"

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

const char* selected_flipnote_path = "fat://flipnotes/0BC769_0A978C458A07A_002.ppm";
PPMReader* ppm_reader;

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
public:
	void init() override
	{
		GenericScene256::init();
		soundEnable();
		
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
		require_tiledmap_4bpp(MAIN_BG3, 256, 256, 32*24);
		
		require_bitmap(SUB_BG2, &ROA_bg28);
		
		Debug::log("Player Scene");
		ppm_reader = new PPMReader();
		int res = ppm_reader->read_file(selected_flipnote_path);		
		Debug::log("Player read finished %i", res);
		Debug::log("Animation data size = %i", ppm_reader->getAnimationDataSize());
		Debug::log("Sound data size = %i", ppm_reader->getSoundDataSize());
		Debug::log("Frames count = %i", ppm_reader->getFrameCount());
			
		/*int* d = (int*)ppm_reader;
		int* s = (int*)_0B33C4_0BC1601FB8421_000_bin;
		
		Debug::log("START = %X", d);
		
		for(int i=0;i<253196/4;i++)
			*(d++)=*(s++);*/
		
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
	}
	
	int bgmId;
	int bgmSize;
	int soundFreq;
	
	bool next=false;
	void on_key_down(void*, void* _k)
	{		
		next=true;
	}
	
	int frames_count;
	int frame_index=0;
	
	int* layer1;	
	
	int* buffer1;
	int* buffer2;	
	short* sound_buffer;
	
	int framePbSpeed;
	int bgmPbSpeed;
	
	int k=0;
	
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
	
	void frame() override
	{				
		if(!next) return;
		working=true;
		if(frame_countdown==0)
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
				soundKill(bgmId);
				bgmId=soundPlaySample(sound_buffer, SoundFormat_16Bit, 4*bgmSize, soundFreq, 100, 64, false, 0);
			}
			
			Debug::log("FID = %i", frame_index);
			FrameDecoder::decode(buffer1, buffer2, ppm_reader->getFrame(frame_index++));
			if(frame_index==frames_count)
			{
				next=false;
			}
			frame_index%=frames_count;
		}
		frame_countdown++;
		if(frame_countdown==framePbSpeed) frame_countdown=0;
				
		GenericScene256::frame();		
		
		working=false;
		Debug::log("V=%i",vbl);		
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
		delete[] buffer1;
		delete[] buffer2;		
		delete ppm_reader;		
		free(sound_buffer);
	}
};

Scene* get_player_scene() { return new PlayerScene(); }