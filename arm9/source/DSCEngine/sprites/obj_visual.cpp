#include "DSCEngine/sprites/obj_visual.hpp"

using namespace DSC;

DSC::ObjVisual::ObjVisual(DSC::ObjSize obj_size)
{
	size = obj_get_size(obj_size);
}

int DSC::ObjVisual::add_frame(ObjFrame* frame)
{
	frames.push_back(frame);
	return frames.size()-1;
}


void* DSC::ObjVisual::get_crt_gfx() const
{
	return frames[crt_gfx_id]->gfx_ptr;
}

int DSC::ObjVisual::set_crt_gfx(int gfx_index)
{
	nds_assert(0<=gfx_index && gfx_index<frames.size());
	gfx_changed = (crt_gfx_id != gfx_index);
	crt_gfx_id = gfx_index;
	nds_assert(frames[crt_gfx_id]->asset!=nullptr);
	return get_crt_color_depth();
}

Size<char> DSC::ObjVisual::get_size() const { return size; }

int DSC::ObjVisual::get_crt_color_depth() const
{
	return frames[crt_gfx_id]->asset->get_color_depth();
}

#include <cstdarg>

int DSC::ObjVisual::add_frameset(int frame_id, ...)
{
	Vector<unsigned char> frameset;
	frameset.push_back(frame_id);
	
	Debug::log("Frame id = %i",frame_id);
	va_list args;
    va_start(args, frame_id);
	while((frame_id = va_arg(args, int)) != FRAMESET_END)
	{
		frameset.push_back(frame_id);
		Debug::log("Frame id = %i",frame_id);
		nds_assert(frameset.size()<=16,"Did you forget FRAMESET_END?");
	}        
    va_end(args);
	framesets.push_back(frameset);
	
	Debug::log("Frameset size = %i", frameset.size());
	Debug::log("Last size = %i", framesets.back().size());
	
	return framesets.size()-1;
}

void DSC::ObjVisual::set_animation(int frameset_id, bool reset_current_frame)
{
	anim_enabled = true;	
	anim_frameset = frameset_id;
	if(reset_current_frame)
		anim_crt_frame = 0;
}
	
void DSC::ObjVisual::set_animation_ticks(int ticks)
{
	anim_ticks = ticks;
	crt_anim_ticks = 0;
}

void DSC::ObjVisual::update()
{
	if(anim_enabled)
	{
		crt_anim_ticks++;
		if(crt_anim_ticks>=anim_ticks)
		{		
			crt_anim_ticks = 0;
			if(anim_crt_frame>=framesets[anim_frameset].size())		
				anim_crt_frame = 0;		
			
			Debug::log("Frame = %i",framesets[anim_frameset][anim_crt_frame]);		
			set_crt_gfx(framesets[anim_frameset][anim_crt_frame++]);		
		}
	}
	
	if(!gfx_changed) return;	
	
	gfx_changed = false;
	
}

DSC::ObjVisual::~ObjVisual()
{
	if(auto_delete_frames)
	{
		for(int i=0;i<frames.size();i++)
		{
			delete frames[i];
		}
	}
}