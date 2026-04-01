#pragma once

#include "raylib.h"

void initialiseTerminal(float width, float height, Font font);
void drawTerminal();
void updateTerminal();
void terminalOutput(char *string);

void toggleInfo();

void CloseTerminal();

bool IsTerminalFocused();