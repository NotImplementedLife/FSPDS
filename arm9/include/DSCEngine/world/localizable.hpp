/*!
* \file localizable.hpp
* \brief Localizable object definition
 */

#pragma once

#include "DSCEngine/types/sf24.hpp"

namespace DSC
{
	/*! \brief Abstract declaration of object dependent on its 2D position	
	 */
	class Localizable
	{
	public:
		/*!
		* \brief Get object x coordinate
		* \return object x coordinate
		 */
		virtual sf24 x() const = 0;
		
		/*!
		* \brief Get object y coordinate
		* \return object y coordinate
		 */
		virtual sf24 y() const = 0;
	};
}