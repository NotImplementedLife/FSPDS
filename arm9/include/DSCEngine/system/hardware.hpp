#pragma once

namespace DSC
{
	enum class Engine 
	{
		Main, 
		Sub
	};
}

namespace DSC::Hardware
{		
	typedef short ExtendedPalette[256];
	
	int tellExtendedPaletteIndex(void* pal_offset);
	ExtendedPalette* tellExtendedPaletteBase(void* pal_offset);

	namespace MainEngine
	{
		inline static volatile unsigned int* const DISPCNT  = (volatile unsigned int* const)0x04000000;
		
		inline static short* const BgPalette  = (short* const)0x05000000;
		inline static short* const ObjPalette = (short* const)0x05000200;
		
		inline static short* const Oam        = (short* const)0x07000000;
		
		inline static short* const BgVram     = (short* const)0x06000000;
		inline static short* const ObjVram    = (short* const)0x06400000;
		
		ExtendedPalette* BgExtendedPalette();
		ExtendedPalette* ObjExtendedPalette();		
		
		ExtendedPalette* BgExtendedPalette(int index);
		ExtendedPalette* ObjExtendedPalette(int index);				
				
		
		void objEnable(int mapping_size, bool use_ext_palette=false);
		void objDisable();
		bool objIsEnabled();
		bool objIsExtPaletteEnabled();
		int objGetMappingSize();
	}
	
	namespace SubEngine
	{
		inline static volatile unsigned int* const DISPCNT  = (volatile unsigned int* const)0x04001000;
		
		inline static short* const BgPalette  = (short* const)0x05000400;
		inline static short* const ObjPalette = (short* const)0x05000600;
		
		inline static short* const Oam        = (short* const)0x07000400;
		
		inline static short* const BgVram     = (short* const)0x06200000;
		inline static short* const ObjVram    = (short* const)0x06600000;		
		
		ExtendedPalette* BgExtendedPalette();
		ExtendedPalette* ObjExtendedPalette();
		
		ExtendedPalette* BgExtendedPalette(int index);
		ExtendedPalette* ObjExtendedPalette(int index);
		
		void objEnable(int mapping_size, bool use_ext_palette=false);
		void objDisable();
		bool objIsEnabled();
		bool objIsExtPaletteEnabled();
		int objGetMappingSize();
	}	
	
	extern volatile char* const BanksReg;
		
	class VramBank
	{
	private:
		char bank_name; // 'A'..'I'				
		bool is_lcd = true;
		bool is_main = true;
		bool is_background = true;		
		bool is_vram = true;
		int ofs = 0;		
		
		char backup = 0;
	public:
		VramBank(char bank_name);
		
		VramBank& lcd();
		
		VramBank& main();
		VramBank& sub();
		
		VramBank& background();
		VramBank& sprite();
		
		VramBank& vram();
		VramBank& ext_palette();
		
		VramBank& slot(int slot_number);
				
		void config();
		
		void enable();
		void disable();
		bool is_enabled() const;
		
		VramBank& save_state();
		void restore();
		
		void* lcd_offset() const;
	};
	
}