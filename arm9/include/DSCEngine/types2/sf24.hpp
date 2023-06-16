/*!
* \file sf24.hpp
* \brief Signed Fixed 16:8 floating number definition
 */

#pragma once

namespace DSC
{
	/*! \brief 24-bit (16.8) signed fixed floating point number	
	 */
	class sf24
	{
	private:
		unsigned int data;
		static sf24 create_from_data(unsigned int data);
	public:
		sf24();
		/*!
		* \brief Implicit s16 to sf24 conversion
		* 
		* \param [in] x short number to be converted to sf24		
		 */
		sf24(short x);
		
		/*!
		* \brief Builds a signed fixed number from its separate integral and fractional part
		* 
		* \param [in] x number integral part
		* \param [in] frac number fractional part		
		 */
		sf24(short x, unsigned char frac);
		
		/*!
		* \brief Gets integral part of number
		* 
		* \return number integral part 		
		 */
		short get_int() const;
		
		/*!
		* \brief Gets fractional part of number
		* 
		* \return number fractional part scaled to 256 and rounded to a full byte (128 = 0.5)		
		 */
		unsigned char get_frac() const;
		
		
		sf24 operator + (const sf24& b) const;
		sf24 operator - (const sf24& b) const;
		sf24 operator * (const sf24& b) const;	
		
		sf24 operator -() const;
		sf24& operator += (const sf24& b);
		sf24& operator -= (const sf24& b);
		//void operator *= (const sf24& b);	
		
		/*!
		* \brief Explicit narrowing conversion to integral type (only the integral type is kept)
		* 
		* \details Usage
		* \code{.cpp}
		* sf24 x {12, 128};  // x = 12.5
		* int y = (int)x;    // y = 12
		* \endcode
		 */
		explicit operator int() const;
		
		/*!
		* \brief Explicit narrowing conversion to short integral type (only the integral type is kept)
		* 
		* \details Usage
		* \code{.cpp}
		* sf24 x {12, 128};    // x = 12.5
		* short y = (short)x;  // y = 12
		* \endcode
		 */
		explicit operator short() const;
		
		bool operator < (const sf24& b) const;
		bool operator > (const sf24& b) const;
		bool operator <= (const sf24& b) const;
		bool operator >= (const sf24& b) const;
		bool operator == (const sf24& b) const;
		bool operator != (const sf24& b) const;
		
		/*!
		* \brief Checks if the number belongs to an interval
		* 
		* \param [in] min minimum target value
		* \param [in] max maximum target value
		* \return true if the number is between the specified values, false otherwise		
		 */
		bool in_range(const sf24& min, const sf24& max) const;
		
		char* to_string() const;
		
		sf24 abs() const;
	};
}