#include "ppm_reader.hpp"

#include "DSC"
#include <stdio.h>
#include <fat.h>

int PPMReader::read_metadata(const char* filename)
{
	if(filename==nullptr) return ERR_NULL_ARGUMENT;
	FILE* fp=fopen(filename,"rb");
	if(fp==nullptr) return ERR_FOPEN;
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);	
	
	if(sz>1024*1024)
	{
		fclose(fp);
		return ERR_SIZE_EXCEEDED;
	}
	fseek(fp, 0L, SEEK_SET);
	
	fread(buffer, sizeof(char), 0x06A0, fp);

	
	fclose(fp);
	return sz;
}

int PPMReader::read_file(const char* filename)
{
	if(filename==nullptr) return ERR_NULL_ARGUMENT;
	FILE* fp=fopen(filename,"rb");
	if(fp==nullptr) return ERR_FOPEN;
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);	
	
	if(sz>1024*1024)
	{
		fclose(fp);
		return ERR_SIZE_EXCEEDED;
	}	
	fseek(fp, 0L, SEEK_SET);
	
	fread(buffer, sizeof(char), sz, fp);
	
	
	fclose(fp);
	return sz;
}

int PPMReader::read32(int offset) const 
{
	nds_assert(offset%4==0);
	return reinterpret_cast<const int*>(buffer)[offset/4];
}

short PPMReader::read16(int offset) const
{
	nds_assert(offset%2==0);
	return reinterpret_cast<const short*>(buffer)[offset/2];
}

char PPMReader::read8(int offset) const
{
	return reinterpret_cast <const char*>(buffer)[offset];
}

char* PPMReader::point_at(int offset) const { return (char*)((int)buffer + offset); }

int PPMReader::getAnimationDataSize() const { return read32(0x4); }


// Header
int PPMReader::getSoundDataSize() const { return read32(0x8); }
int PPMReader::getFrameCount() const { return read16(0xC); }
int PPMReader::getFormatVersion() const { return read16(0xE); }
bool PPMReader::isLocked() const { return read16(0x10 + 0x00)!=0; }

// Metadata
int PPMReader::getThumbnailFrameIndex() const { return read16(0x10 + 0x02)!=0; }
int PPMReader::getTimestamp() const { return read32(0x10 + 0x8A)!=0; }
char* PPMReader::getThumbnailImage() const { return point_at(0xA0); }

// Animation header
int PPMReader::getFrameOffsetTableSize() const { return read16(0x6A0 + 0x00); }
int PPMReader::getAnimationHeaderFlags() const { return read16(0x6A0 + 0x06); }
int* PPMReader::getOffsetTable() const { return (int*)(point_at(0x6A0 + 0x08)); }

bool PPMReader::isPlaybackSet() const { return getAnimationHeaderFlags() & 0x2;  }
bool PPMReader::isLayer1Hidden() const { return getAnimationHeaderFlags() & 0x10;  }
bool PPMReader::isLayer2Hidden() const { return getAnimationHeaderFlags() & 0x20;  }

// Animation data

char* PPMReader::getFrame(int index) const
{
	return point_at(0x06A8 + getFrameOffsetTableSize() + getOffsetTable()[index]);
}

static constexpr int frames[] = { 2, 120, 60, 30, 15, 10, 5, 3, 2 };

int PPMReader::getFramePlaybackSpeed() const { return frames[8-(int)read8( ((0x6A0+getAnimationDataSize()+getFrameCount()+3)/4)*4 + 16) ]; } 
int PPMReader::getBgmFramePlaybackSpeed() const { return frames[8-(int)read8(((0x6A0+getAnimationDataSize()+getFrameCount()+3)/4)*4 + 17)]; }