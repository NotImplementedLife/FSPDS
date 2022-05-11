#include "ui_list.h"

#include <stdlib.h>

void lis_init(ListItemsSource* lis)
{
	lis->previous = NULL;
	lis->current = NULL;
	lis->next = NULL;
	lis->current_chunk_id = -100;
	lis->load_chunk = 0;
	lis->items_count = 0;
	lis->selected_index=-1;	
}

void lis_destroy(ListItemsSource* lis)
{
	free(lis->previous);
	free(lis->current);
	free(lis->next);	
}

void lis_set_chunk_procs(ListItemsSource* lis, load_chunk_proc ld_chk_proc, release_chunk_proc rl_chk_proc)
{
	lis->load_chunk = ld_chk_proc;
	lis->release_chunk = rl_chk_proc;
}

ItemsChunk* lis_load_chunk(ListItemsSource* lis, int chunk_id)
{
	if(chunk_id < 0) 
		return NULL;
	return lis->load_chunk(chunk_id);
}

void lis_set_current_chunk(ListItemsSource* lis, int chunk_id)
{	
	if(chunk_id == lis->current_chunk_id) 
	{		
		return;	
	}
	
	if(chunk_id == lis->current_chunk_id + 1) {
		lis->release_chunk(lis->previous);
		lis->previous = lis->current;
		lis->current = lis->next;
		lis->next = lis_load_chunk(lis, chunk_id+1);
		lis->current_chunk_id = chunk_id;
		return;
	}
	if(chunk_id == lis->current_chunk_id - 1) {
		lis->release_chunk(lis->next);
		lis->next = lis->current;
		lis->current = lis->previous;
		lis->previous = lis_load_chunk(lis, chunk_id-1);
		lis->current_chunk_id = chunk_id;
		return;
	}
	
	lis->release_chunk(lis->previous);
	lis->release_chunk(lis->current);
	lis->release_chunk(lis->next);
		
	lis->previous = lis_load_chunk(lis, chunk_id-1);
	lis->current = lis_load_chunk(lis, chunk_id);
	lis->next = lis_load_chunk(lis, chunk_id+1);
	
	lis->current_chunk_id = chunk_id;
}

void* lis_get_item(ListItemsSource* lis, int id)
{
	if(lis==NULL) return NULL;
	if(id<0) return NULL;
	//iprintf("id=%i %i\n",id, id>>CHUNK_MAGNITUDE);
	lis_set_current_chunk(lis, id>>CHUNK_MAGNITUDE);	
	return (*lis->current)[id & (CHUNK_SIZE-1)];
}

void lis_set_count(ListItemsSource* lis, int items_count)
{
	lis->items_count = items_count;
}

void lis_select(ListItemsSource* lis, int index)
{
	lis->selected_index = index;
	lis_get_item(lis, index); // update chunks to fit selected item
}

int lis_item_is_selected(ListItemsSource* lis, int index)
{
	return lis->selected_index == index;
}


void uilist_init(UiList* ul)
{
	ul->top_visible = 0;
	ul->lis = NULL;
}

void uilist_set_write_entry_proc(UiList* ul, write_entry_proc wr_ent_proc)
{
	ul->write_entry = wr_ent_proc;
}

void uilist_attach_lis(UiList* ul, ListItemsSource* lis)
{
	ul->lis = lis;
}

void uilist_write_page(UiList* ul)
{
	ul->top_visible=7*(ul->lis->selected_index/7);
	for(int i=0;i<8;i++)
    {
		int pos = ul->top_visible + i;
		ul->write_entry(lis_get_item(ul->lis, pos), i, lis_item_is_selected(ul->lis, pos));
		//ul->write_entry();
        //writeEntry(7*CurrentPage+i,i,i==PageSelection);
    }
}



