#pragma once

#include "ppm_reader.hpp"
#include "DSC"
#include "filesystem.hpp"

//extern PPMReader* ppm_reader;

void set_fat_inited(bool value);
bool is_fat_inited();

extern DSC::Scene* (*folder_picker_next_scene_gen)();
extern char* picked_folder_path;

extern int selected_location_index;
extern Location* selected_location;

extern int selected_thumbnail_page;
extern int selected_thumbnail_index;

class Config
{
public:
	static bool is_configured();
	static void initialize();	
};