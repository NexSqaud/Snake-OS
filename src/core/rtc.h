#include <stdint.h>

#ifndef _RTC_H
#define _RTC_H

#define RTC_CLOCK_FREQUENCY 1024

void rtcInit(void);
uint32_t rtcGetTicksCount(void);

#endif