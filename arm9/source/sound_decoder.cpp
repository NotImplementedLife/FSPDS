#include "sound_decoder.hpp"

static const int IndexTable[] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8,
};

static const int ADPCM_STEP_TABLE[] =
{
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767, 0
};
		
constexpr int clamp(int n, int l, int h) { return n <= l ? l : n >= h ? h : n; }

#include "DSC"

// thxx RinLovesYou (https://github.com/miso-xyz/PPMLib/blob/c7548bf4cdb0e368af552c71a45eb9f96f2e3385/PPMLib/Extensions/AdpcmDecoder.cs#L46-L119)
void SoundDecoder::ADPCM2PCM16(const char* src, short* dst, int srclen)
{		
	DSC::Debug::log("s=%X", src);
	DSC::Debug::log("d=%X", dst);

	int srcPtr = 0;
	int dstPtr = 0;
	int sample = 0;
	int step_index = 0;
	int predictor = 0;
	bool lowNibble = true;

	while (srcPtr < srclen)
	{		
		// switch between high and low nibble each loop iteration
		// increments srcPtr after every high nibble
		if (lowNibble)		
			sample = src[srcPtr] & 0xF;		
		else		
			sample = src[srcPtr++] >> 4;		
		lowNibble = !lowNibble;
		int step = ADPCM_STEP_TABLE[step_index];
		int diff = step >> 3;

		if ((sample & 1) != 0)
		{
			diff += step >> 2;
		}
		if ((sample & 2) != 0)
		{
			diff += step >> 1;
		}
		if ((sample & 4) != 0)
		{
			diff += step;
		}
		if ((sample & 8) != 0)
		{
			diff = -diff;
		}
		predictor += diff;
		predictor = clamp(predictor, -32768, 32767);

		step_index += IndexTable[sample];
		step_index = clamp(step_index, 0, 88);
		dst[dstPtr++] = (short)predictor;
	}
	DSC::Debug::log("done?");
}