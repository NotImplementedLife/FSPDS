#pragma once

#include "DSC"
#include "ppm_reader.hpp"

using namespace DSC;

Scene* gen_title_scene();
Scene* gen_main_scene();

extern const char* selected_flipnote_path;
extern PPMReader* ppm_reader;

Scene* get_player_scene();