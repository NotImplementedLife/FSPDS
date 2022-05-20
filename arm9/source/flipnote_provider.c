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

file_data* PPM_FILES_SLOT[3][FILES_PER_SLOT];

u8 CRT_SLOT;
u8 PRV_SLOT;
u8 NXT_SLOT;
int CRT_GLOBAL;

int get_global_slot_id(u8 slot)
{
	return CRT_GLOBAL + (slot==PRV_SLOT ? -1 : 0) + (slot==NXT_SLOT ? 1 : 0);
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
}

u8 file_size_checker_active = 0;
int file_size_checker_index = 0;

void start_loading_slot(int slot_id);

void provider_init(const char* flipnotes_path)
{	
	PPM_PATH = flipnotes_path;
	for(int s=0;s<3;s++)
	{
		for(int i=0;i<FILES_PER_SLOT;i++)
			PPM_FILES_SLOT[s][i]=NULL;
	}
	CRT_GLOBAL = 0;
	PRV_SLOT = 0;
	CRT_SLOT = 1;
	NXT_SLOT = 2;
	SEEK_DIR_OFFSETS[CRT_GLOBAL] = 0;	
	
	__provider_register_data reg_data = create_register_data(CRT_SLOT);	
	SEEK_DIR_OFFSETS[CRT_GLOBAL+1] = loadFilesFrom(PPM_PATH, 0, FILES_PER_SLOT, provider_file_discovered, &reg_data);
	if(reg_data.len<FILES_PER_SLOT) // last slot can be incomplete
	{
		SEEK_DIR_OFFSETS[CRT_GLOBAL+1]=-1; // there's no need to further load slots
	}		
	file_size_checker_active = 1;
	file_size_checker_index = FILES_PER_SLOT;
	
	//start_loading_slot(NXT_SLOT);
}

u8 slot_half=0;
file_data* provider_get_nth_flipnote(int n)
{
	if(n<0) return NULL;
	file_data* result = NULL;
	int crt_first = CRT_GLOBAL<<SLOT_MAGNITUDE;
	int rel_id=-1;
	if(crt_first-FILES_PER_SLOT<=n && n<crt_first)
	{
		rel_id = n-crt_first+FILES_PER_SLOT;
		result = PPM_FILES_SLOT[PRV_SLOT][rel_id];		
	}
	else if(crt_first <= n && n<crt_first+FILES_PER_SLOT)
	{	
		rel_id = n-crt_first;
		result = PPM_FILES_SLOT[CRT_SLOT][rel_id];
				
		if(rel_id>=3*FILES_PER_SLOT/4)
		{
			if(slot_half==0)
			{
				slot_half = 1;
				int tmp = PRV_SLOT;
				PRV_SLOT = CRT_SLOT;
				CRT_SLOT = NXT_SLOT;			
				NXT_SLOT = tmp;
				CRT_GLOBAL++;
				start_loading_slot(NXT_SLOT);
			}
		}
		else slot_half=0;		
		
	}	
	else if(crt_first+FILES_PER_SLOT<=n && n<crt_first+2*FILES_PER_SLOT)
	{		
		rel_id = n-crt_first-FILES_PER_SLOT;
		result = PPM_FILES_SLOT[NXT_SLOT][rel_id];
	}	
	return result;
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

u8 loading_slot_active = 0;
u8 loading_slot_id = 0;
int global_slot_id = 0;
int loading_dir_index = 0;
void* loading_dir_ptr = NULL;

void start_loading_slot(int slot_id)
{
	if(loading_slot_active) 
	{
		c_displayError("Slot already loading",true);
		return;
	}
	for(int i=0;i<FILES_PER_SLOT;i++) 
	{
		free(PPM_FILES_SLOT[slot_id][i]);
		PPM_FILES_SLOT[slot_id][i] = NULL;
	}
	
	loading_slot_id = slot_id;
	global_slot_id = get_global_slot_id(loading_slot_id);
	if(global_slot_id<0) return;
	if(global_slot_id>=MAX_SLOTS) return;
	long loading_dir_offset = SEEK_DIR_OFFSETS[global_slot_id];
	
	loading_dir_ptr = __open_dir(PPM_PATH, loading_dir_offset);	
	loading_dir_index = 0;
	loading_slot_active = 1;
}

#include "console.h"

void provider_background(u8* trigger)
{	
	*trigger = 0;
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
				*trigger = 1;
				return;
			}
		}		
	}*/
	if(loading_slot_active)
	{		
		long dir_offset=-1;
		file_data* fd = __read_dir(loading_dir_ptr, &dir_offset);
		if(fd==NULL)
		{			
			SEEK_DIR_OFFSETS[global_slot_id+1]=-1;
			loading_slot_active = 0;
			return;
		}
		PPM_FILES_SLOT[loading_slot_id][loading_dir_index++]=fd;
		c_goto(0,0);
		iprintf("%i    ",loading_dir_index);
		if(loading_dir_index == FILES_PER_SLOT)
		{
			SEEK_DIR_OFFSETS[global_slot_id+1]=dir_offset;
			loading_slot_active = 0;
			return;
		}				
	}
}












