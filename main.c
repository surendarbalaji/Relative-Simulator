#include "raylib.h"
#include "rlgl.h"

#define LIGHTERGRAY (Color) { 235, 235, 235, 230 }
#define SPACEBLUE (Color) { 10, 32, 90, 180 }

typedef struct {
    int x;
    int y;
} Grid;

Grid Point[81][81]; // 81 points per axis

void CreateGrid(Grid Point, Grid PointRight, Grid PointUp) {
    DrawCircleV((Vector2){Point.x, Point.y}, 1.0f, GRAY);
    DrawLineEx((Vector2){Point.x, Point.y}, (Vector2){PointRight.x, PointRight.y}, 1.0f, GRAY);
    DrawLineEx((Vector2){Point.x, Point.y}, (Vector2){PointUp.x, PointUp.y}, 1.0f, GRAY);
}

int main(void) {

    const int screenWidth = 800;
    const int screenHeight = 800;

    const int UNIT = 100;

    int columns = screenWidth / UNIT;
    int rows = screenHeight / UNIT;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");

    for (int x = 0; x < columns + 1; x++) {
        for (int y = 0; y < rows + 1; y++) {
            Point[x][y] = (Grid){.x = x * UNIT, .y = 800 - (UNIT * y)};
        }
    }

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    char input[50 + 1] = "\0";
    int letterCount = 0;
    Font Placeholder  = LoadFont("placeholder.png");

    Rectangle terminal = { screenWidth / 6.0f, screenHeight - 250, screenWidth / 1.5f, 200};

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                input[letterCount] = (char)key;
                input[letterCount+1] = '\0';
                letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            input[letterCount] = '\0';
        }



        BeginDrawing();

            ClearBackground(WHITE);

            for (int x = 0; x < columns; x++) {
                for (int y = 0; y < rows; y++) {
                    DrawCircle(Point[x][y].x, Point[x][y].y, 2.0f, GRAY);
                    CreateGrid(Point[x][y], Point[x+1][y], Point[x][y+1]);
                }
            }

        // BeginMode2D(camera);
        //     rlPushMatrix();
        //         rlRotatef(-90, 1, 0, 0);
        //         DrawGrid(100, 50);
        //     rlPopMatrix();
        // EndMode2D();

            DrawText("Relative Simulator", 190, 210, 20, BLACK);

            DrawRectangleRec(terminal, LIGHTERGRAY);
            DrawTextEx(Placeholder, input, (Vector2){(int)terminal.x + 10, (int)terminal.y + 170}, 20, 2, SPACEBLUE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

bool IsAnyKeyPressed() {
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}