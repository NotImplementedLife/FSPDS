#pragma once
#include "DSCEngine/resources/readonly_data.hpp"

namespace DSC
{
	enum class ExtendedFontChart
	{		
		None  = 0, 
		ExtL1 = 1<<0,
		ExtL2 = 1<<1,
		ExtCy = 1<<2
	};
		
	enum class FontSize
	{
		FONT_8x8   = 1,			
		FONT_8x16  = 2,
		FONT_16x16 = 4
	};
	
	class FontData final : public ReadOnlyData
	{
	private:		
		__attribute__ ((packed))
		short char_size;
		__attribute__ ((packed))
		short charts;		
	public:		
		FontData();
		~FontData() = delete;
		
		int char_width() const;
		int char_height() const;
		int get_header_size() const;
		int get_chart_size() const;
		const void* get_ascii_base_addr() const;
		const void* get_ascii_extended_addr(ExtendedFontChart ext_chart) const;
		
		int get_glyph_width(int code, ExtendedFontChart ext_chart = ExtendedFontChart::None) const;
						
		const void* get_glyph(int code, ExtendedFontChart ext_chart = ExtendedFontChart::None) const;	

		FontSize font_size() const;
	};	
}

namespace DSC::Resources::Fonts
{	
    extern const DSC::FontData default_8x16;
}
