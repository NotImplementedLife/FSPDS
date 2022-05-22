#include "flipnote_provider.h"

#include <stdlib.h>
#include <string.h>
#include "filemanager.h"
#include "console.h"

const char* PPM_PATH = NULL;

#define SLOT_MAGNITUDE 7
#define FILES_PER_SLOT (1<<SLOT_MAGNITUDE)

#define MAX_SLOTS (65536 / FILES_PER_SLOT)
long SEEK_DIR_OFFSETS[MAX_SLOTS + 1];

s8 LOAD_SLOT[MAX_SLOTS+1];

file_data* PPM_FILES_SLOT[5][FILES_PER_SLOT] = { 0 };
int redundant[5] = { 0 };
int chunk[5] = {-1, -1, -1, -1, -1};

struct 
{
	u8 loading_chunk_active;
	long loading_dir_offset;
	void* loading_dir_ptr;
	u16 loading_dir_index;
	u16 loading_chunk_index;
	u16 loading_target_slot;
} background_provider_data = {0,0,NULL,0,0,0};

int unload_slot(int slot_id)
{	
	for(int i=0;i<FILES_PER_SLOT;i++)
	{
		free(PPM_FILES_SLOT[slot_id][i]);
		PPM_FILES_SLOT[slot_id][i]=NULL;
	}
	int chk_id = chunk[slot_id];
	chunk[slot_id]=-1;
	redundant[slot_id]=-1;	
	return chk_id;
}

int get_highest_redundancy_slot()
{
	int red = -1;
	int j = -1;	
	for(int i=0;i<5;i++)
		if(chunk[i]>=0)
			if(redundant[i] > red)
			{
				red = redundant[i];
				j=i;
			}
	return j;
}

int get_free_slot()
{	
	int i=5;
	for(;i--;)	
		if(chunk[i]==-1)
			break;
	return i;
}

int chunk_loaded(int chk_id)
{
	for(int i=0;i<5;i++)
		if(chunk[i]==chk_id)
			return 1;
	return 0;
}


int load_to_slot(int chunk_id)
{
	// pick a new slot for the chunk	
	int slot_id = get_free_slot();
	if(slot_id==-1)
	{
		slot_id = get_highest_redundancy_slot();	
		if(slot_id==-1) 
		{			
			c_displayError("All slots are full", 1);
		}
		unload_slot(slot_id);
	}	
	
	if(chunk_id<0) return -1;
	if(chunk_id>=MAX_SLOTS) return -1;		
	
	background_provider_data.loading_dir_offset = SEEK_DIR_OFFSETS[chunk_id];
	if(background_provider_data.loading_dir_offset<0)
	{
		char* c="Cannot load directory";
		c[0]='0'+chunk_id;
		c_displayError(c,1);
		return -1;		
	}
	
	background_provider_data.loading_dir_ptr = __open_dir(PPM_PATH, background_provider_data.loading_dir_offset);		
	background_provider_data.loading_dir_index = 0;	
	background_provider_data.loading_chunk_index = chunk_id;
	background_provider_data.loading_target_slot = slot_id;
	background_provider_data.loading_chunk_active = 1;
			
	return slot_id;
}

void wait_loading_slot()
{
	while(background_provider_data.loading_chunk_active)
	{
		provider_background(NULL);
	}
}

typedef struct 
{
	file_data** slot;
	int len;
} __provider_register_data;

__provider_register_data create_register_data(int slot_id)
{
	__provider_register_data data = { PPM_FILES_SLOT[slot_id], 0 };
	return data;
}


void provider_file_discovered(file_data* fd, void* arg)
{
	__provider_register_data* rd = (__provider_register_data*)arg;
	rd->slot[rd->len++] = fd;	
	rd->slot[rd->len-1]->size = rd->len;
	long_to_size_string(rd->slot[rd->len-1]->size_str, rd->slot[rd->len-1]->size);
}

u8 file_size_checker_active = 0;
int file_size_checker_index = 0;

void provider_init(const char* flipnotes_path)
{	
	PPM_PATH = flipnotes_path;
	for(int s=0;s<5;s++)
	{
		for(int i=0;i<FILES_PER_SLOT;i++)
			PPM_FILES_SLOT[s][i]=NULL;
	}
	for(int i=0;i<=MAX_SLOTS;i++)
	{
		SEEK_DIR_OFFSETS[i]=-1;
		LOAD_SLOT[i]=-1;
	}					
	// manually (sync) load first chunk
	SEEK_DIR_OFFSETS[0]=0;
	__provider_register_data reg_data = create_register_data(0);
	SEEK_DIR_OFFSETS[1] = loadFilesFrom(PPM_PATH, 0, FILES_PER_SLOT, provider_file_discovered, &reg_data);
	if(reg_data.len<FILES_PER_SLOT) // last slot can be incomplete
	{
		SEEK_DIR_OFFSETS[1]=-1; // there's no need to further load slots
	}
	chunk[0]=0;
	LOAD_SLOT[0]=0;
	redundant[0]=0;
	
	load_to_slot(1); wait_loading_slot();
	
	load_to_slot(2); wait_loading_slot();
	
	load_to_slot(3); wait_loading_slot();
	
	load_to_slot(4); wait_loading_slot();
	
	file_size_checker_active = 1;
	file_size_checker_index = FILES_PER_SLOT;	
}

void update_redundancy(int chk_id)
{
	for(int i=0;i<5;i++)
	{		
		if(chunk[i]>=0)			
		{
			redundant[i] = chunk[i] - chk_id;
			if(redundant[i]<0)
				redundant[i] = -redundant[i];			
		}		
	}
	
	consoleSelect(&consoleFG);
	c_goto(0,10);
	for(int i=0;i<5;i++) iprintf("%i ",redundant[i]);
	iprintf("    ");
	
}

void load_chunk(int chk_id)
{
	if(background_provider_data.loading_chunk_active)
		return;
	if(chk_id < 0)
		return;
	if(SEEK_DIR_OFFSETS[chk_id]<0)
		return;
	if(chunk_loaded(chk_id))
		return;		
	load_to_slot(chk_id); // async!
	consoleSelect(&consoleFG);
	c_goto(23,0);
	for(int i=0;i<5;i++) iprintf("%i ",chunk[i]);
	iprintf("       ");
	c_goto(23,20);
	for(int i=0;i<10;i++)
		iprintf("%i",SEEK_DIR_OFFSETS[i]>=0?1:0);
	
}

file_data* provider_get_nth_flipnote(int n)
{
	if(n<0) return NULL;	
	int chk_id = n >> SLOT_MAGNITUDE;	
	int slot_id = LOAD_SLOT[chk_id];
	if(slot_id<0)
		return NULL;	
	update_redundancy(chk_id);
	load_chunk(chk_id-1);
	load_chunk(chk_id+1);	
	
	int index = n & (FILES_PER_SLOT-1);
	return PPM_FILES_SLOT[slot_id][index];
}

char* provider_get_flipnote_full_path(file_data* fd)
{
	if(fd==NULL) return NULL;
	int dir_path_len = strlen(PPM_PATH);
	char* result = malloc(dir_path_len + 33);
	strcpy(result, PPM_PATH);
	strcat(result, "/");		
	strcat(result, fd->name);	
	return result;
}

#include "console.h"

void provider_background(u8* trigger)
{	
	if(trigger) *trigger = 0;
	/*if(file_size_checker_active)
	{		
		int slot = file_size_checker_index >> SLOT_MAGNITUDE;
		int index = file_size_checker_index & (FILES_PER_SLOT - 1);
		
		file_size_checker_index++;
		if(file_size_checker_index == 3*FILES_PER_SLOT)
		{
			file_size_checker_index = 0;
			file_size_checker_active = 0;			
		}
		
		file_data* fd = PPM_FILES_SLOT[slot][index];
		if(fd!=NULL)
		{
			if(fd->size_str[0]=='\0') // size not loaded yet
			{
				char* path = provider_get_flipnote_full_path(fd);
				fd->size=get_file_size(path);
				long_to_size_string(fd->size_str, fd->size);
				free(path);
				if(trigger) *trigger = 1;
				return;
			}
		}		
	}*/
	if(background_provider_data.loading_chunk_active)
	{				
		if(SEEK_DIR_OFFSETS[background_provider_data.loading_chunk_index]==-1)
		{
			c_displayError("Seek dir offset not set", 1);
		}
		long dir_offset=-1;		
		file_data* fd = __read_dir(background_provider_data.loading_dir_ptr, &dir_offset);
		if(fd==NULL) // read dir ends 
		{									
			LOAD_SLOT[background_provider_data.loading_chunk_index] = background_provider_data.loading_target_slot;
			for(int i=background_provider_data.loading_chunk_index + 1;i<=MAX_SLOTS;i++)
				SEEK_DIR_OFFSETS[i] = -2; // mark all following slots as invalid
		
			LOAD_SLOT[background_provider_data.loading_chunk_index] = background_provider_data.loading_target_slot;
			chunk[background_provider_data.loading_target_slot] = background_provider_data.loading_chunk_index;
			redundant[background_provider_data.loading_target_slot] = 0;			
			background_provider_data.loading_chunk_active = 0;
			
			consoleSelect(&consoleFG);
			c_goto(23,0);
			for(int i=0;i<5;i++) iprintf("%i ",chunk[i]);
			iprintf("       ");
			return;
		}
		fd->size=background_provider_data.loading_chunk_index*10+background_provider_data.loading_target_slot;
		long_to_size_string(fd->size_str, fd->size);
		PPM_FILES_SLOT[background_provider_data.loading_target_slot][background_provider_data.loading_dir_index++]=fd;
		consoleSelect(&consoleFG);
		c_goto(0,0);
		iprintf("%i %i %i    ",
			background_provider_data.loading_chunk_index,  
			background_provider_data.loading_target_slot,
			background_provider_data.loading_dir_index);
		if(background_provider_data.loading_dir_index == FILES_PER_SLOT)
		{
			/*if(background_provider_data.loading_chunk_index==1)
			{				
				c_displayError("1",0);
				c_goto(3,3);
				iprintf("%li",dir_offset);
				while(1) swiWaitForVBlank();
			}*/
			if(SEEK_DIR_OFFSETS[background_provider_data.loading_chunk_index + 1] == -1)
				SEEK_DIR_OFFSETS[background_provider_data.loading_chunk_index + 1]=dir_offset;			
			LOAD_SLOT[background_provider_data.loading_chunk_index] = background_provider_data.loading_target_slot;
			chunk[background_provider_data.loading_target_slot] = background_provider_data.loading_chunk_index;
			redundant[background_provider_data.loading_target_slot] = 0;			
			background_provider_data.loading_chunk_active = 0;			
			
			consoleSelect(&consoleFG);
			c_goto(23,0);
			for(int i=0;i<5;i++) iprintf("%i ",chunk[i]);
			iprintf("       ");
			return;
		}				
	}
}












