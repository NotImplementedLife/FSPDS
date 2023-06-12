/*!
* \file obj_visual.hpp
* \brief Object Visual definition
 */

#pragma once

#include "DSCEngine/types/vector.hpp"
#include "DSCEngine/types/size.hpp"
#include "DSCEngine/sprites/obj_frame.hpp"
#include "DSCEngine/sprites/obj_size.hpp"

namespace DSC
{
	class ObjFrame;
	/*! \brief OAM Object graphics handler class	
	 */
	class ObjVisual
	{		
		private:
			Size<char> size;
			
			Vector<ObjFrame*> frames;
			unsigned char crt_gfx_id = 0;
			
			bool gfx_changed = true;
			
			Vector<Vector<unsigned char>> framesets;
			
			int visual_stat = 0;
			
			bool anim_enabled = false;
			int anim_frameset = 0;
			int anim_crt_frame = 0;
			
			int anim_ticks = 1;
			int crt_anim_ticks = 0;
			
			bool auto_delete_frames = true;
		public:
			ObjVisual(DSC::ObjSize obj_size);
			
			Size<char> get_size() const;
			
			/*! \brief adds a new frame to the visual
				\param frame object frame to be added
			 */
			int add_frame(ObjFrame* frame);
			
			
			/*! \brief gets the current graphics from VRAM
				\return pointer to loaded frame graphics
			 */
			void* get_crt_gfx() const;
			
			/*! \brief sets the id of the current (displayed) frame
				\param gfx_index index of the intended frame, as present in the frames vector
				\return color depth of the asset containing the frame
			 */
			int set_crt_gfx(int gfx_index);
			
			int get_crt_color_depth() const;
			
			void update();
						
			int add_frameset(int frame_id, ...);
			
			void set_animation(int frameset_id, bool reset_current_frame = false);
			
			void set_animation_ticks(int ticks);
			
			inline static constexpr int FRAMESET_END = -1;
			
			~ObjVisual();
	};	
}