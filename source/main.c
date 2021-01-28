#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAXFILESCOUNT 1024

int filescount=0;
char files[MAXFILESCOUNT][29];
char sizes[MAXFILESCOUNT][7];

void long_to_size_string(char dest[7],long sz)
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
        iprintf("Error: size limit exceeded. \n");
        exit(-1);
    }
    if(r>=100) r/=10;
    dest[3]=(r%10)+'0';
    r/=10;
    dest[2]=(r%10)+'0';
    dest[1]='.';
    dest[0]=q+'0';
}

void loadFiles()
{
    if(!fatInitDefault())
    {
        iprintf("Fatal error: FAT init failed.\n");
        exit(-1);
    }
    DIR *root;
    struct dirent *entry;
    root=opendir("/flipnotes");
    if (root)
    {
        while((entry=readdir(root))!=NULL && filescount<MAXFILESCOUNT)
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            if(entry->d_type != DT_DIR)
            {
                if(strlen(entry->d_name)==28 && strcmp(".ppm",entry->d_name+24)==0)
                {
                    strcpy(files[filescount],entry->d_name);
                    char fn[40]="/flipnotes/";
                    strcat(fn,entry->d_name);
                    FILE* fp=fopen(fn,"rb");
                    fseek(fp,0L,SEEK_END);
                    long_to_size_string(sizes[filescount],ftell(fp));
                    fclose(fp);
                    filescount++;
                }
            }
        }
        closedir(root);
    }
    else
    {
        iprintf ("Fatal error: Open directory failed.\n");
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();
    iprintf("Loading...\n");
    loadFiles();
    for(int i=0;i<filescount;i++)
    {
        iprintf("  %s\n\t\t\t\t\t\t\t%s\n",files[i],sizes[i]);
    }

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysDown()&KEY_START) break;
	}

	return 0;
}

