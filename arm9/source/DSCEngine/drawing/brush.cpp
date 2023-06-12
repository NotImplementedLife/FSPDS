#include "DSCEngine/drawing/brush.hpp"

DSC::Brush::~Brush() {}

DSC::SolidColorBrush::SolidColorBrush(int color) : color(color) { }

int DSC::SolidColorBrush::get_color(int x, int y) const 
{
	return color;
}

DSC::SolidColorBrush::~SolidColorBrush() {}
