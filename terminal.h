#pragma once

#include "raylib.h"

void InitialiseTerminal(float x, float y, float width, float height, Font font);
void DrawTerminal();
void UpdateTerminal();
void terminalOutput(char *string);
void CloseTerminal();

bool IsAnyKeyPressed();