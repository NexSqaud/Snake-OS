#include <stdint.h>

#ifndef _DEBUG_H
#define _DEBUG_H

void debugInit(void);
void printString(const char* str);
void printDec(uint32_t num);
void printHex(uint32_t num);

#endif