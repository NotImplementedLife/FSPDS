/*!
* \file colors.hpp
* \brief Colors definitions to improve readability
 */

#pragma once

#include "DSCEngine/types/templates.hpp"

namespace DSC::Colors
{
	extern const unsigned short Black; /*!< Black color*/
	extern const unsigned short DarkBlue; /*!< Dark Blue color*/
	extern const unsigned short DarkGreen; /*!< Dark Green color*/
	extern const unsigned short DarkCyan; /*!< Dark Cyan color*/
	extern const unsigned short DarkRed; /*!< Dark Red color*/
	extern const unsigned short DarkMagenta; /*!< Dark Magenta color*/
	extern const unsigned short DarkYellow; /*!< Dark Yellow color*/
	extern const unsigned short DarkGray; /*!< Dark Gray color*/
	extern const unsigned short Blue; /*!< Blue color*/
	extern const unsigned short Green; /*!< Green color*/
	extern const unsigned short Cyan; /*!< Cyan color*/
	extern const unsigned short Red; /*!< Red color*/
	extern const unsigned short Magenta; /*!< Magenta color*/
	extern const unsigned short Yellow; /*!< Yellow color*/
	extern const unsigned short Gray; /*!< Gray color*/
	extern const unsigned short White; /*!< White color*/

	extern const unsigned short SYS_COLORS[]; /*!< The 16 system console colors */
	
	//void set_bg_palette_system_colors(int palette_bank);
	
	inline static constexpr int fromRGB15(unsigned char r, unsigned char g, unsigned char b)
	{
		r&=31, g&=31, b&=31;
		return r|(g<<5)|(b<<10);
	}
	
	inline static constexpr int fromRGB24(unsigned char r, unsigned char g, unsigned char b)
	{
		return fromRGB15(r/8, g/8, b/8);
	}
	
	inline static constexpr int fromRGB24(int color_code)
	{
		return fromRGB24(color_code&0xFF, (color_code>>8)&0xFF, (color_code>>16)&0xFF);
	}
}