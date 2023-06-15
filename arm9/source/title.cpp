#include <nds.h>
#include <fat.h>

#include "logo_front_bin.h"
#include "logo_back_bin.h"
#include "logo_side_bin.h"
#include "logo_tb_bin.h"
#include "esodev_logo.h"
#include "background.h"
#include "title_spr.h"
#include "opt_box.h"
#include "fat_fail.h"

#include "scenes.hpp"

extern "C" char fake_heap_start[];
extern "C" char fake_heap_end[];


static char BK_RESERVED[1024*512];

using namespace DSC;

class TitleScene : public GenericScene256
{	
	int textureID[4];
	int i;
	float rotateX = 0.0;
	float rotateY = 0.0;	
		
	Sprite* title[2];			
	Sprite* fat_fail;
	
	void* ______ = BK_RESERVED;
	
	Sprite* esodev_logo;
	VwfEngine* vwf = new VwfEngine(Resources::Fonts::default_8x16);
	
	void init() override
	{								
		int* ptt = (int*)fake_heap_start;
		Debug::warn("Heap start = %X", *ptt);
		ptt = (int*)fake_heap_end;
		Debug::log("Heap end = %X", *ptt);
	
		GenericScene256::init();		
		
		key_down.add_event(&TitleScene::on_key_down, this);
		key_held.add_event(&TitleScene::on_key_held, this);								
		
		init3d();
				
		require_tiledmap(SUB_BG2, 256, 256, &ROA_background4);
		require_tiledmap_4bpp(SUB_BG0, 256, 256, 32*24);		
		
		title[0] = create_sprite(new Sprite(SIZE_64x64, Engine::Main));
		title[1] = create_sprite(new Sprite(SIZE_64x64, Engine::Main));	
		
		
		begin_sprites_init();		
		
		title[0]->add_frame(new ObjFrame(&ROA_title_spr8, 0,0));
		title[1]->add_frame(new ObjFrame(&ROA_title_spr8, 0,1));			
		
		title[0]->set_position(128-64, 116);
		title[1]->set_position(128, 116);
		
		if(!fatInitDefault())
		{
			fat_fail = create_sprite(new Sprite(SIZE_64x32, Engine::Sub));
			fat_fail->add_frame(new ObjFrame(&ROA_fat_fail8, 0,0));
			fat_fail->set_position(14,112);
		}			
		
		esodev_logo =  create_sprite(new Sprite(SIZE_32x32, Engine::Sub));
		esodev_logo->add_frame(new ObjFrame(&ROA_esodev_logo8, 0,0));
		esodev_logo->set_position(15,149);
		
		end_sprites_init();
	}
	
	bool sinit=false;
	
	void frame() override
	{
		if(!sinit)
		{			
			sinit=true;			
			BG_PALETTE_SUB[0x91]=Colors::Red;							
			
			Debug::log("MAPPPPP = %X", bgGetGfxPtr(4));
			Debug::log("MAPPPPP = %X", bgGetMapPtr(4));
			vwf->set_render_space(bgGetGfxPtr(4),24,32);
			VwfEngine::prepare_map(*vwf, 4, 32, 0, 0, 0x9);
			vwf->clear(Pal4bit);			
						
			
			vwf->set_cursor(1, 150);
			vwf->put_text("Browse", Pal4bit, SolidColorBrush(0x1));
			
			vwf->set_cursor(3, 150);
			vwf->put_text("Settings", Pal4bit, SolidColorBrush(0x1));
			
			vwf->set_cursor(5, 150);
			vwf->put_text("Help", Pal4bit, SolidColorBrush(0x1));
			
			vwf->set_cursor(7, 150);
			vwf->put_text("Credits", Pal4bit, SolidColorBrush(0x1));
			
			
		}		
		
		frame3d();
		
		GenericScene256::frame();
	}	
	
	typedef Scene*(*fgen)();
	
	inline static constexpr fgen scenegens[4] = { &get_player_scene, &gen_main_scene, &gen_main_scene, &gen_main_scene  };
	
	void on_key_down(void* sender, void* args)
	{
		int keys = (int)args;

		if(keys & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);
			if(touch.px<137) return;
			
			int y=touch.py-8;
			if(y<0 || y%32>28) return;
			int option = y/32;
			if(option<0 || option>=4) return;						
			close()->next(scenegens[option]());
			
		}		
	}
	
	void on_key_held(void* sender, void* args)
	{
		int keys = (int)args;		
		if((keys & KEY_UP))
		{
			if(rotateX<30)
				rotateX += 2;
		}
		if((keys & KEY_DOWN)) 
		{
			if(rotateX>-30)
				rotateX -= 2; 
		}
	}	
	
	void init3d()
	{
		//set mode 0, enable BG0 and set it to 3D
		videoSetMode(MODE_0_3D);

		// initialize gl
		glInit();		
		
		glResetMatrixStack();
		glResetTextures();
		
		//enable textures
		glEnable(GL_TEXTURE_2D);
		
		//this should work the same as the normal gl call
		glViewport(0,0,255,191);
		
		// enable antialiasing
		glEnable(GL_ANTIALIAS);
		
		// setup the rear plane
		glClearColor(30,27,25,31); // BG must be opaque for AA to work
		glClearPolyID(63); // BG must have a unique polygon ID for AA to work
		glClearDepth(0x7FFF);

		vramSetBankB(VRAM_B_LCD);
		REG_DISPCAPCNT = 
				DCAP_MODE(DCAP_MODE_BLEND) //blend source A and source B
			//|	DCAP_SRC_ADDR //this is not used since we are setting the display to render from VRAM
			|	DCAP_SRC_B(DCAP_SRC_B_VRAM)
			|	DCAP_SRC_A(DCAP_SRC_A_3DONLY)
			|	DCAP_SIZE(DCAP_SIZE_256x192)
			|	DCAP_OFFSET(0) //where to write the captured data within our chosen VRAM bank
			|	DCAP_BANK(DCAP_BANK_VRAM_B)
			|	DCAP_B(8) //blend mostly from B to make a very dramatic effect
			|	DCAP_A(8) //and blend only a little bit from the new scene
			;
		//but, dramatic effects tend to leave some garbage on the screen since the precision of the math is low,
		//and we're not putting a lot of dampening on the effect.
		//a more realistic value might be 8 and 8, but perhaps in a more complex 3d scene the garbage isn't such a bad thing
		//since the scene is changing constantly

		//DisplayEnableMotionBlur();		
		DisplayEnableNormal();


		vramSetBankA(VRAM_A_TEXTURE);

		glGenTextures(4, textureID);
		
		glBindTexture(0, textureID[0]);
		glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_32 , TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD, (u8*)logo_front_bin);
		glBindTexture(1, textureID[1]);
		glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_32 , TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD, (u8*)logo_back_bin);
		glBindTexture(2, textureID[2]);
		glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_32 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)logo_side_bin);
		glBindTexture(3, textureID[3]);
		glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_32 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)logo_tb_bin);		
		
		//any floating point gl call is being converted to fixed prior to being implemented
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70, 256.0 / 192.0, 0.1, 40);
		
		gluLookAt(	0.0, 0.4, 1.3,		//camera possition 
					0.0, 0.0, 0.0,		//look at
					0.0, 1.0, 0.0);		//up
	}
	
	
	//verticies for the cube
	inline static constexpr v16 CubeVectors[24] = {
			floattov16(-0.15), floattov16(-0.5), floattov16(0.5), 
			floattov16(0.15),  floattov16(-0.5), floattov16(0.5),
			floattov16(0.15),  floattov16(-0.5), floattov16(-0.5),
			floattov16(-0.15), floattov16(-0.5), floattov16(-0.5),
			floattov16(-0.15), floattov16(0.5),  floattov16(0.5), 
			floattov16(0.15),  floattov16(0.5),	floattov16(0.5),
			floattov16(0.15),  floattov16(0.5),  floattov16(-0.5),
			floattov16(-0.15), floattov16(0.5),  floattov16(-0.5)
	};
	
	//polys
	inline static constexpr u8 CubeFaces[24] = {
		3,2,1,0,
		0,1,5,4,
		1,2,6,5,
		2,3,7,6,
		3,0,4,7,
		5,6,7,4
	};	

	inline static constexpr u32 normals[6] =
	{
		NORMAL_PACK(0,(u32)floattov10(-.97),0),
		NORMAL_PACK(0,0,floattov10(.97)),
		NORMAL_PACK(floattov10(.97),0,0),
		NORMAL_PACK(0,0,(u32)floattov10(-.97)),
		NORMAL_PACK((u32)floattov10(-.97),0,0),
		NORMAL_PACK(0,floattov10(.97),0)	
	};
	
	//draw a cube face at the specified color
	void drawQuad(int poly, int u, int v)
	{	
		
		u32 f1 = CubeFaces[poly * 4] ;
		u32 f2 = CubeFaces[poly * 4 + 1] ;
		u32 f3 = CubeFaces[poly * 4 + 2] ;
		u32 f4 = CubeFaces[poly * 4 + 3] ;


		glNormal(normals[poly]);

		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(u), 0));
		glVertex3v16(CubeVectors[f1*3], CubeVectors[f1*3 + 1], CubeVectors[f1*3 +  2] );
		
		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(u),inttot16(v)));
		glVertex3v16(CubeVectors[f2*3], CubeVectors[f2*3 + 1], CubeVectors[f2*3 + 2] );
		
		GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(v)));
		glVertex3v16(CubeVectors[f3*3], CubeVectors[f3*3 + 1], CubeVectors[f3*3 + 2] );

		GFX_TEX_COORD = (TEXTURE_PACK(0,0));
		glVertex3v16(CubeVectors[f4*3], CubeVectors[f4*3 + 1], CubeVectors[f4*3 + 2] );
	}

	void DisplayEnableMotionBlur()
	{
		u32 dispcnt = REG_DISPCNT;
		//set main display to display from VRAM
		dispcnt &= ~(0x00030000); dispcnt |= 2<<16; //choose to display screen from VRAM
		dispcnt &= ~(0x000C0000); dispcnt |= 1<<18; //choose to display screen from VRAM_B
		REG_DISPCNT = dispcnt;
	}

	void DisplayEnableNormal()
	{
		u32 dispcnt = REG_DISPCNT;
		dispcnt &= ~(0x00030000); dispcnt |= 1<<16; //choose to display screen from normal layer composition
		REG_DISPCNT = dispcnt;
	}
	
	void frame3d()
	{
		glLight(0, RGB15(31,31,31) , 0,				  floattov10(-1.0),		 0);
		glLight(1, RGB15(31,13,21),   0,				  floattov10(1) - 1,			 0);
		glLight(2, RGB15(31,13,21) ,   floattov10(-1.0), 0,					 0);
		glLight(3, RGB15(31,13,21) ,   floattov10(1.0) - 1,  0,					 0);

		glPushMatrix();

		//move it away from the camera
		glTranslatef32(0, 0, floattof32(-1));
				
		glRotateX(rotateX);
		glRotateY(rotateY);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
		glMatrixMode(GL_MODELVIEW);

		glMaterialf(GL_AMBIENT, RGB15(8,8,8));
		glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
		glMaterialf(GL_EMISSION, RGB15(5,5,5));

		//ds uses a table for shinyness..this generates a half-ass one
		glMaterialShinyness();

		//not a real gl function and will likely change
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 | 
													POLY_FORMAT_LIGHT2 | POLY_FORMAT_LIGHT3 ) ;
		
		
		for(int i=0, u=32, v=32;i<6;i++)
		{
			u=v=32;
			if(i==2)
				glBindTexture(0, textureID[1]);
			else if(i==4)			
				glBindTexture(0, textureID[0]);
			else if(i==0 || i==5)
			{
				glBindTexture(0, textureID[3]);
				v=16;
			}
			else 
			{
				glBindTexture(0, textureID[2]);
				v=16;
			}
			
			glBegin(GL_QUAD);
				drawQuad(i,u,v);
			glEnd();
		}
		
		glPopMatrix(1);
			
		glFlush(0);		
		
		rotateY += 3;

		//the display capture enable bit must be set again each frame if you want to continue capturing.
		REG_DISPCAPCNT |= DCAP_ENABLE;		
	}
	
	~TitleScene()
	{
		key_down.remove_event(&TitleScene::on_key_down, this);
		key_held.remove_event(&TitleScene::on_key_held, this);
		delete vwf;	

		delete title[0];
		delete title[1];
		delete fat_fail;
		delete esodev_logo;
		
		Sprite::oam_deploy_main();		
		Sprite::oam_deploy_sub();		
	}
};

Scene* gen_title_scene()
{
	return new TitleScene();
}

dsc_launch(TitleScene)