#pragma once

// Converts file size to string ( e.g. 2704 => "2.64KB" )
void long_to_size_string(char dest[7],long sz);

void loadFiles();

// UI

void writeEntry(int i,int listpos, bool highlight);

void writePage();

void nextPage();

void prevPage();

void nextEntry();

void prevEntry();
