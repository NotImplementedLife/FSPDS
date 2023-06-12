#include "DSCEngine/drawing/colors.hpp"

inline static constexpr int RGB5(int r, int g, int b)
{
	return r | (g<<5) | (b<<10);
}

const unsigned short DSC::Colors::Black         = RGB5(0x00,0x00,0x00);
const unsigned short DSC::Colors::DarkBlue      = RGB5(0x00,0x00,0x10);
const unsigned short DSC::Colors::DarkGreen     = RGB5(0x00,0x10,0x00);
const unsigned short DSC::Colors::DarkCyan      = RGB5(0x00,0x10,0x10);
const unsigned short DSC::Colors::DarkRed       = RGB5(0x10,0x00,0x00);
const unsigned short DSC::Colors::DarkMagenta   = RGB5(0x10,0x00,0x10);
const unsigned short DSC::Colors::DarkYellow    = RGB5(0x10,0x10,0x00);
const unsigned short DSC::Colors::DarkGray      = RGB5(0x10,0x10,0x10);
const unsigned short DSC::Colors::Blue          = RGB5(0x00,0x00,0x1F);
const unsigned short DSC::Colors::Green         = RGB5(0x00,0x1F,0x00);
const unsigned short DSC::Colors::Cyan          = RGB5(0x00,0x1F,0x1F);
const unsigned short DSC::Colors::Red           = RGB5(0x1F,0x00,0x00);
const unsigned short DSC::Colors::Magenta       = RGB5(0x1F,0x00,0x1F);
const unsigned short DSC::Colors::Yellow        = RGB5(0x1F,0x1F,0x00);
const unsigned short DSC::Colors::Gray          = RGB5(0x18,0x18,0x18);
const unsigned short DSC::Colors::White         = RGB5(0x1F,0x1F,0x1F);

const unsigned short DSC::Colors::SYS_COLORS[] =
{ 
	Black,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkRed,
	DarkMagenta,
	DarkYellow,
	DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	Gray,
	White
};

/*#include "DSCEngine/video/background.hpp"

void DSC::Colors::set_bg_palette_system_colors(int palette_bank)
{
	for(int i=0; i < 16; i++)
	{
		Video::BG_PALETTE_BANK(palette_bank)[i] = SYS_COLORS[i];
	}
}*/











