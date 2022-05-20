#pragma once

// Converts file size to string ( e.g. 2704 => "2.64KB" )
void long_to_size_string(char* dest,long sz);

typedef struct
{
	char name[32];
	long size;
	char size_str[8];
} file_data;

void fsInit();

typedef void (*discovered_file_callback)(file_data*, void* arg);

long get_file_size(const char* path);

long loadFilesFrom(const char* source, int nskip, int max_files, discovered_file_callback callback, void* arg);

void* __open_dir(const char* source, int seek_offset);
file_data* __read_dir(void* dir, long* offset);
void __close_dir(void* dir);