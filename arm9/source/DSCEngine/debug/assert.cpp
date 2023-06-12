#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/error.hpp"

void DSC::__assert__(bool condition, const char* message)
{	
	if(!condition)
		DSC::fatal("Assertion failed : %s", message);
}