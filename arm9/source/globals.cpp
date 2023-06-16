#include "globals.hpp"

PPMReader* ppm_reader = nullptr;

unsigned int state_flags = 0;

constexpr int STATE_FAT_INITED_BIT = 0;
constexpr int STATE_FAT_INITED_MASK = 1<<STATE_FAT_INITED_BIT;

void set_fat_inited(bool value) 
{
	state_flags &= ~STATE_FAT_INITED_MASK;
	state_flags |= value << STATE_FAT_INITED_BIT;
}

bool is_fat_inited() { return state_flags & STATE_FAT_INITED_MASK; }