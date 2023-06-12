#include "DSCEngine/video/vram_loader.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/log.hpp"

#include <nds.h>

using namespace DSC;

void DSC::VramLoader::load(const AssetData* asset, void* dest, short* pal_indices, int map_width)
{	
	nds_assert(asset!=nullptr);
	if(!asset->is_bitmap() || (asset->is_bitmap() && (map_width == 0 || map_width == 8*asset->width)))
	{		
		if(pal_indices==nullptr || asset->get_color_depth()!=8)
		{					
			asset->extract(dest, 0, asset->get_gfx_length());									
			return;
		}		
		int len = asset->get_gfx_length();		
		unsigned char* buffer = new unsigned char[len];		
		
		asset->extract(buffer, 0, len);
		
		for(int i=0;i<len;i++)
		{
			if(buffer[i])
				buffer[i] = pal_indices[buffer[i]];
		}				
		
		dmaCopy(buffer, dest, len);	// <-- beware for bugs idk			
			
		delete[] buffer;
		
		//Debug::log("fin");
	}
	else // if(asset->is_bitmap() && 8*asset->width != map_width
	{				
		nds_assert(8*asset->width < map_width);
		
		int len = asset->get_gfx_length();
		unsigned char* bufferx = new unsigned char[len];		
		
		asset->extract(bufferx, 0, len);
		
		for(int i=0;i<len;i++)
		{
			if(bufferx[i])
				bufferx[i] = pal_indices[bufferx[i]];
		}				
		
		int stride = asset->width * asset->get_color_depth();
		
		unsigned char* dest8 = (unsigned char*)dest;
		unsigned char* buffer = (unsigned char*)bufferx;
		for(int y=0;y<8*asset->height;y++)
		{			
			dmaCopy(buffer, dest8, stride);
			buffer += stride;
			dest8 += map_width * asset->get_color_depth() / 8;
		}
		
		delete[] bufferx;		
	}	
}

void DSC::VramLoader::load(const AssetData* asset, int offset, int size, void* dest, short* pal_indices, int map_width)
{
	nds_assert(asset!=nullptr);
	nds_assert(offset+size<=asset->get_gfx_length());
	
	if(!asset->is_bitmap() || (asset->is_bitmap() && (map_width == 0 || map_width == 8*asset->width)))
	{		
		if(pal_indices==nullptr || asset->get_color_depth()!=8)
		{					
			asset->extract(dest, offset, size);
			return;
		}				
		nds_assert(offset+size<=asset->get_gfx_length());
		unsigned char* buffer = new unsigned char[size];
		
		asset->extract(buffer, offset, size);
		
		for(int i=0;i<size;i++)
		{
			if(buffer[i])
				buffer[i] = pal_indices[buffer[i]];
		}				
		
		dmaCopy(buffer, dest, size);	// <-- beware for bugs idk			
			
		delete[] buffer;
				
	}
	else // if(asset->is_bitmap() && 8*asset->width != map_width
	{				
		nds_assert(false, "Partial stride-dependent bitmap loading is not supported");
		/*nds_assert(8*asset->width < map_width);
				
		unsigned char* bufferx = new unsigned char[size];
		
		asset->extract(bufferx, 0, size);
		
		for(int i=0;i<size;i++)
		{
			if(bufferx[i])
				bufferx[i] = pal_indices[bufferx[i]];
		}				
		
		int stride = asset->width * asset->get_color_depth();
		
		unsigned char* dest8 = (unsigned char*)dest;
		unsigned char* buffer = (unsigned char*)bufferx;						
		
		for(int y=0;y<8*asset->height;y++)
		{			
			dmaCopy(buffer, dest8, stride);
			buffer += stride;
			dest8 += map_width * asset->get_color_depth() / 8;
		}
		
		delete[] bufferx;*/
	}	
}

void DSC::VramLoader::clear(void* dest, int size)
{
	Debug::log("Clear vram : 0x%X (%i)", dest, size);
	nds_assert(size%2 == 0);
	short* dest16 = (short*) dest;
	const int zero = 0;
	for(int i=0;i<size/2;i++)
	{		
		
		__asm("STRH %[_0], [%[dst]]"				
			:
			: [dst] "r" (&dest16[i]), [_0] "r" (zero));				
	}
}