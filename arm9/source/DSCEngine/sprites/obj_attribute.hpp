#pragma once

#include "DSCEngine/sprites/obj_size.hpp"
#include "DSCEngine/sprites/obj_bit_depth.hpp"

namespace DSC
{		
	/*! \class ObjAttribute
		\brief describes OAM object attributes, works only with 256-color sprites	
	 */
	class ObjAttribute
	{
	private:
		short attr0, attr1, attr2, attr3;
		ObjAttribute() = default;
	public:
		/*! \brief creates an object attribute using 8-bit palette
			\param size       object size
			\param x          object position - horizontal coordinate
			\param y          object position - vertical coordinate
			\param tile_index tile number in OAM Tiles section of VRAM
			\param hflip      horizontal flip (0 = no ; non-0 = yes)
			\param hflip      vertical flip (0 = no ; non-0 = yes)
			\param priority   object priority (0..3)
		 */ 
		static ObjAttribute create256(ObjSize size, short x=0, short y=0, unsigned short tile_index=0, unsigned short hflip=0, unsigned short vflip=0, unsigned short priority=0);
		
		/*! \brief creates an object attribute using 4-bit palette
			\param size         object size
			\param x            object position - horizontal coordinate
			\param y            object position - vertical coordinate
			\param tile_index   tile number in OAM Tiles section of VRAM
			\param palette_bank palette bank (0..15)
			\param hflip        horizontal flip (0 = no ; non-0 = yes)
			\param hflip        vertical flip (0 = no ; non-0 = yes)
			\param priority     object priority (0..3)
		 */ 
		static ObjAttribute create16(ObjSize size, short x=0, short y=0, unsigned short tile_index=0, unsigned short palette_bank=0, unsigned short hflip=0, unsigned short vflip=0, unsigned short priority=0);
		
		/*! \brief set object size
			\param size object size
		 */
		void set_size(ObjSize size);
		
		/*! \brief get object size		
			\return object size 
		 */
		ObjSize get_size() const;
		
		/*! \brief flip object vertically 	
		 */
		void flip_v();
		
		/*! \brief set object vertical flip
			\param flip 0 if no flip, non-0 value if flipped
		 */
		void set_flip_v(unsigned short flip);
		
		/*! \brief get object vertical flip
			\return 0 if no flip, 1 if flipped
		 */
		unsigned short get_flip_v() const;
		
		/*! \brief flip sprite horizontally
		 */
		void flip_h();
		
		/*!	\brief set sprite horizontal flip
			\param flip 0 if no flip, non-0 value if flipped
		 */
		void set_flip_h(unsigned short flip);
		
		/*! \brief get sprite horizontal flip
			\return 0 if no flip, 1 if flipped
		 */
		unsigned short get_flip_h() const;
		
		/*! \brief set sprite X coordinate
			\param x on-screen sprite X-coordinate (9 bits)
		 */
		void set_x(short x);
		
		/*! \brief set sprite Y coordinate
			\param y on-screen sprite Y-coordinate (8 bits)
		 */
		void set_y(short y);
		
		/*! \brief get sprite X coordinate
			\return on-screen sprite X coordinate
		 */
		short get_x() const;
		
		/*! \brief get sprite Y coordinate
			\return on-screen sprite Y coordinate
		 */
		short get_y() const;
		
		/*! \brief set sprite priority
			\param priority sprite priority (0..3)
		 */
		void set_priority(unsigned short priority);
		
		/*! \brief set index of the first sprite tile 
			\param tile_index sprite tile index
		 */
		void set_tile_index(unsigned short tile_index);	
		
		/*! \brief gets tile index of the first sprite tile			
			\returns object tile index
		 */
		unsigned short get_tile_index() const;
		
		/*! \brief makes object visible on the screen
		 */
		void show();
		
		/*! \brief makes object invisible on the screen
		 */
		void hide();

		/*! \brief checks whether object is visible or not
			\returns true if object is hidden, false otherwise
		 */
		bool is_hidden() const;
		
		/*! \brief checks if object is 8-bit
			\returns true if object is 8-bit, false otherwise
		 */
		bool is_8bit() const;
		
		/*! \brief checks if object is 4-bit
			\returns true if object is 4-bit, false otherwise
		 */
		bool is_4bit() const;
		
		void set_color_depth(int bpp);
		
		int get_palette_number() const;
		
		void set_palette_number(int slot);
		
		bool is_rotation_scaling() const;
		void set_rotation_scaling(bool value);
		
		bool is_double_size() const;
		void set_double_size(bool value);		
		
		
		void set_affine_matrix(int matrix_id);
		int get_affine_matrix() const;
	};
}