#pragma once

#include "DSCEngine/resources/asset_data.hpp"

namespace DSC
{
	class VramLoader
	{
	public:
		static void load(const AssetData* asset, void* dest, short* pal_indices = nullptr, int map_width = 0);
		static void load(const AssetData* asset, int offset, int size, void* dest, short* pal_indices = nullptr, int map_width = 0);
		
		static void clear(void* dest, int size);
	};
}