#pragma once

#include <cstdarg>

namespace DSC::Debug
{	
	void log_to_buffer(char* buffer, const char* message, va_list args);	// private method that logs to string
}