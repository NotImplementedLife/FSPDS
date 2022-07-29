#include "dialog.h"

#include <nds.h>
#include "console.h"
#include "tabsystem.h"
#include <string.h>

char dialog_content[1024];
int dialog_options[16];
int options_count = 0;
int current_option = 0;

void select_option(int option_id)
{
	int prev_x = dialog_options[current_option] & 0xFF;
	int prev_y = (dialog_options[current_option]>>8) & 0xFF;
	int crt_x = dialog_options[option_id] & 0xFF;
	int crt_y = (dialog_options[option_id]>>8) & 0xFF;
	current_option=option_id;
	iprintf("\033[%d;%dH \033[%d;%dH\x10", prev_y, prev_x, crt_y, crt_x);
}

int show_dialog(int x, int y, int w, int h, const char* title, const char* content)
{
	int k=0;
	int py=0;
	int px=0;
	int len=strlen(content);
	options_count = 0;
	for(int i=0;i<len;i++)
	{
		if(content[i]=='\n')
		{
			for(;px<w;px++)
			{
				dialog_content[k++]=' ';
			}
			px=0;
			py++;
		}
		else if(content[i]=='*')
		{
			dialog_options[options_count++]=(py<<8)|px;
			dialog_content[k++]=' ';
			px++;
			if(px==w)
			{
				px=0, py++;
			}
		}
		else
		{
			dialog_content[k++]=content[i];
			px++;
			if(px==w)
			{
				px=0, py++;
			}
		}
	}
	dialog_content[k++]='\0';
	
	consoleSelect(&consoleFG);
	iprintf("\x1b[39m");
	c_drawBorder(y-1, x-1, y+h, x+w);
	
	consoleSetWindow(&consoleBG, x-1, y-1, w+2, h+2);
	consoleSetWindow(&consoleFG, x, y, w, h);	
	
	consoleSelect(&consoleBG);
	consoleClear();
	consoleSelect(&consoleFG);	
	iprintf("\x1b[39m");
	consoleClear();
	iprintf(dialog_content);
	if(options_count>0)
		select_option(0);

	int result = -1;
	while(1)
	{
		swiWaitForVBlank();
		scanKeys();
		int keys = keysDown();
		if(keys & KEY_B) break;
		if(keys & KEY_DOWN)
		{
			if(current_option<options_count-1)
			{
				select_option(current_option+1);
			}			
		}
		else if(keys & KEY_UP)
		{
			if(current_option>0)
			{
				select_option(current_option-1);
			}
		}
		else if(keys & KEY_A)
		{
			result=current_option;
			break;
		}
	}
		
	consoleSetWindow(&consoleBG, 0, 0, 32, 24);
	consoleSetWindow(&consoleFG, 0, 0, 32, 24);
	CurrentTab->drawingProc();
	return result;
}








