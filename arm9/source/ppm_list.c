#include "ppm_list.h"

#include "filesystem.h"
#include "console.h"

#include <stdlib.h>

UiList ppm_list;
ListItemsSource ppm_source;

#define MAX_CHUNKS (65536 / CHUNK_SIZE)

long chk_offset[MAX_CHUNKS];

typedef struct 
{
	ItemsChunk* chk;
	int index;
} chk_index_pair;

void discovered_ppm_callback(file_data* fd, void* arg)
{
	chk_index_pair* cip = (chk_index_pair*)arg;
	(*(cip->chk))[cip->index++] = fd;
}

ItemsChunk* load_ppm_files_chunk(int id) 
{		
	ItemsChunk* chk = malloc(sizeof(ItemsChunk));	
	for(int i=0;i<CHUNK_SIZE;i++) 
	{
		(*chk)[i] = NULL;
	}
	
	chk_index_pair cip;
	cip.chk = chk;
	cip.index = 0;
	
	
	int offset = 0;
	if(id>=0)
	{		
		offset = chk_offset[id];
		if(offset==-1) 
		{
			c_displayError("Chunk Id Error.", true);
		}
	}
	
	long len = loadFilesFrom("/flipnotes", offset,  CHUNK_SIZE, discovered_ppm_callback, &cip);
	chk_offset[id+1] = len;	
	
	return chk;
}

void release_ppm_files_chunk(ItemsChunk* chunk)
{
	for(int i=0;i<CHUNK_SIZE;i++) 
	{
		free((*chunk)[i]);
	}
	free(chunk);
}

void initPPMLists() 
{
	chk_offset[0] = 0;
	for(int i=1;i<MAX_CHUNKS;i++)
		chk_offset[i]=-1;
	
	uilist_init(&ppm_list);	
	
	lis_init(&ppm_source);
	lis_set_chunk_procs(&ppm_source, load_ppm_files_chunk, release_ppm_files_chunk);
	lis_select(&ppm_source, 1);
	
	uilist_attach_lis(&ppm_list, &ppm_source);	
}