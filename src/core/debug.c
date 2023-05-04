#include <debug.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <ports.h>

#define COM1_PORT 0x3F8

static const char conversionTable[] = "0123456789ABCDEF";

uint8_t debugInitFailed = 0;

void debugInit(void)
{
	portWriteByte(COM1_PORT + 1, 0x00);
	portWriteByte(COM1_PORT + 3, 0x80);
	portWriteByte(COM1_PORT + 0, 0x03);
	portWriteByte(COM1_PORT + 1, 0x00);
	portWriteByte(COM1_PORT + 3, 0x03);
	portWriteByte(COM1_PORT + 2, 0xC7);
	portWriteByte(COM1_PORT + 4, 0x0B);
	portWriteByte(COM1_PORT + 4, 0x1E);
	portWriteByte(COM1_PORT + 0, 0xAE);
	
	if(portReadByte(COM1_PORT) != 0xAE)
	{
		debugInitFailed = 1;
		return;
	}
	
	portWriteByte(COM1_PORT + 4, 0x0F);
	return;
}

uint8_t isCom1Ready()
{
	return portReadByte(COM1_PORT + 5) & 0x20;
}

void com1WriteChar(char ch)
{
	while(isCom1Ready() == 0);
	
	portWriteByte(COM1_PORT, ch);
}

void debugPrintString(const char* str)
{
	if(str == NULL)
	{
		debugPrintString("NULL");
		return;
	}
	
	while(*str != 0)
	{
		com1WriteChar(*str);
		str++;
	}
}

void debugPrintNewString(String* str)
{
	if(str == NULL)
	{
		debugPrintString("NULL");
		return;
	}
	
	if(str->buffer == NULL)
	{
		debugPrintString("NULL BUFFER");
		return;
	}
	
	for(int i = 0; i < str->count; i++)
	{
		com1WriteChar(str->buffer[i]);
	}
}

void debugPrintDec(uint32_t value)
{
	uint32_t n, d = 1000000000;

    while ((value / d == 0) && (d >= 10)) {
        d /= 10;
    }
    n = value;

    while (d >= 9) {
        com1WriteChar(conversionTable[n / d]);
        n = n % d;
        d /= 10;
    }
    com1WriteChar(conversionTable[n]);
}

void debugPrintHex(uint32_t value) 
{
    uint32_t n, d = 0x10000000;

    debugPrintString("0x");

    while ((value / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = value;

    while (d >= 0xF) {
        com1WriteChar(conversionTable[n / d]);
        n = n % d;
        d /= 0x10;
    }
    com1WriteChar(conversionTable[n]);
}

void debugPrintFloat(float value, uint8_t afterPoint)
{
	if (isnan(value)) 
	{
		debugPrintString("NaN");
		return;
	}
	
	if(value < 0.f)
	{
		com1WriteChar('-');
		value = -value;
	}
	
	if(value == POSITIVE_INFINITY)
	{
		debugPrintString("inf");
		return;
	}
	
	uint32_t decimalPart = (uint32_t)value;
	uint32_t interalPart = (uint32_t)(((value - (float)decimalPart) * pow(10, afterPoint)) + 0.5f);
	
	debugPrintDec(decimalPart);
	if(afterPoint > 0)
	{
		com1WriteChar('.');
		debugPrintDec(interalPart);
	}
}

void debugPrintFormated(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	uint32_t stackCount = 0;
	
	while(*format)
	{
		if(*format == '%')
		{
			format++;
			switch(*format)
			{
				case 'd':
				case 'i':
					debugPrintDec(va_arg(args, uint32_t));
					break;
				case 'x':
				case 'X':
				case 'p':
					debugPrintHex(va_arg(args, uint32_t));
					break;
				case 'f':
					debugPrintFloat(va_arg(args, double), 2);
					stackCount++;
					break;
				case 's':
					debugPrintString(va_arg(args, char*));
					break;
				case 'c':
					com1WriteChar(va_arg(args, char));
					break;
				case 'n':
					{
						if(*(format + 1) == 's')
						{
							debugPrintNewString(va_arg(args, String*));
						}
						else
						{
							com1WriteChar('n');
							com1WriteChar(*format);
						}
						format++;
					}
					break;
				default:
					com1WriteChar(*format);
					break;
			}
			stackCount++;
		}
		else
		{
			com1WriteChar(*format);
		}
		format++;
	}
	
	va_start(args, format);
	
#if 0
	debugPrintString("Stack content:\n");
	for(uint32_t i = 0; i < stackCount; i++)
	{
		com1WriteChar('\t');
		
		debugPrintDec(i + 1);
		debugPrintString(" (");
		debugPrintHex((uint32_t)args);
		com1WriteChar(')');
		debugPrintString(": ");
		debugPrintHex(va_arg(args, uint32_t));
		
		com1WriteChar('\n');
	}
#endif
	
	//va_end(args);
}