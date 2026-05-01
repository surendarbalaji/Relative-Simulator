#include "ship.h"

#include <math.h>
#include <stdio.h>

#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "terminal.h"
#include "oscilloscope.h"

void initialiseShip() {

    // ship configuration
    ship.position = (Vector2){ 0, 0};
    ship.width = 50.0f;
    ship.height = 45.0f;
    ship.nose = (Vector2){ship.height/2, 0};
    ship.centre = (Vector2){ 0 };
    ship.leftWing = (Vector2){ -ship.height/2, -ship.width/2};
    ship.rightWing = (Vector2){ -ship.height/2, ship.width/2};
    ship.velocity = (Vector2){ 0 };
    ship.acceleration = (Vector2){ 0 };
    ship.thrust = 1.0f;
    ship.angle = 0.0f;
    ship.timer = 0.0f;
    ship.constantThrust = false;
    ship.target = (Vector2){ 0 };
    ship.targetSet = false;
    ship.distanceTarget = 0.0f;
    ship.colour = SPACEBLUE;

}

void updateShip(Camera2D* camera, float dt) {

    ship.timer += GetFrameTime();

    if (!ship.constantThrust) {
        ship.acceleration.x = 0;
        ship.acceleration.y = 0;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !focusOscilloscope()) {

        ship.constantThrust = false;

        // world based acceleration (affected by zoom)
        // ship.acceleration.x = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);
        // ship.acceleration.y = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y);

        // screen based acceleration (not affected by zoom)
        ship.acceleration.x = 0.01f * (GetMousePosition().x - screenWidth/2) * ship.thrust;
        ship.acceleration.y = 0.01f * (GetMousePosition().y - screenHeight/2) * ship.thrust;

        ship.angle = atan2(GetMousePosition().y - screenHeight/2, GetMousePosition().x - screenWidth/2);

    }

    if (ship.targetSet) {

        // just found out there are Vector2 operator functions in raylib (god damn it)
        float distanceRemaning = Vector2Length(Vector2Subtract(ship.position, ship.target));
        Vector2 midpoint = (Vector2){0.5 * (ship.position.x + ship.target.x), 0.5 * (ship.position.y + ship.target.y)};

        if (distanceRemaning > 0.5 * ship.distanceTarget) {
            ship.angle = atan2(ship.target.y - ship.position.y, ship.target.x - ship.position.x);
            DrawLineEx(GetWorldToScreen2D(ship.position, *camera), GetWorldToScreen2D(midpoint, *camera), 1.0f, BLUE);
            DrawLineEx(GetWorldToScreen2D(midpoint, *camera), GetWorldToScreen2D(ship.target, *camera) , 1.0f, RED);
        }

        if (distanceRemaning < 0.5 * ship.distanceTarget) {
            ship.angle = atan2(ship.target.y - ship.position.y, ship.target.x - ship.position.x) + PI;
            DrawLineEx(GetWorldToScreen2D(ship.position, *camera), GetWorldToScreen2D(ship.target, *camera) , 1.0f, RED);
        }

        ship.acceleration = (Vector2){cosf(ship.angle), sinf(ship.angle)};


        if (distanceRemaning < 5.0f) {

            ship.acceleration = (Vector2){0};
            ship.velocity = (Vector2){0};

            ship.targetSet = false;

            terminalOutput("target reached");
        }

    }

    ship.velocity.x += ship.acceleration.x;
    ship.velocity.y += ship.acceleration.y;

    ship.position.x += ship.velocity.x * dt;
    ship.position.y += ship.velocity.y * dt;

    ship.speed = sqrtf(ship.velocity.x * ship.velocity.x + ship.velocity.y * ship.velocity.y);

    // optimised ship speed clamping
    if (ship.speed >= C) {
        ship.velocity.x *= (C - 1.0f) / ship.speed;
        ship.velocity.y *= (C - 1.0f) / ship.speed;

        ship.speed = C - 1.0f;
    }

    camera->target = ship.position;

}