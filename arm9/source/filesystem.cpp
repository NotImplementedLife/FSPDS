#include "filesystem.hpp"

#include <stdio.h>

bool DirStream::open(const char* dir)
{
	if(root!=nullptr) 
		closedir(root);
	return nullptr == (root = opendir(dir));
}

bool DirStream::next(char* dest_name)
{
	if(root==nullptr) return false;	
	struct dirent *entry;
	while((entry=readdir(root))!=NULL)
	{
		if(strcmp(".", entry->d_name)==0 || strcmp("..", entry->d_name)==0)
			continue;
		if(entry->d_type != DT_DIR)
			continue;
		strcpy(dest_name, entry->d_name);
		return true;
	}	
	return false;
}

void DirStream::close()
{
	closedir(root);
	root = nullptr;
}

// https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
bool file_exists(const char *filename) 
{
	struct stat buffer;
	return stat(filename, &buffer)==0;
}

const char* const data_dir = "/data";
const char* const config_dir = "/data/FSPDS";
const char* const locations_path = "/data/FSPDS/locations";

LocationsProvider::LocationsProvider()
{
	if(!file_exists(locations_path))
	{
		DSC::Debug::log("locations file not exists");
		struct stat st = {0};		
		if (stat(data_dir, &st) == -1) mkdir(data_dir, 0777);		
		if (stat(config_dir, &st) == -1) mkdir(config_dir, 0777);				
				
		
		locations.clear();
		FILE* fptr = fopen(locations_path, "wb");		
		int count=0;
		fwrite(&count, sizeof(int), 1, fptr);	
		fclose(fptr);
		DSC::Debug::log("Created locations file");
	}
	else
	{
		//FILE* fptr = fopen();
	}
}


void LocationsProvider::save()
{
	FILE* fptr = fopen(locations_path, "wb");		
	int count=locations.size();
	fwrite(&count, sizeof(int), 1, fptr);	
	
	for(int i=0;i<count;i++)
	{
		int s = strlen(locations[i]->path);
		fwrite(&s, sizeof(int), 1, fptr);			
		fwrite(locations[i]->path, sizeof(char), s, fptr);
		s = locations[i]->filenames.size();
		fwrite(&s, sizeof(int), 1, fptr);
		for(int j=0;j<s;j++)
			fwrite(locations[i]->filenames[j].chars, sizeof(char), 24, fptr);
	}
	
	fclose(fptr);
	
}

void LocationsProvider::add_location(Location* location)
{
	locations.push_back(location);
	save();	
}

Location* LocationsProvider::get_at(int i) const
{
	if(i<0 || i>=locations.size()) return nullptr;
	return locations[i];
}

Location* LocationsProvider::get_by_path(const char* path) const
{
	for(int i=0;i<locations.size();i++)	
		if(strcmp(locations[i]->path, path)==0)
			return locations[i];
	return nullptr;
}

LocationsProvider::~LocationsProvider()
{	
	for(int i=0;i<locations.size();i++)	
		delete locations[i];	
}

int LocationsProvider::get_count() const { return locations.size(); }