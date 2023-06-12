#include "obj_attribute.hpp"

#include "DSCEngine/debug/assert.hpp"

using namespace DSC;

DSC::ObjAttribute DSC::ObjAttribute::create256(
	DSC::ObjSize size, 
	short x, 
	short y, 
	unsigned short tile_index, 
	unsigned short hflip, 
	unsigned short vflip, 
	unsigned short priority
	)
{
	ObjAttribute obj;
	obj.attr0 = y	        
			| (1<<13)              // 256 colors	
			| ((size & 0xC )<<12); // shape
	obj.attr1 = x
			| ((!!hflip) << 12) 
			| ((!!vflip) << 13) 
			| (size<<14);           // size
	obj.attr2 = tile_index
			| ((priority&3) << 10);
	return obj;
}

DSC::ObjAttribute DSC::ObjAttribute::create16(
	DSC::ObjSize size, 
	short x, 
	short y, 
	unsigned short tile_index, 
	unsigned short palette_bank, 
	unsigned short hflip, 
	unsigned short vflip, 
	unsigned short priority
	)
{
	ObjAttribute obj;
	obj.attr0 = y	        
			| (0<<13)              // 16 colors	
			| ((size & 0xC )<<12); // shape
	obj.attr1 = x
			| ((!!hflip) << 12) 
			| ((!!vflip) << 13) 
			| (size<<14);          // size
	obj.attr2 = tile_index
			| ((priority&3) << 10)
			| (palette_bank << 12);	
	return obj;
}

void DSC::ObjAttribute::set_size(DSC::ObjSize size)
{
	// set shape
	attr0 = (attr0 & 0x3FFF) | ((size & 0xC )<<12);
	// set size	
	attr1 = (attr1 & 0x3FFF) | (size<<14);
}

DSC::ObjSize DSC::ObjAttribute::get_size() const
{
	//u8 shape = (attr0 & 0xC000)>>12;
	//u8 size  = (attr1 & 0xC000)>>12;
	//return 	(shape<<2) | size;
	return (DSC::ObjSize)(((attr0 & 0xC000) | (attr1>>2))>>12);
}

void DSC::ObjAttribute::flip_v()
{
	attr1 ^= (1<<13);
}

void DSC::ObjAttribute::set_flip_v(unsigned short flip)
{
	attr1 = (attr1 & (~(1<<13))) | ((!!flip)<<13);
}
		
unsigned short DSC::ObjAttribute::get_flip_v() const
{
	return !!(attr1&(1<<13));
}

void DSC::ObjAttribute::flip_h()
{
	attr1 ^= (1<<12);
}

void DSC::ObjAttribute::set_flip_h(unsigned short flip)
{
	attr1 = (attr1 & (~(1<<12))) | ((!!flip)<<12);
}
		
unsigned short DSC::ObjAttribute::get_flip_h() const
{
	return !!(attr1&(1<<12));
}

void DSC::ObjAttribute::set_x(short x)
{	
	attr1 &= ~0x01FF;
	if(x < -64) 
		x=-64;
	else if(x > 240+64) 
		x = 240+64;
	attr1 |= x & 0x01FF;
}
	
void DSC::ObjAttribute::set_y(short y)
{
	if(y < -64) 
		y=-64;
	else if(y > 160+64) 
		y = 160+64;
	*((char*)(&attr0)) = (char)y;
}

short DSC::ObjAttribute::get_x() const
{
	return attr1 & 0xFF;
}
	
short DSC::ObjAttribute::get_y() const
{
	return attr0 & 0x1FF;
}

void DSC::ObjAttribute::set_priority(unsigned short priority)
{
	attr2 &= ~(3<<10);
	attr2 |= (priority&3)<<10;
}

void DSC::ObjAttribute::set_tile_index(unsigned short tile_index)
{
	attr2 &= ~0x03FF;
	attr2 |= tile_index & 0x03FF;
}

unsigned short DSC::ObjAttribute::get_tile_index() const
{
	return attr2 & 0x03FF;
}

void DSC::ObjAttribute::show()
{
	attr0 &= ~(1<<9);	
}
	
void DSC::ObjAttribute::hide()
{
	attr0 |= 1<<9;
}

bool DSC::ObjAttribute::is_hidden() const
{
	return (attr0 & (1<<9));
}

bool DSC::ObjAttribute::is_8bit() const
{	
	return attr0 & (1<<13);
}

bool DSC::ObjAttribute::is_4bit() const
{
	return !(attr0 & (1<<13));
}

int DSC::ObjAttribute::get_palette_number() const
{
	nds_assert(is_4bit(), "Accessing palette number of an 8-bit sprite");	
	return attr2 >> 12;
}
		
void DSC::ObjAttribute::set_palette_number(int slot)
{
	nds_assert(is_4bit(), "Accessing palette number of an 8-bit sprite");		
	attr2 &= 0x0FFF;
	attr2 |= slot<<12;
	return;
}


bool DSC::ObjAttribute::is_rotation_scaling() const
{
	return attr0 & (1<<8);
}

void DSC::ObjAttribute::set_rotation_scaling(bool value)
{	
	attr0 &= ~(1<<8);
	attr0 |= value<<8;
}
		
bool DSC::ObjAttribute::is_double_size() const
{
	nds_assert(is_rotation_scaling(), "Getting double size of non-affine sprite");		
	return attr0 & (1<<9);	
}

void DSC::ObjAttribute::set_double_size(bool value)
{
	nds_assert(is_rotation_scaling(), "Setting double size of non-affine sprite");			
	attr0 &= ~(1<<9);
	attr0 |= value<<9;
	return;
}

void DSC::ObjAttribute::set_affine_matrix(int matrix_id)
{
	nds_assert(is_rotation_scaling(), "Setting affine matrix of non-affine sprite");
	
	matrix_id&=31;
	
	attr1 &= ~(31<<9);
	attr1 |= matrix_id<<9;
	
	return;
}

int DSC::ObjAttribute::get_affine_matrix() const
{
	nds_assert(is_rotation_scaling(), "Setting affine matrix of non-affine sprite");
	return (attr1>>9)&31;
}

void DSC::ObjAttribute::set_color_depth(int bpp)
{
	if(bpp==8)
		attr0 |= (1<<13);
	else if(bpp==4)
		attr0 &= ~(1<<13);	
}