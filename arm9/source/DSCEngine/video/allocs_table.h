#pragma once

extern const int allocs_count;
extern /*__attribute__((section(".iwram.alloc_buffer")))*/ int allocs_table_stamps[];
extern /*__attribute__((section(".iwram.alloc_buffer")))*/ unsigned char allocs_table_ids[];

extern bool allocs_table_inited;

void init_allocs_table();

bool add_to_allocs_table(int id, int stamp);

int remove_from_allocs_table(int id, int offset);

void remove_allocs_by_id(int id);

int* allocs_last_stamp();

unsigned char* allocs_last_id();

