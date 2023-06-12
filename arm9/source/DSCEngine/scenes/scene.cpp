#define DSC_INTERNAL

#include "DSCEngine/scenes/scene.hpp"
#include "DSCEngine/debug/log.hpp"
#include "../entrypoint.hpp"

#include <nds.h>

void DSC::Scene::init() { }

void DSC::Scene::frame() { }

__attribute__((noinline))
void DSC::Scene::run()
{
	while(1)
	{
		swiWaitForVBlank();
				
		scanKeys();		
		
		int keys_down_vals = keysDown();
		if(keys_down_vals)
		{
			key_down.trigger(this, (void*)keys_down_vals);
		}
		
		int keys_held_vals = keysHeld();
		if(keys_held_vals)
		{
			key_held.trigger(this, (void*)keys_held_vals);
		}
		
		int keys_up_vals = keysUp();
		if(keys_up_vals)
		{
			key_up.trigger(this, (void*)keys_up_vals);
		}	
		
		frame();		
	}	
}


DSC::Scene* DSC::__MAIN_SCENE__ = nullptr;
DSC::SceneCom DSC::__SceneComInstance__;

void DSC::SceneCom::next(Scene* new_scene)
{	
	__MAIN_SCENE__ = new_scene;		
	__entrypoint_return();
	Debug::error("Scene transition missed");	
}

DSC::SceneCom* DSC::Scene::close()
{		
	//Debug::log("Deleting scene");
	delete this;
	//Debug::log("Deleted scene");
	return &__SceneComInstance__;
}