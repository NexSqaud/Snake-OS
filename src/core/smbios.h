#include <stdint.h>

#ifndef _SMBIOS_H
#define _SMBIOS_H

typedef struct
{
	char signature[4];
	uint8_t checksum;
	uint8_t length;
	uint8_t majorVersion;
	uint8_t minorVersion;
	uint16_t maxStructSize;
	uint8_t revision;
	char formattedArea[5];
	char string[5];
	uint8_t checksum2;
	uint16_t tableLength;
	uint32_t tableAddress;
	uint16_t numberOfStructs;
	uint8_t bcdRevision;
} __attribute__((packed)) SMBIOSEntryPoint;

typedef struct
{
	uint8_t type;
	uint8_t length;
	uint16_t handle;
} __attribute__((packed)) SMBIOSHeader;

typedef struct
{
	SMBIOSHeader header;
	uint8_t socket;
	uint8_t processorType;
	uint8_t processorFamily;
	uint8_t processorManufacturer;
	uint64_t processorId;
	uint8_t processorVersion;
	uint8_t voltage;
	uint16_t externalClock;
	uint16_t maxSpeed;
	uint16_t currentSpeed;
	uint8_t status;
	uint8_t processorUpgrade;
	uint16_t l1CacheHandle;
	uint16_t l2CacheHandle;
	uint16_t l3CacheHandle;
	uint8_t serialNumber;
	uint8_t assetTag;
	uint8_t partNumber;
	uint8_t coreCount;
	uint8_t coreEnabled;
	uint8_t threadCount;
	uint16_t processorCharacteristics;
	uint16_t processorFamily2;
} __attribute__((packed)) SMBIOSProcessorInfo;

SMBIOSEntryPoint* smbiosFindEntryPoint(void);
SMBIOSHeader* smbiosGetNextEntry(SMBIOSHeader* header);
SMBIOSHeader* smbiosFindTableEntry(uint8_t type);
const char* smbiosGetString(SMBIOSHeader* header, uint8_t index);

#endif