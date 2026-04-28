#include "terminal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "commander.h"
#include "oscilloscope.h"
#include "ship.h"

#include "raylib.h"
#include "raymath.h"
#include "string.h"


// input and terminal configuation
static Rectangle terminal = { 0 };
static Rectangle infoDisplay = { 0 };
static Rectangle stopwatchDisplay = { 0 };
static Rectangle stopwatchClockDisplay = { 0 };
static Rectangle CFractionBarDisplay = { 0 };
static Rectangle lorentzFactorBarDisplay = { 0 };
static Rectangle lengthContractionBarDisplay = { 0 };
static Rectangle barInfoDisplay = { 0 };
static Camera2D terminalCam = { 0 };
static Vector2 vectorDialCentre = { 0 };
static Vector2 stopwatchCentre = { 0 };
static int vectorDialRadius = 40;
static int stopwatchRadius = 80;

static bool showInfoDisplay = true;
static bool showOscilliscope = true;
int infoItemsCount = 0;

static bool focusTerminal = false;

char input[STRING_CHARACTERS_MAX] = "\0";
char displayText[64];
int letterCount = 0;
char outputChannel[TERMINAL_HISTORY_MAX][STRING_CHARACTERS_MAX];
int historyCount = 0;

Font terminalFont = {0};
Font dejavu12 = {0};
Font dejavu8 = {0};

Sound keyPress[10] = { 0 };
int currentPress = 0;

float CFraction = 0.0f;
float lorentzFactor = 0.0f;

float typeTimer = 0.0f;
float stationaryStopwatchTimer = 0.0f;
float photonTimer = 0.0f;
float stationaryPhotonTimer = 0.0f;


void initialiseTerminal(float width, float height, Font font) {

    terminal.x = screenWidth/2.0f - width/2.0f;
    terminal.y = screenHeight - 250;

    terminal = (Rectangle){terminal.x, terminal.y, width, height};
    infoDisplay = (Rectangle){50, screenHeight - 250, terminal.x - 100, height};
    stopwatchDisplay = (Rectangle){terminal.x + terminal.width + 50, screenHeight - 250, terminal.x - 100, height};
    stopwatchCentre = (Vector2){stopwatchDisplay.x + stopwatchDisplay.width/2, stopwatchDisplay.y + stopwatchDisplay.height/2};
    stopwatchClockDisplay = (Rectangle){stopwatchCentre.x - 15.0f, stopwatchCentre.y + 15.0f, 30.0f, 5.0f};

    initialiseOscilloscope(infoDisplay.x, infoDisplay.y - 10.0f - 120.0f, infoDisplay.width, 110.0f);


    terminalCam.offset = (Vector2){0};
    terminalCam.target = (Vector2){0};
    terminalCam.rotation = 0.0f;
    terminalCam.zoom = 1.0f;

    terminalFont = font;
    dejavu12  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 12, 0, 0);
    dejavu8  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 8, 0, 0);

    InitAudioDevice();
    // keyPress[0] = LoadSound("../resources/keypress.mp3");
    keyPress[0] = LoadSound("../resources/test2.mp3");
    for (int i = 1; i < 10; i++) keyPress[i] = LoadSoundAlias(keyPress[0]);

}


void updateTerminal() {

    vectorDialCentre = (Vector2){infoDisplay.x + 10.0f + vectorDialRadius,  infoDisplay.y + 180.0f - (infoItemsCount-1) * 18.0f - 55.0f};
    CFractionBarDisplay = (Rectangle){vectorDialCentre.x + vectorDialRadius + 15.0f, vectorDialCentre.y - vectorDialRadius + 2.0f, 8.0f, vectorDialRadius*2 - 4.0f};
    lorentzFactorBarDisplay = (Rectangle){CFractionBarDisplay.x + CFractionBarDisplay.width + 10.0f, vectorDialCentre.y - vectorDialRadius + 2.0f, 8.0f, vectorDialRadius*2 - 4.0f};
    lengthContractionBarDisplay = (Rectangle){lorentzFactorBarDisplay.x + lorentzFactorBarDisplay.width + 10.0f, vectorDialCentre.y - vectorDialRadius + 2.0f, 8.0f, vectorDialRadius*2 - 4.0f};
    barInfoDisplay = (Rectangle){lengthContractionBarDisplay.x + lengthContractionBarDisplay.width + 10.0f, vectorDialCentre.y - vectorDialRadius + 2.0f, 25.0f, vectorDialRadius*2 - 4.0f};

    int key = GetCharPressed();

    while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (letterCount < STRING_CHARACTERS_MAX) {

                    SetSoundPitch(keyPress[currentPress], 1.05f + GetRandomValue(0, 10) * 0.015f);
                    SetSoundVolume(keyPress[currentPress], 0.3f + GetRandomValue(0, 3) * 0.1f);
                    PlaySound(keyPress[currentPress]);
                    currentPress++;
                    if (currentPress >= 10) currentPress = 0;

                    input[letterCount] = (char)key;
                    input[letterCount+1] = '\0';
                    letterCount++;
                    typeTimer = 0.0f;
                }
            }

            key = GetCharPressed();
        }

        // ctrl + backspace word deletion
        if ((IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) && ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))) {
            if (letterCount > 0) {

                while (input[letterCount-1] == ' ') {
                    letterCount--;
                    input[letterCount] = '\0';
                    typeTimer = 0.0f;
                }

                while ((input[letterCount-2] != ' ') && (input[letterCount-2] != '\0')) {
                    letterCount--;
                    input[letterCount] = '\0';
                    typeTimer = 0.0f;
                }
            }
        }

        // character deletion
        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            input[letterCount] = '\0';
            typeTimer = 0.0f;
        }

        // send command
        if (IsKeyPressed(KEY_ENTER)) {

            if (historyCount == TERMINAL_HISTORY_MAX) historyCount = 0;

            if (input[0] == '\0') {
                strcpy(input, " ");
            }

            char output[STRING_CHARACTERS_MAX];
            strcpy(output, "> ");
            strcat(output, input);

            strcpy(outputChannel[historyCount], output);
            historyCount++;

            // who knows how to make the parser santisize inputs properly this works for now
            if (!(input[0] == ' ' && input[1] == '\0')) {
                executeCommand(parseCommand(input));
            }

            letterCount = 0;
            input[0] = '\0';
            typeTimer = 0.0f;
        }

        if (IsKeyPressed(KEY_UP)) {
            strcpy(input, outputChannel[historyCount-2] + 2); //temp simple feature for now
            letterCount = strlen(input);
            typeTimer = 0.0f;
        }

        // arrow before current typing (once there are responses this will be added to input history too)
        strcpy(displayText, "> ");
        strcat(displayText, input);

        // little | animation (quite proud of this)
        typeTimer += GetFrameTime();
        if (typeTimer <= 0.5f) strcat(displayText, "|");
        if (typeTimer >= 1.0f) typeTimer = 0.0f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), terminal)) {
        focusTerminal = true;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), terminal)) {
        focusTerminal = false;
    }

    updateOscilloscope();

}


void drawTerminal() {
    // scroll up and down
    float scroll = GetMouseWheelMove();
    if (historyCount > 5 && focusTerminal) {
        terminalCam.target.y -= scroll*fontSize*0.5f;
        if (terminalCam.target.y < (-20 - (historyCount - 6) * 30.0f)) terminalCam.target.y = (-20 - (historyCount - 6) * 30.0f);
    }
    if (terminalCam.target.y > 0) terminalCam.target.y = 0;

    // drawing terminal
    DrawRectangleRec(terminal, LIGHTERGRAY);
    DrawRectangleLinesEx(terminal, 1.0f, GRAY);
    DrawLineEx((Vector2){terminal.x, terminal.y + terminal.height - 30.0f}, (Vector2){ terminal.x + terminal.width, terminal.y + terminal.height - 30.0f}, 1.0f, GRAY);
    DrawTextEx(terminalFont, displayText, (Vector2){terminal.x + 10.0f, terminal.y + 175.0f}, fontSize, 0.7f, SPACEBLUETEXT);

    // separate camera for terminal history
    BeginScissorMode(terminal.x, terminal.y, terminal.width, terminal.height - 30.0f);
    BeginMode2D(terminalCam);

    for (int i = 0; i < historyCount; i++) {
        DrawTextEx(terminalFont, outputChannel[historyCount - i - 1], (Vector2){terminal.x + 10.0f, terminal.y + 170.0f - (i+1) * 30.0f}, fontSize, 0.7f, SPACEBLUETEXT);
    }

    EndMode2D();
    EndScissorMode();

    if (showInfoDisplay) {

        //drawing ship info display

        DrawRectangleRec(infoDisplay, LIGHTERGRAY);
        DrawRectangleLinesEx(infoDisplay, 1.0f, GRAY);

        // char speedString[STRING_CHARACTERS_MAX];
        // snprintf(speedString, sizeof(speedString), "Speed: %0.2f", ship.speed);
        // DrawTextEx(terminalFont, speedString, (Vector2){infoDisplay.x + 10.0f, infoDisplay.y + 170.0f}, fontSize, 0.7f, SPACEBLUETEXT);

        infoItemsCount = 0;
        for (int i = 0; infoItems[i].name != NULL; i++) {
            if (infoItems[i].visible) {

                infoItemsCount++;

                if (infoItems[i].type == infoFloat) {
                    DrawTextEx(dejavu12, TextFormat("%s: %0.2f", infoItems[i].name, *(float*)infoItems[i].dataPtr), (Vector2){infoDisplay.x + 10.0f, infoDisplay.y + 180.0f - (infoItemsCount-1) * 18.0f}, 12, 0.7f, GRAY);
                } else if (infoItems[i].type == infoVector2) { //i'm pretty sure these pointer memory shenanigans below may cause the program to explode but oh well
                    DrawTextEx(dejavu12, TextFormat("%s: (%.2f, %.2f)", infoItems[i].name, (*(Vector2*)infoItems[i].dataPtr).x, (*(Vector2*)infoItems[i].dataPtr).y), (Vector2){infoDisplay.x + 10.0f, infoDisplay.y + 180.0f - (infoItemsCount-1) * 18.0f}, 12, 0.5f, GRAY);
                }

            }
        }

        // drawing vector dial

        DrawCircleLinesV(vectorDialCentre, vectorDialRadius, GRAY);
        DrawCircleLinesV(vectorDialCentre, 20.0f, GRAY);
        DrawCircleLinesV(vectorDialCentre, 10.0f, GRAY);

        DrawLineV((Vector2){vectorDialCentre.x, vectorDialCentre.y + vectorDialRadius}, (Vector2){vectorDialCentre.x, vectorDialCentre.y - vectorDialRadius}, GRAY);
        DrawLineV((Vector2){vectorDialCentre.x + vectorDialRadius, vectorDialCentre.y}, (Vector2){vectorDialCentre.x - vectorDialRadius, vectorDialCentre.y}, GRAY);

        // using (probably) temporary random boundaries for the different velocity scalings

        float x = 0;

        if (ship.speed < 2000) {
            x = (0.5 * vectorDialRadius) * ship.speed / 2000;
        } else if (ship.speed < 20000) {
            x = (0.5 * vectorDialRadius) + (0.4 * vectorDialRadius) * log(ship.speed / 2000) / log(20000 / 2000);
        } else if (ship.speed < C) {
            x = (0.90 * vectorDialRadius) + (0.10 * vectorDialRadius) * log(ship.speed / 20000) / log(C / 20000);
        }

        DrawLineEx(vectorDialCentre, Vector2Add(vectorDialCentre, Vector2Scale(Vector2Normalize(ship.velocity), x)), 1.5f, BLUE);
        DrawLineV(vectorDialCentre, Vector2Add(vectorDialCentre, Vector2Scale(Vector2Normalize(ship.acceleration), 20.0f)), RED);


        // drawing bars

        DrawRectangleLinesEx(CFractionBarDisplay, 1.0f, GRAY);
        DrawRectangleLinesEx(lorentzFactorBarDisplay, 1.0f, GRAY);

        CFraction = ship.speed / C;
        if (CFraction >= 0.99f) CFraction = 0.99f;
        DrawLineEx((Vector2){CFractionBarDisplay.x - 2.0f, CFractionBarDisplay.y + CFractionBarDisplay.height - 5.0f - ((CFractionBarDisplay.height - 10.0f) * CFraction)}, (Vector2){CFractionBarDisplay.x + CFractionBarDisplay.width + 2.0f, CFractionBarDisplay.y + CFractionBarDisplay.height - 5.0f - ((CFractionBarDisplay.height - 10.0f) * CFraction)}, 1.5f, GRAY);
        DrawTextEx(dejavu8, "c", (Vector2){CFractionBarDisplay.x + 2.0f, CFractionBarDisplay.y + 2.3f}, 8.0f, 0.7f, DARKGRAY);

        lorentzFactor = 1 / (sqrt(1 - (ship.speed * ship.speed / (C*C))));
        DrawLineEx((Vector2){lorentzFactorBarDisplay.x - 2.0f, lorentzFactorBarDisplay.y + lorentzFactorBarDisplay.height - 5.0f - ((lorentzFactorBarDisplay.height - 10.0f) * (1 - 1/lorentzFactor))}, (Vector2){lorentzFactorBarDisplay.x + lorentzFactorBarDisplay.width + 2.0f, lorentzFactorBarDisplay.y + lorentzFactorBarDisplay.height - 5.0f - ((lorentzFactorBarDisplay.height - 10.0f) * (1 - 1/lorentzFactor))}, 1.5f, GRAY);
        DrawTextEx(dejavu8, "y", (Vector2){lorentzFactorBarDisplay.x + 2.1f, lorentzFactorBarDisplay.y + 1.3f}, 8.0f, 0.7f, DARKGRAY);

        DrawRectangleV((Vector2){lengthContractionBarDisplay.x, lengthContractionBarDisplay.y + 5.0f + ((lengthContractionBarDisplay.height - 10.0f) / 2) * (1 - 1 / lorentzFactor)}, (Vector2){lengthContractionBarDisplay.width, ((lengthContractionBarDisplay.height - 10.0f) / lorentzFactor)}, LIGHTGRAY);
        DrawRectangleLinesEx(lengthContractionBarDisplay, 1.0f, GRAY);
        DrawTextEx(dejavu8, "L", (Vector2){lengthContractionBarDisplay.x + 2.0f, lengthContractionBarDisplay.y + 2.3f}, 8.0f, 0.7f, DARKGRAY);


        // DrawRectangleLinesEx(barInfoDisplay, 1.0f, GRAY);
        DrawTextEx(dejavu12, TextFormat("%.2fc", CFraction), (Vector2){barInfoDisplay.x, barInfoDisplay.y + 2.0f}, 12.0f, 0.7f, GRAY);
        DrawTextEx(dejavu12, TextFormat("%.2f(y)", lorentzFactor), (Vector2){barInfoDisplay.x, barInfoDisplay.y + 2.0f + 15.0f}, 12.0f, 0.7f, GRAY);
        DrawTextEx(dejavu12, TextFormat("%.2fm", 1.0f/lorentzFactor), (Vector2){barInfoDisplay.x, barInfoDisplay.y + 2.0f + 30.0f}, 12.0f, 0.7f, GRAY);
    }

    // drawing stopwatch

    DrawRectangleRec(stopwatchDisplay, LIGHTERGRAY);
    DrawRectangleLinesEx(stopwatchDisplay, 1.0f, GRAY);
    for (int i = 0; i < 60; i++) {
        DrawLineEx(Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 4.0f) * cosf(i * PI/30), (stopwatchRadius - 4.0f) * sinf(i * PI/30)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius) * cosf(i * PI/30), (stopwatchRadius) * sinf(i * PI/30)}), 1.0f, GRAY);
        if (i % 5 == 0) DrawLineEx(Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 8.0f) * cosf((i/5) * PI/6), (stopwatchRadius - 8.0f) * sinf((i/5) * PI/6)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius) * cosf((i/5) * PI/6), (stopwatchRadius) * sinf((i/5) * PI/6)}), 2.0f, GRAY); // if it works it works okay
    }

    stationaryStopwatchTimer += GetFrameTime() * lorentzFactor;

    DrawCircleLinesV(stopwatchCentre, stopwatchRadius, GRAY);
    DrawCircleV(stopwatchCentre, 2.0f, GRAY);

    DrawLineEx(Vector2Subtract(stopwatchCentre, (Vector2){12.0f * cosf((stationaryStopwatchTimer * PI/30) - PI/2), 12.0f * sinf((stationaryStopwatchTimer * PI/30) - PI/2)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 6.0f) * cosf((stationaryStopwatchTimer * PI/30) - PI/2), (stopwatchRadius - 6.0f) * sinf((stationaryStopwatchTimer * PI/30) - PI/2)}), 0.5f, RED);
    DrawLineEx(Vector2Subtract(stopwatchCentre, (Vector2){8.0f * cosf((stationaryStopwatchTimer * PI/1800) - PI/2), 8.0f * sinf((stationaryStopwatchTimer * PI/1800) - PI/2)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 12.0f) * cosf((stationaryStopwatchTimer * PI/1800) - PI/2), (stopwatchRadius - 12.0f) * sinf((stationaryStopwatchTimer * PI/1800) - PI/2)}), 0.75f, RED);

    DrawLineEx(Vector2Subtract(stopwatchCentre, (Vector2){12.0f * cosf((ship.timer * PI/30) - PI/2), 12.0f * sinf((ship.timer * PI/30) - PI/2)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 6.0f) * cosf((ship.timer * PI/30) - PI/2), (stopwatchRadius - 6.0f) * sinf((ship.timer * PI/30) - PI/2)}), 1.0f, GRAY);
    DrawLineEx(Vector2Subtract(stopwatchCentre, (Vector2){8.0f * cosf((ship.timer * PI/1800) - PI/2), 8.0f * sinf((ship.timer * PI/1800) - PI/2)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 12.0f) * cosf((ship.timer * PI/1800) - PI/2), (stopwatchRadius - 12.0f) * sinf((ship.timer * PI/1800) - PI/2)}), 1.5f, GRAY);

    DrawRectangleLinesEx(stopwatchClockDisplay, 1.0f, GRAY);

    // this is a temporary solution (i hope)
    photonTimer += GetFrameTime();

     if (lorentzFactor < 15.0f) stationaryPhotonTimer += GetFrameTime() * lorentzFactor;
     else stationaryPhotonTimer += GetFrameTime() * 15.0f;

    if (stationaryPhotonTimer <= 1.0f) {
        DrawLineEx((Vector2){stopwatchClockDisplay.x + 2.0f + (stationaryPhotonTimer * 26.0f), stopwatchClockDisplay.y}, (Vector2){stopwatchClockDisplay.x + 2.0f + (stationaryPhotonTimer * 26.0f), stopwatchClockDisplay.y + stopwatchClockDisplay.height}, 1.0f, RED);
    } else {
        DrawLineEx((Vector2){stopwatchClockDisplay.x + 2.0f + ((2 - stationaryPhotonTimer) * 26.0f), stopwatchClockDisplay.y}, (Vector2){stopwatchClockDisplay.x + 2.0f + ((2 - stationaryPhotonTimer) * 26.0f), stopwatchClockDisplay.y + stopwatchClockDisplay.height}, 1.0f, RED);
    }

    if (photonTimer <= 1.0f) {

        DrawLineEx((Vector2){stopwatchClockDisplay.x + 2.0f + (photonTimer * 26.0f), stopwatchClockDisplay.y}, (Vector2){stopwatchClockDisplay.x + 2.0f + (photonTimer * 26.0f), stopwatchClockDisplay.y + stopwatchClockDisplay.height}, 2.0f, GRAY);
    } else {
        DrawLineEx((Vector2){stopwatchClockDisplay.x + 2.0f + ((2 - photonTimer) * 26.0f), stopwatchClockDisplay.y}, (Vector2){stopwatchClockDisplay.x + 2.0f + ((2 - photonTimer) * 26.0f), stopwatchClockDisplay.y + stopwatchClockDisplay.height}, 2.0f, GRAY);
    }

    if (photonTimer >= 2) photonTimer = 0;
    if (stationaryPhotonTimer >= 2) stationaryPhotonTimer = 0;


    if (showOscilliscope) drawOscilloscope(dejavu8, dejavu12);

}


void terminalOutput(char *string) {
    strcpy(outputChannel[historyCount], string);
    historyCount++;
}


void toggleInfo() {
    showInfoDisplay = !showInfoDisplay;
    terminalOutput("info display toggled");
}

void toggleOscilliscope() {
    showOscilliscope = !showOscilliscope;
    terminalOutput("oscilliscope toggled");
}


void CloseTerminal() {
    for (int i = 1; i < 10; i++) UnloadSoundAlias(keyPress[i]);
    UnloadSound(keyPress[0]);
    UnloadFont(dejavu12);
    UnloadFont(dejavu8);
}

bool IsTerminalFocused() {
    return focusTerminal;
}