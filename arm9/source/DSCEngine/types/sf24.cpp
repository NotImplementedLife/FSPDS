#include "DSCEngine/types/sf24.hpp"

DSC::sf24::sf24() {}

DSC::sf24::sf24(short x) : sf24(x,0) {	}

DSC::sf24::sf24(short x, unsigned char frac)
{
	this->data = (x<<16)|(frac<<8);
}

short DSC::sf24::get_int() const
{
	return data>>16;
}

unsigned char DSC::sf24::get_frac() const
{
	return ((data>=0 ? data: -data) & 0x0000FF00)>>8;
}

DSC::sf24 DSC::sf24::operator + (const sf24& b) const
{	
	return create_from_data(data+b.data);
}

DSC::sf24 DSC::sf24::operator - (const sf24& b) const
{	
	return create_from_data(data-b.data);
}

DSC::sf24 DSC::sf24::operator * (const sf24& b) const
{			
	unsigned int sgn = (data ^ b.data) & 0x80000000;
	
	unsigned int d1 = (data&(1<<31)) ? 1+(~data) : data;
	unsigned int d2 = (b.data&(1<<31)) ? 1+(~b.data) : b.data;	
	
	
	unsigned int res = d1 * (d2>>16); // d1* int(d2)
	d2 = (d2 & 0x0000FF00)>>8; // frac(d2)	
	res+= (d1 * d2)>>8;
	
	if(sgn)
		res = 1+~res;
	
	return create_from_data(res);
}

DSC::sf24 DSC::sf24::operator -() const
{
	return create_from_data(-data);
}

DSC::sf24& DSC::sf24::operator += (const sf24& b)
{
	data+=b.data;
	return *this;
}

DSC::sf24& DSC::sf24::operator -= (const sf24& b)
{
	data-=b.data;
	return *this;
}

/*void DSC::sf24::operator *= (const sf24& b)
{
	unsigned int sgn = (data ^ b.data) & 0x80000000;
	unsigned int d1 = data, d2=b.data;
	d1 = d1>=0 ? d1: -d1;
	d2 = d2>=0 ? d2: -d2;
	
	unsigned int res = d1 * (d2>>16); // d1* int(d2)
	d2 = (d2 & 0x0000FF00)>>8; // frac(d2)	
	res+= (data * d2)>>8;
	res &= 0x7FFFFFFF;
	res |= sgn;	
	data=res;
}*/

DSC::sf24::operator int() const
{
	return get_int();
}

DSC::sf24::operator short() const
{
	return get_int();
}

bool DSC::sf24::operator < (const sf24& b) const
{
	return (int)data < (int)b.data;
}

bool DSC::sf24::operator > (const sf24& b) const
{
	return (int)data > (int)b.data;
}

bool DSC::sf24::operator <= (const sf24& b) const
{
	return (int)data <= (int)b.data;
}

bool DSC::sf24::operator >= (const sf24& b) const
{
	return (int)data >= (int)b.data;
}

bool DSC::sf24::operator == (const sf24& b) const
{
	return data == b.data;
}

bool DSC::sf24::operator != (const sf24& b) const
{
	return data != b.data;
}

DSC::sf24 DSC::sf24::create_from_data(unsigned int data)
{
	sf24 result;	
	result.data=data;
	return result;
}

bool DSC::sf24::in_range(const sf24& min, const sf24& max) const
{
	return (int)min.data<=(int)data && (int)data<=(int)max.data;
}

DSC::sf24 DSC::sf24::abs() const
{
	return (int)data>=0 ? create_from_data(data) : create_from_data(1+(~data));
}

char* DSC::sf24::to_string() const
{
	short i = get_int();
	int sgn=0;
	unsigned int f = get_frac();
	
	if(i<0) 
	{
		i=-i;
		sgn=1;
		f=-f;
	}
	
	f=10000+f*10000/256;
	
	
	char si[8];
	char sf[5];
	unsigned char li=0, lf=0;
	
	if(i==0) 
		si[0]='0', li=1;
	for(;i>0;i/=10)	si[li++]='0'+i%10;
	if(sgn==1)
		si[li++]='-';
	
	if(f==0) 
		sf[0]='0', lf=1;
	for(;f>1;f/=10)	sf[lf++]='0'+f%10;
	
	char* result = new char[li+lf+2];
	unsigned char k=0;
	for(;li--;) result[k++]=si[li];
	result[k++]='.';
	for(;lf--;) result[k++]=sf[lf];
	result[k]='\0';
	return result;
}