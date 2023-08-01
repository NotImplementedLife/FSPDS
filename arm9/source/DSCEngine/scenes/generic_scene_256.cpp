#include "DSCEngine/scenes/generic_scene_256.hpp"

#include "DSCEngine/video/measure.hpp"
#include "DSCEngine/video/palette_manager.hpp"
#include "DSCEngine/video/palette_loader.hpp"
#include "DSCEngine/video/vram_loader.hpp"
#include "DSCEngine/video/allocator.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/error.hpp"

#include "DSCEngine/system/hardware.hpp"

#include "DSCEngine/utils/math.hpp"

#include "DSCEngine/sprites/obj_allocator.hpp"

#include <nds.h>

using namespace DSC;
using namespace DSC::Hardware;

namespace
{
	struct BackgroundRequirement
	{
		bool enabled = false;
		bool is_bitmap;
		int color_depth;		
		int width;
		int height;
		int data_length;
		const AssetData* src_asset = nullptr;
	};
}

struct DSC::GenericScene256::__privates__
{
	BackgroundRequirement bg_requirements[8];
	
	PaletteManager main_palette = PaletteManager(Hardware::MainEngine::BgPalette);
	PaletteManager sub_palette = PaletteManager(Hardware::SubEngine::BgPalette);
	
	Vector<PaletteManager*> ext_palettes[8];
	PaletteLoader palette_loader[8];
	
	PaletteManager* main_obj_ext_palettes[7];
	PaletteManager* sub_obj_ext_palettes[7];
	
	PaletteLoader* main_obj_palette_loader;
	PaletteLoader* sub_obj_palette_loader;
		
	Allocator main_obj_vram_allocator;		
	Allocator sub_obj_vram_allocator;
	
	ObjAllocator* main_obj_allocator; 
	ObjAllocator* sub_obj_allocator;
	
	Vector<Sprite*> sprites;
};

DSC::GenericScene256::GenericScene256()
{
	privates = new __privates__();
	Debug::log("Created privates!!!!");
}

void DSC::GenericScene256::init()
{	
	REG_DISPCNT = REG_DISPCNT_SUB = 0;		
	Sprite::oam_reset();
	
	set_banks();
	
	privates->main_obj_vram_allocator = Allocator((int)Hardware::MainEngine::ObjVram, 64*1024);		
	privates->sub_obj_vram_allocator = Allocator((int)Hardware::SubEngine::ObjVram, 128*1024);
	
	// init obj palettes (TO DO: extract to separate function)	
	
	privates->main_obj_palette_loader = new PaletteLoader(nullptr, {});
	privates->sub_obj_palette_loader = new PaletteLoader(nullptr, {});
	
	for(int i=0;i<7;i++)
	{
		privates->main_obj_ext_palettes[i] = new PaletteManager(Hardware::MainEngine::ObjExtendedPalette(i));
		privates->main_obj_palette_loader->add_extended_palette_manager(privates->main_obj_ext_palettes[i]);
		
		privates->sub_obj_ext_palettes[i] = new PaletteManager(Hardware::SubEngine::ObjExtendedPalette(i));
		privates->sub_obj_palette_loader->add_extended_palette_manager(privates->sub_obj_ext_palettes[i]);
	}
	
	privates->main_obj_palette_loader->set_default_allocation_mode(PaletteLoader::ALLOC_MODE_EXTENDED_PALETTES);
	privates->sub_obj_palette_loader->set_default_allocation_mode(PaletteLoader::ALLOC_MODE_EXTENDED_PALETTES);
	
	privates->main_obj_allocator = new ObjAllocator(&privates->main_obj_vram_allocator, privates->main_obj_palette_loader, 64);
	privates->sub_obj_allocator = new ObjAllocator(&privates->sub_obj_vram_allocator, privates->sub_obj_palette_loader, 128);
		
	
	Debug::log("GenericScene256 inited");
}

ObjAllocator* DSC::GenericScene256::get_obj_allocator_main() const { return privates->main_obj_allocator; }
ObjAllocator* DSC::GenericScene256::get_obj_allocator_sub() const { return privates->sub_obj_allocator; }

Sprite* DSC::GenericScene256::create_sprite(Sprite* sprite)
{
	sprite->set_default_allocator(sprite->get_engine()==Engine::Main 
		? privates->main_obj_allocator
		: privates->sub_obj_allocator);			
	privates->sprites.push_back(sprite);
	return sprite;	
}

void DSC::GenericScene256::begin_sprites_init()
{
	VramBank('F').lcd()        .config();	
	VramBank('I').lcd()        .config();
}

void DSC::GenericScene256::end_sprites_init()
{
	VramBank('F').main().sprite()    .ext_palette()        .config();	
	VramBank('I').sub() .sprite()    .ext_palette()        .config();
}

__attribute__((noinline))
void DSC::GenericScene256::run()
{
	solve_map_requirements();
	load_assets();
	
	Hardware::MainEngine::objEnable(64, true); // set to 64
	Hardware::SubEngine::objEnable(128, true); // set to 128
	
	Scene::run();	
}

void DSC::GenericScene256::frame()
{
	for(int i=0;i<privates->sprites.size();i++)
	{
		privates->sprites[i]->update_position();
		privates->sprites[i]->update_visual();	
	}
	Sprite::oam_deploy_main();
	Sprite::oam_deploy_sub();
}


void DSC::GenericScene256::solve_map_requirements_main()
{
	int izone = 0;
	int imap_base = 0;
	
	int* tile_base = new int[4];
	int* map_base = new int[4];	
	
	int bmp_cnt = 0;	
	
	// solve main
	for(int i=0;i<4;i++)
	{
		BackgroundRequirement& req = privates->bg_requirements[i];		
		
		if(!req.enabled) continue;
		
		if(imap_base>=32)
		{
			fatal("Map base exceeded for main BG%i",i);
		}		
		
		map_base[i] = imap_base;
		
		if(!req.is_bitmap) // is it tilemap? then, update the map base index
		{
			// get the number of 2KB blocks that would fit the map data for this bg			
			int map_blocks = Measure()
				.map_size(req.width, req.height) 
				.fit()
				.blocks(2)
				.kilobytes();
				
			imap_base+=map_blocks; // suppose we have already allocated this bg
		}					
		else
		{
			nds_assert(i>=2, "Backgrounds 0,1 cannot be bitmaps");			
			bmp_cnt++;
		}
		
		// get the number of zones required for the background data (tileset/bitmap)
		
		// reserve the entire map space for bitmaps even though the bitmap doesn't fully use it
		// If this is not done, then scrolling down will reveal unwanted artifacts from VRAM that
		// reside in the bitmap's space		
		
		MeasureValue value = req.is_bitmap 
			? Measure().bpp(req.color_depth).bitmap(req.width, req.height)
			: MeasureValue(req.data_length);
			
		int gfx_blocks = value
			.fit()
			.blocks(16)
			.kilobytes();
			
		tile_base[i] = izone;
		izone+=gfx_blocks;
	}	
	
	// get the "offset" zone - the tile base right after all the map data (1 zone == 8 map blocks)
	int ozone = MeasureValue(imap_base).fit().blocks(8).value();
	if(ozone + izone > 16) // exceeded 256KB limit?
	{
		fatal("Main backgrounds data does not fit in allocated VRAM");
	}
			
	for(int i=0;i<4;i++) tile_base[i] += ozone;
	
	if(bmp_cnt==2)
	{
		// manually set video mode without touching anything else in the register
		REG_DISPCNT &= ~0x10007;
		REG_DISPCNT |= MODE_5_2D;		
	}
	else if(bmp_cnt==1)
	{
		if(privates->bg_requirements[2].is_bitmap)
		{
			REG_DISPCNT &= ~0x10007;
			REG_DISPCNT |= MODE_5_2D;		
		}
		else
		{
			REG_DISPCNT &= ~0x10007;
			REG_DISPCNT |= MODE_3_2D;		
		}
	}	
	else 
	{
		REG_DISPCNT &= ~0x10007;
		REG_DISPCNT |= MODE_0_2D;
	}
	
	
	Vector<int> bg_use_ext;
	
	// init backgrounds	
	for(int i=0;i<4;i++)
	{
		privates->palette_loader[i] = PaletteLoader(&privates->main_palette);
		BackgroundRequirement& req = privates->bg_requirements[i];
		if(req.enabled)
		{
			int config = validate_bg_size(req.width, req.height, req.color_depth, req.is_bitmap);
			BgType bg_type = (BgType)((config>>16)&0xFFFF);
			BgSize bg_size = (BgSize)(config & 0xFFFF);			
			
			Debug::log("MAIN %i: type=%i, size=%i", i, bg_type, bg_size);
			bgInit(i, bg_type, bg_size, map_base[i],tile_base[i]);
			
			if(!req.is_bitmap && req.color_depth==8)
				bg_use_ext.push_back(i);
		}
	}		
	
	int ext_pal_index = 0;
	if(bg_use_ext.size()>0)
	{
		// how many extended palettes should each eligible background use
		int palcnt = max(16, 32 / bg_use_ext.size());
		
		for(int i=0;i<bg_use_ext.size();i++)
		{
			for(int j=0;j<palcnt;j++)
			{
				privates->ext_palettes[i].push_back(new PaletteManager(MainEngine::BgExtendedPalette(ext_pal_index++)));
			}
			// update palette loader for that background
			privates->palette_loader[i] = PaletteLoader(&privates->main_palette, privates->ext_palettes[i]);
		}
	}	
	
	delete[] tile_base;
	delete[] map_base;
}

void DSC::GenericScene256::solve_map_requirements_sub()
{
	int izone = 0;
	int imap_base = 0;
	
	int* tile_base = new int[4];
	int* map_base = new int[4];	
	
	int bmp_cnt = 0;
	
	// solve main
	for(int i=0;i<4;i++)
	{
		BackgroundRequirement& req = privates->bg_requirements[4+i];
		
		if(!req.enabled) continue;
		
		if(imap_base>=32)
		{
			fatal("Map base exceeded for sub BG%i",i);
		}		
		
		map_base[i] = imap_base;
		
		if(!req.is_bitmap) // is it tilemap? then, update the map base index
		{
			// get the number of 2KB blocks that would fit the map data for this bg			
			int map_blocks = Measure()
				.map_size(req.width, req.height) 
				.fit()
				.blocks(2)
				.kilobytes();
				
			imap_base+=map_blocks; // suppose we have already allocated this bg
		}					
		else
		{
			nds_assert(i>=2, "Backgrounds 0,1 cannot be bitmaps");			
			bmp_cnt++;
		}
		
		// get the number of zones required for the background data (tileset/bitmap)
		
		// reserve the entire map space for bitmaps even though the bitmap doesn't fully use it
		// If this is not done, then scrolling down will reveal unwanted artifacts from VRAM that
		// reside in the bitmap's space		
		
		MeasureValue value = req.is_bitmap 
			? Measure().bpp(req.color_depth).bitmap(req.width, req.height)
			: MeasureValue(req.data_length);
			
		int gfx_blocks = value
			.fit()
			.blocks(16)
			.kilobytes();
			
		tile_base[i] = izone;
		izone+=gfx_blocks;		
	}	
	
	// get the "offset" zone - the tile base right after all the map data (1 zone == 8 map blocks)
	int ozone = MeasureValue(imap_base).fit().blocks(8).value();
	if(ozone + izone > 8) // exceeded 128KB limit?
	{
		fatal("Main backgrounds data does not fit in allocated VRAM");
	}
			
	for(int i=0;i<4;i++) tile_base[i] += ozone;
	
	Debug::log("Sub?");
	if(bmp_cnt==2)
	{
		REG_DISPCNT_SUB &= ~0x10007;
		REG_DISPCNT_SUB |= MODE_5_2D;
	}
	else if(bmp_cnt==1)
	{
		if(privates->bg_requirements[4+2].is_bitmap)
		{
			REG_DISPCNT_SUB &= ~0x10007;
			REG_DISPCNT_SUB |= MODE_5_2D;
		}
		else
		{
			REG_DISPCNT_SUB &= ~0x10007;
			REG_DISPCNT_SUB |= MODE_3_2D;
		}
	}		
	else
	{
		REG_DISPCNT_SUB &= ~0x10007;
		REG_DISPCNT_SUB |= MODE_0_2D;
	}
	
	
	Vector<int> bg_use_ext;
	// init backgrounds	
	for(int i=0;i<4;i++)
	{
		privates->palette_loader[4+i] = PaletteLoader(&privates->sub_palette);
		BackgroundRequirement& req = privates->bg_requirements[4+i];
		if(req.enabled)
		{
			int config = validate_bg_size(req.width, req.height, req.color_depth, req.is_bitmap);
			BgType bg_type = (BgType)((config>>16)&0xFFFF);
			BgSize bg_size = (BgSize)(config & 0xFFFF);
			Debug::log("t/m = %i, %i", tile_base[i], map_base[i]);
			bgInitSub(i, bg_type, bg_size, map_base[i], tile_base[i]);
			
			if(!req.is_bitmap && req.color_depth==8)
				bg_use_ext.push_back(i);
		}		
	}	


	int ext_pal_index = 0;
	if(bg_use_ext.size()>0)
	{
		// how many extended palettes should each eligible background use
		int palcnt = max(16, 32 / bg_use_ext.size());
		
		for(int i=0;i<bg_use_ext.size();i++)
		{
			for(int j=0;j<palcnt;j++)
			{
				privates->ext_palettes[4+i].push_back(new PaletteManager(SubEngine::BgExtendedPalette(ext_pal_index++)));
			}
			// update palette loader for that background
			privates->palette_loader[4+i] = PaletteLoader(&privates->sub_palette, privates->ext_palettes[4+i]);
		}
	}
	
	delete[] tile_base;
	delete[] map_base;
}

void DSC::GenericScene256::load_assets()
{
	for(int i=0;i<8;i++)
	{		
		BackgroundRequirement& req = privates->bg_requirements[i];
		int pal4=0;
		if(req.enabled && req.src_asset!=nullptr)
		{			
			Debug::log("Loading %i", i);
			
			PaletteLoader* pal_loader = &privates->palette_loader[i];
			
			pal_loader->set_default_allocation_mode(PaletteLoader::ALLOC_MODE_STANDARD_PALETTE);
			PaletteAllocationResult palloc = pal_loader->try_load(req.src_asset, PaletteLoader::ALLOC_MODE_DEFAULT);
			pal4 = palloc.indices[0];
			
			nds_assert(palloc.succeeded, "Palette allocation failed");
			
			//Debug::log("%i %i", req.width, req.src_asset->width);
			//Debug::log("Pal len = %i", palloc.length);
			//Debug::log("Pal ind = %X", palloc.indices);
			VramLoader::load(req.src_asset, bgGetGfxPtr(i), palloc.indices, req.width);					
		}		
		if(!req.is_bitmap && req.color_depth==4)
		{					
			for(int y=0;y<req.height/8;y++)
			{
				for(int x=0;x<req.width/8;x++)
					bgGetMapPtr(i)[32*y+x]=(32*y+x) | pal4<<12;
			}
		}
	}
}

void DSC::GenericScene256::solve_map_requirements()
{	
	solve_map_requirements_main();
	solve_map_requirements_sub();
}


void DSC::GenericScene256::require_tiledmap_4bpp(int id, int px_width, int px_height, int number_of_tiles)
{
	nds_assert(0<=id && id<8);
	privates->bg_requirements[id] = 
	{
		enabled      : true,
		is_bitmap    : false,
		color_depth  : 4,
		width        : px_width,
		height       : px_height,
		data_length  : Measure()._4bpp().tiles(number_of_tiles),
		src_asset    : nullptr
	};
}

void DSC::GenericScene256::require_tiledmap_8bpp(int id, int px_width, int px_height, int number_of_tiles)
{
	nds_assert(0<=id && id<8);
	privates->bg_requirements[id] = 
	{
		enabled      : true,
		is_bitmap    : false,
		color_depth  : 8,
		width        : px_width,
		height       : px_height,
		data_length  : Measure()._8bpp().tiles(number_of_tiles),
		src_asset    : nullptr
	};
}

void DSC::GenericScene256::require_tiledmap(int id, int px_width, int px_height, const AssetData* tileset)
{
	nds_assert(tileset!=nullptr);	
	nds_assert(!tileset->is_bitmap());
	
	int color_depth = tileset->get_color_depth();
	nds_assert(color_depth == 4 || color_depth == 8);
	
	privates->bg_requirements[id] = 
	{
		enabled      : true,
		is_bitmap    : false,
		color_depth  : color_depth,
		width        : px_width,
		height       : px_height,
		data_length  : Measure()._8bpp().bitmap(px_width, px_height),
		src_asset    : tileset
	};	
}

void fitting_bitmap8_size(int px_width, int px_height, int* res_width, int* res_height)
{
	static const short w8[] = { 128, 256, 512, 512, 512, 1024};
	static const short h8[] = { 128, 256, 256, 512, 1024, 512};
	static const int size = sizeof(w8)/sizeof(short);	
	
	*res_width = -1;
	*res_height = -1;
	
	for(int i=0;i<size;i++)
	{
		if(px_width <= w8[i] && px_height <= h8[i])
		{
			*res_width = w8[i];
			*res_height = h8[i];
			return;
		}
	}	
}

void DSC::GenericScene256::require_bitmap(int id, int px_width, int px_height)
{
	nds_assert(0<=id && id<8);
	
	int fitting_width, fitting_height;
	fitting_bitmap8_size(px_width, px_height,&fitting_width, &fitting_height);	
	
	privates->bg_requirements[id] = 
	{
		enabled      : true,
		is_bitmap    : true,
		color_depth  : 8,
		width        : fitting_width,
		height       : fitting_height,
		data_length  : Measure()._8bpp().bitmap(px_width, px_height),
		src_asset    : nullptr
	};
}

void DSC::GenericScene256::require_bitmap(int id, const AssetData* bitmap)
{
	nds_assert(bitmap!=nullptr);	
	nds_assert(bitmap->is_bitmap());
	
	require_bitmap(id, 8*bitmap->width, 8*bitmap->height);
	privates->bg_requirements[id].src_asset = bitmap;
}

void fitting_bitmap16_size(int px_width, int px_height, int* res_width, int* res_height)
{
	static const short w16[] = { 128, 256, 512, 512};
	static const short h16[] = { 128, 256, 256, 512};
	static const int size = sizeof(w16)/sizeof(short);	
	
	*res_width = -1;
	*res_height = -1;
	
	for(int i=0;i<size;i++)
	{
		if(px_width <= w16[i] && px_height <= h16[i])
		{
			*res_width = w16[i];
			*res_height = h16[i];
			return;
		}
	}	
}

void DSC::GenericScene256::require_bitmap_16bpp(int id, int px_width, int px_height)
{
	nds_assert(0<=id && id<8);
	
	nds_assert(0<=id && id<8);
	
	int fitting_width, fitting_height;
	fitting_bitmap16_size(px_width, px_height,&fitting_width, &fitting_height);
	
	privates->bg_requirements[id] = 
	{
		enabled      : true,
		is_bitmap    : true,
		color_depth  : 16,
		width        : fitting_width,
		height       : fitting_height,
		data_length  : Measure()._16bpp().bitmap(px_width, px_height),
		src_asset    : nullptr
	};
}

void DSC::GenericScene256::require_bitmap_16bpp(int id, const AssetData* bitmap)
{
	nds_assert(bitmap!=nullptr);
	nds_assert(bitmap->is_bitmap());
	
	require_bitmap_16bpp(id, 8*bitmap->width, 8*bitmap->height);
	privates->bg_requirements[id].src_asset = bitmap;
}

DSC::GenericScene256::~GenericScene256()
{		
	delete privates->main_obj_palette_loader;
	delete privates->sub_obj_palette_loader;
	
	for(int i=0;i<7;i++)
	{
		delete privates->main_obj_ext_palettes[i];				
		delete privates->sub_obj_ext_palettes[i];		
	}	
	
	delete privates->main_obj_allocator;
	delete privates->sub_obj_allocator;
	
	for(int i=0;i<8;i++)
		for(int j=0;j<privates->ext_palettes[i].size();j++)
		{
			delete privates->ext_palettes[i][j];
		}
	
	delete privates;
	
	
	Hardware::MainEngine::objDisable(); 
	Hardware::SubEngine::objDisable();
}

#include <nds.h>

int DSC::GenericScene256::validate_bg_size(int w, int h, int color_depth, bool is_bitmap)
{		
	nds_assert((w&(w-1))==0, "Map width must be a power of 2");
	nds_assert((h&(h-1))==0, "Map height must be a power of 2");
	
	nds_assert(128<=w && w<=1024);
	nds_assert(128<=h && h<=1024);
	
	int type;
	int size;
	bool ok  = false;
	
	// this looks terrible but I have no ideas how to make it cleaner
	// no (ex)rotation backgrounds yet!!!!!!!!!!!!!!!!
	if(is_bitmap)
	{				
		if(color_depth==8)
		{
			type = BgType_Bmp8;
			ok |= (w== 128 && h== 128); if(ok) { size = BgSize_B8_128x128;  goto __validate_bg_size__success; }
			ok |= (w== 256 && h== 256); if(ok) { size = BgSize_B8_256x256;  goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 256); if(ok) { size = BgSize_B8_512x256;  goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 512); if(ok) { size = BgSize_B8_512x512;  goto __validate_bg_size__success; }
			ok |= (w== 512 && h==1024); if(ok) { size = BgSize_B8_512x1024; goto __validate_bg_size__success; }
			ok |= (w==1024 && h== 512); if(ok) { size = BgSize_B8_1024x512; goto __validate_bg_size__success; }
		}
		else if(color_depth==16)
		{
			type = BgType_Bmp16;
			ok |= (w== 128 && h== 128); if(ok) { size = BgSize_B16_128x128; goto __validate_bg_size__success; }
			ok |= (w== 256 && h== 256); if(ok) { size = BgSize_B16_256x256; goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 256); if(ok) { size = BgSize_B16_512x256; goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 512); if(ok) { size = BgSize_B16_512x512; goto __validate_bg_size__success; }
		}
	}
	else
	{
		if(color_depth!=16)
		{
			type = color_depth==4 ? BgType_Text4bpp : BgType_Text8bpp;
			ok |= (w== 256 && h== 256); if(ok) { size = BgSize_T_256x256;  goto __validate_bg_size__success; }
			ok |= (w== 256 && h== 512); if(ok) { size = BgSize_T_256x512;  goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 256); if(ok) { size = BgSize_T_512x256;  goto __validate_bg_size__success; }
			ok |= (w== 512 && h== 512); if(ok) { size = BgSize_T_512x512;  goto __validate_bg_size__success; }
		}
	}	
	
	fatal("Invalid map size (%i,%i) for %i-bpp %s", w, h, color_depth, is_bitmap ? "bitmap" : "tiled map");	
	
	return -1;
	
	__validate_bg_size__success:
	return size | (type<<16);
	
}

void DSC::GenericScene256::set_banks()
{	
	VramBank('A').main().background().vram()       .slot(0).config();	
	VramBank('B').main().background().vram()       .slot(1).config();
	VramBank('C').sub() .background().vram()               .config();	
	VramBank('D').sub() .sprite()    .vram()               .config();	
	VramBank('E').main().sprite()    .vram()               .config();	
	VramBank('F').main().sprite()    .ext_palette()        .config();
	VramBank('G').main().background().ext_palette().slot(1).config();	
	VramBank('H').sub() .background().ext_palette()        .config();
	VramBank('I').sub() .sprite()    .ext_palette()        .config();
}