#include "raylib.h"

int main(void) {

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Relative Simulator");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("Relative Simulator", 190, 200, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
