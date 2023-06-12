/*! \file obj_frame.hpp
	Contains definition of inidivudal sprite frame
 */

#pragma once

#include "DSCEngine/resources/asset_data.hpp"
#include "DSCEngine/events/event.hpp"


namespace DSC
{
	class ObjFrame
	{
		friend class ObjAllocator;
		friend class ObjVisual;
		friend class Sprite;
	private:		
		const AssetData* asset = nullptr;
		short metatile_row = 0;
		short metatile_col = 0;
		void* gfx_ptr = nullptr;
		Event unload;
	public:	
		/*! \brief creates new object frame
			\param asset source tiled asset aka spritesheet (must not be tile reduced)
			\param metatile_row row of the frame graphics
			\param metatile_col column of the frame graphics
			
			\details A frame's graphics size is dictated by the asset's metatile size. For a 256x256px
			asset image with 32x32px metatiles, the frame situated at row 1, column 2 will have be have
			the top-left coordinates at x=64, y=32 vizualized in the original asset.			
		 */
		ObjFrame(const AssetData* asset, int metatile_row, int metatile_col);		
		
		/*! \brief destroys an object frame instance */
		~ObjFrame();
	};
}