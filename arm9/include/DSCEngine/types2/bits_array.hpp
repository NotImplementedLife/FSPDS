#pragma once

#include "DSCEngine/debug/assert.hpp"

namespace DSC
{
	/*! \brief Specialized class for handling arrays of bits
		\tparam N the size of the bits array
		\details The bits are packed into full words in a contiguous memory zone, 
		so the size of the structure BitsArray<N> is the size of the minimum number 
		of 32-bit integers required to contain all the N bits. 
		
		Actually, sizeof(BitsArray<N>) == 4*((N+31) / 32).
	 */
	template<const int N>
	class BitsArray
	{
	private:
		static constexpr int BUFFER_SIZE = (N+31)/32;
		int buffer[BUFFER_SIZE];
		
		/*! \brief gets the position of the first unset bit in a number's binary representation */
		static int free_bit_pos(int n, int k=32);		
	public:		
		/*! \brief proxy bit access
		 */
		struct __bit // not safe for Debug::log(...)
		{														
			int& buf_stored;
			int	bit_pos;
								
			__bit(int& buf_stored, int bit_pos) : buf_stored(buf_stored), bit_pos(bit_pos) { value(); }
			
			bool value() const { return buf_stored & (1 << bit_pos); }
			
			operator bool() const { return value(); }	
			operator int() const { return value(); }
			
			void operator = (bool bit) { buf_stored&=~(1 << bit_pos), buf_stored|=(bit << bit_pos); }
			
			bool operator == (bool bit) const { return value() == bit; }
			
			bool operator !() const { return !value();}
		};
	
		/*! \brief creates a new prefilled BitsArray instance
			\param default_value the initial value of all bits
		 */
		BitsArray(bool default_value = false);

		/*! \brief random access iterator in const contexts
			\param index the position of the bit 0..N-1
			\return the value of the bit at the specified index
		 */
		const bool& operator[] (int index) const;
		
		/*! \brief random access iterator for both l-value and r-value contexts
			\param index the position of the bit 0..N-1
			\return a reference to the value of the bit at the specified index
			
			\warning Do not use the reference as a substitute for the bit's value!
				The value of a bit is extracted from DSC::BitsArray::__bit by conversion to
				bool. This conversion is done implicitely wherever the compiler thinks it's
				necessary. However, if unsure whetehr the conversion takes place or not, use
				the explicit (bool)(...) conversion, or use DSC::BitsArray::at().
				
				\code{.cpp}
					BitsArray<N> bits;
					bits[0] = true;                       // ok
					bool b = bits[0];                     // ok
					if(bits[0]) {...}                     // ok
					if(!bits[1]) {...}                    // ok
					DSC::Debug::log("%i", bits[0]);       // wrong, will display the int constructed from the first 4 bytes of _the reference_ pushed to stack
					DSC::Debug::log("%i", (bool)bits[0]); // ok, the actual bit value will be pushed onto stack
				\endcode
		 */
		__bit operator[] (int index);
		
		/*! \brief alternative to random access iterator
			\param index the position of the bit 0..N-1
			\return the value of the bit at the specified index
		 */
		bool at(int index) const;
		
		/*! \brief finds a position of the first unset bit in the array
			\return position of the unset bit (0..N-1), or -1 if no free bit was found		
		 */
		int find_free_bit() const;
		
		/*! \return the number of bits in the array */
		inline int size() const { return N; }
		
		/*! \brief clears all bits of the array (sets them all to 0) */
		void clear();
		
		/*! \brief sets all bits of the array to 1 */
		void set_all();
		
		/*! \brief sets all bits of the array to 0 */
		void unset_all();
		
		/*! \brief Makes the memory at a certain offset available as a bits array and provides direct bit access to it		
			\param offset the start of the bits array (must be word-aligned)
			\tparam N the size of the bits array (must be multiple of 32)
			\return pointer to BitsArray instance over the specified offset			
		
		 */
		static BitsArray<N>* take_over(void* offset);
		
	};	
}

template<const int N>
DSC::BitsArray<N>::BitsArray(bool default_value)
{
	int val = -default_value; // default_value ? 0xFFFFFFFF : 0x00000000;
	for(int i=0;i<BUFFER_SIZE;i++)
	{
		buffer[i] = val;
	}
}

template<const int N>
const bool& DSC::BitsArray<N>::operator[] (int index) const
{
	nds_assert(index>=0);
	nds_assert(index<N);
	
	return __bit(buffer[index>>5], index &31);
}

template<const int N>
typename DSC::BitsArray<N>::__bit DSC::BitsArray<N>::operator[] (int index)
{
	nds_assert(index>=0);
	nds_assert(index<N);
	
	return __bit(buffer[index>>5], index &31);
}

template<const int N>
bool DSC::BitsArray<N>::at(int index) const
{
	return (*this)[index];
}

template<const int N>
int DSC::BitsArray<N>::find_free_bit() const
{
	for(int i=0;i<BUFFER_SIZE;i++)
	{
		int p = free_bit_pos(buffer[i]);
		if(p>=0)
		{
			int result = 32*i+p;
				
			return result<N ? result : -1;						
		}
	}
	return -1;
}

template<const int N>
int DSC::BitsArray<N>::free_bit_pos(int n, int k)
{		
	if((unsigned)n==0xFFFFFFFF) return -1; // full : all bits are set
	if(k==0) return 0;		
	
	k/=2;
	int _0f = (1<<(k-1))-1 + (1<<(k-1)); // build bit-1 masks for the LOW 
	//int _f0 = _0f << (k/2);				 // and HIGH parts
	
	if((n & _0f)==_0f)                   // if the LOW part is full
		return k+free_bit_pos(n>>k, k);  // look in the HIGH part
	else 
		return free_bit_pos(n & _0f, k); // otherwise check the LOW part
}

template<const int N>
void DSC::BitsArray<N>::clear()
{
	unset_all();
}

template<const int N>
void DSC::BitsArray<N>::set_all()
{
	for(int i=0;i<BUFFER_SIZE;i++)
		buffer[i] = 0xFFFFFFFF;
}

template<const int N>
void DSC::BitsArray<N>::unset_all()
{
	for(int i=0;i<BUFFER_SIZE;i++)
		buffer[i] = 0x00000000;
}

template<const int N>
typename DSC::BitsArray<N>* DSC::BitsArray<N>::take_over(void* offset)
{
	nds_assert(((int)offset)%4==0, "Bad alignment for bits array");
	nds_assert(N%32==0);
	return (BitsArray<N>*)offset;
}