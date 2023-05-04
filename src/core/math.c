#include <math.h>

float pow(float x, uint32_t y)
{
	float res = x;
	
	for(uint32_t i = 0; i < y; i++)
	{
		res *= res;
	}
	
	return res;
}

float sqrt(float x)
{	
	float res = 0.f;
	__asm__("fsqrt" : "=t"(res) : "0"(x));
	return res;
}

bool isnan(float x)
{		
	union {
		float xf;
		uint32_t xi;
	} u;
	u.xf = x;
	
	// NaN looks like:  ?    11111111  ???????????????????????
	//                sign   exponent          mantisa
	
	// 0xFF is:              11111111
	
	// after XOR NaN should be: 000000000???????????????????????
	// where ? - unknown bit
	// at least one bit should be 1
	
	return !((u.xi & (0xFF << 23)) ^ (0xFF << 23)) && (((u.xi & ~(1 << 31)) ^ (0xFF << 23)) != 0);
}