#include "DSCEngine/debug/error.hpp"

#include "log.h"

#include <cstdarg>
#include <nds.h>

using namespace DSC;

void DSC::fatal_freeze(const char* message, ...)
{	
	char* buffer = new char[1024];		
	
	va_list args;
	va_start(args, message);
	Debug::log_to_buffer(buffer, message, args);
	va_end(args);
		
	// do something here...
		
	while(1)
	{
		swiWaitForVBlank();
	}
}