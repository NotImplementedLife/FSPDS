#pragma once

#include "DSCEngine/scenes/scene.hpp"
#include "DSCEngine/resources/asset_data.hpp"
#include "DSCEngine/sprites/sprite.hpp"


namespace DSC
{
	/*! \brief general-purpose scene with 256KB BG-Main VRAM
		\details Size limits:
		- 256 KB for main backgrounds
		-  64 KB for main objects
		-   8    extended palettes for each main background
		-  16    extended palettes for main objects
		- 128 KB for sub backgrounds
		- 128 KB for sub objects
		-  16    extended palettes for each sub backgrounds
		-  16    extended palettes for sub objects
	 */
	class GenericScene256 : public Scene
	{
	private:
		struct __privates__;
		__privates__* privates;
		
		void set_banks();
		void solve_map_requirements();		
		
		void solve_map_requirements_main();
		void solve_map_requirements_sub();
		
		void solve_video_mode();
		
		void load_assets();
		
		static int validate_bg_size(int w, int h, int color_depth, bool is_bitmap);
	public:
		GenericScene256();
		virtual void init() override;		
		
		__attribute__((noinline))
		virtual void run() override;
		
		virtual void frame() override;
		
		
		void require_tiledmap_4bpp(int id, int px_width, int px_height, int number_of_tiles);
		void require_tiledmap_8bpp(int id, int px_width, int px_height, int number_of_tiles);
		
		void require_tiledmap(int id, int px_width, int px_height, const AssetData* tileset);
		
		void require_bitmap(int id, int px_width, int px_height);
		void require_bitmap(int id, const DSC::AssetData* bitmap);
		
		void require_bitmap_16bpp(int id, int px_width, int px_height);
		void require_bitmap_16bpp(int id, const DSC::AssetData* bitmap);
		
				
		Sprite* create_sprite(Sprite* sprite);
		
		void begin_sprites_init();		
		void end_sprites_init();
		
		~GenericScene256();
		
		static constexpr int MAIN_BG0 = 0;
		static constexpr int MAIN_BG1 = 1;
		static constexpr int MAIN_BG2 = 2;
		static constexpr int MAIN_BG3 = 3;
		
		static constexpr int SUB_BG0 = 4;
		static constexpr int SUB_BG1 = 5;
		static constexpr int SUB_BG2 = 6;
		static constexpr int SUB_BG3 = 7;
	};
}