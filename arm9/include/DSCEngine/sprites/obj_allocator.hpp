#pragma once

#include "DSCEngine/resources/asset_data.hpp"
#include "DSCEngine/sprites/obj_frame.hpp"
#include "DSCEngine/types/hash_map.hpp"
#include "DSCEngine/types/hash_set.hpp"
#include "DSCEngine/types/point.hpp"
#include "DSCEngine/video/allocator.hpp"
#include "DSCEngine/video/palette_loader.hpp"

namespace DSC
{
	class ObjFrame;
	
	class ObjAllocator
	{
	private:
		HashMap<const AssetData*, HashMap<Point<short>, int>> loaded_frames;
		
		Allocator* allocator;
		PaletteLoader* palette_loader;
		
		int bytes_per_entry = 32;
		
		void remove(const AssetData* asset, Point<short> metatile); //!< removes frame key from the loaded frames		
		
		void on_frame_unload(void* sender, void* args); //!< handles automatic frame deallocation
	public:
		/*! \brief creates a new ObjAllocator instance
			\param allocator VRAM allocator to be used
			\param bytes_per_entry minimum bytes allocation block count (32, 64, 128, 256)			
		 */
		ObjAllocator(Allocator* allocator, PaletteLoader* palette_loader, int bytes_per_entry = 32);
		
		/*! \brief allocates new block for the specified frame
			\param frame object frame data
		 */
		bool allocate(ObjFrame* frame);
		
		/*! \brief deallocates new block for the specified frame
			\param frame object frame data
		 */
		void deallocate(ObjFrame* frame);			
		
		~ObjAllocator() = default;
	};
}