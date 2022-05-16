#pragma once

#define CHUNK_MAGNITUDE 4
#define CHUNK_SIZE (1<<CHUNK_MAGNITUDE)

typedef void* ItemsChunk[CHUNK_SIZE];

typedef ItemsChunk* (*load_chunk_proc)(int chunk_id);
typedef void (*release_chunk_proc)(ItemsChunk*);

typedef struct 
{	
	ItemsChunk* previous;
	ItemsChunk* current;
	ItemsChunk* next;
	ItemsChunk* next2;
	int current_chunk_id;
	load_chunk_proc load_chunk;
	release_chunk_proc release_chunk;
	int items_count; 
	int selected_index;
} ListItemsSource;

void lis_init(ListItemsSource* lis);

void lis_destroy(ListItemsSource* lis);

void lis_set_items_count(ListItemsSource* lis, int items_count);

void lis_set_chunk_procs(ListItemsSource* lis, load_chunk_proc ld_chk_proc, release_chunk_proc rl_chk_proc);

void* lis_get_item(ListItemsSource* lis, int id);

void* lis_get_selected_item(ListItemsSource* lis);

void lis_set_count(ListItemsSource* lis, int items_count);

void lis_select(ListItemsSource* lis, int index);

int lis_item_is_selected(ListItemsSource* lis, int index);


typedef void (*write_entry_proc)(void* item,int listpos, int is_highlighted);

typedef struct 
{
	int top_visible;
	write_entry_proc write_entry;
	ListItemsSource* lis;
		
} UiList;

void uilist_init(UiList* ul);

void uilist_set_write_entry_proc(UiList* ul, write_entry_proc wr_ent_proc);

void uilist_attach_lis(UiList* ul, ListItemsSource* lis);

void uilist_write_page(UiList* ul);

void write_page(UiList* ul);