#include <keyboard.h>
#include <string.h>

#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef void(*consoleCommandHandler)();

void consoleAddCommand(String command, consoleCommandHandler handler);
void consoleRunCommand(String* command);

void consoleHandleKeyboard(KeyboardKey key);
void consoleUpdate();
void consoleRender();

#endif