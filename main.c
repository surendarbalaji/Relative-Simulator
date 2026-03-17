#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"
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
    ship.colour = SPACEBLUE;

    // camera configuration
    Camera2D camera = { 0 };
    camera.target = ship.position;
    camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Font dejavu  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 20, 0, 0);
    InitialiseTerminal(screenWidth / 6.0f, screenHeight - 250, screenWidth / 1.5f, 200, dejavu);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UpdateTerminal();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            ship.acceleration.x = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);
            ship.acceleration.y = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y);

            ship.velocity.x += ship.acceleration.x;
            ship.velocity.y += ship.acceleration.y;

            ship.angle = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y, GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);

        }

        ship.position.x += ship.velocity.x * GetFrameTime();
        ship.position.y += ship.velocity.y * GetFrameTime();

        ship.speed = sqrtf(ship.velocity.x * ship.velocity.x + ship.velocity.y * ship.velocity.y);

        camera.target = ship.position;

        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);



                // grid lines (i could include the horizontal lines in the below grid dots loop but why would i do that)
                for (int x = -1000000; x <=1000000; x+= 100) DrawLine(x, -1000000, x, 1000000, GRAY);
                for (int y = -1000000; y <=1000000; y+= 100) DrawLine(-1000000, y, 1000000, y, GRAY);

                // grid dots (except drawing circles is expensive so its a small area with dots for now)
                for (int x = -2000; x <=2000; x+=UNIT) {
                    for (int y = -2000; y <=2000; y += UNIT) {
                        DrawCircle(x, y, 2.0f, GRAY);
                    }
                }

                // drawing ship
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.leftWing, ship.angle).x + ship.position.x, RotateCorner(ship.leftWing, ship.angle).y + ship.position.y}, ship.colour);
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.rightWing, ship.angle).x + ship.position.x, RotateCorner(ship.rightWing, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, ship.colour);

            EndMode2D();

            DrawTextEx(dejavu, "Relative Simulator", (Vector2){190, 210}, fontSize, 0.7, BLACK);

            DrawTerminal();




        EndDrawing();
    }

    UnloadFont(dejavu);
    CloseTerminal();
    CloseWindow();

    return 0;
}