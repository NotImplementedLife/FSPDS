#pragma once

#include <fat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

class DirStream
{
private:
	DIR* root = nullptr;
public:
	bool open(const char* dir);
	bool next(char* dest_name);
	void close();
};