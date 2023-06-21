#include "globals.hpp"
#include "scenes.hpp"

//PPMReader* ppm_reader = nullptr;

unsigned int state_flags = 0;

#define define_bit(name, pos) \
		constexpr int STATE_ ## name ##_BIT = (pos); \
		constexpr int STATE_ ## name ##_MASK = 1<<(STATE_ ## name ##_BIT);

define_bit(FAT_INITED, 0);
define_bit(PLAYER_AUTOPLAY, 1);
define_bit(PLAYER_SHUFFLE, 2);

#define define_bit_methods(name, bit_name) \
	void set_ ## name (bool value) { \
		state_flags &= ~STATE_ ## bit_name ## _MASK; \
		state_flags |= value << STATE_ ## bit_name ## _BIT; \
	}\
	bool is_ ##name () { return state_flags & STATE_ ## bit_name ##_MASK;  }

define_bit_methods(fat_inited, FAT_INITED);
define_bit_methods(player_autoplay, PLAYER_AUTOPLAY);
define_bit_methods(player_shuffle, PLAYER_SHUFFLE);

DSC::Scene* (*folder_picker_next_scene_gen)() = gen_title_scene;
char* picked_folder_path = nullptr;

int selected_location_index = -1;
Location* selected_location = nullptr;
int location_flipnotes_count=0;

int selected_thumbnail_page = 0;
int selected_thumbnail_index = 0;

#include <stdio.h>

void load_app_state()
{
	int flags;
	
	FILE* fptr = fopen("data/FSPDS/config", "rb");	
	if(fptr!=nullptr)
	{
		fread(&flags, sizeof(int), 1, fptr);			
		fclose(fptr);
	}
	
	set_player_autoplay(flags & STATE_PLAYER_AUTOPLAY_MASK);
	set_player_shuffle(flags & STATE_PLAYER_SHUFFLE_MASK);
}

void save_app_state()
{
	int flags = 0;
	flags|= is_player_autoplay()<<STATE_PLAYER_AUTOPLAY_BIT;
	flags|= is_player_shuffle()<<STATE_PLAYER_SHUFFLE_BIT;
	
	FILE* fptr = fopen("data/FSPDS/config", "wb");	
	if(fptr==nullptr) return;
	fwrite(&flags, sizeof(int), 1, fptr);		
	fclose(fptr);	
}