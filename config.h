#pragma once
#include "raylib.h"

#define LIGHTERGRAY (Color) { 235, 235, 235, 230 }
#define SPACEBLUETEXT (Color) { 10, 32, 90, 180 }
#define SPACEBLUE (Color) { 10, 50, 90, 230 }

#define STRING_CHARACTERS_MAX (44 + 1)
#define TERMINAL_HISTORY_MAX 100

#define C 299792.458 // (km/s) i feel like this definition being just 'C' is somehow a terrible idea but oh well

static const int screenWidth = 1200;
static const int screenHeight = 800;

static const int UNIT = 100; //grid unit

static int fontSize = 20;
