#include "DSCEngine/system/hardware.hpp"

#include "DSCEngine/types/templates.hpp"

#include "DSCEngine/debug/log.hpp"

using namespace DSC;
using namespace DSC::Hardware;


ExtendedPalette* DSC::Hardware::MainEngine::BgExtendedPalette()
{
	if(VramBank('E').is_enabled())	
		return (ExtendedPalette*)VramBank('E').lcd_offset();			
	
	int _f = 0, _g = 0xF000000;
	
	if(VramBank('F').is_enabled())	
		_f = (int)VramBank('F').lcd_offset();			
	
	if(VramBank('G').is_enabled())	
		_g = (int)VramBank('G').lcd_offset();			
	
	if(_f==0 && _g==0)
	{
		Debug::warn("Bank E/F/G not enabled");
		return nullptr;		
	}
	
	if(_f==0) 
		return (ExtendedPalette*)_g;
	if(_g==0)
		return (ExtendedPalette*)_f;
	return (ExtendedPalette*)min(_f, _g);
}

ExtendedPalette* DSC::Hardware::MainEngine::ObjExtendedPalette()
{
	if(VramBank('F').is_enabled())	
		return (ExtendedPalette*)VramBank('F').lcd_offset();		
	if(VramBank('G').is_enabled())	
		return (ExtendedPalette*)VramBank('G').lcd_offset();		
	Debug::warn("Bank F/G not enabled");
	return nullptr;
}


ExtendedPalette* DSC::Hardware::SubEngine::BgExtendedPalette()
{
	if(VramBank('H').is_enabled())	
		return (ExtendedPalette*)VramBank('H').lcd_offset();	
	Debug::warn("Bank H not enabled");
	return nullptr;
}

ExtendedPalette* DSC::Hardware::SubEngine::ObjExtendedPalette()
{
	if(VramBank('I').is_enabled())	
		return (ExtendedPalette*)VramBank('I').lcd_offset();
	Debug::warn("Bank I not enabled");
	return nullptr;
}

ExtendedPalette* DSC::Hardware::MainEngine::BgExtendedPalette(int index)
{
	return (ExtendedPalette*)((short*)MainEngine::BgExtendedPalette() + 256*index);
}

ExtendedPalette* DSC::Hardware::MainEngine::ObjExtendedPalette(int index)
{
	return (ExtendedPalette*)((short*)MainEngine::ObjExtendedPalette() + 256*index);
}

ExtendedPalette* DSC::Hardware::SubEngine::BgExtendedPalette(int index)
{
	return (ExtendedPalette*)((short*)SubEngine::BgExtendedPalette() + 256*index);
}

ExtendedPalette* DSC::Hardware::SubEngine::ObjExtendedPalette(int index)
{
	return (ExtendedPalette*)((short*)SubEngine::ObjExtendedPalette() + 256*index);
}

#define EXT_PAL_SLOT_SIZE (16*512)
#define EXT_PAL_SLOT_ALIGN_MASK (EXT_PAL_SLOT_SIZE-1)

// beware that no checks on offset are done to ensure it is a valid palette address

int DSC::Hardware::tellExtendedPaletteIndex(void* pal_offset)
{	
	return ((int)pal_offset & EXT_PAL_SLOT_ALIGN_MASK)/512;
}

ExtendedPalette* DSC::Hardware::tellExtendedPaletteBase(void* pal_offset)
{
	return (ExtendedPalette*)((int)pal_offset & ~EXT_PAL_SLOT_ALIGN_MASK);
}
