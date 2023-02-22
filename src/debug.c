#include <debug.h>

#include <stdint.h>

#include <ports.h>

#define COM1_PORT 0x3F8

static const char conversionTable[] = "0123456789ABCDEF";

uint8_t debugInitFailed = 0;

void debugInit(void)
{
	writePortByte(COM1_PORT + 1, 0x00);
	writePortByte(COM1_PORT + 3, 0x80);
	writePortByte(COM1_PORT + 0, 0x03);
	writePortByte(COM1_PORT + 1, 0x00);
	writePortByte(COM1_PORT + 3, 0x03);
	writePortByte(COM1_PORT + 2, 0xC7);
	writePortByte(COM1_PORT + 4, 0x0B);
	writePortByte(COM1_PORT + 4, 0x1E);
	writePortByte(COM1_PORT + 0, 0xAE);
	
	if(readPortByte(COM1_PORT) != 0xAE)
	{
		debugInitFailed = 1;
		return;
	}
	
	writePortByte(COM1_PORT + 4, 0x0F);
	return;
}

uint8_t isCom1Ready()
{
	return readPortByte(COM1_PORT + 5) & 0x20;
}

void com1WriteChar(char ch)
{
	while(isCom1Ready() == 0);
	
	writePortByte(COM1_PORT, ch);
}

void printString(const char* str)
{
	while(*str != 0)
	{
		com1WriteChar(*str);
		str++;
	}
}

void printDec(uint32_t num)
{
	uint32_t n, d = 10000000;

    while ((num / d == 0) && (d >= 10)) {
        d /= 10;
    }
    n = num;

    while (d >= 9) {
        com1WriteChar(conversionTable[n / d]);
        n = n % d;
        d /= 10;
    }
    com1WriteChar(conversionTable[n]);
}

void printHex(uint32_t num) 
{
    uint32_t n, d = 0x10000000;

    printString("0x");

    while ((num / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = num;

    while (d >= 0xF) {
        com1WriteChar(conversionTable[n / d]);
        n = n % d;
        d /= 0x10;
    }
    com1WriteChar(conversionTable[n]);
}

// TODO: printFormated