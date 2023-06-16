#pragma once

#include "ppm_reader.hpp"

extern PPMReader* ppm_reader;

void set_fat_inited(bool value);
bool is_fat_inited();


class Config
{
public:
	static bool is_configured();
	static void initialize();
	
	
};