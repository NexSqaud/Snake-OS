#include <utils.h>

static uint32_t nextRand = 1;

uint16_t rand(void)
{
    nextRand = nextRand * 1103515245 + 12345;
    return (uint16_t)(nextRand/65536) % 32768;
}

void srand(uint16_t seed)
{
    nextRand = seed;
}

uint32_t getTime(void)
{
	uint16_t high = 0;
	uint16_t low = 0;
	
	__asm("int $0x1A" : "=c"(high), "=d"(low) : "a"(0x0));
	
	return (high << 16) | low;
}