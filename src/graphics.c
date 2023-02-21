#include <graphics.h>

#include <memory.h>

#define FRONT_BUFFER_ADDRESS 0xA0000

uint8_t* backBuffer;

void vgaSetFrontbufferPixel(uint16_t x, uint16_t y, uint8_t color)
{
	static uint8_t* frontBuffer = (uint8_t*)FRONT_BUFFER_ADDRESS;
	//__asm("int $0x10" :: "a"(0x0c00 | color), "c"(x), "d"(y));
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT || frontBuffer == NULL) return;
	
	frontBuffer[y * SCREEN_WIDTH + x] = color;
}

void vgaSetPixel(uint16_t x, uint16_t y, uint8_t color)
{
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT || backBuffer == NULL) return;
	
	backBuffer[y * SCREEN_WIDTH + x] = color;
}

int16_t vgaFontIndex(const char ch)
{
	for(size_t i = 0; i < sizeof(alphabet) - 1; i++)
	{
		if(ch == alphabet[i]) return i;
	}
	
	return -1;
}

void vgaPrintString(uint16_t x, uint16_t y, const char* str, uint8_t scale, uint8_t color)
{
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;
	
	for(size_t i = 0; *str; i++, str++)
	{
		int16_t fontIndex = vgaFontIndex(*str);
		if(fontIndex == -1) 
		{
			continue;
		}
		
		uint16_t dx = x + ((FONT_WIDTH + 1) * scale) * i;
		uint16_t dy = y;
		
		for(int x = 0; x < FONT_WIDTH * scale; x++)
		{
			for(int y = 0; y < FONT_HEIGHT * scale; y++)
			{
				uint8_t px = x / scale;
				uint8_t py = y / scale;
				
				if(font[fontIndex][px] & (0b1 << py))
				{
					vgaSetPixel(dx + x, dy + y, color);
				}
			}
		}
	}
}

void vgaFillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color)
{
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;
	
	for(size_t dy = 0; dy < height; dy++)
	{
		for(size_t dx = 0; dx < width; dx++)
		{
			vgaSetPixel(x + dx, y + dy, color);
		}
	}
}

void vgaFillCircle(uint16_t x, uint16_t y, uint8_t radius, uint8_t color)
{
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;
	
	for(size_t dy = 0; dy < radius * 2; dy++)
	{
		for(size_t dx = 0; dx < radius * 2; dx++)
		{
			if((x + dx) * (x + dx) + (y + dy) * (y * dy) < radius * radius)
			{
				vgaSetPixel(x + dx, y + dy, color);
			}
		}
	}
}

void vgaFlipBuffer(void)
{
	if(backBuffer == NULL) return;
	
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		vgaSetFrontbufferPixel(i % SCREEN_WIDTH, i / SCREEN_WIDTH, backBuffer[i]);
	}
}

void vgaClearBuffer(uint8_t color)
{
	if(backBuffer == NULL) return;
	
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		backBuffer[i] = color;
	}
}

void vgaInit(void)
{
	printString("Set video mode\n");
	__asm ("int $0x10" :: "a"(0x03));
	__asm ("int $0x10" :: "a"(0x13));
	
	backBuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
}