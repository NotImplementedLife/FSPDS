#include <nds.h>
#include "scenes.hpp"
#include "globals.hpp"
#include "simple_scene.hpp"

#include "back_arrow.h"
#include "error_thumbnail.h"
#include "thumbnail_selector.h"
#include "opt_box.h"
#include "strings.hpp"

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
	
	ObjFrame* opt_frame = nullptr;
	Sprite* opt_box = nullptr;
	
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
	int ppm_states[9];
	
	touchPosition touch;
	bool touch_in_rect(int x, int y, int w, int h)
	{
		if(touch.px<x || touch.px>=x+w) return false;
		if(touch.py<y || touch.py>=y+h) return false;
		return true;
	}
	
public:
	void init() override
	{
		Debug::log("__________________________________________________________________________________________ %X", this);
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
		
		opt_frame = new ObjFrame(&ROA_opt_box8,0,0);
		get_obj_allocator_sub()->allocate(opt_frame);
		
		opt_box = create_sprite(new Sprite(SIZE_64x32, Engine::Sub));
		opt_box->set_default_allocator(nullptr);
		opt_box->add_frame(opt_frame);
		opt_box->set_position(play_button_x, 152);
		opt_box->set_priority(3);
		
		end_sprites_init();
		
		Debug::log("SELECTED LOCATION = %i", selected_location_index);
		selected_location = LocationsProvider::peek_location(selected_location_index);
		
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
		
		if(total_pages_count==0) thumbnail_selector->hide();
		opt_box->hide();
		
		crt_page = selected_thumbnail_page;
		
		thumbnail_sel_row = selected_thumbnail_index/3;
		thumbnail_sel_col = selected_thumbnail_index%3;
				
	}	
	
	void on_key_down(void*, void* _keys)
	{
		int keys = (int)_keys;		
		if(keys & KEY_UP)
		{
			if(thumbnail_sel_row>0)
			{
				thumbnail_sel_row--;			
				display_info();
			}
		}
		else if(keys & KEY_DOWN)
		{
			if(thumbnail_sel_row<2 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col+3<selected_location->filenames.size())
			{
				thumbnail_sel_row++;			
				display_info();
			}
		}
		else if(keys & KEY_LEFT)
		{
			if(thumbnail_sel_col>0)
			{
				thumbnail_sel_col--;
				display_info();
			}
			else if(crt_page>0)
			{
				crt_page--;
				thumbnail_sel_col=2;
				thumbnail_selector->hide();
				frame();
				display_page();				
				if(total_pages_count>0) thumbnail_selector->show();
				display_info();
			}		
		}
		else if(keys & KEY_RIGHT)
		{
			if(thumbnail_sel_col<2 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col+1<selected_location->filenames.size())
			{
				thumbnail_sel_col++;
				display_info();
			}
			else if(crt_page+1<total_pages_count)
			{
				crt_page++;					
				thumbnail_sel_col=0;
				while(thumbnail_sel_row>0 && 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col>=selected_location->filenames.size())
					thumbnail_sel_row--;
				thumbnail_selector->hide();
				frame();
				display_page();
				if(total_pages_count>0) thumbnail_selector->show();
				display_info();
			}
		}
		else if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
			if(touch_in_rect(0,0,32,32))
			{
				close()->next(get_playlists_scene());
			}
			else if(touch_in_rect(play_button_x,152,64,24))
			{
				go_to_player();
			}
		}
		else if(keys & KEY_B)
		{
			close()->next(get_playlists_scene());
		}
		else if(keys & KEY_A)
		{								
			int index = 9*crt_page+3*thumbnail_sel_row+thumbnail_sel_col;
			if(0<=index && index<selected_location->filenames.size())
				go_to_player();
		}
	}	
	
	void go_to_player()
	{
		int index = 3*thumbnail_sel_row+thumbnail_sel_col;
		if(ppm_states[index]<0)
			return;			
		load_path(9*crt_page+index);
		
		Debug::log(flipnote_path);
		
		selected_flipnote_path = flipnote_path;
		flipnote_path = nullptr;
		
		selected_thumbnail_page = crt_page;
		selected_thumbnail_index = index;			
		location_flipnotes_count = selected_location->filenames.size();

		delete[] selected_location->path;
		selected_location->path = nullptr;
		delete selected_location;
		close()->next(get_player_scene());			
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
				ppm_states[i]=status;
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
		vwf->put_text(str_print(vwf_buffer, "Page %i of %i", crt_page+1, total_pages_count), Pal4bit, SolidColorBrush(0x1));
	}
	
	inline static constexpr const char* months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	inline static constexpr int months_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	// does not work for year 2100	
	void get_datetime(unsigned int timestamp, int &day, int &month, int &year) 
	{		
		year = 2000;
		unsigned int leap_year = 86400 * 366;
		unsigned int norm_year = 86400 * 365;		
		unsigned int _4years = leap_year + 3*norm_year;
	
		while(timestamp>=_4years)
		{
			timestamp -= _4years;
			year+=4;
		}
		
		if(timestamp>=leap_year)
		{
			timestamp -= leap_year;	
			year++;
			while(timestamp>=norm_year)
			{
				timestamp-=norm_year;
				year++;
			}
		}
		
		month=0;
		while(month<11 && timestamp>=((unsigned)months_days[month] + ((year%4==0 && month==1)?1:0))*86400)
		{
			timestamp-=(months_days[month] + ((year%4==0 && month==1)?1:0))*86400;
			month++;
		}		
		month++;
		
		day = 1+timestamp/86400;
	}
	
	int play_button_x = Strings::pcx_play+(Strings::len_play-64)/2;
	
	void display_info()
	{
		opt_box->hide();
		for(int i=2;i<=10;i++) 
		{
			Debug::log("Clearing row %i", i);
			vwf->clear_row(i, Pal4bit);
		}
		
		if(total_pages_count==0)
		{			
			vwf->set_cursor(5, 84);
			vwf->put_text("No flipnotes here", Pal4bit, SolidColorBrush(0x1));
			return;
		}		
		int index = 3*thumbnail_sel_row+thumbnail_sel_col;
		if(ppm_states[index]<0)
		{
			vwf->set_cursor(4, 16);
			vwf->put_text("Can't load this flipnote", Pal4bit, SolidColorBrush(0x1));
			vwf->set_cursor(5, 16);
			vwf->put_text("The file may be corrupted or does", Pal4bit, SolidColorBrush(0x1));
			vwf->set_cursor(6, 16);
			vwf->put_text("not exist.", Pal4bit, SolidColorBrush(0x1));
			vwf->set_cursor(7, 16);
			vwf->put_text("Reindexing the location might solve", Pal4bit, SolidColorBrush(0x1));
			vwf->set_cursor(8, 16);
			vwf->put_text("this issue.", Pal4bit, SolidColorBrush(0x1));
			return;
		}		
				
		load_path(9*crt_page+index);
		vwf->set_cursor(3, 8);		
		vwf->clear_row(3, Pal4bit);
		vwf->put_text(str_print(vwf_buffer, "File: %s", file_part_path), Pal4bit, SolidColorBrush(0x1));				
		
		unsigned int timestamp = 0;
		timestamp|=ppm_metadata[index][0x9A];
		timestamp|=ppm_metadata[index][0x9B]<<8;
		timestamp|=ppm_metadata[index][0x9C]<<16;
		timestamp|=ppm_metadata[index][0x9D]<<24;		
		
		char author_name[24];
		
		int d=1;
		int m=1;
		int y=2000;
		get_datetime(timestamp, d,m,y);
		
		vwf->set_cursor(4, 8);		
		vwf->put_text(str_print(vwf_buffer, "Date: %i-%s-%i", d,months[m-1],y), Pal4bit, SolidColorBrush(0x1));
		
		vwf->set_cursor(5, 8);		
		vwf->put_text(str_print(vwf_buffer, "Locked: %s", ppm_metadata[index][10] ? "Yes" : "No"), Pal4bit, SolidColorBrush(0x1));
		
		convert_author_name(author_name, (unsigned short*)(&ppm_metadata[index][0x14]));		
		vwf->set_cursor(6, 8);		
		vwf->put_text(str_print(vwf_buffer, "Root author: %s", author_name), Pal4bit, SolidColorBrush(0x1));
		
		convert_author_name(author_name, (unsigned short*)(&ppm_metadata[index][0x2A]));
		vwf->set_cursor(7, 8);		
		vwf->put_text(str_print(vwf_buffer, "Parent author: %s", author_name), Pal4bit, SolidColorBrush(0x1));
		
		convert_author_name(author_name, (unsigned short*)(&ppm_metadata[index][0x40]));
		vwf->set_cursor(8, 8);		
		vwf->put_text(str_print(vwf_buffer, "Current author: %s", author_name), Pal4bit, SolidColorBrush(0x1));

		opt_box->show();
		vwf->set_cursor(10, Strings::pcx_play);
		vwf->put_text(Strings::str_play, Pal4bit, SolidColorBrush(0x1));		
	}
	
	inline static constexpr int charsets20xx[] = 
	{
		0x201C, 0x201D, 0x2022, 0x2026, 0x2033, 0x203B, 0x20AC, 0x2122, 
		0x2190, 0x2191, 0x2192, 0x2193, 0x221E, 0x2234, 0x25A0, 0x25A1, 
		0x25B2, 0x25B3, 0x25BC, 0x25BD, 0x25C6, 0x25C7, 0x25CB, 0x25CE, 
		0x25CF, 0x2605, 0x2606, 0x266A, 0x266D
	};	
	
	inline static constexpr int index_of20xx(int v)
	{
		for(int i=0;i<29;i++)
		{
			if(charsets20xx[i]==v)
				return 0x82+i;
		}
		return 0;
	}	

	inline static constexpr int charsetsE0xx[] = 
	{
		0xE000, 0xE001, 0xE002, 0xE003, 0xE004, 0xE005, 0xE006, 0xE007, 
		0xE008, 0xE009, 0xE00A, 0xE00B, 0xE00C, 0xE00D, 0xE00E, 0xE00F, 
		0xE010, 0xE011, 0xE012, 0xE013, 0xE015, 0xE016, 0xE017, 0xE018, 
		0xE019, 0xE01A, 0xE01B, 0xE01C, 0xE028
	};
	
	inline static constexpr int index_ofE0xx(int v)
	{
		for(int i=0;i<29;i++)
		{
			if(charsetsE0xx[i]==v)
				return 0x20+i;
		}
		return 0;
	}	
	
	inline static constexpr int charsetsFFxx[] = { 0xFF01,0xFF0F,0xFF1F,0xFF3C,0xFF5C,0xFF5E };
	
	inline static constexpr int index_ofFFxx(int v)
	{
		for(int i=0;i<6;i++)
		{
			if(charsetsFFxx[i]==v)
				return 0xB2+i;
		}
		return 0;
	}	
	
	static void convert_author_name(char* dest, const unsigned  short* src)
	{		
		for(int i=0;i<11;i++)
		{
			Debug::log("Char = %X", src[i]&0xFFFF);
			if(src[i]<0x80) 
			{
				*(dest++) = src[i];			
			}
			else if(src[i]<0xFF)
			{
				*(dest++) = 0x11;
				*(dest++) = src[i];
			}
			else if((src[i]&0xFFFE)==0x0152)
			{
				*(dest++) = 0x80+(src[i]&1);
			}
			else if((src[i]&0xFF00)==0x3000)
			{				
				*(dest++) = src[i]<0x3080 ? 0x12: 0x14;
				*(dest++) = 0x80|(src[i] & 0x7F);
			}
			else if((src[i]&0xF000)==0x2000)
			{
				int code = index_of20xx(src[i]);
				if(code!=0)
				{
					*(dest++) = 0x11;
					*(dest++) = code;
				}			
			}
			else if((src[i]&0xF000)==0xE000)
			{				
				int code = index_ofE0xx(src[i]);
				if(code!=0)
				{
					*(dest++) = 0x12;
					*(dest++) = 0x80|code;
				}			
			}
			else if((src[i]&0xFF00)==0xFF00)
			{
				int code = index_ofFFxx(src[i]);
				if(code!=0)
				{
					*(dest++) = 0x11;
					*(dest++) = code;
				}							
			}				
			
		}		
		*(dest++)='\0';
	}
	
	char vwf_buffer[64];
	int thumbnail_sel_row = 0;
	int thumbnail_sel_col = 0;

	int z=0;
	
	void frame() override
	{
		/*z++;
		if(z<10 && selected_thumbnail_index!=0)
			close()->next(new LocationViewerScene());*/
		display_path();
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
	
	int sel_location_width = 0;
	short* sel_buffer = nullptr;
	int sel_scroll = 0;
	
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
								
								
		sel_location_width = (measure_string(selected_location->path)+8+8)/8;
		sel_buffer = new short[2*sel_location_width*32]();
		
		vwf->set_render_space(sel_buffer,2,sel_location_width);
		vwf->set_cursor(0, 8);
		vwf->put_text(selected_location->path, Pal4bit, SolidColorBrush(0x2));
		display_path();
			
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
		display_info();

		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		
		Scene::run();	
	}
	
	static int measure_string(const char* text)
	{
		int result=0;
		for(;*text;++text)		
			result+=Resources::Fonts::default_8x16.get_glyph_width(*text);		
		return result;
	}
	
	void display_path()
	{		
		short* gfx = (short*)(bgGetGfxPtr(2));
		short* src = (short*)sel_buffer;
				
		if(sel_location_width>32)
		{
			int w = 32*(sel_scroll/16);
			src+=w;
			int sz = sel_location_width*32 - w;
			int k=0;
			for(;k<min(32*32, sz);k++) *(gfx++) = *(src++);			
			volatile int zero=0;			
			for(;k<32*32;k++) *(gfx++) = zero;
			sel_scroll++;
			if(sel_scroll>=16*sel_location_width)
				sel_scroll=0;		
		}
		else
		{
			for(int k=0;k<min(24*32, sel_location_width*32);k++)			
				*(gfx++) = *(src++);			
		}
	}
	
	
	~LocationViewerScene()
	{		
		key_down.remove_event(&LocationViewerScene::on_key_down, this);
		delete vwf;
		delete[] sel_buffer;   
		
		delete back_arrow;
		delete thumbnail_selector;
		
		opt_box->set_frame(0, nullptr);
		delete opt_box;
		
		delete opt_frame;
	}
	
};

Scene* get_location_viewer_scene()
{
	return new LocationViewerScene();
}