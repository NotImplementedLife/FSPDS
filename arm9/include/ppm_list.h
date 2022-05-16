#pragma once

#include "ui_list.h"
#include "filesystem.h"

extern char* ppm_current_path;

char* get_selected_file_name();
int get_selected_file_index();
file_data* get_selected_file_data();

extern UiList ppm_list;
extern ListItemsSource ppm_source;

void initPPMLists();