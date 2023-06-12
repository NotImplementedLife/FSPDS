#include "DSCEngine/video/measure.hpp"

#include "DSCEngine/debug/assert.hpp"

using namespace DSC;


DSC::MeasureValue::MeasureValue(int raw) : raw_value(raw) { }

MeasureValue& DSC::MeasureValue::cut() { conversion_fit = false; return *this; }

MeasureValue& DSC::MeasureValue::fit() { conversion_fit = true; return *this; }

MeasureValue& DSC::MeasureValue::blocks(int quant)
{
	if(quant>0)
		block_cnt = quant;
	return *this;
}

int DSC::MeasureValue::value() const
{
	return (raw_value + (conversion_fit ? block_cnt - 1:0)) / block_cnt;
}

int DSC::MeasureValue::kilobytes() const
{
	int b = block_cnt * 1024;
	return (raw_value + (conversion_fit ? b - 1:0)) / b;
}

DSC::MeasureValue::operator int() const
{
	return value();
}


Measure& DSC::Measure::_4bpp() { color_depth = 4; return *this; }
Measure& DSC::Measure::_8bpp() { color_depth = 8; return *this; }
Measure& DSC::Measure::_16bpp() { color_depth = 16; return *this; }

Measure& DSC::Measure::metatile(int tw, int th) { tile_w = tw, tile_h = th; return *this; }

Measure& DSC::Measure::text() { bmp = false; return *this; }
Measure& DSC::Measure::bitmap() { bmp = true; return *this; }

MeasureValue DSC::Measure::tiles(int count) const
{
	return count * 8*tile_w * 8*tile_h * color_depth / 8;
}

MeasureValue DSC::Measure::tiles(int rows_count, int cols_count) const
{
	return tiles(rows_count * cols_count);
}
	
MeasureValue DSC::Measure::bitmap(int width, int height) const
{
	//Debug::log("M %i %i %i", width, height, color_depth);
	return width * height * color_depth / 8;
}
	
int DSC::Measure::tiles_count(int size) const
{
	return size * 8 / (color_depth * 8 * tile_w * 8 * tile_h);
}


MeasureValue DSC::Measure::map_size(int width, int height) const
{
	nds_assert(!bmp, "Can't compute map_size(w,h) for bitmaps. Use .bitmap(w,h) instead");
	return width/8 * height/8 * bytes_per_map_entry();
}

int DSC::Measure::bytes_per_map_entry() const
{
	return 2; // would be 1 for rotation bg's
}

Measure& DSC::Measure::bpp(int _color_depth)
{
	nds_assert(_color_depth==4 || _color_depth==8 || _color_depth==16);
	this->color_depth = _color_depth;
	return *this;
}

int DSC::Measure::tile_id(void* address, void* base, int mapping_step) const
{
	// some sanity checks should be done here...
	return ((int)address-(int)base)/mapping_step;
}