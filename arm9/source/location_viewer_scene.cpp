#include <nds.h>
#include "scenes.hpp"
#include "globals.hpp"
#include "simple_scene.hpp"

#include "back_arrow.h"
#include "error_thumbnail.h"
#include "thumbnail_selector.h"

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
	Sprite* thumbnail_selector = nullptr;
	
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
	
	char ppm_metadata[9][0x6A0];
	
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
		
		thumbnail_selector = create_sprite(new Sprite(SIZE_64x64, Engine::Main));
		thumbnail_selector->add_frame(new ObjFrame(&ROA_thumbnail_selector8,0,0));
		thumbnail_selector->set_position(16, 16);
		
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
		total_pages_count = (selected_location->filenames.size()+8)/9;
		
		key_down.add_event(&LocationViewerScene::on_key_down, this);
	}	
	
	void on_key_down(void*, void* _keys)
	{
		int keys = (int)_keys;		
		if(keys & KEY_UP)
		{
			if(thumbnail_sel_row>0)
				thumbnail_sel_row--;			
		}
		else if(keys & KEY_DOWN)
		{
			if(thumbnail_sel_row<2 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col+3<selected_location->filenames.size())
				thumbnail_sel_row++;			
		}
		else if(keys & KEY_LEFT)
		{
			if(thumbnail_sel_col>0)
				thumbnail_sel_col--;
			else
			{
				if(crt_page>0)
				{
					crt_page--;
					thumbnail_sel_col=2;
					thumbnail_selector->hide();
					frame();
					display_page();
					thumbnail_selector->show();
				}
			}			
		}
		else if(keys & KEY_RIGHT)
		{
			if(thumbnail_sel_col<2 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col+1<selected_location->filenames.size())
				thumbnail_sel_col++;
			else
			{
				if(crt_page+1<total_pages_count)
				{
					crt_page++;					
					thumbnail_sel_col=0;
					while(thumbnail_sel_row>0 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col>=selected_location->filenames.size())
						thumbnail_sel_row--;
					thumbnail_selector->hide();
					frame();
					display_page();
					thumbnail_selector->show();
				}
			}			
		}
		
	}
	
	inline static constexpr int THUMBNAIL_NONE = 0;
	inline static constexpr int THUMBNAIL_CORRUPT = 1;
	inline static constexpr int THUMBNAIL_READER = 2;
	
	void display_thumbnail(int index, int mode)
	{
		unsigned short* gfx = bgGetGfxPtr(1) + 16*2 + 48*16*index;
		
		switch(mode)
		{
			case THUMBNAIL_NONE:
			{
				volatile short zero = 0;							
				for(int i=0;i<6*8*16;i++) gfx[i]=zero;		
				break;
			}
			case THUMBNAIL_CORRUPT:
			{			
				ROA_error_thumbnail4.extract_gfx(gfx);
				break;
			}
			case THUMBNAIL_READER:
			{
				volatile short* src = (volatile short*)&ppm_metadata[index][0xA0];
				for(int i=0;i<6*8*16;i++) gfx[i]=*(src++);		
				break;
			}
		}		
	}
	
	int total_pages_count = 0;
	int crt_page=0;
	void display_page()
	{
		Debug::log("Display page");
		int index = crt_page*9;		
		for(int i=0;i<9;i++)
		{			
			if(index < selected_location->filenames.size())
			{				
				load_path(index++);
				Debug::log(flipnote_path);
				int status = PPMReader::peek_only_metadata(flipnote_path, &ppm_metadata[i][0]);
				Debug::log("Status = %i", status);
				if(status<0)
					display_thumbnail(i, THUMBNAIL_CORRUPT);
				else
					display_thumbnail(i, THUMBNAIL_READER);
			}
			else
			{
				Debug::log("None %i", i);
				display_thumbnail(i, THUMBNAIL_NONE);
			}			
		}				
		vwf->clear_row(1, Pal4bit);
		vwf->set_cursor(1, 90);
		vwf->put_text(str_print(page_label, "Page %i of %i", crt_page+1, total_pages_count+1), Pal4bit, SolidColorBrush(0x1));
	}
	
	char page_label[32];
	int thumbnail_sel_row = 0;
	int thumbnail_sel_col = 0;
	
	void frame() override
	{
		thumbnail_selector->set_position(16+thumbnail_sel_col*80, 16+thumbnail_sel_row*56);
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
		
		volatile short zero = 0;
				
		for(int i=0;i<32*24;i++)
			bgGetMapPtr(1)[i]=zero;		
		
		unsigned short* gfx = bgGetGfxPtr(1);
		for(int i=0;i<32*24*16;i++)
		{
			gfx[i]=zero;
		}	
		
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

		display_page();		

		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		
		Scene::run();	
	}
	
	
	~LocationViewerScene()
	{
		key_down.remove_event(&LocationViewerScene::on_key_down, this);
		delete vwf;
		delete ppm_reader;
	}
	
};

Scene* get_location_viewer_scene()
{
	return new LocationViewerScene();
}