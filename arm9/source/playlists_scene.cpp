#include <nds.h>

#include "scenes.hpp"
#include "simple_scene.hpp"
#include "folder_icon.h"
#include "filesystem.hpp"
#include "globals.hpp"

#include "back_arrow.h"
#include "strings.hpp"


Scene* folder_picker_return_to_playlists();


class PlaylistsScene : public SimpleScene
{
private:		
	Sprite* folder_icons[4];
	ObjFrame* folder_highlighted_frame;	
	Sprite* back_arrow = nullptr;	

#if USE_NTFS!=1	
	Sprite* reindex_icon = nullptr;
	Sprite* remove_icon = nullptr;
	Sprite* add_folder_icon=nullptr;
#endif

	
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	LocationsProvider* locations_provider;
	
	touchPosition touch;
	bool touch_in_rect(int x, int y, int w, int h)
	{
		if(touch.px<x || touch.px>=x+w) return false;
		if(touch.py<y || touch.py>=y+h) return false;
		return true;
	}
	
	bool is_indexing;
	
	static LocationsProvider* bak_locations_provider;

public:
	PlaylistsScene(bool is_indexing=false) : is_indexing(is_indexing)
	{		
		locations_provider = bak_locations_provider;
		bak_locations_provider = nullptr;
	}

	void on_key_down(void*, void* _k)
	{		
		int keys = (int)_k;
		
		if(keys & KEY_DOWN)
		{
			if(loc_selected_index<locations_provider->get_count()-1)
				loc_selected_index++;
			display_page();
		}
		else if(keys & KEY_UP)
		{
			if(loc_selected_index>0)
				loc_selected_index--;
			display_page();
		}		
		else if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
		#if USE_NTFS!=1
			if(touch_in_rect(220, 0, 32, 32))
			{
				folder_picker_next_scene_gen = folder_picker_return_to_playlists;
				bak_locations_provider = locations_provider;
				locations_provider = nullptr;
				close()->next(get_folder_picker_scene());
			}
			else if(touch_in_rect(180,0,32,32))
			{
				if(show_yes_no_dialog(Strings::str_location_ask_reindex, Strings::pcx_location_ask_reindex))
				{
					Debug::log("Dialog accepted");
					vwf->clear(Pal4bit);
					for(int i=0;i<4;i++)
						folder_icons[i]->hide();					
					vwf->set_cursor(6, Strings::pcx_please_wait);	
					vwf->put_text(Strings::str_please_wait, Pal4bit, SolidColorBrush(0x1));
					GenericScene256::frame();
					
					reindex_selected();		
					display_page();
				}
			}
			else if(touch_in_rect(140, 0, 32, 32))
			{
				if(show_yes_no_dialog(Strings::str_location_ask_remove, Strings::pcx_location_ask_remove))
				{
					Debug::log("Dialog accepted");
					vwf->clear(Pal4bit);
					for(int i=0;i<4;i++)
						folder_icons[i]->hide();					
					vwf->set_cursor(6, Strings::pcx_please_wait);	
					vwf->put_text(Strings::str_please_wait, Pal4bit, SolidColorBrush(0x1));
					GenericScene256::frame();
					
					locations_provider->remove_location(loc_selected_index);
					if(loc_selected_index>0) loc_selected_index--;
					display_page();
				}	
			}
			else if(touch_in_rect(0,0,32,32))
			{
				close()->next(gen_title_scene());
			}
		#endif // USE_NTFS!=1
		}		
		else if(keys & KEY_B)
		{
			close()->next(gen_title_scene());
		}
		else if(keys & KEY_A)	
		{
			if(loc_selected_index<locations_provider->get_count())
			{				
				selected_location_index = loc_selected_index;				
				delete locations_provider;
				locations_provider = nullptr;
				
				selected_thumbnail_page = selected_thumbnail_index=0;
				close()->next(get_location_viewer_scene());
			}
		}
	}


	void init() override
	{
		SimpleScene::init();
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);		
		require_tiledmap_4bpp(SUB_BG2, 256, 256, 32*24);
		
		begin_sprites_init();
		
		folder_highlighted_frame = new ObjFrame(&ROA_folder_icon8,0,1);
		get_obj_allocator_sub()->allocate(folder_highlighted_frame);
		
		for(int i=0;i<4;i++)
		{
			folder_icons[i] = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
			folder_icons[i]->set_default_allocator(nullptr);
			folder_icons[i]->add_frame(folder_highlighted_frame);
			folder_icons[i]->set_position(8, 32 + 32*i);
		}
		
	#if USE_NTFS!=1
		add_folder_icon = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		add_folder_icon->add_frame(new ObjFrame(&ROA_folder_icon8,0,3));
		add_folder_icon->set_position(220, 0);
		
		back_arrow = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		back_arrow->add_frame(new ObjFrame(&ROA_back_arrow8,0,0));
		back_arrow->set_position(0, 0);
		
		reindex_icon = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		reindex_icon->add_frame(new ObjFrame(&ROA_folder_icon8,0,4));
		reindex_icon->set_position(180, 0);

		remove_icon = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		remove_icon->add_frame(new ObjFrame(&ROA_folder_icon8,0,5));
		remove_icon->set_position(140, 0);
	#endif
		

		end_sprites_init();
		
		key_down.add_event(&PlaylistsScene::on_key_down, this);
		
	}
	
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
		
		vwf->set_render_space(bgGetGfxPtr(2),24,32);
		VwfEngine::prepare_map(*vwf, MAIN_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
						
		vwf->set_cursor(6, Strings::pcx_browse_flipnotes);
		vwf->put_text(Strings::str_browse_flipnotes, Pal4bit, SolidColorBrush(0x1));		
		
			
		vwf->set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(*vwf, SUB_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
		
		vwf->set_cursor(6, Strings::pcx_please_wait);	
		vwf->put_text(Strings::str_please_wait, Pal4bit, SolidColorBrush(0x1));
					
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		
		for(int i=0;i<4;i++)
			folder_icons[i]->hide();
		
		if(locations_provider==nullptr)
		{
			GenericScene256::frame();		
			swiWaitForVBlank();
			swiWaitForVBlank();
			locations_provider = new LocationsProvider();
			display_page();
		}		
		if(is_indexing && picked_folder_path!=nullptr)
		{
			if(locations_provider->get_by_path(picked_folder_path)==nullptr)
			{
				vwf->clear(Pal4bit);					
				vwf->set_cursor(5, Strings::pcx_indexing_files);
				vwf->put_text(Strings::str_indexing_files, Pal4bit, SolidColorBrush(0x2));				
			#if USE_NTFS!=1		
				add_folder_icon->hide();
				reindex_icon->hide();
				remove_icon->hide();
			#endif
				back_arrow->hide();
				GenericScene256::frame();		
				swiWaitForVBlank();
				swiWaitForVBlank();
				
				char* buff=new char[32];
				
				DIR* dir = opendir(picked_folder_path);
				struct dirent *entry;
				int cnt=0;
				
				Location* location = new Location();
				location->path = new char[strlen(picked_folder_path)+1];
				strcpy(location->path, picked_folder_path);
				
				vwf->clear_row(6, Pal4bit);
				vwf->set_cursor(6, Strings::pcx_found_n_flipnotes);			
				vwf->put_text(str_print(buff, Strings::str_found_n_flipnotes, cnt), Pal4bit, SolidColorBrush(0x2));
				swiWaitForVBlank();
				
				Char24* dest_name = new Char24();
				while((entry=readdir(dir))!=NULL)
				{
					if(strcmp(".", entry->d_name)==0 || strcmp("..", entry->d_name)==0)
						continue;
					if(entry->d_type != DT_REG)
						continue;
					
					Debug::log(entry->d_name);
										
					int len = strlen(entry->d_name);
					if(len>28) continue;
					if(strcmp(".ppm", entry->d_name+len-4)!=0 && strcmp(".PPM", entry->d_name+len-4)!=0)
						continue;
					
					len-=4;
					
					Debug::log("Here?");					
					
					for(int i=0;i<len;i++) 
					{
						Debug::log("%i %i",len, i);
						dest_name->chars[i]=entry->d_name[i];
					}
					for(int i=len;i<24;i++)
					{
						Debug::log("%i",i);
						dest_name->chars[i]=0;		
					}
										
					location->filenames.push_back(*dest_name);					
					
					vwf->clear_row(6, Pal4bit);
					vwf->set_cursor(6, Strings::pcx_found_n_flipnotes);										
					vwf->put_text(str_print(buff, Strings::str_found_n_flipnotes, ++cnt), Pal4bit, SolidColorBrush(0x2));
					swiWaitForVBlank();
				}	
				delete dest_name;
				delete[] buff;
				delete[] picked_folder_path;
				picked_folder_path = nullptr;				
				
				
				for(int i=10;i<30;i++) swiWaitForVBlank();				
				vwf->set_cursor(7, Strings::pcx_saving);
				vwf->put_text(Strings::str_saving, Pal4bit, SolidColorBrush(0x2));
				for(int i=10;i<10;i++) swiWaitForVBlank();							
				locations_provider->add_location(location);
				
				vwf->clear_row(7, Pal4bit);
				vwf->set_cursor(7, Strings::pcx_done);			
				vwf->put_text(Strings::str_done, Pal4bit, SolidColorBrush(0x2));
				for(int i=0;i<60;i++)
					swiWaitForVBlank();
			}
			
		#if USE_NTFS!=1
			add_folder_icon->show();
			reindex_icon->show();
			remove_icon->show();
			back_arrow->show();
		#endif
			display_page();
		}
		
		Scene::run();	
	}
	
	bool show_yes_no_dialog(const char* message, int x)
	{		
		vwf->clear(Pal4bit);
		
		for(int i=0;i<4;i++)
			folder_icons[i]->hide();
		GenericScene256::frame();
		
		vwf->set_cursor(6, x);
		vwf->put_text(message, Pal4bit, SolidColorBrush(0x1));

		vwf->set_cursor(8, 86);
		vwf->put_text("Yes", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(8, 156);
		vwf->put_text("No", Pal4bit, SolidColorBrush(0x1));		

		int val=1;
		
		bool working=true;
		while(working)
		{
			swiWaitForVBlank();			
			scanKeys();
			int keys = keysDown();
			if(keys & (KEY_LEFT | KEY_RIGHT)) val^=1;
			else if(keys & KEY_A)
			{
				working=false;				
			}
			else if(keys & KEY_B)
			{
				working=false;
				val=0;
			}
			
			vwf->clear_row(8, Pal4bit);
			vwf->set_cursor(8, 86);
			vwf->put_text("Yes", Pal4bit, SolidColorBrush(0x1+val));
			vwf->set_cursor(8, 156);
			vwf->put_text("No", Pal4bit, SolidColorBrush(0x2-val));			
		}
		
		display_page();
		
		return val;
	}
	
	static int measure_string(const char* text)
	{
		int result=0;
		for(;*text;++text)		
			result+=Resources::Fonts::default_8x16.get_glyph_width(*text);		
		return result;
	}

	VwfEngine sel_vwf = VwfEngine(Resources::Fonts::default_8x16);
	char* list_buffer[4] = { nullptr, nullptr, nullptr, nullptr };
	int list_tiles_len [4]= { 0,0,0,0 };
	int sel_scroll = 0;

	int loc_selected_index=0;
	void display_page()
	{
	#if USE_NTFS!=1
		reindex_icon->show();
		remove_icon->show();
	#endif
		vwf->clear(Pal4bit);
		for(int i=0;i<4;i++)
			folder_icons[i]->hide();
		
		if(locations_provider->get_count()==0)
		{			
		#if USE_NTFS!=1
			reindex_icon->hide();
			remove_icon->hide();
		#endif
			vwf->set_cursor(6, Strings::pcx_no_flipnote_locations);	
			vwf->put_text(Strings::str_no_flipnote_locations, Pal4bit, SolidColorBrush(0x3));
			return;
		}		
		
		int page_id = loc_selected_index/4;				
		int index = page_id*4;
		
		char* buff=new char[32];
		for(int i=0;i<4;i++) 
		{
			delete[] list_buffer[i];
			list_buffer[i] = nullptr;
			list_tiles_len[i]=0;
		}
				
		for(int i=0;i<4 && index<locations_provider->get_count();i++, index++)
		{			
			Location* location = locations_provider->get_at(index);			
			folder_icons[i]->show();			
									
			list_tiles_len[i] = (measure_string(location->path)+8)/8;
			list_buffer[i] = new char[list_tiles_len[i]*64]();
			sel_vwf.set_render_space(list_buffer[i], 2, list_tiles_len[i]);
			sel_vwf.put_text(location->path, Pal4bit, SolidColorBrush(0x1+(index==loc_selected_index)));
			sel_scroll = 0;
			display_item(i);
										
			//vwf->set_cursor(2+2*i, 48);
			//vwf->put_text(location->path, Pal4bit, SolidColorBrush(0x1));			
			vwf->set_cursor(3+2*i, Strings::pcx_n_flipnotes);
			vwf->put_text(str_print(buff, Strings::str_n_flipnotes, location->filenames.size()), Pal4bit, SolidColorBrush(index==loc_selected_index ? 0x2 : 0x4));
		}
		
		delete[] buff;
	}
	
	void display_item(int i)
	{		
		short* gfx = (short*)((int)bgGetGfxPtr(6) + 64*32*(2+2*i)+64*6);
		short* src = (short*)list_buffer[i];		
		
		if(loc_selected_index%4 == i && list_tiles_len[i]>24)
		{			
			int w = 32*(sel_scroll/16);
			src+=w;
			int sz = list_tiles_len[i]*32 - w;
			int k=0;
			for(;k<min(24*32, sz);k++) *(gfx++) = *(src++);			
			volatile int zero=0;			
			for(;k<24*32;k++) *(gfx++) = zero;
			sel_scroll++;
			if(sel_scroll>=16*list_tiles_len[i])
				sel_scroll=0;
		}
		else
		{
			for(int k=0;k<min(24*32, list_tiles_len[i]*32);k++)
			{
				*(gfx++) = *(src++);
			}	
		}		
	}
	
	
	void frame() override
	{		
		display_item(loc_selected_index%4);
		GenericScene256::frame();
	}
	
	void reindex_selected()
	{
		vwf->clear(Pal4bit);					
		vwf->set_cursor(5, Strings::pcx_indexing_files);					
		vwf->put_text(Strings::str_indexing_files, Pal4bit, SolidColorBrush(0x2));				
	#if USE_NTFS!=1	
		add_folder_icon->hide();
		reindex_icon->hide();
		remove_icon->hide();
	#endif
		back_arrow->hide();
		GenericScene256::frame();		
		swiWaitForVBlank();
		swiWaitForVBlank();
		
		char* buff=new char[32];
		
		Location* location = locations_provider->get_at(loc_selected_index);
		
		DIR* dir = opendir(location->path);
		struct dirent *entry;
		int cnt=0;
		
		location->filenames.clear();		
		
		vwf->clear_row(6, Pal4bit);
		vwf->set_cursor(6, Strings::pcx_found_n_flipnotes);										
		vwf->put_text(str_print(buff, Strings::str_found_n_flipnotes, cnt), Pal4bit, SolidColorBrush(0x2));
		swiWaitForVBlank();
		
		Char24* dest_name = new Char24();
		while((entry=readdir(dir))!=NULL)
		{
			if(strcmp(".", entry->d_name)==0 || strcmp("..", entry->d_name)==0)
				continue;
			if(entry->d_type != DT_REG)
				continue;
			
			Debug::log(entry->d_name);
								
			int len = strlen(entry->d_name);
			if(len>28) continue;
			if(strcmp(".ppm", entry->d_name+len-4)!=0 && strcmp(".PPM", entry->d_name+len-4)!=0)
				continue;
			
			len-=4;
			
			Debug::log("Here?");					
			
			for(int i=0;i<len;i++) 
			{
				Debug::log("%i %i",len, i);
				dest_name->chars[i]=entry->d_name[i];
			}
			for(int i=len;i<24;i++)
			{
				Debug::log("%i",i);
				dest_name->chars[i]=0;		
			}
			
			location->filenames.push_back(*dest_name);					
			
			vwf->clear_row(6, Pal4bit);
			vwf->set_cursor(6, Strings::pcx_found_n_flipnotes);										
			vwf->put_text(str_print(buff, Strings::str_found_n_flipnotes, ++cnt), Pal4bit, SolidColorBrush(0x2));
			swiWaitForVBlank();
		}	
		delete dest_name;
		delete[] buff;
		delete[] picked_folder_path;
		picked_folder_path = nullptr;				
		
		
		for(int i=10;i<30;i++) swiWaitForVBlank();				
		vwf->set_cursor(7, 110);
		vwf->put_text("Saving", Pal4bit, SolidColorBrush(0x2));
		for(int i=10;i<10;i++) swiWaitForVBlank();							
		locations_provider->save();
		
		vwf->clear_row(7, Pal4bit);
		vwf->set_cursor(7, 114);			
		vwf->put_text("Done", Pal4bit, SolidColorBrush(0x2));
		for(int i=0;i<60;i++)
			swiWaitForVBlank();
		
	#if USE_NTFS!=1			
		add_folder_icon->show();
		reindex_icon->show();
		remove_icon->show();
	#endif
		back_arrow->show();
	}
	
	~PlaylistsScene()
	{		
		key_down.remove_event(&PlaylistsScene::on_key_down, this);
		delete vwf;		
		for(int i=0;i<4;i++)
		{
			folder_icons[i]->set_frame(0, nullptr);
			delete folder_icons[i];
			delete[] list_buffer[i];
		}
			
		delete back_arrow;			
		delete folder_highlighted_frame;		
		delete locations_provider;		
	#if USE_NTFS!=1	
		delete add_folder_icon;
		delete reindex_icon;
		delete remove_icon;
	#endif	
	}
};

LocationsProvider* PlaylistsScene::bak_locations_provider = nullptr;

Scene* get_playlists_scene()
{
	return new PlaylistsScene();
}

Scene* folder_picker_return_to_playlists()
{	
	return new PlaylistsScene(true);
}