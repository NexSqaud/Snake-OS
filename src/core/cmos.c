#include <cmos.h>

#include <ports.h>

#define READ_CMOS_REGISTER(variable, register) \
portWriteByte(0x70, register); \
variable = portReadByte(0x71)

DateTime getCurrentDateTime()
{
	DateTime dateTime;
	
	READ_CMOS_REGISTER(dateTime.seconds, 0x80);
	READ_CMOS_REGISTER(dateTime.minutes, 0x82);
	READ_CMOS_REGISTER(dateTime.hours, 0x84);
	READ_CMOS_REGISTER(dateTime.weekDay, 0x86);
	READ_CMOS_REGISTER(dateTime.monthDay, 0x87);
	READ_CMOS_REGISTER(dateTime.month, 0x88);
	READ_CMOS_REGISTER(dateTime.year, 0x89);
	
	return dateTime;
}
