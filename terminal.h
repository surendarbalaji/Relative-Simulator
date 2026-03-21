#pragma once

#include "raylib.h"

void InitialiseTerminal(float width, float height, Font font);
void DrawTerminal();
void UpdateTerminal();
void terminalOutput(char *string);

void toggleInfo();

void CloseTerminal();

bool IsTerminalFocused();