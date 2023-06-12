#include "DSCEngine/system/hardware.hpp"

#include <nds.h>
#include "DSCEngine/debug/log.hpp"

void DSC::Hardware::MainEngine::objEnable(int mapping_size, bool use_ext_palette)
{
	int ext_palette_flag = use_ext_palette ? DISPLAY_SPR_EXT_PALETTE : 0;
	*MainEngine::DISPCNT &= ~DISPLAY_SPRITE_ATTR_MASK;
	
	int mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_32;
	switch(mapping_size)
	{
		case  64: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64; break;
		case 128: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128; break;
		case 256: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256; break;
	}
	
	*MainEngine::DISPCNT |=  DISPLAY_SPR_ACTIVE | mapping | ext_palette_flag;
}

void DSC::Hardware::MainEngine::objDisable()
{
	*MainEngine::DISPCNT &= ~DISPLAY_SPRITE_ATTR_MASK;
}

bool DSC::Hardware::MainEngine::objIsEnabled()
{
	return *MainEngine::DISPCNT & DISPLAY_SPR_ACTIVE;
}

bool DSC::Hardware::MainEngine::objIsExtPaletteEnabled()
{
	return *MainEngine::DISPCNT & DISPLAY_SPR_EXT_PALETTE;
}

void DSC::Hardware::SubEngine::objEnable(int mapping_size, bool use_ext_palette)
{
	int ext_palette_flag = use_ext_palette ? DISPLAY_SPR_EXT_PALETTE : 0;	
	*SubEngine::DISPCNT &= ~DISPLAY_SPRITE_ATTR_MASK;
	
	int mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_32;
	switch(mapping_size)
	{
		case  64: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64; break;
		case 128: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128; break;
		case 256: mapping = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256; break;
	}
	
	*SubEngine::DISPCNT |=  DISPLAY_SPR_ACTIVE | mapping | ext_palette_flag;
}

void DSC::Hardware::SubEngine::objDisable()
{
	*SubEngine::DISPCNT &= ~DISPLAY_SPRITE_ATTR_MASK;
}

bool DSC::Hardware::SubEngine::objIsEnabled()
{
	return *SubEngine::DISPCNT & DISPLAY_SPR_ACTIVE;
}

bool DSC::Hardware::SubEngine::objIsExtPaletteEnabled()
{
	return *SubEngine::DISPCNT & DISPLAY_SPR_EXT_PALETTE;
}

int DSC::Hardware::MainEngine::objGetMappingSize()
{
	int dispcnt = *MainEngine::DISPCNT;
	
	switch((dispcnt & (3 << 20))>>20)
	{
		case 0: return 32;
		case 1: return 64;
		case 2: return 128;
		case 3: return 256;
	}
	return 32;
}

int DSC::Hardware::SubEngine::objGetMappingSize()
{
	int dispcnt = *SubEngine::DISPCNT;
	
	switch((dispcnt & (3 << 20))>>20)
	{
		case 0: return 32;
		case 1: return 64;
		case 2: return 128;
		case 3: return 256;
	}
	return 32;
}

