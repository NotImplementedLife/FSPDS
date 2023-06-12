#pragma once

namespace DSC
{
	template<typename T, int S> int default_hash(const T& item) // not safe with VRAM
	{
		int len = sizeof(item);
		int result = 0;
		char* o = (char*)&item;
		for(int i=0;i<len;i++)
			result+=o[i];
		return result % S;
	}
}