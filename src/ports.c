#include <ports.h>

void writePortByte(uint16_t port, uint8_t data)
{
	__asm("outb %0, %1" :: "a"(data), "Nd"(port));
}

void writePortWord(uint16_t port, uint16_t data)
{
	__asm("outw %0, %1" :: "a"(data), "Nd"(port));
}

void writePortLong(uint16_t port, uint32_t data)
{
	__asm("outl %0, %1" :: "a"(data), "Nd"(port));
}

uint8_t readPortByte(uint16_t port)
{
	uint8_t value;
	__asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

uint16_t readPortWord(uint16_t port)
{
	uint8_t value;
	__asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

uint32_t readPortLong(uint16_t port)
{
	uint8_t value;
	__asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}
