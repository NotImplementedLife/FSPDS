#include "scenes.hpp"

#include "player_bg.h"

using namespace DSC;

class MainScene : public GenericScene256
{
	void init() override
	{
		GenericScene256::init();
		require_bitmap(MAIN_BG3, &ROA_player_bg8);
		require_bitmap(SUB_BG3, &ROA_player_bg8);
		
		key_down.add_event(&MainScene::on_key_down, this);
	}
	
	void on_key_down(void*, void*)
	{
		close()->next(gen_title_scene());
	}
};

Scene* gen_main_scene() { return new MainScene(); }
