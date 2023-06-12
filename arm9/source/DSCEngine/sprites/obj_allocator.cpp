#include "DSCEngine/sprites/obj_allocator.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/video/measure.hpp"
#include "DSCEngine/utils/math.hpp"
#include "DSCEngine/video/vram_loader.hpp"

using namespace DSC;

DSC::ObjAllocator::ObjAllocator(Allocator* allocator, PaletteLoader* palette_loader, int bytes_per_entry) 
	: allocator(allocator), palette_loader(palette_loader), bytes_per_entry(bytes_per_entry)
{
	nds_assert((bytes_per_entry & (bytes_per_entry - 1)) == 0 // bytes_per_entry is a power of 2
		&& 32 <= bytes_per_entry 
		&& bytes_per_entry <=256, "Wrong allocation block size");
}

void DSC::ObjAllocator::remove(const AssetData* asset, Point<short> metatile)
{
	if(!loaded_frames.contains_key(asset)) return;
	if(!loaded_frames[asset].contains_key(metatile)) return;
	loaded_frames[asset].remove_key(metatile);
	if(loaded_frames[asset].size()==0)
		loaded_frames.remove_key(asset);
}
		
bool DSC::ObjAllocator::allocate(ObjFrame* frame)
{
	frame->unload.add_event(&ObjAllocator::on_frame_unload, this);
	
	const AssetData* asset = frame->asset;
	Point<short> metatile = { x : frame->metatile_col, y : frame->metatile_row};
	
	loaded_frames[asset][metatile]++;
	
	if(loaded_frames[asset][metatile]==1)
	{
	
		int meta_size = asset->get_metatile_size();
		Debug::log("MS = %i",meta_size);
		int alloc_size = MeasureValue(meta_size).fit().blocks(bytes_per_entry).value() * bytes_per_entry;
		Debug::log("~ALLOC~ Size = %i",alloc_size);
		void* address = allocator->reserve(alloc_size);	
		Debug::log("~ALLOC~ Reserved VRAM : 0x%X", address);
		
		if(address == nullptr) // failed
		{
			remove(asset, metatile);
			return false;
		}
	
		frame->gfx_ptr = address;			
		
		// load frame graphics
		
		PaletteAllocationResult palloc = palette_loader->try_load(frame->asset, PaletteLoader::ALLOC_MODE_DEFAULT);		
		
		nds_assert(palloc.succeeded, "Palette allocation failed");
		
		nds_assert(frame->asset->get_metatile_width() != 0);
		int metatiles_per_row = frame->asset->width / frame->asset->get_metatile_width();
		
		VramLoader::load(frame->asset,  
			Measure()
				.bpp(asset->get_color_depth())
				.metatile(asset->get_metatile_width(), asset->get_metatile_height())
				.tiles(metatiles_per_row * frame->metatile_row + frame->metatile_col),
			Measure()
				.bpp(frame->asset->get_color_depth())
				.metatile(asset->get_metatile_width(), asset->get_metatile_height())
				.tiles(1),
			frame->gfx_ptr, palloc.indices);
	}
	
	return true;
}

void DSC::ObjAllocator::deallocate(ObjFrame* frame)
{
	Debug::log("Deallocating frame");
	const AssetData* asset = frame->asset;
	Point<short> metatile = { x : frame->metatile_col, y : frame->metatile_row};
	
	if(!loaded_frames.contains_key(asset)) return;
	if(!loaded_frames[asset].contains_key(metatile)) return;
	
	loaded_frames[asset][metatile]--;
	
	// forgot palette unload here...
	if(loaded_frames[asset][metatile] == 0)
	{
		Debug::log("Clear %X : %i",frame->gfx_ptr, Measure()
			.bpp(asset->get_color_depth())
			.metatile(asset->get_metatile_width(), asset->get_metatile_height())
			.tiles(1).value());
		VramLoader::clear(frame->gfx_ptr, Measure()
			.bpp(asset->get_color_depth())
			.metatile(asset->get_metatile_width(), asset->get_metatile_height())
			.tiles(1));
		allocator->release(frame->gfx_ptr);		
		frame->gfx_ptr = nullptr;
		
		loaded_frames[asset].remove_key(metatile);
	}	
	if(loaded_frames[asset].size()==0)
		loaded_frames.remove_key(asset);
}

void DSC::ObjAllocator::on_frame_unload(void* sender, void* args)
{
	ObjFrame* frame = (ObjFrame*)sender;
	deallocate(frame);
}