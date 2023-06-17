#pragma once

#include "DSCEngine/drawing/brush.hpp"
#include "DSCEngine/resources/font_data.hpp"

namespace DSC
{
	enum BgPaletteType
	{
		Pal4bit = 0,
		Pal8bit = 1		
	};	
	
	class VwfEngine
	{
	private:
		const FontData& font;		
		short char_width = 0;
		short char_height = 0;
		ExtendedFontChart ext_chart = ExtendedFontChart::None;
		
		
		unsigned short* render_area = nullptr;
		short render_rows_count;
		short render_columns_count;
		int crt_row = 0, crt_col = 0;
		int px_offset = 0;				
		
	public:
		VwfEngine(const FontData& font, ExtendedFontChart ext_chart = ExtendedFontChart::None);
		void set_render_space(void* buffer, int rows_count, int columns_count);
		
		int put_char(char code, BgPaletteType pal_type, const Brush& text_brush);
		int put_word(const char* word, BgPaletteType pal_type, const Brush& text_brush);
		int put_text(const char* text, BgPaletteType pal_type, const Brush& text_brush);
		
		void clear(BgPaletteType pal_type);
		
		void set_cursor(int row, int col_px);
		int get_cursor_x() const;
		
		void clear_row(int row, BgPaletteType pal_type) const;
		
		inline const FontData& get_font() const { return font; }
		
		static void prepare_map(const VwfEngine& vwf, int map_id, int map_stride, int map_x, int map_y, int palette_bank = 0);
	};	
}

