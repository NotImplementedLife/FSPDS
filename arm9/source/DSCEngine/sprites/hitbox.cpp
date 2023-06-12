#include "DSCEngine/sprites/hitbox.hpp"

DSC::Hitbox::Hitbox()
{
	left = top = width = height = 0;
}

DSC::Hitbox::Hitbox(char left, char top, char width, char height)
{
	this->left   = left;
	this->top    = top;
	this->width  = width;
	this->height = height;
}

static const char objsize_to_width  [] = {  8, 16, 32, 64, 16, 32, 32, 64,  8,  8, 16, 32 };
static const char objsize_to_height [] = {  8, 16, 32, 64,  8,  8, 16, 32, 16, 32, 32, 64 };

DSC::Hitbox::Hitbox(DSC::ObjSize size)
{
	left = top = 0;
	width  = objsize_to_width[size];
	height = objsize_to_height[size];
}