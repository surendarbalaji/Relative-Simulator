#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>

#include "commander.h"
#include "config.h"

#include "raylib.h"
#include "string.h"


// input and terminal configuation
static Rectangle terminal = { 0 };
static Rectangle infoDisplay = { 0 };
static Camera2D terminalCam = { 0 };

static bool showInfoDisplay = true;
int infoItemsCount = 0;

static bool focusTerminal = false;

char input[STRING_CHARACTERS_MAX] = "\0";
char displayText[64];
int letterCount = 0;
char outputChannel[TERMINAL_HISTORY_MAX][STRING_CHARACTERS_MAX];
int historyCount = 0;

Font terminalFont = {0};
Font dejavu12 = {0};

Sound keyPress[10] = { 0 };
int currentPress = 0;

float timer = 0.0f;

void InitialiseTerminal(float width, float height, Font font) {

    terminal.x = screenWidth/2.0f - width/2.0f;
    terminal.y = screenHeight - 250;

    terminal = (Rectangle){terminal.x, terminal.y, width, height};
    infoDisplay = (Rectangle){50, screenHeight - 250, terminal.x - 100, height};

    terminalCam.offset = (Vector2){0};
    terminalCam.target = (Vector2){0};
    terminalCam.rotation = 0.0f;
    terminalCam.zoom = 1.0f;

    terminalFont = font;
    dejavu12  = LoadFontEx("../resources/dejavu-mono-latin-400-normal.ttf", 12, 0, 0);

    InitAudioDevice();
    // keyPress[0] = LoadSound("../resources/keypress.mp3");
    keyPress[0] = LoadSound("../resources/test2.mp3");
    for (int i = 1; i < 10; i++) keyPress[i] = LoadSoundAlias(keyPress[0]);

}


void UpdateTerminal() {

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
                    timer = 0.0f;
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
                    timer = 0.0f;
                }

                while ((input[letterCount-2] != ' ') && (input[letterCount-2] != '\0')) {
                    letterCount--;
                    input[letterCount] = '\0';
                    timer = 0.0f;
                }
            }
        }

        // character deletion
        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            input[letterCount] = '\0';
            timer = 0.0f;
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
            timer = 0.0f;
        }

        if (IsKeyPressed(KEY_UP)) {
            strcpy(input, outputChannel[historyCount-2] + 2); //temp simple feature for now
            letterCount = strlen(input);
            timer = 0.0f;
        }

        // arrow before current typing (once there are responses this will be added to input history too)
        strcpy(displayText, "> ");
        strcat(displayText, input);

        // little | animation (quite proud of this)
        timer += GetFrameTime();
        if (timer <= 0.5f) strcat(displayText, "|");
        if (timer >= 1.0f) timer = 0.0f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), terminal)) {
        focusTerminal = true;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), terminal)) {
        focusTerminal = false;
    }

}


void DrawTerminal() {

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
                    DrawTextEx(dejavu12, TextFormat("%s: (%0.2f, %0.2f)", infoItems[i].name, (*(Vector2*)infoItems[i].dataPtr).x, (*(Vector2*)infoItems[i].dataPtr).y), (Vector2){infoDisplay.x + 10.0f, infoDisplay.y + 180.0f - (infoItemsCount-1) * 18.0f}, 12, 0.5f, GRAY);
                }

            }
        }

    }
}


void terminalOutput(char *string) {
    strcpy(outputChannel[historyCount], string);
    historyCount++;
}


void toggleInfo() {
    showInfoDisplay = !showInfoDisplay;
    terminalOutput("info display toggled");
}


void CloseTerminal() {
    for (int i = 1; i < 10; i++) UnloadSoundAlias(keyPress[i]);
    UnloadSound(keyPress[0]);
    UnloadFont(dejavu12);
}

bool IsTerminalFocused() {
    return focusTerminal;
}