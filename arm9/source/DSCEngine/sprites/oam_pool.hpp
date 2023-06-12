#pragma once

#include <nds.h>
#include "obj_attribute.hpp"

#include "DSCEngine/types/bits_array.hpp"

namespace DSC
{
	/*! \brief Low level OAM handler
	* 
	 */		
	class OamPool
	{				
	private:
		static constexpr unsigned int OBJ_COUNT = 128;
		static constexpr unsigned int OBJ_BUFFER_SIZE = OBJ_COUNT * sizeof(ObjAttribute);
				
		BitsArray<128> freeSlots;			 
				
		short __obj_attr_buffer[OBJ_BUFFER_SIZE / 2];
		ObjAttribute* obj_attr() const { return (ObjAttribute*)__obj_attr_buffer; }		
				
		bool _is_main = false;
		
		OamPool(bool is_main=true);
		int id_of(const ObjAttribute* attr) const;
		void clear_entry(ObjAttribute* attr);
		short* oam_offset() const;
		
	public:
		/*!
		* \brief sets OAM to its original state
		 */
		void reset();
		
		/*! \brief Adds new object to OAM
		 
			\param [in] attr new object attributes
			\return An identificator pointer of the object
		 
			\details **Caution!** Use the returned pointer attribute to work with the object. Do not attempt to free the pointer.
				Use remove_obj() to safely dispose of the object.
			\details This function operates on a shadow memory zone. Use OamPool::deploy() to make the changes visible on the screen.
		 */
		ObjAttribute* add_obj(ObjAttribute attr);
		
		/*! \brief Removes object from OAM by its identificator pointer
			\param [in] attr pointer to object's attributes		  
			\details **Caution!** Use only the pointers obtained from add_obj() as parameters in this function.
			\details This function operates on a shadow memory zone. Use OamPool::deploy() to make the changes visible on the screen.
		 */
		void remove_obj(ObjAttribute* attr);
		
		/*! \brief Updates all objects on the screen according to their OAM state.
		 */
		void deploy();		
		
		/*! \brief Sets the rotation matrix components		
			\param id id of rotation matrix slot (0..31)
			\param pa top-left matrix entry
			\param pb top-right matrix entry
			\param pc bottom-left matrix entry
			\param pd bottom-right matrix entry
		*/
		void set_rotation_matrix(int id, int pa, int pb, int pc, int pd);
		
		/*! \brief gets the main OamPool instance
			\return pointer to main OamPool
		 */
		static OamPool* const main();
		
		/*! \brief gets the sub OamPool instance
			\return pointer to sub OamPool
		 */
		static OamPool* const sub();
		
		bool is_main() const;
	};
}