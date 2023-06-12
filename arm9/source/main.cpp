#include "scenes.hpp"

#include "bg2.h"

using namespace DSC;

class MainScene : public GenericScene256
{
	void init() override
	{
		GenericScene256::init();
		require_bitmap(MAIN_BG3, &ROA_bg28);
		require_bitmap(SUB_BG3, &ROA_bg28);
		
		key_down.add_event(&MainScene::on_key_down, this);
	}
	
	void on_key_down(void*, void*)
	{
		close()->next(gen_title_scene());
	}
};

Scene* gen_main_scene() { return new MainScene(); }

dsc_launch(MainScene)