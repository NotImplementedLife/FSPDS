#include "DSCEngine/system/hardware.hpp"

#include "DSCEngine/debug/assert.hpp"
#include "DSCEngine/debug/log.hpp"

using namespace DSC::Hardware;

volatile char* const DSC::Hardware::BanksReg = (volatile char* const)0x04000240;

DSC::Hardware::VramBank::VramBank(char bank_name) : bank_name(bank_name)
{
	nds_assert('A'<=bank_name && bank_name<='I');
}

VramBank& DSC::Hardware::VramBank::lcd() { is_lcd = true; return *this; }

VramBank& DSC::Hardware::VramBank::main() { is_lcd = false, is_main = true; return *this; }

VramBank& DSC::Hardware::VramBank::sub() { is_lcd = false, is_main = false; return *this; }

VramBank& DSC::Hardware::VramBank::background() { is_lcd = false,is_background = true; return *this; }

VramBank& DSC::Hardware::VramBank::sprite() { is_lcd = false, is_background = false; return *this; }

VramBank& DSC::Hardware::VramBank::vram() { is_lcd = false, is_vram = true; return *this; }

VramBank& DSC::Hardware::VramBank::ext_palette() { is_lcd = false, is_vram = false; return *this; }

VramBank& DSC::Hardware::VramBank::slot(int slot_number) 
{
	nds_assert(0<=slot_number && slot_number<=3);
	is_lcd = false, ofs = slot_number; 
	return *this; 
}

namespace
{
	void set_bank(char bank_name, char value)
	{
		*((volatile unsigned char*)0x04000240+bank_name-'A'+(bank_name>='H')) = 0x80 | value;
	}
	
	char get_bank(char bank_name)
	{
		return *((volatile unsigned char*)0x04000240+bank_name-'A'+(bank_name>='H'));
	}
	
}

void DSC::Hardware::VramBank::config()
{
	if(is_lcd) 
	{
		set_bank(bank_name, 0);
		//BanksReg[bank_name-'A'] = 0x80 | 0; 
		return;
	}
	
	int mst=0;	
	
	if(is_main)
	{
		if(is_background && is_vram)
		{
			nds_assert(bank_name<='G');
			mst = 1;
			if(bank_name=='E')
			{			
				ofs = 0;			
			}					
		}		
		else if(!is_background && is_vram)
		{
			mst = 2;
			if(bank_name=='A' || bank_name=='B') 
				ofs &= 1;
			else if(bank_name=='E') 
				ofs = 0;
			else 
				nds_assert(false, "Wrong bank MainBgExtPal");
		}
		else if(is_background && !is_vram)
		{
			nds_assert('E'<=bank_name && bank_name<='G');
			mst = 4;
			if(bank_name!='E') ofs &= 1;
		}
		else if(!is_background && !is_vram)
		{
			nds_assert(bank_name=='F' || bank_name=='G');
			mst = 5; ofs = 0;
		}
	}
	else 		
	{
		ofs = 0;
		if(is_background && is_vram)
		{
			if(bank_name=='C') 
				mst = 4;
			else if(bank_name>='H')
				mst = 1;
			else nds_assert(false, "Wrong bank SubBgVram");
		}
		else if(!is_background && is_vram)
		{
			if(bank_name=='D')
				mst = 4;
			else if(bank_name=='I')
				mst = 2;
			else nds_assert(false, "Wrong bank SubObjVram");
		}
		else if(is_background && !is_vram)
		{
			nds_assert(bank_name=='H');
			mst = 2;
		}
		else if(!is_background && !is_vram)
		{
			nds_assert(bank_name=='I');
			mst = 3;
		}
	}	
	
	set_bank(bank_name, mst | (ofs<<3));
	//BanksReg[bank_name-'A'] = 0x80 | mst | (ofs<<3);
}
	
void DSC::Hardware::VramBank::enable() { BanksReg[bank_name-'A'+(bank_name>='H')] |= 0x80; }

void DSC::Hardware::VramBank::disable() { BanksReg[bank_name-'A'+(bank_name>='H')] &= 0x7f; }

bool DSC::Hardware::VramBank::is_enabled() const { return BanksReg[bank_name-'A'+(bank_name>='H')] & 0x80; }


VramBank& DSC::Hardware::VramBank::save_state()
{
	backup = get_bank(bank_name);
	return *this;
}

void DSC::Hardware::VramBank::restore()
{	
	set_bank(bank_name, backup);
	//BanksReg[bank_name-'A'] = backup;
}
	
void*  DSC::Hardware::VramBank::lcd_offset() const
{
	if(bank_name<='E')
		return (void*)(0x06800000 + 0x20000*(bank_name-'A'));
	if(bank_name=='I')
		return (void*)0x068A0000;
	return (void*)(0x06890000 + 0x4000*(bank_name-'F'));
}