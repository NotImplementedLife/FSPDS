#include <nds.h>
#include "scenes.hpp"
#include "simple_scene.hpp"

#include "back_arrow.h"
#include "opt_box.h"
#include <stdio.h>

class SettingsScene : public SimpleScene
{
private:
	VwfEngine vwf = VwfEngine(Resources::Fonts::default_8x16);
	Sprite* back_arrow = nullptr;
	Sprite* wipe_button = nullptr;
	
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
		
		wipe_button = create_sprite(new Sprite(SIZE_64x32, Engine::Sub));
		wipe_button->add_frame(new ObjFrame(&ROA_opt_box8,0,0));
		wipe_button->set_position(96, 39);
		wipe_button->set_priority(3);
		end_sprites_init();
		
		key_down.add_event(&SettingsScene::on_key_down, this);
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
		
		vwf.set_render_space(bgGetGfxPtr(2),24,32);
		VwfEngine::prepare_map(vwf, MAIN_BG2, 32, 0, 0, 0x9);
		vwf.clear(Pal4bit);			
						
		vwf.set_cursor(6, 104);
		vwf.put_text("Settings", Pal4bit, SolidColorBrush(0x1));				
			
		vwf.set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(vwf, SUB_BG2, 32, 0, 0, 0x9);
		vwf.clear(Pal4bit);					
		
		vwf.set_cursor(3, 100);
		vwf.put_text("Wipe data", Pal4bit, SolidColorBrush(0x1));
					
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128		
	
		Scene::run();	
	}
	
	void frame() override
	{
		SimpleScene::frame();
	}
	
	void on_key_down(void*, void* _keys)
	{
		int keys=(int)_keys;
		if(keys & KEY_B)
		{
			close()->next(gen_title_scene());
		}
		if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
			if(touch_in_rect(0,0,32,32)) 
			{								
				close()->next(gen_title_scene());
			}
			else if(touch_in_rect(96,39,64,32))
			{
				
				int x=remove("/data/FSPDS/locations");
				Debug::log("Removing file = %i", x);				
				close()->next(gen_title_scene());
			}
		}
			
	}


	~SettingsScene()
	{
		key_down.remove_event(&SettingsScene::on_key_down, this);
		delete back_arrow;
		delete wipe_button;
	}	
};

Scene* get_settings_scene() { return new SettingsScene(); }