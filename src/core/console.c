#include <console.h>

#include <graphics.h>
#include <string.h>
#include <memory.h>

typedef struct
{
	String commandName;
	consoleCommandHandler handler;
} ConsoleCommand;

ConsoleCommand* commands = NULL;
uint32_t commandsLength = 10;
uint32_t commandsCount = 0;

String consoleString = {0};

void consoleAddCommand(String command, consoleCommandHandler handler)
{
	if(commands == NULL)
	{
		commands = malloc(commandsLength * sizeof(ConsoleCommand));
	}
	
	if (commandsCount == commandsLength)
	{
		commandsLength *= 2;
		ConsoleCommand* oldCommands = commands;
		commands = malloc(commandsLength * sizeof(ConsoleCommand));
		
		for(uint32_t i = 0; i < commandsCount; i++)
		{
			commands[i] = oldCommands[i];
		}
		
		free(oldCommands);
	}
	
	commands[commandsCount++] = (ConsoleCommand){ .commandName = command, .handler = handler };
}

void consoleRunCommand(String* command)
{
	for(uint32_t i = 0; i < commandsCount; i++)
	{
		if(stringIsEqual(&commands[i].commandName, command))
		{
			commands[i].handler();
			
			break;
		}
	}
}

typedef enum
{
	MainMenu,
	Game,
	GameOver,
	Console
} GameState;

extern GameState state;

bool runCommand = false;

void consoleHandleKeyboard(KeyboardKey key)
{
	if(state != Console) return;
	
	if(stringLength(keyStrings[key]) != 1 && 
		key != KeyboardKey_Backspace && 
		key != KeyboardKey_Enter) return;
	
	if(key == KeyboardKey_Backspace)
	{
		if(consoleString.count == 0) return;
		consoleString.count--;
		return;
	}
	
	if(key == KeyboardKey_Enter)
	{
		runCommand = true;
		return;
	}
	
	stringAppendChar(&consoleString, keyStrings[key][0]);
}

void consoleUpdate()
{
	if(runCommand)
	{	
		consoleRunCommand(&consoleString);
		stringClear(&consoleString);
		
		runCommand = false;
	}
}

void consoleRender()
{
	static Color clearColor = { .red = 0x0, .green = 0x0, .blue = 0x0 };
	static Color textColor = { .red = 0xDF, .green = 0xDF, .blue = 0xDF };
	
	vgaClearBuffer(clearColor);
	
	vgaPrintString(0, 0, "> ", 3, textColor);
	vgaPrintStringNew(((FONT_WIDTH + 1) * 3) * 2, 0, consoleString, 3, textColor);
	
	vgaFlipBuffer();
}