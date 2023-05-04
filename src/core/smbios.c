#include <smbios.h>

#include <stddef.h>

SMBIOSEntryPoint* entryPoint = NULL;

SMBIOSEntryPoint* smbiosFindEntryPoint(void)
{
	static void* startAddress = (void*)0xF0000;
	static void* endAddress = (void*)0xFFFFF;
	
	void* currentAddress = startAddress;
	
	while (currentAddress < endAddress)
	{
		SMBIOSEntryPoint* point = (SMBIOSEntryPoint*)currentAddress;
		if (point->signature[0] == '_' && point->signature[1] == 'S' && point->signature[2] == 'M' && point->signature[3] == '_')
		{
			entryPoint = point;
			return point;
		}
		
		currentAddress += 16;
	}
	
	return NULL;
}

SMBIOSHeader* smbiosGetNextEntry(SMBIOSHeader* entry)
{
	char* stringsPointer = (char*)entry + entry->length;
	
	uint32_t i = 1;
	while (stringsPointer[i - 1] != '\0' || stringsPointer[i] != '\0') i++;
	
	return (SMBIOSHeader*)(stringsPointer + i + 1);
}

const char* smbiosGetString(SMBIOSHeader* header, uint8_t index)
{
	if(index == 0) return NULL;
	
	char* stringsPointer = (char*)header + header->length;
	
	index--;
	
	while (index > 0)
	{
		uint32_t i = 0;
		while (stringsPointer[i] != '\0') i++;
		if (stringsPointer[i + 1] == '\0') return NULL;
		stringsPointer += i + 1;
		index--;
	}

	return stringsPointer;
}

SMBIOSHeader* smbiosFindTableEntry(uint8_t type)
{
	if (entryPoint == NULL)
	{
		smbiosFindEntryPoint();
	}
	
	if (entryPoint == NULL)
	{
		return NULL;
	}
	
	SMBIOSHeader* entry = (SMBIOSHeader*)entryPoint->tableAddress;
	
	while (entry->type != type)
	{
		if (((uint32_t)entry - (uint32_t)entryPoint->tableAddress) >= entryPoint->tableLength) return NULL;
		entry = smbiosGetNextEntry(entry);
	}
	
	return entry;
}
