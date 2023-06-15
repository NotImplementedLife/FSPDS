#include "simple_scene.hpp"

#include "simple_bg.h"

using namespace DSC;

void SimpleScene::init()
{
	GenericScene256::init();
	require_tiledmap(MAIN_BG3, 256, 256, &ROA_simple_bg4);
	require_tiledmap(SUB_BG3, 256, 256, &ROA_simple_bg4);
	
	//for(int i=0;i<32*24;i++)
}

#include "scenes.hpp"
Scene* get_simple_scene() { return new SimpleScene(); }