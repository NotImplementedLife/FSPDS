#pragma once

typedef enum
{
	AMV,
	DPG4,
	MP4,
} ConversionFormat;

void convert(const char* ppm_filename, ConversionFormat format);