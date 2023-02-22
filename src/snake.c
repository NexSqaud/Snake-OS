__asm (".code16gcc");

#include <stddef.h>
#include <debug.h>
#include <graphics.h>
#include <memory.h>
#include <utils.h>

#define CELL_WIDTH 10
#define CELL_HEIGHT 10

#define CELLS_COUNT_X (SCREEN_WIDTH / CELL_WIDTH)
#define CELLS_COUNT_Y (SCREEN_HEIGHT / CELL_HEIGHT)

#define LOG(x) printString("["); \
				printString(__FUNCTION__); \
				printString(":" stringify(__LINE__) "]: " x "\n")

typedef enum
{
	MainMenu,
	Game,
	GameOver
} GameState;

typedef enum
{
	Up,
	Right,
	Down,
	Left
} MoveDirection;

typedef struct SnakeNode
{
	int16_t x;
	int16_t y;
	struct SnakeNode* next;
} SnakeNode;

void update(uint8_t delta);
void render(void);

void __attribute__ ((section (".text.main"))) main(void) 
{
	debugInit();
	
	LOG("Debug initialized");
	
	memoryInit();
	LOG("Memory managment initialized");
	
	vgaInit();
	LOG("Video initialized");
	
	LOG("Initialization done!");
	
	srand(getTime());
	
	uint32_t lastTime = getTime();
	
	while(true)
	{
		uint32_t currentTime = getTime();
		uint8_t deltaTime = currentTime - lastTime;
		
		update(deltaTime);
		render();
		
		lastTime = currentTime;
	}
	
	while(true) __asm("hlt");
}

GameState state = MainMenu;
uint8_t pressedKey;

SnakeNode* tailNode;
MoveDirection currentDirection;

void handleKeyboard(void)
{
	__asm(
			"movw $0, %%ax\n"
			"movb $1, %%ah\n"
			"int $0x16\n"
			"movb %%al, (%0)"
			:: "b"(&pressedKey) : "ax");
	
	if(pressedKey)
	{
		__asm("int $0x16" :: "a"(0x0));
	}
}

void getNextCellPosition(uint16_t x, uint16_t y, uint16_t* rx, uint16_t* ry)
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

uint8_t isSnakeOutside(void)
{
	SnakeNode* node = tailNode;
	
	while(node)
	{
		if(node->x < 0 || node->x >= CELLS_COUNT_X
			|| node->y < 0 || node->y >= CELLS_COUNT_Y)
		{
			return 1;
		}
		
		node = node->next;
	}
	
	return 0;
}

void update(uint8_t delta)
{	
	static uint8_t lastUpdateDelta = 0;
	
	lastUpdateDelta += delta;
	if(state == Game && lastUpdateDelta < 10) return;
	lastUpdateDelta = 0;

	handleKeyboard();
	switch(state)
	{
		case MainMenu:
			
			if(pressedKey)
			{
				tailNode = (SnakeNode*)malloc(sizeof(SnakeNode));
				tailNode->next = (SnakeNode*)malloc(sizeof(SnakeNode));
				tailNode->next->next = (SnakeNode*)malloc(sizeof(SnakeNode));
				
				do
				{
					currentDirection = rand() % 4;
					tailNode->x = rand() % CELLS_COUNT_X;
					tailNode->y = rand() % CELLS_COUNT_Y;
					
					getNextCellPosition(tailNode->x, tailNode->y, &tailNode->next->x, &tailNode->next->y);
					getNextCellPosition(tailNode->next->x, tailNode->next->y, &tailNode->next->next->x, &tailNode->next->next->y);
				} while(isSnakeOutside());
				
				state = Game;
				pressedKey = 0;
			}
			
		break;
		case Game:
		{
			if(pressedKey)
			{
				switch(pressedKey)
				{
					case 'w':
						if(currentDirection != Down)
						{
							currentDirection = Up;
						}
					break;
					case 'd':
						if(currentDirection != Left)
						{
							currentDirection = Right;
						}
					break;
					case 's':
						if(currentDirection != Up)
						{
							currentDirection = Down;
						}
					break;
					case 'a':
						if(currentDirection != Right)
						{
							currentDirection = Left;
						}
					break;
				}
				
				pressedKey = 0;
			}
			
			SnakeNode* node = tailNode;
			tailNode = tailNode->next;
			
			free(node);
			node = tailNode;
			
			while(node->next)
			{
				node = node->next;
			}
			
			SnakeNode* head = (SnakeNode*)malloc(sizeof(SnakeNode));
			node->next = head;
			getNextCellPosition(node->x, node->y, &head->x, &head->y);
			
			if(isSnakeOutside())
			{
				printString("Game over!\n");
				node = tailNode;
				while(node)
				{
					SnakeNode* nextNode = node->next;
					free(node);
					node = nextNode;
				}
				state = GameOver;
			}
		}
		break;
		case GameOver:
			if(pressedKey)
			{
				tailNode = (SnakeNode*)malloc(sizeof(SnakeNode));
				tailNode->next = (SnakeNode*)malloc(sizeof(SnakeNode));
				tailNode->next->next = (SnakeNode*)malloc(sizeof(SnakeNode));
				
				do
				{
					currentDirection = rand() % 4;
					tailNode->x = rand() % CELLS_COUNT_X;
					tailNode->y = rand() % CELLS_COUNT_Y;
					
					getNextCellPosition(tailNode->x, tailNode->y, &tailNode->next->x, &tailNode->next->y);
					getNextCellPosition(tailNode->next->x, tailNode->next->y, &tailNode->next->next->x, &tailNode->next->next->y);
				} while(isSnakeOutside());
				
				state = Game;
				pressedKey = 0;
			}
		break;
	}
}

void render(void)
{
	switch(state)
	{
		case MainMenu:
			
			vgaClearBuffer(0x0);
			
			vgaPrintString(40, 30, "SNAKE-OS", 5, 0xF);
			vgaPrintString(85, 170, "Press any button to start", 1, 0xF);
			
			vgaFlipBuffer();
			
		break;
		case Game:
			vgaClearBuffer(0x0);
			
			SnakeNode* node = tailNode;
			while(node)
			{
				vgaFillRectangle(node->x * CELL_WIDTH, node->y * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, 0xF);
				
				node = node->next;
			}
			
			vgaFlipBuffer();
		break;
		case GameOver:
			vgaClearBuffer(0x0);
			
			vgaPrintString(20, 30, "Game over!", 5, 0xF);
			vgaPrintString(85, 170, "Press any button to start", 1, 0xF);
			
			vgaFlipBuffer();
		break;
	}
}