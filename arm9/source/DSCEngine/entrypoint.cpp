#define DSC_INTERNAL
#include "DSC"
#include "DSCEngine/testmod.hpp"

#include "entrypoint.hpp"
#include <nds.h>

using namespace DSC;

volatile int __entrypoint_r13;
volatile int __entrypoint_r14;
volatile int __entrypoint_r15;

int main()
{
	if(IS_TESTMOD)
	{
		Debug::log("This is a DSC Test Module");		
	}
	
	// On startup, some emulators may close/open the LID (?)
	// which counts as a key event potentially captured by Scene.
	// so we leave a window of some VBlanks for the KEY_LID 
	// stuff to pass
	for(int i=0;i<3;i++) swiWaitForVBlank();
	
	#ifndef TESTMOD
	{
		init_main_scene();
		
		__entrypoint_capture();
		
		while(__MAIN_SCENE__)
		{
			__MAIN_SCENE__->init();
			__MAIN_SCENE__->run();
		}	
	}
	#else
	{
		testmod_init();
		testmod_capture();
		
		Debug::log("-------------------------------");
		
		if(testmod_fatal_raised())
		{
			if(testmod_expected_fatal())
				testmod_report_success();
			else 
				testmod_report_fail();
		}
		while(!testmod_ready())
		{
			//Debug::log("Running testmod");
			testmod_reset();
			testmod_load_next();			
			testmod_run();
			if(testmod_expected_fatal())
			{				
				testmod_report_fail();
			}
			else
			{
				testmod_report_success();
			}
		}
		Debug::log("-------------------------------");
		testmod_terminate();
	}
	#endif
	
	while(1) swiWaitForVBlank();
	return 0;
}