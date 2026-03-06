#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "rlgl.h"

#define LIGHTERGRAY (Color) { 235, 235, 235, 230 }
#define SPACEBLUETEXT (Color) { 10, 32, 90, 180 }
#define SPACEBLUE (Color) { 10, 50, 90, 230 }

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
    float angle;
    Color colour;
} Ship;

int main(void) {

    const int screenWidth = 800;
    const int screenHeight = 800;

    const int UNIT = 100;

    int columns = screenWidth / UNIT;
    int rows = screenHeight / UNIT;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");

    Ship ship = { 0 };
    ship.position = (Vector2){ 0, 0};
    ship.width = 50.0f;
    ship.height = 45.0f;
    ship.nose = (Vector2){ship.height/2, 0};
    ship.centre = (Vector2){ 0 };
    ship.leftWing = (Vector2){ ship. };
    ship.velocity = (Vector2){ 0 };
    ship.acceleration = (Vector2){ 0 };
    ship.angle = 0.0f;
    ship.colour = SPACEBLUE;

    for (int x = 0; x < columns + 1; x++) {
        for (int y = 0; y < rows + 1; y++) {
            Point[x][y] = (Grid){.x = x * UNIT, .y = 800 - (UNIT * y)};
        }
    }

    Camera2D camera = { 0 };
    // camera.target = ship.position; this is here and commented because there are no separate points of reference yet (we wouldn't be able to see movement)
    camera.target = ship.position;
    camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
    camera.rotation = 0.0f;
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

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            ship.acceleration.x = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).x - 400);
            ship.acceleration.y = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).y - 400);

            ship.velocity.x += ship.acceleration.x;
            ship.velocity.y += ship.acceleration.y;

            ship.angle = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y - 400, GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x - 400);

            printf("%f \n", ship.position.y);

        }

        ship.position.x += ship.velocity.x * GetFrameTime();
        ship.position.y += ship.velocity.y * GetFrameTime();

        camera.target = ship.position;

        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);

                // for (int x = 0; x < columns; x++) {
                //     for (int y = 0; y < rows; y++) {
                //         DrawCircle(Point[x][y].x, Point[x][y].y, 2.0f, GRAY);
                //         CreateGrid(Point[x][y], Point[x+1][y], Point[x][y+1]);
                //     }
                // }

                for (int x = -2000; x <=2000; x+=100) {
                    for (int y = -2000; y <=2000; y += 100) {
                        DrawCircle(x, y, 2.0f, GRAY);
                    }
                }

                for (int x = -2000; x <=2000; x+= 100) DrawLine(x, -2000, x, 2000, GRAY);
                for (int y = -2000; y <=2000; y+= 100) DrawLine(-2000, y, 2000, y, GRAY);

                // Rectangle arrow = { ship.position.x - ship.width/2, ship.position.y - ship.height/2, ship.width, ship.height};
                // DrawRectangleRec(arrow, BLACK);
                // DrawTriangle((Vector2){ship.position.x, ship.position.y - ship.height/2}, (Vector2){ship.position.x - ship.width/2, ship.position.y + ship.height/2}, (Vector2){ship.position.x + ship.width/2, ship.position.y + ship.height/2}, BLACK);
                // DrawCircleV(ship.position, 2.0f, GOLD);

                // DrawTriangle(ship.position, (Vector2){(ship.position.x + ship.width/2), ship.position.y}, (Vector2){(ship.position.x - ship.width/2), (ship.position.y - ship.height/2)}, ship.colour);
                // DrawTriangle(ship.position, (Vector2){(ship.position.x - ship.width/2), (ship.position.y + ship.height/2)}, (Vector2){(ship.position.x + ship.width/2), ship.position.y}, ship.colour);

                DrawTriangle(ship.position, (Vector2){(ship.position.x + ship.width/2)*cosf(ship.angle) - ship.position.y*sinf(ship.angle), (ship.position.x + ship.width/2)*sinf(ship.angle) + ship.position.y*cosf(ship.angle)}, (Vector2){(ship.position.x - ship.width/2)*cosf(ship.angle) - (ship.position.y - ship.height/2)*sinf(ship.angle), (ship.position.x - ship.width/2)*sinf(ship.angle) + (ship.position.y - ship.height/2)*cosf(ship.angle)}, ship.colour);
                DrawTriangle(ship.position, (Vector2){(ship.position.x - ship.width/2)*cosf(ship.angle) - (ship.position.y + ship.height/2)*sinf(ship.angle), (ship.position.x - ship.width/2)*sinf(ship.angle) + (ship.position.y + ship.height/2)*cosf(ship.angle)}, (Vector2){(ship.position.x + ship.width/2)*cosf(ship.angle) - ship.position.y*sinf(ship.angle), (ship.position.x + ship.width/2)*sinf(ship.angle) + ship.position.y*cosf(ship.angle)}, ship.colour);

            EndMode2D();

            DrawText("Relative Simulator", 190, 210, 20, BLACK);

            DrawRectangleRec(terminal, LIGHTERGRAY);
            DrawTextEx(Placeholder, input, (Vector2){(int)terminal.x + 10, (int)terminal.y + 170}, 20, 2, SPACEBLUETEXT);

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