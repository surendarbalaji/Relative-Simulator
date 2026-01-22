#include "raylib.h"
#include "rlgl.h"

int main(void) {

    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();

            ClearBackground(WHITE);

        BeginMode2D(camera);
            rlPushMatrix();
                rlRotatef(-90, 1, 0, 0);
                DrawGrid(100, 50);
            rlPopMatrix();
        EndMode2D();

            DrawText("Relative Simulator", 190, 210, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
