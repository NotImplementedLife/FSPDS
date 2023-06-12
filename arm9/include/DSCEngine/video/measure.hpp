#pragma once

namespace DSC
{
	/*! \brief Wrapper around int to convert units
	 */
	class MeasureValue
	{
	private:
		int raw_value;		
		bool conversion_fit = false;
		int block_cnt = 1;
	public:		
		/*! \brief creates measure data from numeric value
			\raw numeric value
		 */
		MeasureValue(int raw = 0);
						
		/*! \brief specifies to use the lower integer bound for non-integer value conversion
		 */
		MeasureValue& cut();
		
		/*! \brief specifies to use the upper integer bound for non-integer value conversion
		 */
		MeasureValue& fit();
		
		/*! \brief specifies the number of packets of fixed quantities the value must be divided into
			\param quant a packet size
			\example
			\code{.cpp}
				MeasureValue(20).blocks(5).value() // 4 = 20/5
				MeasureValue(10).cut().blocks(3).value() // 3 = floor(10/3)
				MeasureValue(10).fit().blocks(3).value() // 4 = floor(10/3) + 1
			\endcode
		 */
		MeasureValue& blocks(int quant);
	
		/*! \brief gets the value of the measure, after applying the value modifier
			\return resulted value
		 */
		int value() const;
		
		/*! \brief converts the value of the measure in kilobytes. Roughly equivalent to MeasureValue::blocks(1024).
			\return resulted value in kilobytes
		 */
		int kilobytes() const;
				
		operator int() const;
		
	};
	
	/*! \brief Helper class to deal with graphics size computations
	 */
	class Measure
	{
	private:		
		bool bmp = false;
		bool rot = false;
		int color_depth = 4;
		int tile_w = 1;
		int tile_h = 1;
	public:
		/*! \brief chain function to set color depth to 4 bpp
			\return the current instance with modified settings
		 */
		Measure& _4bpp();
		
		/*! \brief chain function to set color depth to 8 bpp
			\return the current instance with modified settings
		 */
		Measure& _8bpp();
		
		/*! \brief chain function to set color depth to 16 bpp
			\return the current instance with modified settings
		 */
		Measure& _16bpp();
		
		/*! \brief alternative to _4bpp(), _8bpp() or _16bpp() with dynamic parameter
			\return the current instance with modified settings
		 */
		Measure& bpp(int color_depth);
		
		/*! \brief chain function to set meta tile size
			\param tw meta tile width (e.g. for 32x16px tiles, tw equals 4)
			\param th meta tile height (e.g. for 32x16px tiles, th equals 2)
			\return the current instance with modified settings
		 */
		Measure& metatile(int tw, int th);
		
		/*! \brief chain function to set map type as text (tiles)
		 */
		Measure& text();
		
		/*! \brief chain function to set map type as bitmap
		 */
		Measure& bitmap();
		
		/*! \brief computes the size of a number of tiles given the measure settings
			\param count the number of tiles
			\return size of a graphics with the given number of tiles and the defined settings
			
			\details In computing the tiles size, the color depth and metatile data are taken into account.
			By default, tiles are 8x8px with 4bpp color-depth
		 */
		MeasureValue tiles(int count) const;
			
		/*! \brief computes the size of a matrix of tiles given the measure settings
			\param rows_count number of rows in the tiles matrix
			\param cols_count number of columns in the tiles matrix
			\return size of a graphics with the given number of tiles and the defined settings
			
			\details In computing the tiles size, the color depth and metatile data are taken into account.
			By default, tiles are 8x8px with 4bpp color-depth
		 */			
		MeasureValue tiles(int rows_count, int cols_count) const;
		
		/*! \brief computes the size of a bitmap given the measure settings
			\param width bitmap's width in pixels
			\param height bitmap's height in pixels
			\return size of a bitmap with the given size and the defined color depth
		 */		
		MeasureValue bitmap(int width, int height) const;
		
		/*! \brief finds how many tiles can fit in memory block of a certain size, given the measure settings
			\param size the memory block size
			\return maximum number of tiles whose size are at most the specified size
		 */
		int tiles_count(int size) const;
		
		MeasureValue map_size(int width, int height) const;
		
		int bytes_per_map_entry() const;
		
		int tile_id(void* address, void* base, int mapping_step) const;
		
		
		
	};
}
