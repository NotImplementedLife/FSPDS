#pragma once

#include <fat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "DSC"

class DirStream
{
private:
	DIR* root = nullptr;
public:
	bool open(const char* dir);
	bool next(char* dest_name);
	void close();
};

struct Char24
{
	char chars[24];
};

class Location
{
public:
	char* path = nullptr;
	DSC::Vector<Char24> filenames;
	~Location() = default;
};

class LocationsProvider
{
private:
	DSC::Vector<Location*> locations;	
	
	void load();
	void save();
public:
	static Location* peek_location(int i);

	int get_count() const;
	
	Location* get_by_path(const char* path) const;
	Location* get_at(int i) const;
	
	void add_location(Location* location);
	
	Location* detach_location(int index);
	
	LocationsProvider();
	~LocationsProvider();
	
	
};