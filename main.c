#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"
#include "grid.h"
#include "terminal.h"

// typedef struct {
//     int x;
//     int y;
// } Grid;
//
// Grid Point[81][81]; // 81 points per axis

// void CreateGrid(Grid Point, Grid PointRight, Grid PointUp) {
//     DrawCircleV((Vector2){Point.x, Point.y}, 1.0f, GRAY);
//     DrawLineEx((Vector2){Point.x, Point.y}, (Vector2){PointRight.x, PointRight.y}, 1.0f, GRAY);
//     DrawLineEx((Vector2){Point.x, Point.y}, (Vector2){PointUp.x, PointUp.y}, 1.0f, GRAY);
// }

Vector2 RotateCorner(Vector2 corner, float angle) {
    return (Vector2){corner.x * cosf(angle) - corner.y * sinf(angle), corner.x * sinf(angle) + corner.y * cosf(angle)};
}


Ship ship;

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");

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
    ship.angle = 0.0f;
    ship.constantThrust = false;
    ship.colour = SPACEBLUE;

    // camera configuration
    Camera2D camera = { 0 };
    camera.target = ship.position;
    camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Font dejavu20  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 20, 0, 0);
    InitialiseTerminal(533.0f, 200, dejavu20);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UpdateTerminal();

        if (!ship.constantThrust) {
            ship.acceleration.x = 0;
            ship.acceleration.y = 0;
        }

        if (!IsTerminalFocused()) {
            camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            ship.constantThrust = false;

            // world based acceleration (affected by zoom)
            // ship.acceleration.x = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);
            // ship.acceleration.y = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y);

            // screen based acceleration (not affected by zoom)
            ship.acceleration.x = 0.1f * GetMousePosition().x - 40.0f;
            ship.acceleration.y = 0.1f * GetMousePosition().y - 40.0f;

            ship.angle = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y, GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);

        }

        ship.velocity.x += ship.acceleration.x;
        ship.velocity.y += ship.acceleration.y;

        ship.position.x += ship.velocity.x * GetFrameTime();
        ship.position.y += ship.velocity.y * GetFrameTime();

        ship.speed = sqrtf(ship.velocity.x * ship.velocity.x + ship.velocity.y * ship.velocity.y);

        camera.target = ship.position;

        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);

                drawGrid();

                // drawing ship (oh my god)
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.leftWing, ship.angle).x + ship.position.x, RotateCorner(ship.leftWing, ship.angle).y + ship.position.y}, ship.colour);
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.rightWing, ship.angle).x + ship.position.x, RotateCorner(ship.rightWing, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, ship.colour);

            EndMode2D();

            DrawTextEx(dejavu20, "Relative Simulator", (Vector2){190, 210}, fontSize, 0.7, BLACK);

            DrawTerminal();

        EndDrawing();
    }

    UnloadFont(dejavu20);
    CloseTerminal();
    CloseWindow();

    return 0;
}