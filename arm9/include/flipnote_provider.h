#pragma once

#include "filemanager.h"

void provider_init(const char* flipnotes_path);

file_data* provider_get_nth_flipnote(int n);

char* provider_get_flipnote_full_path(file_data* fd);

void provider_background();