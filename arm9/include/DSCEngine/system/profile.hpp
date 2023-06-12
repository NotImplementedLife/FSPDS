#pragma once

#include "DSCEngine/types/string.hpp"

namespace DSC
{
	/*! \brief access to user's personal data
	 */
	class Profile
	{
	public:
		struct Birthday 
		{
			unsigned char day;
			unsigned char month;
		};
		
		static Birthday birthday();
		
		// HOLY user profile text is UTF16.......
		/*static String name();
		
		static String message();*/ 
	};
}