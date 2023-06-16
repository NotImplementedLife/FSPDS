#include "filesystem.hpp"

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