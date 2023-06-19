#include "globals.hpp"
#include "scenes.hpp"

//PPMReader* ppm_reader = nullptr;

unsigned int state_flags = 0;

constexpr int STATE_FAT_INITED_BIT = 0;
constexpr int STATE_FAT_INITED_MASK = 1<<STATE_FAT_INITED_BIT;

void set_fat_inited(bool value) 
{
	state_flags &= ~STATE_FAT_INITED_MASK;
	state_flags |= value << STATE_FAT_INITED_BIT;
}

bool is_fat_inited() { return state_flags & STATE_FAT_INITED_MASK; }

DSC::Scene* (*folder_picker_next_scene_gen)() = gen_title_scene;
char* picked_folder_path = nullptr;

int selected_location_index = -1;
Location* selected_location = nullptr;

int selected_thumbnail_page = 0;
int selected_thumbnail_index = 0;