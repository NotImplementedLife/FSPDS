#include "allocs_table.h"
#include "DSCEngine/debug/log.hpp"

constexpr int allocs_count = 4096;
/*__attribute__((section(".iwram.alloc_buffer")))*/ int allocs_table_stamps[allocs_count];
/*__attribute__((section(".iwram.alloc_buffer")))*/ unsigned char allocs_table_ids[allocs_count];

int allocs_table_free_head;
static constexpr int FREE_POS = 0x80000000;
static constexpr int FREE_END = ~FREE_POS;

bool allocs_table_inited = false;

int last_added_pos;

void init_allocs_table()
{
	for(int i=0;i<allocs_count-1;i++)
	{		
		allocs_table_stamps[i] = FREE_POS | (i+1);
	}	
	allocs_table_stamps[allocs_count-1] = FREE_END;
	allocs_table_free_head = 0;
	last_added_pos = 0;
	for(int i=0;i<allocs_count;i++)
		allocs_table_ids[i]=0;
	allocs_table_inited = true;
}

int* allocs_last_stamp()
{
	return &allocs_table_stamps[last_added_pos];
}

unsigned char* allocs_last_id()
{
	return &allocs_table_ids[last_added_pos];
}

bool add_to_allocs_table(int id, int stamp)
{	
	if(allocs_table_free_head==FREE_END)
	{
		// Alloc slots full
		return false;
	}	
	
	int tmp = allocs_table_stamps[allocs_table_free_head] & ~FREE_POS;
	last_added_pos = allocs_table_free_head;	
	allocs_table_free_head = tmp;	
	
	allocs_table_stamps[last_added_pos] = stamp;
	allocs_table_ids[last_added_pos] = id;			
	
	return true;
}

int remove_from_allocs_table(int id, int offset)
{
	offset>>=5;
	for(int i=0;i<allocs_count;i++)
	{				
		if(allocs_table_stamps[i] & FREE_POS) continue;
		
		if(((allocs_table_stamps[i]>>16)&0xFFFF)==offset && allocs_table_ids[i]==id)
		{						
			//printf("found %08X\n", allocs_table_stamps[i]);
			int size = allocs_table_stamps[i] & 0x0000FFFF;
						
			allocs_table_stamps[i] = allocs_table_free_head | FREE_POS;
			allocs_table_free_head = i;
			
			return size<<5;
		}
	}		
	return 0;
}

void remove_allocs_by_id(int id)
{
	for(int i=0;i<allocs_count;i++)
	{
		if(allocs_table_stamps[i] | FREE_POS) continue;
		if(allocs_table_ids[i]==id)
		{							
			allocs_table_stamps[i] = allocs_table_free_head | FREE_POS;
			allocs_table_free_head = i;
		}
	}
}