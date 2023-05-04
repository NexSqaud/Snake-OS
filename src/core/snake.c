#include <console.h>
#include <cmos.h>
#include <debug.h>
#include <exceptions_handlers.h>
#include <graphics.h>
#include <idt.h>
#include <keyboard.h>
#include <math.h>
#include <memory.h>
#include <rtc.h>
#include <string.h>
#include <smbios.h>
#include <utils.h>

#include <games/rt.h>

#define CELL_WIDTH 10
#define CELL_HEIGHT 10

#define CELLS_COUNT_X (SCREEN_WIDTH / CELL_WIDTH)
#define CELLS_COUNT_Y (SCREEN_HEIGHT / CELL_HEIGHT)

#define ALIGN_CENTER_X(x) (SCREEN_WIDTH / 2 - x / 2)
#define ALIGN_CENTER_Y(y) (SCREEN_HEIGHT / 2 - y / 2)

#define LOG_DEBUG(x) \
	debugPrintFormated("[%s:%d] " x, __func__, __LINE__)

#define LOG_DEBUGF(x, ...) \
	debugPrintFormated("[%s:%d] " x, __func__, __LINE__, __VA_ARGS__)

extern uint32_t video_buffer;

uint64_t frequency;

typedef enum
{
	MainMenu,
	Game,
	GameOver,
	Console
} GameState;

typedef enum
{
	Up,
	Right,
	Down,
	Left
} MoveDirection;

typedef struct
{
	int32_t x;
	int32_t y;
} Position;

typedef struct SnakeNode
{
	Position position;
	struct SnakeNode* next;
} SnakeNode;

GameState state = MainMenu;
SnakeNode* tailNode;
MoveDirection currentDirection;
Position applePosition;

void handleKeyboard(KeyboardKey key);
void gameUpdate(uint32_t delta);
void gameRender(void);

void testIntteruptionHandler(Registers regs)
{
	LOG_DEBUGF("Test interrupt handled!\n"
				"EAX: %x\n"
				"ECX: %x\n"
				"EDX: %x\n"
				"EBX: %x\n"
				"ESP: %x\n"
				"EBP: %x\n"
				"ESI: %x\n"
				"EDI: %x\n",
				regs.eax, regs.ecx, regs.edx, regs.ebx,
				regs.esp, regs.ebp, regs.esi, regs.edi);
}

void runSnakeCommand()
{
	LOG_DEBUGF("RUN SNAKE (%d)\n", state);
	state = MainMenu;
}

void __attribute__((section(".text.main"), noreturn)) main(void)
{
	(void)keyStrings;
	
	debugInit();
	LOG_DEBUG("Successfully started!\n");
	
	LOG_DEBUG("Request SMBIOS informtion\n");
	
	SMBIOSEntryPoint* point = smbiosFindEntryPoint();
	LOG_DEBUGF("SMBIOS entry point: %x\nSMBIOS Version: %d.%d\nTable address: %x\nTable length: %d\n", (uint32_t)point, point->majorVersion, point->minorVersion, point->tableAddress, point->tableLength);
	
	SMBIOSHeader* entry = smbiosFindTableEntry(0);
	
	while (entry)
	{
		LOG_DEBUGF("\tEntry %d:\n\t\tLength: %d\n\t\tAddress: %x\n\t\tHandle: %x\n", entry->type, entry->length, (uint32_t)entry, entry->handle);

		uint8_t index = 1;
		const char* string = smbiosGetString(entry, index);
		while(string)
		{
			debugPrintFormated("\t\tEntry string %d: %s\n", index, string);
			index++;
			string = smbiosGetString(entry, index);
		}
		
		if(entry->type == 4)
		{
			SMBIOSProcessorInfo* info = (SMBIOSProcessorInfo*)entry;
			
			const char* socket = smbiosGetString(entry, info->socket);
			const char* processorManufacturer = smbiosGetString(entry, info->processorManufacturer);
			const char* processorVersion = smbiosGetString(entry, info->processorVersion);
			const char* serialNumber = smbiosGetString(entry, info->serialNumber);
			const char* assetTag = smbiosGetString(entry, info->assetTag);
			const char* partNumber = smbiosGetString(entry, info->partNumber);
			
			LOG_DEBUGF("\t\tEntry size: %d/%d\n", info->header.length, sizeof(SMBIOSProcessorInfo));
			LOG_DEBUGF("\t\tSocket: %s\n", socket);
			LOG_DEBUGF("\t\tProcessor type: %d\n", info->processorType);
			LOG_DEBUGF("\t\tProcessor family: %d\n", info->processorFamily);
			LOG_DEBUGF("\t\tProcessor manufacturer: %s\n", processorManufacturer);
			LOG_DEBUGF("\t\tProcessor ID: %d\n", info->processorId);
			LOG_DEBUGF("\t\tProcessor version: %s\n", processorVersion);
			LOG_DEBUGF("\t\tVoltage: %d\n", info->voltage);
			LOG_DEBUGF("\t\tExternal clock: %d\n", info->externalClock);
			LOG_DEBUGF("\t\tMax speed: %d\n", info->maxSpeed);
			LOG_DEBUGF("\t\tCurrent speed: %d\n", info->currentSpeed);
			LOG_DEBUGF("\t\tStatus: %d\n", info->status);
			LOG_DEBUGF("\t\tProcessor upgrade: %d\n", info->processorUpgrade);
			LOG_DEBUGF("\t\tL1 cache: %d\n", info->l1CacheHandle);
			LOG_DEBUGF("\t\tL2 cache: %d\n", info->l2CacheHandle);
			LOG_DEBUGF("\t\tL3 cache: %d\n", info->l3CacheHandle);
			LOG_DEBUGF("\t\tSerial number: %s\n", serialNumber);
			LOG_DEBUGF("\t\tAsset tag: %s\n", assetTag);
			LOG_DEBUGF("\t\tPart number: %s\n", partNumber);
			LOG_DEBUGF("\t\tCore count: %d\n", info->coreCount);
			LOG_DEBUGF("\t\tCore enabled: %d\n", info->coreEnabled);
			LOG_DEBUGF("\t\tThread count: %d\n", info->threadCount);
			LOG_DEBUGF("\t\tProcessor characheristics: %d\n", info->processorCharacteristics);
		}
		
		entry = smbiosGetNextEntry(entry);
		if (((uint32_t)entry - (uint32_t)point->tableAddress) >= point->tableLength) entry = NULL;
	}
	
	LOG_DEBUG("Done!\n");
	
	interruptsInit();
	LOG_DEBUG("IDT initialized!\n");

	interruptAddHandler(0, divisionError);
	interruptAddHandler(6, invalidOpcode);
	interruptAddHandler(11, segmentNotPresent);
	interruptAddHandler(13, generalProtectionFault);
	interruptAddHandler(14, pageFault);
	
	interruptAddHandler(0x80, testIntteruptionHandler);
	
	LOG_DEBUG("Default exception handlers initialized!\n");
	LOG_DEBUG("Call test interrupt\n");
	
	__asm__("int $0x80" :: "a"(0xDEAD), "c"(0xBEEF), "d"(0xF0F0), "b"(0x1234));
	
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Initialize RTC\n");
	
	rtcInit();
	
	__asm__ ("sti");
	
	{
		uint32_t beginRtcTicks = rtcGetTicksCount();
		uint32_t currentRtcTicks = beginRtcTicks;
		
		while((currentRtcTicks = rtcGetTicksCount()) - beginRtcTicks == 0) __asm__ volatile("nop");
		
		beginRtcTicks = currentRtcTicks;
		uint64_t beginTicksCount = getTicksCount();
		
		while((currentRtcTicks = rtcGetTicksCount()) - beginRtcTicks == 0) __asm__ volatile("nop");
		
		uint64_t endTicksCount = getTicksCount();
		
		frequency = ((endTicksCount - beginTicksCount) * 1024) / (currentRtcTicks - beginRtcTicks);
	}
	
	LOG_DEBUGF("CPU Frequency: %d\n", frequency);
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Initializing keyboard\n");
	
	keyboardInit();
	
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Initializing FPU\n");
	
	__asm__("finit");
	
	float s = sqrt(4.f);

	LOG_DEBUGF("%f %f %f %f\n", s, 2.1f, 2.9f, 0.3f);
	
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Initializing VGA\n");
	
	vgaInit(video_buffer);
	
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Initializing RNG\n");
	
	srand(getTicksCount());
	
	LOG_DEBUG("Done!\n");
	LOG_DEBUG("Game statred\n");
	
	keyboardAddKeyPressedHandler(handleKeyboard);
	keyboardAddKeyPressedHandler(consoleHandleKeyboard);
	
	String snakeName = stringNewFromOld("SNAKE");
	
	consoleAddCommand(snakeName, runSnakeCommand);
	
	rtInit();
	
	Position test = { .x = 10, .y = 20 };
	LOG_DEBUGF("TEST: (%d, %d)\n", test.x, test.y);
	
	uint64_t prevTicks = rtcGetTicksCount();
	
	while (1)
	{
		uint64_t ticks = rtcGetTicksCount();
		uint32_t delta = ticks - prevTicks;
		
		if(state == Console)
		{
			consoleUpdate();
			consoleRender();
		}
		else
		{			
			gameUpdate(delta);
			gameRender();
		}
		prevTicks = ticks;
		sleep(RTC_CLOCK_FREQUENCY / 60);
	}
	
	while (1) __asm__ volatile("hlt");
}

void getRandomPosition(int32_t* x, int32_t* y)
{
	*x = rand() % CELLS_COUNT_X;
	*y = rand() % CELLS_COUNT_Y;
}

void getNextCellPosition(uint32_t x, uint32_t y, int32_t* rx, int32_t* ry)
{
	switch(currentDirection)
	{
		case Up:
			*rx = x;
			*ry = y - 1;
		break;
		case Right:
			*rx = x + 1;
			*ry = y;
		break;
		case Down:
			*rx = x;
			*ry = y + 1;
		break;
		case Left:
			*rx = x - 1;
			*ry = y;
		break;
	}
}

bool isInsideSnake(Position position)
{
	SnakeNode* node = tailNode;
	
	while(node)
	{
		if(position.x == node->position.x &&
			position.y == node->position.y)
		{
			return true;
		}
		
		node = node->next;
	}
	
	return false;
}

bool isSnakeOutside(void)
{
	SnakeNode* node = tailNode;
	
	while(node)
	{
		if(node->position.x < 0 || node->position.x >= CELLS_COUNT_X
			|| node->position.y < 0 || node->position.y >= CELLS_COUNT_Y)
		{
			return true;
		}
		
		node = node->next;
	}
	
	return false;
}

bool isSnakeInsideItself(void)
{
	SnakeNode* node = tailNode;
	
	while(node)
	{
		SnakeNode* checkNode = node->next;
		
		while(checkNode)
		{
			if(checkNode->position.x == node->position.x &&
				checkNode->position.y == node->position.y)
			{
				return true;
			}
			
			checkNode = checkNode->next;
		}
		
		node = node->next;
	}
	
	return false;
}

bool frameKeyHandled = false;

void handleKeyboard(KeyboardKey key)
{
	if(frameKeyHandled) return;
	
	if(key == KeyboardKey_W)
	{
		if(currentDirection != Down)
		{
			currentDirection = Up;
			return;
		}
	}
	if(key == KeyboardKey_D)
	{
		if(currentDirection != Left)
		{
			currentDirection = Right;
			return;
		}
	}
	if(key == KeyboardKey_S)
	{
		if(currentDirection != Up)
		{
			currentDirection = Down;
			return;
		}
	}
	if(key == KeyboardKey_A)
	{
		if(currentDirection != Right)
		{
			currentDirection = Left;
			return;
		}
	}
	
	if (key == KeyboardKey_Escape && state != Game)
	{
		state = Console;
	}
	
	frameKeyHandled = true;
}

void spawnNewApple(void)
{
	do
	{
		getRandomPosition(&applePosition.x, &applePosition.y);
	} while(isInsideSnake(applePosition));
}

void initSnake(void)
{
	tailNode = (SnakeNode*)malloc(sizeof(SnakeNode));
	tailNode->next = (SnakeNode*)malloc(sizeof(SnakeNode));
	tailNode->next->next = (SnakeNode*)malloc(sizeof(SnakeNode));
	
	do
	{
		currentDirection = rand() % 4;
		getRandomPosition(&tailNode->position.x, &tailNode->position.y);
		
		getNextCellPosition(tailNode->position.x, tailNode->position.y,
							&tailNode->next->position.x, &tailNode->next->position.y);
		getNextCellPosition(tailNode->next->position.x, tailNode->next->position.y,
							&tailNode->next->next->position.x, &tailNode->next->next->position.y);
	} while(isSnakeOutside());
	
	spawnNewApple();
}

void gameUpdate(uint32_t delta)
{
	static uint32_t lastUpdateDelta = 0;
	lastUpdateDelta += delta;
	if(state == Game && lastUpdateDelta < (RTC_CLOCK_FREQUENCY / 100)) return;
	lastUpdateDelta = 0;
	
	switch(state)
	{
		case MainMenu:
			if(keyboardIsAnyKeyPressed())
			{
				initSnake();
				
				state = Game;
			}
		break;
		case Game:
			SnakeNode* node = tailNode;
			
			while(node->next)
			{
				node = node->next;
			}
			
			SnakeNode* head = (SnakeNode*)malloc(sizeof(SnakeNode));
			node->next = head;
			getNextCellPosition(node->position.x, node->position.y, &head->position.x, &head->position.y);
			
			if(!isInsideSnake(applePosition))
			{
				node = tailNode;
				tailNode = tailNode->next;
				free(node);
			}
			else
			{
				spawnNewApple();
			}
			
			if(isSnakeOutside() || isSnakeInsideItself())
			{
				debugPrintString("Game over!\n");
				node = tailNode;
				while(node)
				{
					SnakeNode* nextNode = node->next;
					free(node);
					node = nextNode;
				}
				state = GameOver;
			}
			
		break;
		case GameOver:
			if(keyboardIsAnyKeyPressed())
			{
				initSnake();
				
				state = Game;
			}
		break;
		case Console:
			
		break;
	}
	
	frameKeyHandled = false;
}

void gameRender(void)
{
	static Color clearColor = {0};
	static Color textColor = { .red = 0xFF, .green = 0xFF, .blue = 0xFF };
	static Color snakeColor = { .red = 0xFF, .green = 0xFF, .blue = 0xFF };
	static Color appleColor = { .red  = 0xFF,  .green = 0x20, .blue = 0x20 };
	
	static const char* headerText = "SNAKE-OS";
	static const char* pressAnyButtonText = "Press any button to start";
	static const char* gameOverText = "Game over!";
	
	vgaClearBuffer(clearColor);
	switch(state)
	{
		case MainMenu:
			vgaPrintString(ALIGN_CENTER_X(vgaMeasureString(headerText, 10)), 50, headerText, 10, textColor);
			vgaPrintString(ALIGN_CENTER_X(vgaMeasureString(pressAnyButtonText, 3)), SCREEN_HEIGHT - 50, pressAnyButtonText, 3, textColor);
		break;
		case Game:
			SnakeNode* node = tailNode;
			while(node)
			{
				vgaFillRectangle(node->position.x * CELL_WIDTH, node->position.y * CELL_HEIGHT, 
								CELL_WIDTH, CELL_HEIGHT, snakeColor);
				
				node = node->next;
			}
			
			vgaFillRectangle(applePosition.x * CELL_WIDTH, applePosition.y * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, appleColor);
		break;
		case GameOver:
			vgaPrintString(ALIGN_CENTER_X(vgaMeasureString(gameOverText, 10)), 50, gameOverText, 10, textColor);
			vgaPrintString(ALIGN_CENTER_X(vgaMeasureString(pressAnyButtonText, 3)), SCREEN_HEIGHT - 50, pressAnyButtonText, 3, textColor);
		break;
		case Console:
			
		break;
	}
	
#if 0
	static Color debugColor = { .red = 0xFF, .green = 0xFF, .blue = 0x7F };
	static const char* frequencyText = "Frequency: ";
	
	char* frequencyStr = stringifyDec(frequency);
	
	vgaPrintString(0, 0, frequencyText, 2, debugColor);
	vgaPrintString(vgaMeasureString(frequencyText, 2), 0, frequencyStr, 2, debugColor);
	
	free(frequencyStr);
#endif
	
	vgaFlipBuffer();
}
