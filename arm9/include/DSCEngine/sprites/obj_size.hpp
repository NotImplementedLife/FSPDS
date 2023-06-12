#pragma once

#include "DSCEngine/types/size.hpp"

namespace DSC
{
	/*! \enum ObjSize
		\brief all possible GBA sprite sizes
	 */
	enum ObjSize 
	{
		SIZE_8x8   =  0, //!< Sprite size 8x8  
		SIZE_16x16 =  1, //!< Sprite size 16x16 
		SIZE_32x32 =  2, //!< Sprite size 32x32 
		SIZE_64x64 =  3, //!< Sprite size 64x64 
		SIZE_16x8  =  4, //!< Sprite size 16x8  
		SIZE_32x8  =  5, //!< Sprite size 32x8  
		SIZE_32x16 =  6, //!< Sprite size 32x16 
		SIZE_64x32 =  7, //!< Sprite size 64x32 
		SIZE_8x16  =  8, //!< Sprite size 8x16  
		SIZE_8x32  =  9, //!< Sprite size 8x32  
		SIZE_16x32 = 10, //!< Sprite size 16x32 
		SIZE_32x64 = 11  //!< Sprite size 32x64 
	};	
	
	/*! \brief Converts ObjSize enum item to actual Size object
		\param objsize size flag, as used by hardware
		\return Size object telling the width and the height mapped to the ObjSize item  
	 */
	inline static Size<int> obj_get_size(ObjSize objsize)
	{
		int val = (int)objsize;
		int shape = val/4;
		int sz = val & 3;
		
		if(shape==0)
		{
			sz = 8<<sz;
			return {sz, sz};
		}
			
		int d1 = sz<2 ? sz+1 : sz;
		int d2 = sz<1 ? sz+1 : sz;
		
		if(shape==1)
			return {8*(1<<d1), 4<<d2};
		else
			return {4<<d2, 8*(1<<d1)};		
	}	
}