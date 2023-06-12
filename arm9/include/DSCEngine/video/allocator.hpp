/*!
 * \file allocator.hpp
 */

#pragma once

namespace DSC
{
	/*! \brief flexible VRAM manager */
	class Allocator
	{
	private:
		unsigned char id;
		int base_offset;
		int base_length;		
		int free_space_head;
	public:
		Allocator();
	
		/*! \brief Creates a new allocator instance
		 *	\param [in] offset base segment offset
		 *	\param [in] length base segment length
		 *  \details This object allocates addresses in range [offset, offset+length-1]
		 *  \warning Overlapping allocator segments are a bad idea! Make sure allocators' range don't conflict with each other.
		 */
		Allocator(int offset, int length);		
		
		/*! \brief The equivalent of <code>malloc(size)</code>.
		 *	\param [in] size Data length (works best in units of 32 bytes a.k.a. the size of a 4bpp tile)
		 *	\param [in] desired_offset If positive, the block of specified size tries to be placed at [base segment offset + desired_offset]
		 *  \returns the allocated block offset, or <code>nullptr</code> if allocation failed.
		 */
		void* reserve(int size, int desired_offset = -1);
		
		/*! \brief frees the specified offset
		 *	\param [in] address offset to free
		 *	\details No penalties if the address isn't in the allocator's range, it points 
		 *	to unallocated memory or has been previosuly released.
		 */
		void release(void* address);		
		
		~Allocator();
		
		static Allocator defaultMainSpriteVram();
		static Allocator defaultSubSpriteVram();
	};
}