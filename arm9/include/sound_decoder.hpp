#pragma once

class SoundDecoder
{
public:
	static void ADPCM2PCM16(const char* src, short* dest, int srclen);
};