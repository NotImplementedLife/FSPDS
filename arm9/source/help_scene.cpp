#include <nds.h>
#include "scenes.hpp"
#include "simple_scene.hpp"

#include "back_arrow.h"
#include "folder_icon.h"
#include "green_frog_ss.h"


class HelpScene : public SimpleScene
{
private:
	VwfEngine vwf_top = VwfEngine(Resources::Fonts::default_8x16);
	VwfEngine vwf_bottom = VwfEngine(Resources::Fonts::default_8x16);
	
	Sprite* back_arrow = nullptr;
	
	Vector<Sprite*> help_assets;
	
	int sp_loc_add;
	int sp_loc_reindex;
	int sp_loc_remove;
	int sp_green_frog_ss;
	
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
		SimpleScene::init();
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
		require_tiledmap_4bpp(SUB_BG2, 256, 256, 32*24);
		
		begin_sprites_init();
		
		back_arrow = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		back_arrow->add_frame(new ObjFrame(&ROA_back_arrow8,0,0));
		back_arrow->set_position(0, 0);
		
		sp_loc_add = load_help_asset(&ROA_folder_icon8, SIZE_32x32, 0, 3);
		sp_loc_reindex = load_help_asset(&ROA_folder_icon8, SIZE_32x32, 0, 4);
		sp_loc_remove = load_help_asset(&ROA_folder_icon8, SIZE_32x32, 0, 5);
		sp_green_frog_ss = load_help_asset(&ROA_green_frog_ss8, SIZE_64x64);
		
		end_sprites_init();		
		
		key_down.add_event(&HelpScene::on_key_down, this);
	}
	
	int load_help_asset(const AssetData* asset, DSC::ObjSize size, int row=0, int col=0)
	{		
		Sprite* spr = create_sprite(new Sprite(size, Engine::Main));
		spr->add_frame(new ObjFrame(asset, row, col));
		spr->hide();
		
		help_assets.push_back(spr);
		return help_assets.size()-1;
	}
	
	void show(int sp_id, int x, int y)
	{
		help_assets[sp_id]->set_position(x,y);
		help_assets[sp_id]->show();
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
		
		vwf_top.set_render_space(bgGetGfxPtr(2)+2*16,22,30);
		
		volatile int zero=0;
		for(int i=0;i<32*24;i++)
			bgGetMapPtr(2)[i]=zero;
		
		VwfEngine::prepare_map(vwf_top, MAIN_BG2, 32, 1, 1, 0x9);
		vwf_top.clear(Pal4bit);							

		vwf_bottom.set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(vwf_bottom, SUB_BG2, 32, 0, 0, 0x9);
		vwf_bottom.clear(Pal4bit);					
		
		draw_bottom_menu();
		draw_top();
					
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128		
	
		Scene::run();	
	}
		
	
	int selected_menu_option = 0;
	void draw_bottom_menu()
	{
		vwf_bottom.clear(Pal4bit);
		
		for(int i=0;i<menu_options_count;i++)
		{
			vwf_bottom.set_cursor(2+i, 8);
			vwf_bottom.put_text(menu_options[i], Pal4bit, SolidColorBrush(0x1 + (selected_menu_option==i)));
		}
	}
	
	void on_key_down(void*, void* _keys)
	{
		int keys = (int)_keys;
		
		if(keys & KEY_DOWN)
		{
			if(selected_menu_option+1<menu_options_count)
			{
				selected_menu_option++;
				draw_bottom_menu();				
				draw_top();				
			}
		}
		else if(keys & KEY_UP)
		{
			if(selected_menu_option>0)
			{
				selected_menu_option--;
				draw_bottom_menu();				
				draw_top();
			}
		}				
		else if(keys & KEY_B)
		{
			close()->next(gen_title_scene());
		}
		else if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
			if(touch_in_rect(0,0,32,32))
				close()->next(gen_title_scene());
		}
	}
	
	void draw_top()
	{		
		vwf_top.clear(Pal4bit);	
		
		for(int i=0;i<help_assets.size();i++) help_assets[i]->hide();
		
		switch(selected_menu_option)
		{
			case 0: draw_what_is_fspds(); break;
			case 1: draw_where_to_find_flipnotes(); break;
			case 2: draw_what_is_location_indexing(); break;
			case 3: draw_what_is_the_dead_green_frog_tumbnail(); break;
			case 4: draw_fat_init_failed_why(); break;
		}
	}
	
	void draw_what_is_fspds()
	{
		vwf_top.put_text("FSPDS is a homebrew tool aimed to bring DSi\nFlipnote Studio creations to the original Nintendo DS. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("Flashcards are the intended way to load this application, but it can also run from modded a modded DSi/3DS.", Pal4bit, SolidColorBrush(0x1));		
		vwf_top.put_text("This way, users who don't own a DSi, or don't have access to a legit copy of Flipnote Studio, can as well enjoy ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("playing flipnotes from their DS consoles. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("Note that FSPDS is just a playback utility, not a PPM flipnote creator!", Pal4bit, SolidColorBrush(0x1));
	}
	
	void draw_where_to_find_flipnotes()
	{
		vwf_top.put_text("You can export flipnotes from Flipnote Studio to your SD card. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("The flipnotes will be then found in a \"/private/ds/app/ 4b755XX\" path, where XX is a DSi region-specific code.\n\n", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("If you don't have Flipnote Studio installed, you can find a handful of nicely preserved flipnote archives on the internet. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("Just download and unzip them onto\nyour SD card.", Pal4bit, SolidColorBrush(0x1));
	}
	
	void draw_what_is_location_indexing()
	{
		vwf_top.put_text("Flipnote paths are stored into a configuration file to optimize their access. You can choose which location will FSPDS manage", Pal4bit, SolidColorBrush(0x1));
		show(sp_loc_add, 8, 64+8);
		show(sp_loc_reindex, 8, 96+8);
		show(sp_loc_remove, 8, 128+8);
		
		vwf_top.set_cursor(4, 36);
		vwf_top.put_text("Add a location", Pal4bit, SolidColorBrush(0x2));
		vwf_top.set_cursor(5, 36);
		vwf_top.put_text("Pick a folder to find flipnotes in", Pal4bit, SolidColorBrush(0x1));
		
		vwf_top.set_cursor(6, 36);
		vwf_top.put_text("Reindex selected location", Pal4bit, SolidColorBrush(0x2));
		vwf_top.set_cursor(7, 36);
		vwf_top.put_text("Use it when folder content changes", Pal4bit, SolidColorBrush(0x1));
		
		vwf_top.set_cursor(8, 36);
		vwf_top.put_text("Remove selected location", Pal4bit, SolidColorBrush(0x2));
		vwf_top.set_cursor(9, 36);
		vwf_top.put_text("Removes location from config data.", Pal4bit, SolidColorBrush(0x1));
		vwf_top.set_cursor(10, 36);
		vwf_top.put_text("The actual directory isn't deleted.", Pal4bit, SolidColorBrush(0x1));
	}
	
	void draw_what_is_the_dead_green_frog_tumbnail()
	{
		show(sp_green_frog_ss, 96, 8);
		vwf_top.set_cursor(4, 0);
		vwf_top.put_text("When the dead frog thumbnail appears, it means that something went wrong with loading a flipnote. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("In most cases the folder content has been changed and the indexed location data does not match with the actual files hierarchy. ", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("Reindexing the location might fix the issue.", Pal4bit, SolidColorBrush(0x1));
	}
	
	void draw_fat_init_failed_why()
	{
		vwf_top.put_text("When the \"FAT init FAILED\" warning appears\non the title screen, it means that FSPDS does not have access to an external file system e.g. the SD card.\n", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("This happens, for example, when you launch FSPDS from an emulator.\n", Pal4bit, SolidColorBrush(0x1));
		vwf_top.put_text("In this situation, you can't view flipnotes from the FAT system. FSPDS provides some built-in flipnotes to play instead.", Pal4bit, SolidColorBrush(0x1));
	}
	
	~HelpScene()
	{
		key_down.remove_event(&HelpScene::on_key_down, this);
		
		for(int i=0;i<help_assets.size();i++)
			delete help_assets[i];
		delete back_arrow;
	}	
	
	inline static constexpr const char* menu_options[] = 
	{
		"What is FSPDS?",
		"Where to find flipnotes?",
		"What is location indexing?",		
		"What is the dead green frog thumbnail?",
		"\"FAT init FAILED\", why?",
	};
	inline static constexpr int menu_options_count = 5;
	
};

Scene* get_help_scene() { return new HelpScene(); }