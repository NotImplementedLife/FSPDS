#include "DSCEngine/system/profile.hpp"

#include <nds.h>

DSC::Profile::Birthday DSC::Profile::birthday()
{
	return 
	{
		day   : PersonalData->birthDay,
		month : PersonalData->birthMonth
	};
}