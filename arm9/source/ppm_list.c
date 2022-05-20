#include "ppm_list.h"

#include "filemanager.h"
#include "console.h"
#include "flipnote_provider.h"

#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

// https://playnote.studio/filehelp/
const ppm_location ppm_locations[] = 
{
	{ "/flipnotes", "    Default FSPDS path" },
	{ "/private/ds/app/4b475556", "    Flipnote Studio EU/AU SD" },
	{ "/private/ds/app/4B475545", "    Flipnote Studio NA SD" },
	{ "/private/ds/app/4B47554A", "    Flipnote Studio JP SD" },	
	{ "/", "About FSPDS"}
};

const int ppm_locations_length = sizeof(ppm_locations) / sizeof(ppm_location);

UiList path_selector_list;
ListItemsSource path_selector_source;

void ppm_list_reset();

const char *ppm_current_path = ppm_locations[0].path;

void set_current_path(const char* path)
{
	ppm_current_path = path;
	ppm_list_reset();
}

char* get_selected_file_name()
{
	file_data* fd = (file_data*)lis_get_selected_item(&ppm_source);
	if(fd==NULL) 
	{
		c_displayError("yeah",true);
	}
	return provider_get_flipnote_full_path(fd);
}

int get_selected_file_index()
{
	return ppm_source.selected_index;
}

file_data* get_selected_file_data()
{
	return (file_data*)lis_get_selected_item(&ppm_source);
}

UiList ppm_list;
ListItemsSource ppm_source;

// "The maximum number of files within a directory of a FAT file system is 65,536"
// ... or less, if there are longer file names
// Calculate the maximum number of file chunks a DAT32 directory can be split into
#define MAX_CHUNKS (65536 / CHUNK_SIZE)
// This saves the offset of the first file in each chunk. It can be used with seekdir() 
// in order to reload the chunk without manually parsing the entire directory
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

void ppm_list_reset()
{
	chk_offset[0] = 0;
	for(int i=1;i<MAX_CHUNKS;i++)
		chk_offset[i]=-1;
	uilist_reset(&ppm_list);
	lis_select(&ppm_source, 0);	
}

ItemsChunk* load_ppm_files_chunk(int id) 
{		
	ItemsChunk* chk = malloc(sizeof(ItemsChunk));	
	for(int i=0;i<CHUNK_SIZE;i++) 
	{
		(*chk)[i] = provider_get_nth_flipnote((id<<CHUNK_MAGNITUDE)|i);		
	}		
	
	return chk;
}

void release_ppm_files_chunk(ItemsChunk* chunk)
{
	for(int i=0;i<CHUNK_SIZE;i++) 
	{
		//free((*chunk)[i]);
		(*chunk)[i] = NULL;
	}
	free(chunk);
}

ItemsChunk* load_path_chunk(int id) 
{
	if(id!=0) return NULL;
	ItemsChunk* chk = malloc(sizeof(ItemsChunk));
	
	for(int i=0;i<CHUNK_SIZE;i++)
	{
		(*chk)[i]=NULL;
	}
	int k=0;
	for(int i=0;i<ppm_locations_length;i++)
	{	
		DIR* dir = opendir(ppm_locations[i].path);
		if (dir) {
			// pass int as void*, not sure how safe is, but I can't think it better :))
			// at least make sure it is not null 
			(*chk)[k++]=(void*)(i+1);
			closedir(dir);
		} else if (ENOENT == errno) {
			/* Directory does not exist. */
		} else {
			/* opendir() failed for some other reason. */
		}
	}
	
	return chk;	
}

void release_path_chunk(ItemsChunk* chunk)
{
	free(chunk);
}


void ppm_write_entry(void* item, int listpos, int is_highlighted)
{			
	if(item==NULL) 
	{
		consoleSelect(&consoleFG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }
        consoleSelect(&consoleBG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }		
        return;
	}
		
	file_data* fd = (file_data*)item;		
	
	consoleSelect(&consoleBG);
    iprintf(is_highlighted?"\x1b[39m":"\x1b[30m");
    for(int i=0;i<3;i++)
    {
        c_goto(1+3*listpos+i,1);
        for(int j=0;j<30;j++) iprintf("\x02");
    }
    consoleSelect(&consoleFG);
    c_goto(1+3*listpos,2);
    iprintf(is_highlighted?"\x1b[30m":"\x1b[39m");
    iprintf(fd->name);	
    if(listpos==7) return;
    c_goto(3+3*listpos,1);
	iprintf("                              ");
	c_goto(2+3*listpos,1);
	iprintf("                              ");
    c_goto(2+3*listpos,20);		
	iprintf(fd->size_str);
}

void path_write_entry(void* item, int listpos, int is_highlighted)
{	
	if(item==NULL)
	{
		consoleSelect(&consoleFG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }
        consoleSelect(&consoleBG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }		
        return;
	}
	int index = (int)item;
	index--;	
	
	consoleSelect(&consoleBG);	
    iprintf(is_highlighted?"\x1b[39m":"\x1b[30m");
    for(int i=0;i<3;i++)
    {
        c_goto(1+3*listpos+i,1);
        for(int j=0;j<30;j++) iprintf("\x02");
    }	
	
	consoleSelect(&consoleFG);
	c_goto(1+3*listpos+1, 2);	
	iprintf(is_highlighted?"\x1b[30m":"\x1b[39m");
	iprintf(ppm_locations[index].description);
}


void initPPMLists() 
{
	chk_offset[0] = 0;
	for(int i=1;i<MAX_CHUNKS;i++)
		chk_offset[i]=-1;
	
	uilist_init(&ppm_list);	
	
	lis_init(&ppm_source);
	lis_set_chunk_procs(&ppm_source, load_ppm_files_chunk, release_ppm_files_chunk);
	lis_select(&ppm_source, 0);
	
	uilist_attach_lis(&ppm_list, &ppm_source);		
	
	uilist_set_write_entry_proc(&ppm_list, ppm_write_entry);
	
	
	uilist_init(&path_selector_list);
	lis_init(&path_selector_source);
	lis_set_chunk_procs(&path_selector_source, load_path_chunk, release_path_chunk);
	lis_select(&path_selector_source, 0);
	
	uilist_attach_lis(&path_selector_list, &path_selector_source);
	
	uilist_set_write_entry_proc(&path_selector_list, path_write_entry);
}













