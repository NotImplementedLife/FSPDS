#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAXFILESCOUNT 4096

char* files[MAXFILESCOUNT];

int main(int argc, char **argv)
{
	consoleDemoInit();
    if(!fatInitDefault())
    {
        iprintf("fatInitDefault failure: terminating\n");
        return 0;
    }
    DIR *root;
    struct dirent *entry;
    root=opendir("/flipnotes");
    if (root)
    {
        while (entry=readdir(root))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            if(entry->d_type != DT_DIR)
            {
                if(strlen(entry->d_name)==28 && strcmp(".ppm",entry->d_name+24)==0)
                    iprintf("> %s\n", entry->d_name);
            }
        }
        closedir(root);
    } else {
        iprintf ("opendir() failure; terminating\n");
    }

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysDown()&KEY_START) break;
	}

	return 0;
}
