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

typedef struct
{
	uint16_t attributes;
	uint8_t windowA;
	uint8_t windowB;
	uint16_t granularity;		
	uint16_t windowSize;
	uint16_t segmentA;
	uint16_t segmentB;
	uint32_t windowFunctionPtr;
	uint16_t pitch;
	
	uint16_t width;
	uint16_t height;
	uint8_t wChar;
	uint8_t yChar;
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;
	uint8_t memoryModel;
	uint8_t bankSize;
	uint8_t imagePages;
	uint8_t reserved0;
 
	uint8_t redMask;
	uint8_t redPosition;
	uint8_t greenMask;
	uint8_t greenPosition;
	uint8_t blueMask;
	uint8_t bluePosition;
	uint8_t reservedMask;
	uint8_t reservedPosition;
	uint8_t directColorAttributes;
 
	uint32_t framebuffer;
	uint32_t offScreenMemOff;
	uint16_t offScreenMemSize;
	uint8_t reserved1[206];
} __attribute__ ((packed)) VBEModeInfoStructure;

typedef struct
{
	uint8_t signature[4];
	uint16_t version;
	uint32_t oemStringPtr;
	uint32_t capabilities;
	uint16_t* videoModesListPtr;
	uint16_t totalMemory;
	
	uint16_t oemSoftwareRevision;
	uint32_t oemVendorNamePtr;
	uint32_t oemProductNamePtr;
	uint32_t oemProductRevisionPtr;
	uint8_t reserved[222];
	
	uint8_t oemData[256];
} __attribute__ ((packed)) VBEInfoBlock;

#endif