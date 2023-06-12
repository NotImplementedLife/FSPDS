/*! \file palette_manager.hpp
	\brief PaletteManager class definitions
 */
#pragma once

#include "DSCEngine/types/hash_map.hpp"
#include "DSCEngine/resources/asset_data.hpp"

namespace DSC
{	
	struct PaletteAllocationResult;

	/*! \brief Automatic palette manager
	 */
	class PaletteManager
	{
	private:
		static int hashColor(const short& color);
	
		void* pal_offset;		
		int free_space[8];
		
		short records4bpp[16];
		
		HashMap<short, int, hashColor, 128> colors_map;						
		
		int get_chksum(short* colors);
		int get_pal16(short* colors, int chksum);
	public:
		/*! \brief creates a new PaletteManager instance
			\param palettes_offset  the address of the palette data managed by the instance			
		 */
		PaletteManager(void* palettes_offset);
		
		/*! \brief choose an index for the specified color
			\param color a BGR15 color value
			\return a value V that tells the palette index it has been placed at.
			
			If the color already exists in the palette, it is not added once again and the position of
			the already existing color is returned.
		 */		 
		int reserve1(int color);
		
		/*! \brief loads a 4-bit palette
			\param palette4 address to a stream of 16 unsigned shorts represing the BGR15 color codes 
			\return the id of the palette slot assigned to the given 4-bit palette has been, a number from 0 (0x0) to 15 (0xF)
			
			\details This function does not have the expected effect unless the target palette is a standard palette.
			Do not load 4-bit palettes in extended slots. They may be loaded, but can't be used in the intended ways.			
		 */
		int reserve16(const void* palette4);
		
		
		/*! \brief removes a certain color from the palette
			\param color a BGR15 color value
			
			\details If the color is being used by more than one asset, it is still kept in the palette even after unloading it.
			A color is fully released only after no other resources use it.
			Attempts to unload an inexistent color are simply ignored.
		 */
		void unload1(int color);
		
		/*! \brief removes a 4-bit palette
			\param palette4 address to a stream of 16 unsigned shorts represing the BGR15 color codes 
			
			\details A 4-bit palette is fully released only after no other resources use it. 
			Attempts to unload an inexistent palette are simply ignored.
		 */
		void unload16(const void* palette4);
		
		
		/*! \brief Attempts to load colors from asset data palette
			\param asset asset data source
			\return allocation result information			
		 */
		PaletteAllocationResult try_load(const AssetData* asset);
		
		
		/*! \brief Unloads colors from a previously loaded asset data palette
			\param asset asset data source
			\warning No checks are performed to ensure that the asset's palette
			has been previously loaded before deallocation happens. Accidentally
			unloading "unloaded" data may result in possible data loss and corruption.
		 */
		void unload(const AssetData* asset);
		
		~PaletteManager();
	};
	
	/*! \brief Struct that contains information following an asset allocation attempt via PaletteManager::try_load().		
	 */
	struct PaletteAllocationResult
	{
		/*! \brief The positions colors have been dynamically allocated to
			\details
			  - if asset is 4-bit, then indices contains a single value, which is the 4-bit palette slot that has been
				assigned to the asset's palette. (0x0..0xF)
			  - if asset is 8-bit, then for any index i, indices[i] is the position of the color that has the index i in
				the readonly asset data
		 */
		short* indices = nullptr;
		
		/*! \brief size of the provided indices
			\details
			  - if asset is 4-bit, then the length is 1
			  - if asset is 8-bit, then the length is equal to the asset's palette length
		 */
		int length;		
		
		/*! \brief color depth of the asset 
		 */
		char color_depth; 	
		
		/*! \brief flag that specifies whether the operation succeeded or failed. 
			This struct contains relevant data only if succeeded == true.
			*/
		bool succeeded;		
		
		~PaletteAllocationResult(); // this is in order not to worry about the indices deallocation
		
		PaletteAllocationResult(short* indices = nullptr, int length = 0, char color_depth = 0, bool succeeded = false);
		PaletteAllocationResult(const PaletteAllocationResult&);
		PaletteAllocationResult(PaletteAllocationResult&&);
		
		PaletteAllocationResult& operator = (const PaletteAllocationResult& other);
		PaletteAllocationResult& operator = (PaletteAllocationResult&& other);
		
		bool operator == (const PaletteAllocationResult& other) const;
	};
}