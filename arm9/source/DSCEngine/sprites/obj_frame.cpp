#include "DSCEngine/sprites/obj_frame.hpp"

using namespace DSC;

DSC::ObjFrame::ObjFrame(const AssetData* asset, int metatile_row, int metatile_col)
	: asset(asset), metatile_row(metatile_row), metatile_col(metatile_col)
{
	
}
		
DSC::ObjFrame::~ObjFrame()
{
	unload.trigger(this, nullptr);
}