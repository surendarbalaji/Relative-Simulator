#include "terminal.h"

#include <stdio.h>

#include "commander.h"
#include "config.h"

#include "raylib.h"
#include "string.h"


// input and terminal configuation
static Rectangle terminal = { 0 };
static Camera2D terminalCam = { 0 };

char input[STRING_CHARACTERS_MAX] = "\0";
char displayText[64];
int letterCount = 0;
char outputChannel[TERMINAL_HISTORY_MAX][STRING_CHARACTERS_MAX];
int historyCount = 0;

Font terminalFont = {0};

Sound keyPress[10] = { 0 };
int currentPress = 0;

float timer = 0.0f;

void InitialiseTerminal(float x, float y, float width, float height, Font font) {

    terminal = (Rectangle){ screenWidth / 6.0f, screenHeight - 250, screenWidth / 1.5f, 200};

    terminalCam.offset = (Vector2){0};
    terminalCam.target = (Vector2){0};
    terminalCam.rotation = 0.0f;
    terminalCam.zoom = 1.0f;

    terminalFont = font;

    InitAudioDevice();
    // keyPress[0] = LoadSound("../resources/keypress.mp3");
    keyPress[0] = LoadSound("../resources/test2.mp3");
    for (int i = 1; i < 10; i++) keyPress[i] = LoadSoundAlias(keyPress[0]);

}


void DrawTerminal() {

    // scroll up and down
    float scroll = GetMouseWheelMove();
    if (historyCount > 5) {
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

            char output[STRING_CHARACTERS_MAX];
            strcpy(output, "> ");
            strcat(output, input);

            strcpy(outputChannel[historyCount], output);
            historyCount++;
            executeCommand(parseCommand(input));

            letterCount = 0;
            input[0] = '\0';
            timer = 0.0f;
        }

        // arrow before current typing (once there are responses this will be added to input history too)
        strcpy(displayText, "> ");
        strcat(displayText, input);

        // little | animation (quite proud of this)
        timer += GetFrameTime();
        if (timer <= 0.5f) strcat(displayText, "|");
        if (timer >= 1.0f) timer = 0.0f;

}

void terminalOutput(char *string) {
    strcpy(outputChannel[historyCount], string);
    historyCount++;
}

void CloseTerminal() {
    for (int i = 1; i < 10; i++) UnloadSoundAlias(keyPress[i]);
    UnloadSound(keyPress[0]);
}


// bool IsAnyKeyPressed()
// {
//     bool keyPressed = false;
//     int key = GetKeyPressed();
//
//     if (((key >= 32) && (key <= 126) || (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_ENTER)))) keyPressed = true;
//
//     return keyPressed;
// }
