#pragma once

class PPMReader
{
private:
	int read32(int offset) const;
	short read16(int offset) const;
	char read8(int offset) const;
	char* point_at(int offset) const;
	
	int getSoundHeaderOffset() const;
public:
	static int peek_only_metadata(const char* filename, void* dest);

	unsigned char buffer[1<<20];
	
	void clear();
	
	int getAnimationDataSize() const;
	int getSoundDataSize() const;
	int getFrameCount() const;
	int getFormatVersion() const;
	bool isLocked() const;
	int getThumbnailFrameIndex() const;		
	int getTimestamp() const;	
	
	char* getRootAuthor() const;
	char* getParentAuthor() const;
	char* getCurrentAuthor() const;
	
	
	char* getThumbnailImage() const;
	
	int getFrameOffsetTableSize() const;
	int getAnimationHeaderFlags() const;	
	int* getOffsetTable() const;
	
	bool isPlaybackSet() const;
	bool isLayer1Hidden() const;
	bool isLayer2Hidden() const;
	
	int getFramePlaybackSpeed() const;
	int getBgmFramePlaybackSpeed() const;
	
	char* getFrame(int index) const;
	
	int getBgmTrackSize() const;
	int getSfxTrackSize(int i) const;
	
	char* getBgmTrack() const;
	char* getSfxTrack(int i) const;
	
	char* getSfxMapping() const;
	
	int getSoundFreq() const;
	int getSamplesPerFrame() const;
	
	
	int read_metadata(const char* filename);
	int read_file(const char* filename);
	
	inline static constexpr int ERR_NULL_ARGUMENT = -1;
	inline static constexpr int ERR_FOPEN = -2;
	inline static constexpr int ERR_SIZE_EXCEEDED = -3;
	inline static constexpr int ERR_READ_COUNT = -4;
	
};