#include "DSCEngine/resources/font_data.hpp"
#include "DSCEngine/debug/assert.hpp"

using namespace DSC;

namespace
{
	const int ExtFontCharSelLookupTable[8][3] =
	{
		{0,0,0},
		{1,0,0},
		{0,1,0},	
		{1,2,0},
		{0,0,1},	
		{1,0,2},
		{0,1,2},
		{1,2,3}
	};
	const int FontCharWidth[5] = { 0, 8, 8, 0, 16 };
	const int FontCharHeight[5] = {0, 8, 16, 0, 16 };

}

DSC::FontData::FontData(): ReadOnlyData(sizeof(DSC::FontData))  // this line is important
{ }

int DSC::FontData::char_width() const
{
	return FontCharWidth[char_size];
}

int DSC::FontData::char_height() const
{
	return FontCharHeight[char_size];
}
	
int DSC::FontData::get_header_size() const
{
	return sizeof(*this);
}

int DSC::FontData::get_chart_size() const
{
	return 128 * char_size * 8;
}

const void* DSC::FontData::get_ascii_base_addr() const
{
	return (const void*)(((int)this)+get_header_size());
}

const void* DSC::FontData::get_ascii_extended_addr(ExtendedFontChart ext_chart) const
{
	int id = ExtFontCharSelLookupTable[charts][(int)ext_chart/2]; // 1,2,4-->0,1,2
	if(id==0) return nullptr;
	return (const void*)(((int)this)+get_header_size() + (get_chart_size() + 128) * (id));
}

const void* DSC::FontData::get_glyph(int code, ExtendedFontChart ext_chart) const
{
	nds_assert(0<=code && code<256);
	if(code<128)
	{
		return (const void*)((int)get_ascii_base_addr() + 8*char_size*code);
	}
	code-=128;
	int address = (int)get_ascii_extended_addr(ext_chart);
	if(address==0) return nullptr;
	return (const void*)(address + 8*char_size*code);
}

int DSC::FontData::get_glyph_width(int code, ExtendedFontChart ext_chart) const
{
	nds_assert(0<=code && code<256);
	if(code<128)
	{
		return *((unsigned char*)((int)get_ascii_base_addr() + get_chart_size() + code));
	}
	code-=128;
	int address = (int)get_ascii_extended_addr(ext_chart);
	if(address==0) return 0;
	return *((unsigned char*)address + get_chart_size() + code);
}

FontSize DSC::FontData::font_size() const
{
	return (FontSize)char_size;
}