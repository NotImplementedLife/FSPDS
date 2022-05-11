#include "filesystem.h"

#include <fat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <filesystem.h>

#include "vars.h"
#include "console.h"
#include "player.h"

// Converts file size to string ( e.g. 2704 => "2.64KB" )
void long_to_size_string(char dest[7], long sz)
{
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
	if(!nitroFSInit(NULL)) {
		c_displayError("Fatal :\n\nNTFS init failed.",true);
	}
	return;
	if(!fatInitDefault())
    {
        c_displayError("Fatal :\n\nFAT init failed.",true);
    }	
}

void loadFiles()
{
	loadFilesFrom("/flipnotes", 0, 0, 0, 0);
}

long loadFilesFrom(const char* source, int nskip,  int max_files, discovered_file_callback callback, void* arg)
{
    DIR *root;
    struct dirent *entry;
    root=opendir(source);		
    if (root)
    {
		int ppm_offset = strlen(source);
		char* fn = malloc(ppm_offset+32);
		strcpy(fn, source);
		strcat(fn, "/");
		ppm_offset++;
		
		int count = 0;
		seekdir(root, nskip);
		long result=0;
        while((entry=readdir(root))!=NULL && count<max_files)
        {
			result = telldir(root);
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;		
            if(entry->d_type != DT_DIR)
            {				
                if(strlen(entry->d_name)==28 && strcmp(".ppm", entry->d_name+24)==0)
                {					                   					
                    strcpy(fn + ppm_offset, entry->d_name);
					//iprintf("%s\n", entry->d_name);
                    //FILE* fp=fopen(fn, "rb");
                    //fseek(fp, 0L, SEEK_END);
                    //long fsize=ftell(fp);					
					//fclose(fp);
					long fsize=1;
					//iprintf("%i\n", fsize);
                    /// Accept only files under 1MB
                    if(fsize<=1024*1024)
                    {					
						file_data* fd = malloc(sizeof(file_data));				
                        strcpy(fd->name, entry->d_name);
						fd->size = fsize;
						long_to_size_string(fd->size_str, fsize);
						callback(fd, arg);
                        count++;						
                    }
                }
            }
        }		
        closedir(root);
		return result;
    }
    else
    {		
        c_displayError("Fatal :\n\nOpen directory failed.\n\nMake sure the /flipnotes\ndirectory exists on the rootof your SD card.", true);
    }
	return -1;
}

// UI

void writeEntry(int i,int listpos, bool highlight)
{
    if(i>=filescount)
    {
        consoleSelect(&consoleFG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }
        consoleSelect(&consoleBG);
        for(int i=0;i<3-2*(listpos==7);i++)
        {
            c_goto(1+3*listpos+i,1);
            for(int j=1;j<31;j++) iprintf(" ");
        }
        return;
    }
    consoleSelect(&consoleBG);
    iprintf(highlight?"\x1b[39m":"\x1b[30m");
    for(int i=0;i<3;i++)
    {
        c_goto(1+3*listpos+i,1);
        for(int j=0;j<30;j++) iprintf("\x02");
    }
    consoleSelect(&consoleFG);
    c_goto(1+3*listpos,2);
    iprintf(highlight?"\x1b[30m":"\x1b[39m");
    iprintf(files[i]);
    if(listpos==7) return;
    c_goto(2+3*listpos,20);
    iprintf(sizes[i]);
}

void writePage()
{
    for(int i=0;i<8;i++)
    {
        writeEntry(7*CurrentPage+i,i,i==PageSelection);
    }
}

void nextPage()
{
    if(CurrentPage+1>=PagesCount)
        return;
    CurrentPage++;
    PageSelection=0;
    displayThumbnail();
    writePage();
}

void prevPage()
{
    if(CurrentPage==0)
        return;
    CurrentPage--;
    PageSelection=0;
    displayThumbnail();
    writePage();
}

void nextEntry()
{
    int id=7*CurrentPage+PageSelection;
    if(id+1==filescount)
        return;
    if(PageSelection==6)
    {
        CurrentPage++;
        PageSelection=0;
        displayThumbnail();
        writePage();
    }
    else
    {
        writeEntry(id++,PageSelection++,false);
        writeEntry(id  ,PageSelection  ,true);
        displayThumbnail();
    }
}

void prevEntry()
{
    int id=7*CurrentPage+PageSelection;
    if(id==0)
        return;
    if(PageSelection==0)
    {
        CurrentPage--;
        PageSelection=6;
        displayThumbnail();
        writePage();
    }
    else
    {
        writeEntry(id--,PageSelection--,false);
        writeEntry(id  ,PageSelection  ,true);
        displayThumbnail();
    }
}
