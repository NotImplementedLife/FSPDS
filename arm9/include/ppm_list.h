#pragma once

#include "ui_list.h"
#include "filemanager.h"

typedef struct
{
	const char* path;
	const char* description;
} ppm_location;

extern UiList path_selector_list;
extern ListItemsSource path_selector_source;

extern const ppm_location ppm_locations[];
extern const int ppm_locations_length;

extern const char* ppm_current_path;

void set_current_path(const char* path);

char* get_selected_file_name();
int get_selected_file_index();
file_data* get_selected_file_data();

extern UiList ppm_list;
extern ListItemsSource ppm_source;

void initPPMLists();