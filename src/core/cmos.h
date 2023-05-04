#include <stdint.h>

#ifndef _CMOS_H
#define _CMOS_H

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t weekDay;
	uint8_t monthDay;
	uint8_t month;
	uint8_t year;
} DateTime;

DateTime getCurrentDateTime();

#endif