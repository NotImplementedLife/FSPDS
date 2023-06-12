/*!
* \file hitbox.hpp
* \brief Hitbox definitions
 */

#pragma once

#include "DSCEngine/sprites/obj_size.hpp"

namespace DSC
{
	/*! \brief Rectangular hitbox for 2d entities
	 */
	class Hitbox
	{
	public:		
		char left;		
		char top;
		char width;
		char height;
		
		/*! \brief creates an empty hitbox*/
		Hitbox();
		
		/*! \brief creates a Hitbox instance 
			\param left    hitbox left coordinate
			\param top     hitbox top coordinate
			\param width   hitbox width
			\param height  hitbox height
		 */
		Hitbox(char left, char top, char width, char height);
		
		/*! \brief creates hitbox component that fits an object size type
			\param size size of an object that may correspond to the hitbox
		 */
		Hitbox(DSC::ObjSize size);
	} __attribute__((packed));
}
