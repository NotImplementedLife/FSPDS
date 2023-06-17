#include <nds.h>
#include "scenes.hpp"
#include "simple_scene.hpp"
#include "folder_icon.h"
#include "filesystem.hpp"
#include "opt_box.h"
#include "globals.hpp"

class FolderPickerScene : public SimpleScene
{
	const char* flipnote_locations[4] = { "/flipnotes", "/private/ds/app/4b475556", "/private/ds/app/4B475545", "/private/ds/app/4B47554A" };
	
	ObjFrame* folder_normal_frame;
	ObjFrame* folder_highlighted_frame;
	ObjFrame* folder_back_frame;
	
	DirStream dirstream;
	
	Sprite* folder_icons[5];	
	Vector<char*> subfolders;	
	
	char* current_path;
	int depth=0;
	int selected_index=0;
	int max_subfolders = 65536;
	
	Sprite* pick_box;
	
	Stack<int> selected_indices;
	
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	void init() override
	{
		Debug::log("Initing");
		SimpleScene::init();
		Debug::log("Requiring");
		require_tiledmap_4bpp(MAIN_BG2, 256, 256, 32*24);
		require_tiledmap_4bpp(SUB_BG2, 256, 256, 32*24);
		
		begin_sprites_init();		
		
		Debug::log("Framing");
		folder_normal_frame = new ObjFrame(&ROA_folder_icon8,0,0);
		folder_highlighted_frame = new ObjFrame(&ROA_folder_icon8,0,1);
		folder_back_frame = new ObjFrame(&ROA_folder_icon8,0,2);
		
		Debug::log("Allocating");
		Debug::log("    normal frame");
		get_obj_allocator_sub()->allocate(folder_normal_frame);
		Debug::log("    highlighted frame");
		get_obj_allocator_sub()->allocate(folder_highlighted_frame);
		Debug::log("    back frame");
		get_obj_allocator_sub()->allocate(folder_back_frame);
		
		Debug::log("creating sprites");
		for(int i=0;i<5;i++)
		{
			folder_icons[i] = create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
			folder_icons[i]->set_default_allocator(nullptr);
			folder_icons[i]->add_frame(i%2==0?folder_normal_frame : folder_highlighted_frame);
			folder_icons[i]->set_position(8,8+32*i);			
		}
		
		pick_box = create_sprite(new Sprite(SIZE_64x32, Engine::Sub));
		pick_box->add_frame(new ObjFrame(&ROA_opt_box8,0,0));
		pick_box->set_position(188, 168);
		pick_box->set_priority(3);
		
		end_sprites_init();		
		key_down.add_event(&FolderPickerScene::on_key_down, this);
		
		current_path = (char*)malloc(32768);		
		
		if(current_path==nullptr)
		{
			Debug::log("ALLOCATION FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! _ CURRENT PATH");
		}	
		strcpy(current_path, "/");		
		
		dirstream.open(current_path);
		clear_subfolders();						
	}
		
	void on_key_down(void*, void* _k)
	{
		int keys = (int)_k;
		if(keys & KEY_LEFT)
		{
			if(selected_index>=5)
			{
				selected_index-=5;
				display_page(selected_index/5);
			}
		}
		else if(keys & KEY_RIGHT)
		{
			if(selected_index<max_subfolders-5)
			{				
				selected_index+=5;
				display_page(selected_index/5);
			}
		}
		else if(keys & KEY_UP)
		{
			if(selected_index>=1)
			{
				selected_index--;
				display_page(selected_index/5);
			}
		}
		else if(keys & KEY_DOWN)
		{
			if(selected_index<max_subfolders-1)
			{				
				selected_index++;
				display_page(selected_index/5);
			}			
		}
		else if(keys & KEY_A)
		{
			enter_selected();
		}
		else if(keys & KEY_B)
		{
			go_back();
		}
		else if(keys & KEY_SELECT)
		{
			done();
		}		
		else if(keys & KEY_TOUCH)
		{
			touchRead(&touch);
			if(touch_in_rect(188, 168, 64, 32)) 
				done();
		}
	}	

	void done()
	{
		picked_folder_path = new char[strlen(current_path)+1];
		strcpy(picked_folder_path, current_path);
		close()->next(folder_picker_next_scene_gen());
	}
	
	touchPosition touch;
	
	bool touch_in_rect(int x, int y, int w, int h)
	{
		if(touch.px<x || touch.px>=x+w) return false;
		if(touch.py<y || touch.py>=y+h) return false;
		return true;
	}
	
	__attribute__((noinline))
	void run() override
	{
		solve_map_requirements();
		load_assets();
		
		bgSetPriority(7,3);
		bgSetPriority(6,2);
		
		BG_PALETTE[0x91]=BG_PALETTE_SUB[0x91]=Colors::Black;
		BG_PALETTE[0x92]=BG_PALETTE_SUB[0x92]=Colors::Blue;
		
		vwf->set_render_space(bgGetGfxPtr(2),24,32);
		VwfEngine::prepare_map(*vwf, MAIN_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
						
		vwf->set_cursor(6, 74);
		vwf->put_text("Pick a directory", Pal4bit, SolidColorBrush(0x1));		
		
		vwf->set_cursor(10, 8);
		vwf->put_text("Enter in a directory of your choice, then", Pal4bit, SolidColorBrush(0x1));
		vwf->set_cursor(11, 8);
		vwf->put_text("touch the Select button.", Pal4bit, SolidColorBrush(0x1));
		
			
		vwf->set_render_space(bgGetGfxPtr(6),24,32);
		VwfEngine::prepare_map(*vwf, SUB_BG2, 32, 0, 0, 0x9);
		vwf->clear(Pal4bit);			
		
		vwf->set_cursor(10, 20);	
		vwf->put_text("Touch the screen to return to title", Pal4bit, SolidColorBrush(0x1));
					
		Hardware::MainEngine::objEnable(128, true); // set to 128		
		Hardware::SubEngine::objEnable(128, true); // set to 128	
		
		display_page(0);
		
		Scene::run();	
	}
	
	void frame() override
	{		
		GenericScene256::frame();
	}	
	
	bool is_default_location(const char* name)
	{
		char* tmp = (char*)malloc(strlen(current_path) + strlen(name)+1);
		strcpy(tmp, current_path);
		strcat(tmp, name);
		for(int i=0;i<4;i++)
		{
			if(strcmp(tmp, flipnote_locations[i])==0)
			{
				free(tmp);
				return true;
			}
		}
		free(tmp);
		return false;
	}
	
	void display_page(int page)
	{		
		int last_id = page*5+4;
		
		char* name = new char[256];
		while(last_id+1>=subfolders.size() && dirstream.next(name))
		{
			char* dirname = (char*)malloc(strlen(name)+1);			
			if(dirname==nullptr)
			{
				Debug::log("ALLOCATION FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! _ DIRRRR");
			}
			strcpy(dirname, name);
			subfolders.push_back(dirname);
			Debug::log(dirname);
		}
		delete[] name;
		
		if(last_id+1>=subfolders.size())
		{
			max_subfolders = subfolders.size();
		}
		
		vwf->clear(Pal4bit);
	
		int k=page*5;
		int i=0;
		for(;i<5 && k<subfolders.size();i++, k++)
		{
			folder_icons[i]->show();
			if(k==0 && depth>0)				
			{
				folder_icons[i]->set_frame(0, folder_back_frame);				
			}			
			else
			{
				folder_icons[i]->set_frame(0, is_default_location(subfolders[k]) ? folder_highlighted_frame : folder_normal_frame);
			}
			Debug::log(subfolders[k]);
			
			vwf->set_cursor(1+2*i, 42);
			vwf->put_text(subfolders[k], Pal4bit, SolidColorBrush(0x1 + (k==selected_index)));
		}		
		for(;i<5;i++)
		{
			folder_icons[i]->hide();
		}		
		
		vwf->set_cursor(11, 200);
		vwf->put_text("Select", Pal4bit, SolidColorBrush(0x1));
	}
	
	static char* sptr(const char* val)
	{
		char* r = (char*)malloc(strlen(val)+1);
		strcpy(r, val);
		return r;
	}
	
	void enter_selected()
	{		
		if(depth>0 && selected_index==0)
		{
			depth--;
			char* c = current_path + strlen(current_path)-1;
			while(*(--c)!='/');				
			*(++c)='\0';			
			Debug::log("After back: %s", current_path);
			selected_index = selected_indices.pop();
		}		
		else
		{			
			strcat(current_path, subfolders[selected_index]);
			strcat(current_path, "/");
			depth++;
			Debug::log("nav: %s", current_path);
			selected_indices.push(selected_index);
			selected_index=0;
		}
		dirstream.open(current_path);
		clear_subfolders();	
		display_page(selected_index/5);		
	}
	
	void go_back()
	{
		if(depth==0) return;				
		depth--;
		char* c = current_path + strlen(current_path)-1;
		while(*(--c)!='/');				
		*(++c)='\0';			
		Debug::log("After back: %s", current_path);
		selected_index = selected_indices.pop();	
		dirstream.open(current_path);
		clear_subfolders();				
		display_page(selected_index/5);
	}
	
	void clear_subfolders()
	{
		for(int i=0;i<subfolders.size();i++)
			free(subfolders[i]);
		subfolders.clear();
		if(depth>0)
			subfolders.push_back(sptr("Back"));
		max_subfolders=65536;
	}	
	
	~FolderPickerScene()
	{
		key_down.remove_event(&FolderPickerScene::on_key_down, this);
		delete vwf;
		clear_subfolders();		
		
		dirstream.close();
		
		for(int i=0;i<5;i++)
		{
			folder_icons[i]->set_frame(0, nullptr);
			delete folder_icons[i];
		}
		delete pick_box;
		
		delete folder_normal_frame;
		delete folder_highlighted_frame;
		delete folder_back_frame;		
		
		free(current_path);
	}
	
};

Scene* get_folder_picker_scene()
{
	return new FolderPickerScene();
}