#include "scenes.hpp"

#include <fat.h>
#include <filesystem.h>
#include "globals.hpp"

using namespace DSC;

void DSC::init_main_scene()
{	
	DSC::Debug::log("NITRO = %i\n", USE_NTFS);
	#if USE_NTFS==1
		set_fat_inited(nitroFSInit(NULL));		
	#else
		set_fat_inited(fatInitDefault());
	#endif	

	DSC::__MAIN_SCENE__= gen_title_scene();
}

