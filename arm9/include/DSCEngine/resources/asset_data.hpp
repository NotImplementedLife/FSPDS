#pragma once

#include "DSCEngine/resources/readonly_data.hpp"

namespace DSC
{
	class AssetData : public ReadOnlyData
	{
	public:
		__attribute__ ((packed))
		short width;                    /// asset width  (divided by 8)
		__attribute__ ((packed))
		short height;                   /// asset height (divided by 8)		
		
		AssetData();
		~AssetData() = delete;
		
		/*! \brief gets color bit depth of the asset
			\return color depth (4,8 or 16)
		 */
		int get_color_depth() const;
		
		/*! \brief checks if the asset is a tileset or a bitmap
			\return true if it is bitmap, false otherwise
		 */
		bool is_bitmap() const;
		
		
		int get_gfx_length() const;
		int get_pal_length() const;
		int get_pal_count() const;
		void extract_gfx(void* destination) const;
		void extract_palette(void* destination) const;		
		
		/*! \brief gets the width of an asset's metatile.
			If no metatile data is provided, the width of a single 8x8 tile is provided.
			\return If the asset is a tileset, the width of the metatile measured in 8x8 tiles is returned.
			If the asset is a bitmap, the width of the metatile measured in pixels is returned
		 */
		int get_metatile_width() const;
		
		/*! \brief gets the height of an asset's metatile.
			If no metatile data is provided, the height of a single 8x8 tile is provided.
			\return If the asset is a tileset, the height of the metatile measured in 8x8 tiles is returned.
			If the asset is a bitmap, the height of the metatile measured in pixels is returned
		 */
		int get_metatile_height() const;
		
		/*! \brief gets the size in bytes of an asset's metatile.
			If no metatile data is provided, the size of a single 8x8 tile is provided.
			\return The size in bytes of a metatile
		 */		
		int get_metatile_size() const;
		
		static const int ROA_IS_BITMAP;
		static const int ROA_COLOR_DEPTH;
		static const int ROA_METATILE_WIDTH;
		static const int ROA_METATILE_HEIGHT;
	};
}