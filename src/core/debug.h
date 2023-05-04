#include <stdint.h>
#include <string.h>

#ifndef _DEBUG_H
#define _DEBUG_H

#define PRINT_REGISTER(x) \
	printString(stringify(x) ": "); \
	printHex(regs.x); \
	printString("\n")

void debugInit(void);
void debugPrintString(const char* str);
void debugPrintNewString(String* str);
void debugPrintDec(uint32_t value);
void debugPrintHex(uint32_t value);
void debugPrintFloat(float value, uint8_t afterPoint);
void debugPrintFormated(const char* format, ...);

#endif