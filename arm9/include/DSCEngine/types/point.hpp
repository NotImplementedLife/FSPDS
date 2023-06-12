/*!
* \file point.hpp
* \brief Generic point definition
 */

#pragma once

namespace DSC
{
	/*! \brief Generic point of two coordinates
	* 	\tparam T type of the X/Y dimensions
	 */
	template<typename T>
	struct Point
	{		
		T x; /*!< Point X-coordinate */
		T y; /*!< Point Y-coordinate */		
	};
	
	template<typename T>
	inline bool operator == (const Point<T>& p1, const Point<T>& p2) { return p1.x==p2.x && p1.y==p2.y; }
	
	template<typename T>
	inline bool operator != (const Point<T>& p1, const Point<T>& p2) { return p1.x!=p2.x || p1.y!=p2.y; }
}