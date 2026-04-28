#pragma once

#include "raylib.h"

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
    float thrust;
    float speed;
    float angle;
    float timer;
    bool constantThrust;
    Vector2 target;
    bool targetSet;
    float distanceTarget;
    Color colour;
} Ship;

extern Ship ship;

void initialiseShip();
void updateShip(Camera2D* camera, float dt);