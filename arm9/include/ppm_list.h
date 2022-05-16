#pragma once

#include "ui_list.h"

extern char* ppm_current_path;

char* get_selected_file_name();

extern UiList ppm_list;
extern ListItemsSource ppm_source;

void initPPMLists();