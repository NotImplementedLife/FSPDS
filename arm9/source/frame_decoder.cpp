#include "frame_decoder.hpp"

#include "chunks_bin.h"

#include "DSC"

int* chunks32 = (int*)chunks_bin;

void lsh_nibbles(int* buff, int cnt, int shift_pos, int* dest)
{	
	int off = shift_pos/8;
	shift_pos=(shift_pos&7)*4;
	buff+=off;
	cnt-=off;
	
	unsigned int v=*(buff++);
	*dest = v>>shift_pos;
	for(;cnt>1;cnt--)
	{
	    v=*(buff++);
	    *(dest++) |= (v&((1<<shift_pos)-1))<<(32-shift_pos);
		*dest = v>>shift_pos;
	}
	//v=*(buff++);
	//*(dest++) |= (v&((1<<shift_pos)-1))<<(32-shift_pos);
	for(;off--;*(dest++) = 0);	
}

void rsh_nibbles(int* buff, int cnt, int shift_pos, int* dest)
{	
	int off = shift_pos/8;
	shift_pos=(shift_pos&7)*4;	
	
	dest+=cnt;
	cnt-=off;
	buff+=cnt;
	
	unsigned int v=*(--buff);
	*(--dest)= v<<shift_pos;	
		
	for(;cnt>1;cnt--)
	{		
		v=*(--buff);
		*(dest)|=(v>>(32-shift_pos));
		*(--dest) = v<<shift_pos;	    
	}
	for(;off--;*(--dest) = 0);
}

char* typed_decode(char* linedata, char* encoded, int* dest)
{	
	for(int cnt=48;cnt--;)
	{
		unsigned char byte = *(linedata++);
		
		for(int q=4;q--;)
		{				
			int enc_type = byte&3;
			byte>>=2;
			
			switch(enc_type)
			{
				case 0:
					for(int i=32;i--;) *(dest++)=0;
					break;
				case 1:					
				case 2:
				{
					int default_value = enc_type==1 ? 0x00000000 : 0x11111111;
					unsigned int chunk_flags=encoded[3]+(encoded[2]<<8)+(encoded[1]<<16)+(encoded[0]<<24);
					encoded+=4;
					
					//while(chunk_flags)
					for(int i=32;i--;)
					{
						if(chunk_flags & 0x80000000)
						{							
							int pixels = *(encoded++);
							//DSC::Debug::log("px %X %X", pixels, chunks32[pixels]);
							*(dest++)=chunks32[pixels];
						}
						else *(dest++)= default_value;
						chunk_flags <<= 1;
					}		
					break;
				}					
				case 3:
					for(int i=32;i--;)
					{
						int pixels = *(encoded++);
						*(dest++)=chunks32[pixels];
					}					
					break;					
			}			
		}		
	}
	DSC::Debug::log("New encoded = %X", encoded);
	
	return encoded;
}

char* typed_decode_xor(char* linedata, char* encoded, int* dest)
{	
	for(int cnt=48;cnt--;)
	{
		unsigned char byte = *(linedata++);
		
		for(int q=4;q--;)
		{				
			int enc_type = byte&3;
			byte>>=2;
			
			switch(enc_type)
			{
				case 0:
					dest+=32;					
					break;
				case 1:					
				case 2:
				{
					int default_value = enc_type==1 ? 0x00000000 : 0x11111111;
					unsigned int chunk_flags=encoded[3]+(encoded[2]<<8)+(encoded[1]<<16)+(encoded[0]<<24);
					encoded+=4;
					
					//while(chunk_flags)
					for(int i=32;i--;)
					{
						if(chunk_flags & 0x80000000)
						{							
							int pixels = *(encoded++);
							//DSC::Debug::log("px %X %X", pixels, chunks32[pixels]);
							*(dest++)^=chunks32[pixels];
						}
						else *(dest++)^=default_value;
						chunk_flags <<= 1;
					}		
					break;
				}					
				case 3:
					for(int i=32;i--;)
					{
						int pixels = *(encoded++);
						*(dest++)^=chunks32[pixels];
					}					
					break;					
			}			
		}		
	}
	DSC::Debug::log("New encoded = %X", encoded);
	
	return encoded;
}

int FrameDecoder::decode(int* buffer1, int* buffer2, char* frame_data)
{	
	int frame_header = *(frame_data++);
	int dx=0, dy=0;	
	
	char* (*typed_decode_f)(char*, char*, int*) = typed_decode;
	
	if((frame_header & 0x80)==0)
	{
		if(frame_header & 0b01100000)
		{
			dx = *(frame_data++);
			dy = *(frame_data++);
			if(dx>=128) dx=-256+dx;
			if(dy>=128) dy=-256+dy;
		}
		typed_decode_f = typed_decode_xor;
	}
	
	DSC::Debug::log("Frame data = %X", frame_data);
	DSC::Debug::log("Diffing = %s", (frame_header & 0x80)==0 ? "YES" : "NO");
	if((frame_header & 0x80)==0)
	{
		DSC::Debug::log("x,y = %i, %i", dx, dy);
	}
		
	void (*sh_nibbles)(int*, int, int, int*) = rsh_nibbles;
	
	//dx=22;
	if(dx<0)
	{
		sh_nibbles = lsh_nibbles;
		dx=-dx;
	}
	
	if(dy>0)
	{
		for(int y=191;y>=0;y--)
		{
			if(y+dy<192)
			{				
				sh_nibbles(buffer1+32*y, 32, dx, buffer1+32*(y+dy));
				sh_nibbles(buffer2+32*y, 32, dx, buffer2+32*(y+dy));
			}
			else
			{
				for(int i=0;i<32;i++)
				{					
					buffer1[32*y+i]=0;
					buffer2[32*y+i]=0;
				}
			}
		}
	}	
	else
	{
		dy=-dy;
		for(int y=0;y<192;y++)
		{
			if(y+dy<192)
			{
				sh_nibbles(buffer1+32*(y+dy), 32, dx, buffer1+32*y);
				sh_nibbles(buffer2+32*(y+dy), 32, dx, buffer2+32*y);
			}
			else
			{
				for(int i=0;i<32;i++)
				{					
					buffer1[32*y+i]=0;
					buffer2[32*y+i]=0;
				}
			}
		}		
	}
	
	char* decode_src = typed_decode_f(frame_data, frame_data+96, buffer1);
	typed_decode_f(frame_data+48, decode_src, buffer2);
	
	return frame_header;	
}

