#include <stdint.h>

#ifndef _VESA_H
#define _VESA_H

typedef struct
{
	char signature[4];
	uint16_t version;
	char* oemStringPtr;
	uint8_t capabilities;
	void* videoModesPtr;
	uint16_t memoryBlocksCount;
	
	uint16_t oemSoftwareRevision;
	char* oemVendorNamePtr;
	char* oemProductNamePtr;
	char* oemProductRevisionPtr;
	
	uint8_t reserved[222];
	uint8_t oemData[256];
} VbeInfoBlock;

#endif