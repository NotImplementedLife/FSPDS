#include <nds.h>

#include "scenes.hpp"

#include "simple_scene.hpp"

class CreditsScene : public SimpleScene
{	
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	void init() override
	{
		SimpleScene::init();
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
		require_tiledmap_4bpp(SUB_BG2, 256, 256, 32*24);
		
		key_down.add_event(&CreditsScene::on_key_down, this);
	}
	
	void on_key_down(void*, void* _k)
	{
		int keys = (int)_k;
		if(keys & KEY_TOUCH)
		{
			close()->next(gen_title_scene());
		}
	}
	
	
	__attribute__((noinline))
	void run() override
	{
		solve_map_requirements();
		load_assets();
		
		BG_PALETTE[0x91]=BG_PALETTE_SUB[0x91]=Colors::Black;
		BG_PALETTE[0x92]=BG_PALETTE_SUB[0x92]=Colors::Blue;
		
		vwf->set_render_space(bgGetGfxPtr(2),24,32);
		VwfEngine::prepare_map(*vwf, MAIN_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
						
		vwf->set_cursor(1, 8);
		vwf->put_text("Original author : ", Pal4bit, SolidColorBrush(0x1));		
		vwf->put_text("NotImplementedLife", Pal4bit, SolidColorBrush(0x2));
		vwf->set_cursor(2, 8);		
		vwf->put_text("With help from:", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(3, 8);		
		vwf->put_text("-   ", Pal4bit, SolidColorBrush(0x1));
		vwf->put_text("RinLovesYou", Pal4bit, SolidColorBrush(0x2));
		vwf->put_text(" for her ADPCM to PCM", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(4, 8);		
		vwf->put_text("     converter function in PPMLib", Pal4bit, SolidColorBrush(0x1));
		
		vwf->set_cursor(5, 8);
		vwf->put_text("-   ", Pal4bit, SolidColorBrush(0x1));
		vwf->put_text("tomrow", Pal4bit, SolidColorBrush(0x2));
		vwf->put_text(" for their suggestion on", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(6, 8);
		vwf->put_text("    alternate flipnote browse locations", Pal4bit, SolidColorBrush(0x1));
		
		vwf->set_cursor(7, 8);
		vwf->put_text("-   Guys from ", Pal4bit, SolidColorBrush(0x1));
		vwf->put_text("DSiBrew", Pal4bit, SolidColorBrush(0x2));
		vwf->put_text(" & ", Pal4bit, SolidColorBrush(0x1));
		vwf->put_text("Flipnote Collective", Pal4bit, SolidColorBrush(0x2));
		vwf->set_cursor(8, 8);
		vwf->put_text("    for their awesome documentation on .PPM", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(9, 8);
		vwf->put_text("    file format", Pal4bit, SolidColorBrush(0x1));
			
		
		vwf->set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(*vwf, SUB_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
		
		vwf->set_cursor(1, 8);		
		vwf->put_text("Awesome open-source NDS solutions", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(2, 8);	
		vwf->put_text("that helped during development:\n", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(4, 8);	
		vwf->put_text("GodMode9i", Pal4bit, SolidColorBrush(0x2));
		
		vwf->set_cursor(4, 91);	
		vwf->put_text("GameYob", Pal4bit, SolidColorBrush(0x2));
		
		vwf->set_cursor(4, 179);	
		vwf->put_text("Moonshell2", Pal4bit, SolidColorBrush(0x2));
		
		vwf->set_cursor(6, 8);	
		vwf->put_text("Compiled with ", Pal4bit, SolidColorBrush(0x1));
		vwf->put_text("devkitPro", Pal4bit, SolidColorBrush(0x2));
		
		vwf->set_cursor(10, 20);	
		vwf->put_text("Touch the screen to return to title", Pal4bit, SolidColorBrush(0x1));
					
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		Scene::run();	
	}
	
	~CreditsScene()
	{
		key_down.remove_event(&CreditsScene::on_key_down, this);
		delete vwf;
	}
};



Scene* get_credits_scene()
{
	return new CreditsScene();
}