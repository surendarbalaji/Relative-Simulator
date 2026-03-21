#pragma once
#include "raylib.h"

#define LIGHTERGRAY (Color) { 235, 235, 235, 230 }
#define SPACEBLUETEXT (Color) { 10, 32, 90, 180 }
#define SPACEBLUE (Color) { 10, 50, 90, 230 }

#define STRING_CHARACTERS_MAX (44 + 1)
#define TERMINAL_HISTORY_MAX 100

static const int screenWidth = 1200;
static const int screenHeight = 800;

static const int UNIT = 100; //grid unit

static int fontSize = 20;

typedef struct {
    Vector2 position;
    float width;
    float height;
    Vector2 nose;
    Vector2 centre;
    Vector2 leftWing;
    Vector2 rightWing;
    Vector2 velocity;
    Vector2 acceleration;
    float speed;
    float angle;
    bool constantThrust;
    Color colour;
} Ship;

extern Ship ship;
