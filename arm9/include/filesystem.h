#pragma once

// Converts file size to string ( e.g. 2704 => "2.64KB" )
void long_to_size_string(char dest[7],long sz);

typedef struct
{
	char name[32];
	long size;
	char size_str[8];
} file_data;

void fsInit();

typedef void (*discovered_file_callback)(file_data*, void* arg);

long loadFilesFrom(const char* source, int nskip, int max_files, discovered_file_callback callback, void* arg);

void loadFiles();