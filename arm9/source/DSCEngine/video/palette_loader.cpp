#include "DSCEngine/video/palette_loader.hpp"
#include "DSCEngine/debug/assert.hpp"

using namespace DSC;

DSC::PaletteLoader::PaletteLoader()
{
	Debug::log("Created PaletteLoader %X (%i)", this, sizeof(PaletteLoader));
}

DSC::PaletteLoader::PaletteLoader(PaletteManager* standard_pal_mng) : PaletteLoader()
{
	this->standard_pal_mng = standard_pal_mng;
}

DSC::PaletteLoader::PaletteLoader(PaletteManager* standard_pal_mng, const Vector<PaletteManager*>& extended_pal_mng)
	: PaletteLoader(standard_pal_mng)
{
	for(int i=0;i<extended_pal_mng.size();i++)
		this->extended_pal_mng.push_back(extended_pal_mng[i]);
}


void DSC::PaletteLoader::set_default_allocation_mode(int allocation_mode)
{
	default_alloc_mode = allocation_mode;
}
	
PaletteAllocationResult DSC::PaletteLoader::try_load(const AssetData* asset, int allocation_mode)
{		
	if(allocation_mode==0) allocation_mode = default_alloc_mode;	
	nds_assert(0<allocation_mode && allocation_mode<=3, "Bad palette allocation mode");
	
	if(allocation_mode & ALLOC_MODE_STANDARD_PALETTE)
	{
		Debug::log("Alloc standard palette");
		return try_alloc_standard(asset);		
	}
	else if(allocation_mode & ALLOC_MODE_EXTENDED_PALETTES)
	{
		Debug::log("Alloc extended palette");
		for(int i=0;i<extended_pal_mng.size();i++)
		{
			PaletteAllocationResult palloc = try_alloc_extended(asset,i);			
			if(palloc.succeeded)									
			{
				Debug::log("Succeeded palette %i", i);
				return palloc;			
			}
		}
	}
	return {nullptr, 0, 0, false};	
}

void DSC::PaletteLoader::unload(const AssetData* asset)
{
	if(!managed_assets.contains_key(asset))
	{
		return;
	}
	if(managed_assets[asset].size()==0)
	{
		Debug::warn("Managed assets empty vector!");
		managed_assets.remove_key(asset);
		return;
	}	
	if(managed_assets[asset].size()>1)
	{
		Debug::warn("Multiple asset allcations detected. Deleting all.");
	}
	
	for(auto entry : managed_assets[asset])
	{
		int pal_index = entry.key;		
		PaletteManager* pal_mng = pal_index == -1 ? standard_pal_mng : extended_pal_mng[pal_index];
		for(;entry.value--;)
			pal_mng->unload(asset);		
	}	
	managed_assets.remove_key(asset);
}

void DSC::PaletteLoader::unload(const AssetData* asset, int palette_index)
{
	if(!managed_assets.contains_key(asset))
	{
		return;
	}
	if(managed_assets[asset].size()==0)
	{
		Debug::warn("Managed assets empty vector!");
		managed_assets.remove_key(asset);
		return;
	}		
	if(!managed_assets[asset].contains_key(palette_index))
		return;
	
	int val = managed_assets[asset][palette_index];
	if(val>0)
	{
		val--;
		PaletteManager* pal_mng = palette_index == -1 ? standard_pal_mng : extended_pal_mng[palette_index];
		pal_mng->unload(asset);
	}
	if(val==0)
		managed_assets[asset].remove_key(palette_index);
	else
		managed_assets[asset][palette_index] = val;
	if(managed_assets[asset].size()==0)
		managed_assets.remove_key(asset);
}
	
	
PaletteAllocationResult DSC::PaletteLoader::try_alloc_standard(const AssetData* asset)
{
	if(standard_pal_mng==nullptr)
	{
		DSC::Debug::log("Standard palette manager is null");
		return {nullptr, 0, 0, false};
	}
	PaletteAllocationResult palloc = standard_pal_mng->try_load(asset);
	if(palloc.succeeded)
	{		
		managed_assets[asset][-1]++;
		DSC::Debug::log("Succeded");
	}
	return palloc;
}
	
PaletteAllocationResult DSC::PaletteLoader::try_alloc_extended(const AssetData* asset, int pal_index)
{
	PaletteManager* ext_pal_mng = extended_pal_mng[pal_index];
	if(ext_pal_mng==nullptr)
	{
		DSC::Debug::warn("Standard palette manager is null");
		return {nullptr, 0, 0, false};
	}
	PaletteAllocationResult palloc = ext_pal_mng->try_load(asset);
	if(palloc.succeeded)
	{		
		managed_assets[asset][pal_index]++;
	}
	return palloc;
}

DSC::PaletteLoader::~PaletteLoader()
{
	Debug::log("Destroyed PaletteLoader %X (%i)", this, sizeof(PaletteLoader));
}

void DSC::PaletteLoader::add_extended_palette_manager(PaletteManager* pal_mng)
{
	extended_pal_mng.push_back(pal_mng);
}
