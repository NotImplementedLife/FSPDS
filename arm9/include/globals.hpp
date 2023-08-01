#pragma once

#include "ppm_reader.hpp"
#include "DSC"
#include "filesystem.hpp"

//extern PPMReader* ppm_reader;

void set_fat_inited(bool value);
bool is_fat_inited();

void set_player_autoplay(bool value);
bool is_player_autoplay();

void set_player_shuffle(bool value);
bool is_player_shuffle();

extern DSC::Scene* (*folder_picker_next_scene_gen)();
extern char* picked_folder_path;

extern int selected_location_index;
extern Location* selected_location;
extern int location_flipnotes_count;

extern int selected_thumbnail_page;
extern int selected_thumbnail_index;

void load_app_state();
void save_app_state();

extern bool player_screen_on;