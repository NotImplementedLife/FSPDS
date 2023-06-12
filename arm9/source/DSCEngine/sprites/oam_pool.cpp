#include "oam_pool.hpp"

#include "DSCEngine/types/bits_array.hpp"

using namespace DSC;


void DSC::OamPool::reset()
{	
	freeSlots.clear();
	BitsArray<128*4*32>::take_over(__obj_attr_buffer)->clear();	
}

ObjAttribute* DSC::OamPool::add_obj(ObjAttribute attr)
{
	int buffer_id =  freeSlots.find_free_bit();
	nds_assert(buffer_id!=-1, "OAM is full");
	
	freeSlots[buffer_id] = true; // lock	
	obj_attr()[buffer_id] = attr;
	return &obj_attr()[buffer_id];
}

int DSC::OamPool::id_of(const ObjAttribute* attr) const
{
	int id = (((unsigned int)attr)-((unsigned int)obj_attr())) / sizeof(ObjAttribute);
	return id < 128 ? id : -1;
}

void DSC::OamPool::clear_entry(ObjAttribute* attr)
{
	short* raw = (short*)attr;
	raw[0]=raw[1]=raw[2]=0;
}

void DSC::OamPool::remove_obj(ObjAttribute* attr)
{
	int id = id_of(attr);
	if(id < 0) 	
		return;	
	if(freeSlots[id] == true)
		return;	
	
	freeSlots[id] = false;		
	clear_entry(attr);	
}

short* DSC::OamPool::oam_offset() const
{
	return (short*)(0x07000000 + (is_main() ? 0x000 : 0x400));
}

#include <nds.h>
#include "DSCEngine/system/hardware.hpp"

void DSC::OamPool::deploy()
{		
	if(is_main())
	{
		nds_assert(Hardware::MainEngine::objIsEnabled(), "Main: Deploying with OAM disabled");	
	}
	else
	{
		nds_assert(Hardware::SubEngine::objIsEnabled(), "Sub: Deploying with OAM disabled");
	}		
	dmaCopy(__obj_attr_buffer, oam_offset(), OBJ_COUNT * 4 * sizeof(u16));
}

void DSC::OamPool::set_rotation_matrix(int id, int pa, int pb, int pc, int pd)
{
	nds_assert(0<=id && id<32);
		
	short* buff = ((short*)__obj_attr_buffer) + 0x10*id + 0x03;
	
	*buff = pa; buff+=4;
	*buff = pb; buff+=4;
	*buff = pc; buff+=4;
	*buff = pd;		
}

namespace
{
	char init_flags = 0;
	
	constexpr int inited_main = 1<<0;
	constexpr int inited_sub = 1<<1;
	
	// OamPool is private for safety reasons so we will manually create the 
	// Main & Sub instances inside these buffers and make them available as pointers to OamPool.
	char _oam_main_slot[sizeof(OamPool)];
	char _oam_sub_slot[sizeof(OamPool)];
}

DSC::OamPool::OamPool(bool is_main) : _is_main(is_main) {}

bool DSC::OamPool::is_main() const { return _is_main; }

OamPool* const DSC::OamPool::main()
{
	if(!(init_flags & inited_main))
	{		
		*((OamPool*)_oam_main_slot) = OamPool(true);				
		init_flags |= inited_main;
	}	
	return (OamPool* const)&_oam_main_slot;
}

OamPool* const DSC::OamPool::sub()
{
	if(!(init_flags & inited_sub))
	{
		*((OamPool*)_oam_sub_slot) = OamPool(false);
		init_flags |= inited_sub;
	}	
	return (OamPool* const)&_oam_sub_slot;
}