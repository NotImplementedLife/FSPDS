#include "DSCEngine/debug/log.hpp"
#include <cstdarg>

// gcc translates inline asm "mov r7,r7" to "add r7, r7, #0"
// so we have to hardcode the intended instruction
#define MOV_R7_R7 ".BYTE 0x3f, 0x46"
	
#define MOV_R6_R6 ".BYTE 0x36, 0x46"

#define MOV_R5_R5 ".BYTE 0x2D, 0x46"	

#define LOG_CODE(MOV_X_X, message) asm volatile \
	( \
		"push {r6} \r\n" \
		"mov r6, %0 \r\n" \
		MOV_X_X "\r\n" \
		"pop {r6}\r\n" \
		:: "r"(message) \
	);

#define LOG_BUF 8
#define LOG_MSG 7
#define LOG_WRN 6
#define LOG_ERR 5

namespace
{	
	
	char* copy_str(char* dest, const char* src)
	{
		for(;*src;) 
			*(dest++) = *(src++);		
		*dest = '\0';
		return dest;
	}
	
	__attribute__((target("thumb"))) 
	void _log(int role, const char* message)
	{
		switch(role)
		{
		case LOG_MSG:
			LOG_CODE(MOV_R7_R7, message); break;
		case LOG_WRN:
			LOG_CODE(MOV_R6_R6, message); break;
		case LOG_ERR:
			LOG_CODE(MOV_R5_R5, message); break;		
		case LOG_BUF:
			break;
		}		
	}	
	
	static const char* hex_lower = "0123456789abcdef";
	static const char* hex_upper = "0123456789ABCDEF";
		
	__attribute__((target("thumb"))) 
	void _logv(int role, char* dest, const char* message, va_list args)
	{							
		char* result = new char[1024];
		char* built = result;
				
		for(const char* msg=message; *msg;)
		{
			if(*msg=='%')
			{
				++msg;
				if(*msg==0) break;															
				
				switch(*msg)
				{
					case 'i':
					{						
						int val = va_arg(args, int);

						if(val==0)
						{
							*(built++)='0'; 
							++msg;
							break;
						}
						
						if(val<0)						
							*(built++)='-', val = -val;						
						
						int nzeros = 0;
						for(;val%10==0;val/=10) nzeros++;
						
						int temp = 0;
						for(;val;val/=10) temp=temp*10 + val%10;						
						for(;temp;temp/=10) *(built++)='0'+temp%10;
						
						for(;nzeros--;) *(built++)='0';
						
						++msg;
						break;
					}
					case 'u':
					{
						unsigned int val = va_arg(args, unsigned int);
						
						if(val==0)
						{
							*(built++)='0';
							++msg;
							break;
						}
						
						int nzeros = 0;
						for(;val%10==0;val/=10) nzeros++;
												
						int temp = 0;
						for(;val;val/=10) temp=temp*10 + val%10;						
						for(;temp;temp/=10) *(built++)='0'+temp%10;
						
						for(;nzeros--;) *(built++)='0';
						
						++msg;						
						break;
					}
					case 's':
					{
						char* val = va_arg(args, char*);
						
						while(*val) *(built++)=*(val++);
											
						++msg;
						break;
					}
					case 'b':
					{
						int val = va_arg(args, int);
						if(val)
							built = copy_str(built, "T");
						else 
							built = copy_str(built, "F");
						
						++msg;
						break;
					}
					case 'B':
					{
						int val = va_arg(args, int);
						if(val)
							built = copy_str(built, "True");
						else 
							built = copy_str(built, "False");
						
						++msg;
						break;
					}
					case 'x':					
					{
						unsigned val = va_arg(args, unsigned);
						bool num_start = false;
						for(int i=0;i<8;i++)
						{
							int digit = (val & 0xF0000000)>>28;
							val<<=4;
							if(!num_start && digit==0)
								continue;											
							num_start = true;
							*(built++) = hex_lower[digit];
						}		
						if(!num_start)
						{
							*(built++)='0';
						}
						++msg;
						break;
					}
					case 'X':
					{
						unsigned val = va_arg(args, unsigned);
						bool num_start = false;
						for(int i=0;i<8;i++)
						{
							int digit = (val & 0xF0000000)>>28;
							val<<=4;
							if(!num_start && digit==0)
								continue;											
							num_start = true;
							*(built++) = hex_upper[digit];
						}						
						if(!num_start)
						{
							*(built++)='0';
						}						
						++msg;
						break;
					}
				}					
			}
			else
			{
				*(built++)=*(msg++);
			}
		}					
		*(built)='\0';		
		
		_log(role, result);		
		if(role==LOG_BUF)
		{
			copy_str(dest, result);
		}
		
		delete[] result;
	}		
}

void DSC::Debug::log(const char* message, ...) 
{ 
	va_list args;	
	va_start(args, message);
	_logv(LOG_MSG, nullptr, message, args);	
	va_end(args);
}

void DSC::Debug::warn(const char* message, ...)
{ 
	va_list args;
	va_start(args, message);
	_logv(LOG_WRN, nullptr, message, args);	
	va_end(args);
}

void DSC::Debug::error(const char* message, ...) 
{ 
	va_list args;
	va_start(args, message);
	_logv(LOG_ERR, nullptr, message, args);	
	va_end(args);
}

#include "log.h"

void DSC::Debug::log_to_buffer(char* buffer, const char* message, va_list args)
{	
	_logv(LOG_BUF, buffer, message, args);	
}