#include <nds.h>
#include "scenes.hpp"
#include "globals.hpp"
#include "simple_scene.hpp"

#include "back_arrow.h"
#include "error_thumbnail.h"

constexpr short torgb15(int rgb24)
{
	int b = rgb24&0xFF;
	int g = (rgb24>>8)&0xFF;
	int r = (rgb24>>16)&0xFF;
	r/=8, g/=8, b/=8;
	return r|(g<<5)|(b<<10);
}

class LocationViewerScene : public SimpleScene
{	
private:
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	Sprite* back_arrow = nullptr;		
	
	int dir_len;
	char* flipnote_path;
	char* file_part_path;
		
	
	void load_path(int i)
	{
		int j=0;
		for(char* ptr = selected_location->filenames[i].chars;j<24 && *ptr;j++)
		{
			file_part_path[j]=*(ptr++);				
		}
		strcpy(file_part_path+j, ".ppm");		
	}
	
public:
	void init() override
	{
		SimpleScene::init();
		require_tiledmap_4bpp(MAIN_BG1, 256, 256, 32*24);
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);		
		require_tiledmap_4bpp(SUB_BG2, 256, 256, 32*24);
		
		begin_sprites_init();
				
		
		back_arrow = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		back_arrow->add_frame(new ObjFrame(&ROA_back_arrow8,0,0));
		back_arrow->set_position(0, 0);
		
		end_sprites_init();
		
		dir_len = strlen(selected_location->path);
		flipnote_path = new char[dir_len+29];
		file_part_path = flipnote_path+dir_len;
		strcpy(flipnote_path, selected_location->path);
		
		Debug::log("Paths:");
				
		for(int i=0;i<selected_location->filenames.size();i++)
		{			
			load_path(i);
			Debug::log(flipnote_path);
		}
		
	}	
	
	
	int crt_page=0;
	void display_page()
	{
		
	}
	
	void frame() override
	{
		SimpleScene::frame();
	}	
	
	inline static constexpr short thumbail_palette[16] = 
	{
		torgb15(0xFFFFFF), torgb15(0x525252), torgb15(0xFFFFFF), torgb15(0x9C9C9C), 
		torgb15(0xFF4844), torgb15(0xC8514F), torgb15(0xFFADAC), torgb15(0x00FF00), 
		torgb15(0x4840FF), torgb15(0x514FB8), torgb15(0xADABFF), torgb15(0x00FF00), 
		torgb15(0xB657B7), torgb15(0x00FF00), torgb15(0x00FF00), torgb15(0x00FF00)	
	};
	
	__attribute__((noinline))
	void run() override
	{
		solve_map_requirements();
		load_assets();
		
		bgSetPriority(7,3);
		bgSetPriority(6,2);
		
		BG_PALETTE[0x91]=BG_PALETTE_SUB[0x91]=Colors::Black;
		BG_PALETTE[0x92]=BG_PALETTE_SUB[0x92]=Colors::Blue;
		BG_PALETTE[0x93]=BG_PALETTE_SUB[0x93]=Colors::Red;
		BG_PALETTE[0x94]=BG_PALETTE_SUB[0x94]=Colors::DarkGray;
		
		for(int i=0;i<16;i++)
		{
			BG_PALETTE[0xF0+i] = thumbail_palette[i];
		}
		
		
		
		vwf->set_render_space(bgGetGfxPtr(2),24,32);
		VwfEngine::prepare_map(*vwf, MAIN_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
								
		vwf->set_cursor(0, 8);
		vwf->put_text(selected_location->path, Pal4bit, SolidColorBrush(0x2));
		
			
		vwf->set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(*vwf, SUB_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
		
		vwf->set_cursor(6, 90);	
		vwf->put_text("Please wait", Pal4bit, SolidColorBrush(0x1));
		
		volatile short zero = 0;
				
		for(int i=0;i<32*24;i++)
			bgGetMapPtr(1)[i]=zero;
		
		unsigned short* gfx = bgGetGfxPtr(1);
		for(int i=0;i<32*24*16;i++)
		{
			gfx[i]=zero;
		}
		
		gfx+=16*2;
		volatile int xx=0x1111;
		for(int i=0;i<6*8*9*16;i++)
		{
			gfx[i]=xx;
		}
		
		for(int ty=0;ty<3;ty++)		
			for(int tx=0;tx<3;tx++)
				ROA_error_thumbnail4.extract_gfx(gfx+48*16*(3*ty+tx));
		
		
		int gfxq=2;
		
		for(int ty=0;ty<3;ty++)
		{				
			for(int tx=0;tx<3;tx++)
			{
				unsigned short* map = bgGetMapPtr(1) + 32*(7*ty+3) + 10*tx + 2;
				for(int y=0;y<6;y++)
				{
					for(int x=0;x<8;x++)
					{
						map[32*y+x]=0xF000|(gfxq++);
					}
				}
				
			}
		}			
		
		

		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		
		Scene::run();	
	}
	
	
	~LocationViewerScene()
	{
		delete vwf;
	}
	
};

Scene* get_location_viewer_scene()
{
	return new LocationViewerScene();
}