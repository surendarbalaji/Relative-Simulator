#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"
#include "grid.h"
#include "terminal.h"
#include "ship.h"
#include "oscilloscope.h"

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
WaveSource waveSource;

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");

    initialiseShip();

    // camera configuration
    Camera2D camera = { 0 };
    camera.target = ship.position;
    camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Font dejavu20  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 20, 0, 0);
    initialiseTerminal(533.0f, 200, dejavu20);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        updateTerminal();
        updateSource(GetFrameTime());
        updateShip(&camera, GetFrameTime());

        if (!IsTerminalFocused()) {
            camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
        }

        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);

                drawGrid();
                drawSource();

                // drawing ship (oh my god)
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.leftWing, ship.angle).x + ship.position.x, RotateCorner(ship.leftWing, ship.angle).y + ship.position.y}, ship.colour);
                DrawTriangle(ship.position, (Vector2){RotateCorner(ship.rightWing, ship.angle).x + ship.position.x, RotateCorner(ship.rightWing, ship.angle).y + ship.position.y}, (Vector2){RotateCorner(ship.nose, ship.angle).x + ship.position.x, RotateCorner(ship.nose, ship.angle).y + ship.position.y}, ship.colour);
        
            EndMode2D();

            DrawTextEx(dejavu20, "Relative Simulator", (Vector2){190, 210}, fontSize, 0.7, BLACK);

            drawTerminal();

        EndDrawing();
    }

    UnloadFont(dejavu20);
    CloseTerminal();
    CloseWindow();

    return 0;
}