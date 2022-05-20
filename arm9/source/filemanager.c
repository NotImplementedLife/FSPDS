#include "filemanager.h"

#define NITRO

#include <fat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifdef NITRO
#include <filesystem.h>
#endif

#include "vars.h"
#include "console.h"
#include "player.h"
#include "ppm_list.h"

// Converts file size to string ( e.g. 2704 => "2.64KB" )
// dest-> array of length at least 7
void long_to_size_string(char* dest, long sz)
{
	if(sz<0)
	{
		dest[0]='\0';
		return;
	}
    dest[5]='B';
    dest[6]='\0';
    for(int i=0;i<5;i++) dest[i]=' ';
    if(sz==0)
    {
        dest[4]='0';
        return;
    }
    if(sz<1024)
    {
        for(int i=4;sz>0;i--)
        {
            dest[i]=(sz%10)+'0';
            sz/=10;
        }
        return;
    }
    if(sz<1024*1024)
    {
        sz>>=10;
        dest[4]='K';
        for(int i=3;sz>0;i--)
        {
            dest[i]=(sz%10)+'0';
            sz/=10;
        }
        return;
    }
    dest[4]='M';
    sz>>=10;
    int q=sz>>10;
    int r=((sz&0x3FF)*100)>>10;
    if(q>=10)
    {
        iprintf("Fatal error: size limit exceeded. \n");
        exit(-1);
    }
    if(r>=100) r/=10;
    dest[3]=(r%10)+'0';
    r/=10;
    dest[2]=(r%10)+'0';
    dest[1]='.';
    dest[0]=q+'0';
}

void fsInit()
{
	#ifdef NITRO
	if(!nitroFSInit(NULL)) {
		c_displayError("Fatal :\n\nNTFS init failed.",true);
	}	
	#else
	if(!fatInitDefault())
    {
        c_displayError("Fatal :\n\nFAT init failed.",true);
    }
	#endif
}

long get_file_size(const char* path)
{	
	FILE* fp=fopen(path, "rb");
	if(fp==NULL)
	{
		char* msg=malloc(256);
		strcpy(msg, "Failed to open file\n\n");
		strcat(msg, path);
		c_displayError(msg, false);		
		while(1) {swiWaitForVBlank();}		
	}
	long fsize = lseek(fileno(fp), 0, SEEK_END)+1;
	fclose(fp);
	return fsize;
}

void* __open_dir(const char* source, int seek_offset)
{	
	DIR* root = opendir(source);
	if(!root)
	{
		c_displayError("Failed to open directory", true);
	}
	seekdir(root, seek_offset);	
	return root;
}

file_data* __read_dir(void* dir, long* offset)
{
	struct dirent *entry;
	while((entry=readdir(dir))!=NULL)
	{
		*offset = telldir(dir);
        if(entry->d_name[0]=='.') // includes '.' and '..' special paths
            continue;
		if(entry->d_type == DT_REG)
		{				
			if(strlen(entry->d_name)==28 && strcmp(".ppm", entry->d_name+24)==0)
			{					                   					                    											
				file_data* fd = malloc(sizeof(file_data));
				if((u32)fd < 0x00200000) 
				{							
					c_displayError("MALLOC FAILED", true);						
				}					
				strcpy(fd->name, entry->d_name);
				fd->size = -1; // dummy size					
				fd->size_str[0]='\0';												               
				
				return fd;
			}
		}		
	}	
	return NULL;
}

void __close_dir(void* dir)
{
	closedir(dir);	
}

long loadFilesFrom(const char* source, int nskip,  int max_files, discovered_file_callback callback, void* arg)
{	
	if(source==NULL) 
	{		
		return -1;
	}	
    DIR *root;
    struct dirent *entry;
    root=opendir(source);		
    if (root)
    {			
		int count = 0;
		seekdir(root, nskip);
		long result=0;
        while((entry=readdir(root))!=NULL && count<max_files)
        {
			result = telldir(root);
            if(entry->d_name[0]=='.') // includes '.' and '..' special paths
                continue;		
            if(entry->d_type == DT_REG)
            {				
                if(strlen(entry->d_name)==28 && strcmp(".ppm", entry->d_name+24)==0)
                {					                   					                    											
					file_data* fd = malloc(sizeof(file_data));
					if((u32)fd < 0x00200000) 
					{							
						c_displayError("MALLOC FAILED", true);						
					}					
					strcpy(fd->name, entry->d_name);
					fd->size = -1; // dummy size					
					fd->size_str[0]='\0';
					callback(fd, arg);
					count++;						                    
                }
            }
        }			
        closedir(root);
		return result;
    }
    else
    {		
		char* err = malloc(512);
		strcpy(err, "Fatal :\n\nOpen directory failed.\n\n");
		strcat(err, source);
		strcat(err, "\n\nMake sure it exists on your storage device.");
        c_displayError(err, true);
    }
	return -1;
}

