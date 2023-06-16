#include "scenes.hpp"

#include <fat.h>
#include "globals.hpp"

using namespace DSC;

void DSC::init_main_scene()
{	
	set_fat_inited(fatInitDefault());

	DSC::__MAIN_SCENE__= gen_title_scene();
}

