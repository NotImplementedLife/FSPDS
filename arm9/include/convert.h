#pragma once

typedef enum
{
	AMV = 0,
	AVI = 1,
	DPG4 = 2,
	GIF = 3,
	MKV = 4,
	MP4 = 5,
} ConversionFormat;

void convert(const char* ppm_filename, ConversionFormat format);