#include "DSCEngine/text/vwf_engine.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/error.hpp"
#include <nds.h>

using namespace DSC;


DSC::VwfEngine::VwfEngine(const FontData& font, ExtendedFontChart ext_chart)
	: font(font), char_width(font.char_width()), char_height(font.char_height()),
	ext_chart(ext_chart)
{
	
}

int DSC::VwfEngine::get_cursor_x() const { return crt_col*8+px_offset; }

void DSC::VwfEngine::set_cursor(int row, int col_px)
{
	crt_row = row;
	crt_col=col_px/8;
	px_offset=col_px%8;
}

void DSC::VwfEngine::set_render_space(void* buffer, int rows_count, int columns_count)
{	
	render_area = (unsigned short*)buffer;
	render_rows_count = rows_count;
	render_columns_count = columns_count;
	crt_row = crt_col = px_offset = 0;
}

int DSC::VwfEngine::put_char(char code, BgPaletteType pal_type, const Brush& text_brush)
{	
	if((code&0xF0)==0x10) // \0x1X -> switch extended chart
	{
		ext_chart = (ExtendedFontChart)(code&0x0F);
		return 0;
	}
	int h = char_height/8;
	if(code=='\n')
	{		
		if((crt_row+1)*h >= render_rows_count)		
		{			
			//FATAL_ERROR("VWF bounding box exceeded");
			return 0;
		}
		crt_row++;
		crt_col = 0;
		px_offset = 0;		
	}
	int gw = font.get_glyph_width(code, ext_chart);
	const unsigned char* gfx =  (const unsigned char*)font.get_glyph(code, ext_chart);	
	nds_assert(gfx != nullptr);
	
	if(8*crt_col + px_offset + gw >= 8*render_columns_count)
	{
		if((crt_row+1)*h >= render_rows_count)		
		{			
			//FATAL_ERROR("VWF bounding box exceeded");
			return 0;
		}
		crt_row++;
		crt_col = 0;
		px_offset = 0;
	}
		
	unsigned short* dest = 
		char_height==8 ?
		(unsigned short*)(((int)render_area) + (render_columns_count * (crt_row/2)*2 + (crt_row&1) + 2*crt_col) * 32 * (1+(int)pal_type)):
		(unsigned short*)(((int)render_area) + (render_columns_count * crt_row + crt_col) *64 * (1+(int)pal_type));
	if(char_height==8 && (render_rows_count&1) && crt_row==render_rows_count-1)
		dest = (unsigned short*)(((int)render_area) + (render_columns_count * crt_row + crt_col) * 32 * (1+(int)pal_type));
	
	if(char_width==8)
	{						
		for(int i=char_height;i--;)
		{			
			int p = px_offset;
			unsigned short* buff = dest;			
			
			for(int l = *(gfx++); l!=0; l>>=1)
			{			
				if(l&1) 
				{			
					if(pal_type == BgPaletteType::Pal4bit)
					{											
						buff[p/4] |= (0x0F & text_brush.get_color(8*crt_col+px_offset, 8*crt_row))<<((p&3)<<2);
					}
					else
					{
						buff[p/2] |= (0xFF & text_brush.get_color(8*crt_col+px_offset, 8*crt_row))<<((p&1)<<3);
					}	
				}				
				p++;
				if(p==8)
				{
					p=0;					
					if(char_height==8 && (render_rows_count&1) && crt_row==render_rows_count-1)
						buff += 32*(1+(int)pal_type) / sizeof(unsigned short);
					else 
						buff += 64*(1+(int)pal_type) / sizeof(unsigned short);
				}						
			}
			dest+=2*(1+(int)pal_type);
		}			
	}
	
	px_offset += gw;
	crt_col += px_offset>>3;
	px_offset&=7;	
	if(crt_col >= render_columns_count)
	{
		if((crt_row+1)*h >= render_rows_count)		
		{						
			//FATAL_ERROR("VWF bounding box exceeded");
			return 0;
		}
		crt_row++;
		crt_col = 0;
		px_offset = 0;		
	}	
	return 1;
}


int DSC::VwfEngine::put_word(const char* word, BgPaletteType pal_type, const Brush& text_brush)
{		
	if(word==nullptr) return 0;	
	if(word[0]=='\0') 
		return 0;	
	if(word[0]==' ' || word[0]=='\n')
	{
		return put_char(word[0], pal_type, text_brush);
	}
	int slen=0, wlen=0;
	while(word[slen]!='\0' && word[slen]!=' ' && word[slen]!='\n')
	{
		slen++;
		wlen+=font.get_glyph_width(word[slen], ext_chart);
	}
	if(wlen>=8*render_columns_count)  // word too large, write as it is
	{
		int result=0;
		for(int i=0;i<slen;i++)
		{
			int ok = put_char(word[i], pal_type, text_brush);
			if(ok==0) 
				break;
			result+=ok;
		}
		return result;		
	}		
	if(8*crt_col+px_offset+wlen+3>=8*render_columns_count)
		if(!put_char('\n', pal_type, text_brush))
			return 0;
		
	for(int i=0;i<slen;i++)
		put_char(word[i], pal_type, text_brush);
	return slen;		
}

int DSC::VwfEngine::put_text(const char* text, BgPaletteType pal_type, const Brush& text_brush)
{
	if(text==nullptr) return 0;	
	int p=0;	
	for(int k=0; (k=put_word(text+p, pal_type,text_brush)); p+=k);
	return p;
}


void DSC::VwfEngine::clear(BgPaletteType pal_type)
{
	int size = render_columns_count*render_rows_count*32 / sizeof(unsigned short);
	if(pal_type==Pal8bit) size*=2;
	for(int i=0;i<size;i++)
		render_area[i]=0;	
	crt_row = crt_col = px_offset = 0;
}

void DSC::VwfEngine::clear_row(int row, BgPaletteType pal_type) const
{
	unsigned short* dest;
	volatile unsigned short zero = 0x0000;
	
	if(char_height==8)
	{
		if((render_rows_count&1) && row==render_rows_count-1)
		{
			dest = (unsigned short*)(((int)render_area) + (render_columns_count * row) * 32 * (1+(int)pal_type));
			for(int i=0;i<render_columns_count*16*(1+pal_type);i++)
				*(dest++)=zero;
		}
		else
		{
			dest = (unsigned short*)(((int)render_area) + (render_columns_count * (row/2)*2 + (row&1)) * 32 * (1+(int)pal_type));
			for(int i=0;i<render_columns_count;i++)
			{
				for(int i=0;i<16*(1+pal_type);i++) *(dest++)=zero;
				dest+=16;
			}
		}		
	}
	else
	{
		dest = (unsigned short*)(((int)render_area) + (render_columns_count * row) *64 * (1+(int)pal_type));		
		DSC::Debug::log("Clear dest = %X, Len = %X", dest, render_columns_count*32*(1+pal_type));
		for(int i=0;i<render_columns_count*32*(1+pal_type);i++)
				*(dest++)=zero;
	}
}
	

void DSC::VwfEngine::prepare_map(const VwfEngine& vwf, int map_id, int map_stride, int map_x, int map_y, int palette_bank)
{
	palette_bank<<=12;		
	int tile_offset = ((int)vwf.render_area - (int)bgGetGfxPtr(map_id))/32;
	
	Debug::log("tile offset = %X", tile_offset);	
	
	unsigned short* map_ptr = bgGetMapPtr(map_id);
	
	Debug::log("map ptr = %X", map_ptr);

	for(int x = 0; x < vwf.render_columns_count; x++)
	{
		for(int y = 0; y < vwf.render_rows_count-1; y+=2)
		{
			map_ptr[map_stride*(map_y+y)+map_x+x] = palette_bank | (vwf.render_columns_count*y+2*x+tile_offset);
			map_ptr[map_stride*(map_y+y+1)+map_x+x] = palette_bank | (vwf.render_columns_count*y+2*x+tile_offset+1);
		}
		int y=vwf.render_rows_count-1;
		if(!(y & 1))
		{
			map_ptr[map_stride*(map_y+y)+map_x+x] = palette_bank | (vwf.render_columns_count*y + x +tile_offset);
		}
	}
}