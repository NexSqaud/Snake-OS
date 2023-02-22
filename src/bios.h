#include <stddef.h>
#include <stdint.h>

#ifndef _BIOS_H
#define _BIOS_H

#define DETECT_MEMORY_EAX 0xE820
#define SMAP_SIGNATURE 0x534D4150

typedef struct 
{
 
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t acpi;
 
}__attribute__((packed)) SMapEntry;

#endif