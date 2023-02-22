#include <memory.h>

#include <stddef.h>
#include <stdint.h>

#include <bios.h>
#include <debug.h>

typedef enum
{
	Null = 0x0000,
	Free = 0xDEAD,
	Alloced = 0xBEEF
} AllocNodeMagic;

typedef struct AllocNode
{
	uint16_t magic;
	struct AllocNode* previous;
	size_t size;
} AllocNode;

void* heapStart = (void*)0x7E00;
void* heapFarestAlloc = (void*)0x7E00;
void* heapEnd = (void*)0x7FFFF;

void memoryInit(void)
{
	// TODO: Fix
	/*
	uint32_t continiousId = 0;
	size_t entries = 0;
	uint32_t signature;
	size_t bytesCount;
	
	SMapEntry* buffer = (SMapEntry*)0x7E00;
	SMapEntry* entry = buffer;
	size_t entriesMax = 0x2000 / sizeof(SMapEntry);
	
	do
	{
		__asm__ __volatile__ ("int $0x15"
							: "=a"(signature), "=c"(bytesCount), "=b"(continiousId)
							: "a"(DETECT_MEMORY_EAX), "b"(continiousId), "c"(24), "d"(SMAP_SIGNATURE), "D"(entry));
		
		if(signature != SMAP_SIGNATURE)
		{
			// fail
			break;
		}
		
		if(bytesCount > 20 && (entry->acpi & 0x0001) == 0)
		{
			// ignore
		}
		else 
		{
			entry++;
			entries++;
		}
		
	} while(continiousId != 0 && entries < entriesMax);
	
	SMapEntry* maxEntry = NULL;
	
	for(size_t i = 0; i < entries; i++)
	{
		entry = buffer + i;
		if(entry->type != 1)
		{
			continue;
		}
		
		if(maxEntry == NULL || entry->length > maxEntry->length)
		{
			maxEntry = entry;
		}
	}
	
	heapStart = (void*)((uint32_t)maxEntry->base);
	heapFarestAlloc = heapStart;
	heapEnd = (void*)((uint32_t)maxEntry->base + maxEntry->length);
	*/
	printString("Heap start found at: ");
	printHex(heapStart);
	printString("\n");
	
}

void* malloc(size_t size)
{	
	AllocNode* node = (AllocNode*)heapStart;
	AllocNode* previous = NULL;
	while(node != heapEnd && node != heapFarestAlloc)
	{
		if(node->magic == Free && node->size >= size)
		{
			if(node->size > size + sizeof(AllocNode))
			{
				AllocNode* spliter = (AllocNode*)((void*)node + size + sizeof(AllocNode));
				spliter->magic = Free;
				spliter->size = node->size - size - sizeof(AllocNode);
				spliter->previous = node;
				((AllocNode*)((void*)spliter + spliter->size))->previous = spliter;
				node->size -= spliter->size + sizeof(AllocNode);
			}
			break;
		}
		previous = node;
		node = (AllocNode*)((void*)node + node->size + sizeof(AllocNode));
	}
	
	printString("Found block at ");
	printHex((uint32_t)node);
	printString(" (");
	printHex((uint32_t)((void*)node + sizeof(AllocNode)));
	printString(")\n");
	
	if(node->magic == Null)
	{
		node->size = size;
		node->previous = previous;
		heapFarestAlloc += size + sizeof(AllocNode);
	}
	
	node->magic = Alloced;
	
	for(size_t i = 0; i < size; i++)
	{
		*((uint8_t*)node + sizeof(AllocNode) + i) = 0;
	}
	
	return (void*)node + sizeof(AllocNode);
}

void* calloc(size_t count, size_t size)
{
	return malloc(count * size);
}

void free(void* ptr)
{
	AllocNode* node = (AllocNode*)(ptr - sizeof(AllocNode));
	if(node->magic != Alloced)
	{
		return;
	}
	
	while(node->previous != NULL)
	{
		if(node->previous->magic == Free)
		{
			size_t size = node->size + sizeof(AllocNode);
			node = node->previous;
			node->size += size;
		}
		else 
		{
			break;
		}
	}
	
	node->magic = Free;
}