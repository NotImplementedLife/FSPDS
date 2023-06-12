#pragma once

namespace DSC
{
	/*! \brief Generic size of two coordinates
	* 	\tparam T type of the width/height
	 */
	template<typename T>
	struct Size
	{		
		T width; /*!< Size width */
		T height; /*!< Size height */		
		
		template<typename T2>
		inline operator Size<T2>() const { return {(T2)width, (T2)height}; }
	};
		
	
	template<typename T>
	inline bool operator == (const Size<T>& s1, const Size<T>& s2) { return s1.x==s2.x && s1.y==s2.y; }
	
	template<typename T>
	inline bool operator != (const Size<T>& s1, const Size<T>& s2) { return s1.x!=s2.x || s1.y!=s2.y; }
		
}