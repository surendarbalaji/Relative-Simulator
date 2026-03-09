#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "rlgl.h"

#define LIGHTERGRAY (Color) { 235, 235, 235, 230 }
#define SPACEBLUETEXT (Color) { 10, 32, 90, 180 }
#define SPACEBLUE (Color) { 10, 50, 90, 230 }

#define TERMINAL_HISTORY_MAX 100

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

    const int UNIT = 100; //grid unit

    int fontSize = 20;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Relative Simulator");


    // ship configuration
    Ship ship = { 0 };
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


    // input and terminal configuation
    char input[47 + 1] = "\0";
    char displayText[64];
    int letterCount = 0;
    char history[TERMINAL_HISTORY_MAX][47 + 1];
    int historyCount = 0;
    InitAudioDevice();
    Font dejavu  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 20, 0, 0);

    Sound keyPress[10] = { 0 };
    // keyPress[0] = LoadSound("../resources/keypress.mp3");
    keyPress[0] = LoadSound("../resources/test2.mp3");
    for (int i = 1; i < 10; i++) keyPress[i] = LoadSoundAlias(keyPress[0]);
    int currentPress = 0;
    float timer = 0.0f;

    Rectangle terminal = { screenWidth / 6.0f, screenHeight - 250, screenWidth / 1.5f, 200};

    Camera2D terminalCam = { 0 };
    terminalCam.offset = (Vector2){0};
    terminalCam.target = (Vector2){0};
    terminalCam.rotation = 0.0f;
    terminalCam.zoom = 1.0f;


    SetTargetFPS(60);

    while (!WindowShouldClose()) {


        // inputting text
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (letterCount < 47) {

                    SetSoundPitch(keyPress[currentPress], 1.05f + GetRandomValue(0, 10) * 0.015f);
                    SetSoundVolume(keyPress[currentPress], 0.3f + GetRandomValue(0, 3) * 0.1f);
                    PlaySound(keyPress[currentPress]);
                    currentPress++;
                    if (currentPress >= 10) currentPress = 0;

                    input[letterCount] = (char)key;
                    input[letterCount+1] = '\0';
                    letterCount++;
                    timer = 0.0f;
                }
            }

            key = GetCharPressed();
        }

        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            input[letterCount] = '\0';
            timer = 0.0f;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            strcpy(history[historyCount % TERMINAL_HISTORY_MAX], input);
            letterCount = 0;
            input[0] = '\0';
            historyCount++;
            timer = 0.0f;
        }

        // arrow before current typing (once there are responses this will be added to input history too)
        strcpy(displayText, "> ");
        strcat(displayText, input);

        // little | animation (quite proud of this)
        timer += GetFrameTime();
        printf("%f\n", timer);
        if (timer <= 0.5f) strcat(displayText, "|");
        if (timer >= 1.0f) timer = 0.0f;


        float scroll = GetMouseWheelMove();
        if (historyCount > 5) {
            terminalCam.target.y -= scroll*fontSize*0.5f;
            if (terminalCam.target.y < (-20 - (historyCount - 6) * 30.0f)) terminalCam.target.y = (-20 - (historyCount - 6) * 30.0f);
        }
        if (terminalCam.target.y > 0) terminalCam.target.y = 0;


        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            ship.acceleration.x = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);
            ship.acceleration.y = 0.1f * (GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y);

            ship.velocity.x += ship.acceleration.x;
            ship.velocity.y += ship.acceleration.y;

            ship.angle = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - ship.position.y, GetScreenToWorld2D(GetMousePosition(), camera).x - ship.position.x);

        }

        ship.position.x += ship.velocity.x * GetFrameTime();
        ship.position.y += ship.velocity.y * GetFrameTime();

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


            // drawing terminal
            DrawRectangleRec(terminal, LIGHTERGRAY);
            DrawRectangleLinesEx(terminal, 1.0f, GRAY);
            DrawLineEx((Vector2){terminal.x, terminal.y + terminal.height - 30.0f}, (Vector2){ terminal.x + terminal.width, terminal.y + terminal.height - 30.0f}, 1.0f, GRAY);
            DrawTextEx(dejavu, displayText, (Vector2){terminal.x + 10.0f, terminal.y + 175.0f}, fontSize, 0.7f, SPACEBLUETEXT);

            // separate camera for terminal history
            BeginScissorMode(terminal.x, terminal.y, terminal.width, terminal.height - 30.0f);
            BeginMode2D(terminalCam);

                for (int i = 0; i < historyCount; i++) {
                    DrawTextEx(dejavu, history[historyCount - i - 1], (Vector2){terminal.x + 10.0f, terminal.y + 170.0f - (i+1) * 30.0f}, fontSize, 0.7f, SPACEBLUETEXT);
                }

            EndMode2D();
            EndScissorMode();

        EndDrawing();
    }

    UnloadFont(dejavu);
    for (int i = 1; i < 10; i++) UnloadSoundAlias(keyPress[i]);
    UnloadSound(keyPress[0]);
    CloseWindow();

    return 0;
}