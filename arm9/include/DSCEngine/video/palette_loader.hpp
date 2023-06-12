#pragma once

#include "DSCEngine/video/palette_manager.hpp"
#include "DSCEngine/types/vector.hpp"
#include "DSCEngine/types/hash_map.hpp"

namespace DSC
{
	class PaletteLoader
	{
		static constexpr int asset_hashtable_size = 8;
		static constexpr int record_hashtable_size = 4;
	private:
		PaletteManager* standard_pal_mng = nullptr;
		Vector<PaletteManager*> extended_pal_mng;		
		HashMap<const AssetData*, 
			HashMap<int, int, default_hash<int, record_hashtable_size>, record_hashtable_size>,
			default_hash<const AssetData*, asset_hashtable_size>, asset_hashtable_size> 
			managed_assets;
		
		PaletteAllocationResult try_alloc_standard(const AssetData* asset);
		PaletteAllocationResult try_alloc_extended(const AssetData* asset, int pal_index);		
		
		int default_alloc_mode = 0;
	public:
		PaletteLoader();
		PaletteLoader(PaletteManager* standard_pal_mng);
		PaletteLoader(PaletteManager* standard_pal_mng, const Vector<PaletteManager*>& extended_pal_mng);
		
		void add_extended_palette_manager(PaletteManager* pal_mng);
		
		void set_default_allocation_mode(int allocation_mode);
		PaletteAllocationResult try_load(const AssetData* asset, int allocation_mode = 0);
		void unload(const AssetData* asset);
		void unload(const AssetData* asset, int palette_index);
				
		static constexpr int ALLOC_MODE_DEFAULT = 0;
		static constexpr int ALLOC_MODE_STANDARD_PALETTE = 1<<0;
		static constexpr int ALLOC_MODE_EXTENDED_PALETTES = 1<<1;		
		
		static constexpr int STANDARD_PALETTE = -1;
		static constexpr int EXTENDED_PALETTE(int p) { return p; }
		
		~PaletteLoader();		
	};
}