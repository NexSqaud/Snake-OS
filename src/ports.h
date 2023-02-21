#include <stdint.h>

#ifndef _PORTS_H
#define _PORTS_H

void writePortByte(uint16_t port, uint8_t data);
void writePortWord(uint16_t port, uint16_t data);
void writePortLong(uint16_t port, uint32_t data);

uint8_t readPortByte(uint16_t port);
uint16_t readPortWord(uint16_t port);
uint32_t readPortLong(uint16_t port);

#endif