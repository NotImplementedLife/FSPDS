#pragma once

#include "DSCEngine/types/point.hpp"
#include "DSCEngine/sprites/hitbox.hpp"
#include "DSCEngine/sprites/obj_allocator.hpp"
#include "DSCEngine/sprites/obj_visual.hpp"
#include "DSCEngine/world/localizable.hpp"
#include "DSCEngine/system/hardware.hpp"

namespace DSC
{	
	class Sprite : public Localizable
	{
	private:
		void* oam_pool;
		void* attr;
				
		// internal layout
		Hitbox hitbox;
		Point<char> anchor={0,0}, theoretical_anchor={0,0};
		
		Point<sf24> pos;
		
		ObjVisual* visual;
				
		ObjAllocator* obj_allocator = nullptr;
	public:				
		Sprite(ObjSize size, Engine engine);
		
		/*! \brief gets the sprite visual information
			\return pointer to sprite's visual data
		 */
		ObjVisual* get_visual() const;

		/*! \brief gets the sprite size
			\return width and height of sprite, in pixels
		 */
		Size<char> get_size() const;
			
		void update_visual(); /*!< Renders visual data to sprite's OAM attribute */
		void update_position(); /*!< Renders position data to sprite's OAM attribute */
	
		/*! \brief sets the sprite virtual position
			\param x horizontal position
			\param y vertical position
			
			\details The sprite position is actually the logical position and may coincide or not with the real (hardware) position.
			When interacting with a Camera, the sprite's hardware position is computed from the logical position relative to the
			camera offset. If the sprite is not in relation with any Camera, then the hardware position is calculated from the logical position
			relative to the top-left of the screen, when taking into account other details like the anchor and hitbox.
		 */
		void set_position(sf24 x, sf24 y);
		
		/*! \brief moves a sprite relative to its virtual position
			\param x relative horizontal position
			\param y relative vertical position
						
		 */
		void move(sf24 x, sf24 y);
			
		virtual sf24 x() const override; /*!< Gets sprite horizontal position in the virtual space */
		virtual sf24 y() const override; /*!< Gets sprite vertical position in the virtual space */
				
		Engine get_engine() const; /*!< Gets sprite owner engine (Main/Sub)*/
		
		
		/*! \brief Adds a new sprite frame
			\param frame Object Frame specifying the spritesheet and frame coordinates
			\return frame id to be used when setting the frame for display, also when including the frame into animated framesets
		 */
		int add_frame(ObjFrame* frame);
				
		void flip_horizontal(); /*!< Flips the sprite horizontally */
		void flip_vertical(); /*!< Flips the sprite vertically */
		
		/*! \brief Sets the sprite is horizontal flip state
			\param is_flipped true if sprite must be flipped, false otherwise
		 */
		void set_flip_horizontal(bool is_flipped);
		
		/*! \brief Sets the sprite is vertical flip state
			\param is_flipped true if sprite must be flipped, false otherwise
		 */
		void set_flip_vertical(bool is_flipped);
		
		
		bool is_flipped_horizontal() const; /*!< Checks if the sprite is flipped horizontally */
		bool is_flipped_vertical() const; /*!< Checks if the sprite is flipped vertically */
		
		/*! \brief Sets sprite priority
			\param priority Sprite priority 0..3
			\warning Use this only if you know what you are doing. Messing up with priorities can make sprites
			display behind backgrounds, in which case it is a potentially hard to detect bug source in larger contexts.
		 */
		void set_priority(int priority);
		// int get_priority() const;
				
		void show(); /*!< Displays the sprite */
		void hide(); /*!< Hides the sprite */
		bool is_hidden() const; /*!< Checks if the sprite is hidden */
		
		
		/*! \brief Sets sprite anchor relative to hitbox
			\param anchor_x horizontal anchor (0..255) 0=left-most, 255=right-most, 128=center
			\param anchor_y vertical anchor (0..255) 0=top-most, 255=bottom-most, 128=center
		 */
		void set_anchor(int anchor_x, int anchor_y);
		
		
		~Sprite();		
		
		
		// the following are not intended for common use outside the library:
		
		void set_default_allocator(ObjAllocator* obj_allocator);		
		static void oam_deploy_main();
		static void oam_deploy_sub();
	};
}