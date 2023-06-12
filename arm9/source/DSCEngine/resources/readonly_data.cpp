#include "DSCEngine/resources/readonly_data.hpp"
#include "DSCEngine/debug/assert.hpp"

#include <nds.h>

#include <stdio.h>


DSC::ReadOnlyData::ReadOnlyData(int header_size) : header_size(header_size) { }

void DSC::ReadOnlyData::extract(void* destination) const
{
	if(!is_file())
	{		
		// perform 16-bit r/w
		short* src = reinterpret_cast<short*>(data_source);
		if(src == nullptr)
		{
			src = (short*)(((int)this)+header_size);
		}		
		
		short* dst = reinterpret_cast<short*>(destination);
		for(int i=0;i<data_length/2;i++)
			dst[i] = src[i]; // maybe __asm this line if gcc optimizes it to memset/memcpy (<=> byte-to-byte copy)
	}
	else
	{
		char* filename = data_source;		
		FILE* fptr = fopen(filename, "rb");		
		fread(destination, data_length, 1, fptr);		
		fclose(fptr);
	}
}

void DSC::ReadOnlyData::extract(void* destination, int offset, int length) const
{
	nds_assert(offset % 2 == 0);
	nds_assert(length % 2 == 0);
	
	nds_assert(offset+length <= data_length);
	
	if(!is_file())
	{		
		// perform 16-bit r/w
		short* src = reinterpret_cast<short*>(data_source);		
		if(src == nullptr)
		{
			src = (short*)(((int)this)+header_size);
		}		
		src += offset/sizeof(short);
		
		short* dst = reinterpret_cast<short*>(destination);
		for(int i=0;i<length/2;i++)
			dst[i] = src[i]; // maybe __asm this line if gcc optimizes it to memset/memcpy (<=> byte-to-byte copy)
	}
	else
	{
		char* filename = data_source;		
		FILE* fptr = fopen(filename, "rb");
		fseek(fptr, offset, SEEK_SET);
		fread(destination, length, 1, fptr);
		fclose(fptr);
	}
}

int DSC::ReadOnlyData::get_type() const
{
	return (flags & ROD_TYPE) >> 2;
}

bool DSC::ReadOnlyData::is_file() const { return flags & ROD_IS_FILE; }
bool DSC::ReadOnlyData::is_compressed() const { return flags & ROD_IS_COMPRESSED; }

const int DSC::ReadOnlyData::ROD_IS_FILE       = (1<<0);
const int DSC::ReadOnlyData::ROD_IS_COMPRESSED = (1<<1);
const int DSC::ReadOnlyData::ROD_TYPE          = (15<<2);

const int DSC::ReadOnlyData::ROD_TYPE_UNKNOWN = 0;
const int DSC::ReadOnlyData::ROD_TYPE_ASSET   = 1;
const int DSC::ReadOnlyData::ROD_TYPE_FONT    = 2;