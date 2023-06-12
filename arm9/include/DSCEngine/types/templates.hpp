/*!
* \file templates.hpp
* \brief type template definitions
 */

#pragma once

namespace DSC
{
	// type switch (https://en.cppreference.com/w/cpp/types/conditional)
		
	/*! \cond HIDDEN_SYMBOLS */ 
	template<bool B, typename T, typename F> 
	struct __if__ { using type = T; };
	template<typename T, typename F> 
	struct __if__<false, T, F> { using type = F; };
	/*! \endcond */
	
	/*! \brief Decides type based on a boolean expression
	* \tparam B boolean expression
	* \tparam T type if B is true
	* \tparam F type if B is false
	* \details if B is true, then \_if\_<B,T,F> evaluates to T, otherwise it evaluates to F
	* \details Equivalent of std::conditional
	* \code{.cpp} 
	* _if_< (2>3), int, short > x; // <=> short x; 
	* \endcode
	 */		
	template<bool B, typename T, typename F> 
	using _if_ = typename __if__<B,T,F>::type;
	
	//https://en.cppreference.com/w/cpp/types/remove_reference	 
	 			
	/*! \cond HIDDEN_SYMBOLS */ 
	template<typename T> struct __no_ref__      { typedef T type; };	
	template<typename T> struct __no_ref__<T&>  { typedef T type; };
	template<typename T> struct __no_ref__<T&&> { typedef T type; };	
	/*! \endcond */ 
	
	/*!
	* \brief gets rid of type reference
	* 
	* \details Equivalent of std::remove_reference
	* \code{.cpp} 
	* _no_ref_<int> x;   // <=> int x; 
	* _no_ref_<int&> x;  // <=> int x; 
	* _no_ref_<int&&> x; // <=> int x; 
	* \endcode
	 */
	 template<typename T> 
	 using _no_ref_ = typename __no_ref__<T>::type;
	
	
	/*! \brief Chooses the smallest unsigned integer type that fits a constant
	* 	\tparam C an unsigned constant
	* 
	* 	\details 
	* 	- if 0<=C<256, then uint_best_fit<C> is 8-bit
	* 	- if 256<=C<65536, then uint_best_fit<C> is 16-bit
	* 	- if 65536<=C, then uint_best_fit<C> is 32-bit
	 */
	template<unsigned int C> 
	using uint_best_fit = 
		_if_< C<256, unsigned char,  
			_if_<C<65536, unsigned short, unsigned int> 
		>;
	
	/*!
	* \brief Creates rvalue reference from object (useful in move semantics)
	* 	
	* 
	* \param [in] arg the target object
	* \return rvalue reference (&&) to object
	* 
	* \details Equivalent of std::move
	 */
	template<class T>
	_no_ref_<T>&& _move_(T&& arg)
	{   
		return ((T&&) arg);
	}
	
	template<class T>
	_no_ref_<T>&& _move_(T& arg)
	{   
		return ((T&&) arg);
	}
	
	
	template<typename T>
	constexpr const T& min(const T& first, const T& second)
	{
		return first<second ? first : second;
	}
	
	template<typename T, typename... Arguments>
	constexpr const T& min(const T& first, const Arguments& ...args)	
	{		
		return min(first, min(args...));		
	}		
	
	
	template<typename T>
	constexpr const T& max(const T& first, const T& second)
	{
		return first>second ? first : second;
	}
	
	template<typename T, typename... Arguments>
	constexpr const T& max(const T& first, const Arguments& ...args)	
	{		
		return max(first, max(args...));		
	}				
	
	
	template<typename T>
	constexpr void swap(T& first, T& second)
	{
		T tmp = _move_(first);
		first = _move_(second);
		second = _move_(tmp);
	}
}

