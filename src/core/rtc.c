#include <rtc.h>

#include <debug.h>
#include <idt.h>
#include <ports.h>
#include <utils.h>

uint32_t clockTicks = 0;

void clockInterruptHandler(Registers regs)
{
	(void)regs;
	clockTicks++;
	
	portWriteByte(0x70, 0x0C);
	portReadByte(0x71);
}

void rtcInit(void)
{
	interruptAddHandler(0x28, clockInterruptHandler);
	
	portWriteByte(0x70, 0x8B);
	uint8_t previous = portReadByte(0x71);
	portWriteByte(0x70, 0x8B);
	portWriteByte(0x71, previous | 0x40);
}

uint32_t rtcGetTicksCount(void)
{
	return clockTicks;
}
