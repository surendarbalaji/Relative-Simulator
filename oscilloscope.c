#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "oscilloscope.h"
#include "raylib.h"
#include "math.h"
#include "config.h"
#include "raymath.h"
#include "ship.h"

static Rectangle oscilloscopeUI = { 0 };
static Rectangle oscilloscopeDisplay = { 0 };
static Rectangle powerSwitch = { 0 };
static Rectangle pulseSwitch = { 0 };
static Vector2 timebaseDial = { 0 };
static Vector2 amplitudeDial = { 0 };
static Vector2 dispatchPulseButton = { 0 };
static Vector2 pulseStatusLight = { 0 };
static float dialRadius = 7.0f; // adding this because the oscilloscope ui currently looks peculiar and i know i'm going to change it, so may as well make it easy to change
static float buttonRadius = 5.0f;
static Vector2 *wavePoints = NULL;
static Vector2 *dispatchWavePoints = NULL;

static int wavePointsNum = 0;
static float timebaseScale = 0.5f;
static float amplitudeScale = 0.6f;
static float dispatchFrequency = 2.0f;
static float pulseSendTime = 0.0f;
static float timeToSource = 0.0f;
static float sourceCountdown = 0.0f;
static float recieveCountdown = 0.0f;

static PulseStatus pulseStatus = PULSE_IDLE;

static bool timebaseChanging = false;
static bool amplitudeChanging = false;
static bool oscilloscopeOn = true;
static bool pulseConfigOn = false;

void initialiseOscilloscope(float x, float y, float width, float height) {

    oscilloscopeUI = (Rectangle){x, y, width, height};
    oscilloscopeDisplay = (Rectangle){x + 110.0f, y + 10.0f, width - 120.0f, height - 20.0f};
    powerSwitch = (Rectangle){oscilloscopeDisplay.x - 57.0f, oscilloscopeDisplay.y + 3.0f, 30.0f, 13.0f};
    pulseSwitch = (Rectangle){oscilloscopeDisplay.x - 57.0f, powerSwitch.y + powerSwitch.height + 8.0f, 30.0f, 13.0f};
    timebaseDial = (Vector2){oscilloscopeDisplay.x - 72.0f, oscilloscopeDisplay.y + 11.0f};
    amplitudeDial = (Vector2){oscilloscopeDisplay.x - 72.0f, oscilloscopeDisplay.y + 31.0f};
    dispatchPulseButton = (Vector2){oscilloscopeDisplay.x - 90.0f, oscilloscopeDisplay.y + 60.0f};
    pulseStatusLight = (Vector2){oscilloscopeDisplay.x - 50.0f, oscilloscopeDisplay.y + 60.0f};
    wavePointsNum = (int)floor(oscilloscopeDisplay.width);

    wavePoints = malloc(wavePointsNum * sizeof(Vector2));
    dispatchWavePoints = malloc(wavePointsNum * sizeof(Vector2));

    waveSource.position = (Vector2){0};
    waveSource.timer = 0.0f;
    waveSource.frequency = 4.0f;
    waveSource.amplitude = 1.0f;

}

void updateOscilloscope() {
    // updating display based on dials
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), timebaseDial, dialRadius + 2.0f)) timebaseChanging = true;

    if (timebaseChanging) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) timebaseChanging = false;
        else {
            if (!pulseConfigOn) timebaseScale -= GetMouseDelta().y * 0.005f;
            else dispatchFrequency -= GetMouseDelta().y * 0.005f;
            timebaseScale = Clamp(timebaseScale, 0.05f, 5.0f);
            dispatchFrequency = Clamp(dispatchFrequency, 0.001, 100.0f);
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), amplitudeDial, dialRadius + 2.0f)) amplitudeChanging = true;

    if (amplitudeChanging) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) amplitudeChanging = false;
        else {
            amplitudeScale -= GetMouseDelta().y * 0.005f;
            amplitudeScale = Clamp(amplitudeScale, 0.05f, 1.0f);
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), powerSwitch)) oscilloscopeOn = !(oscilloscopeOn);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), pulseSwitch)) pulseConfigOn = !(pulseConfigOn);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), dispatchPulseButton, buttonRadius + 2.0f) && pulseStatus == PULSE_IDLE) dispatchPulse();
    if (pulseStatus != PULSE_IDLE) updatePulse();

    waveSource.timer += GetFrameTime();
    // regularTimer += GetFrameTime();
    // printf("ship distance: %f\n", (Vector2Distance((Vector2){0, 0}, ship.position)));
    // printf("absolute:%f vs seen:%f\n", regularTimer, sourceTimer);

    // this might be the worst way to do this of all time but i can't think of anything else
    for (int i = 0; i < wavePointsNum; i++) {

        float phase = 0;
        if (oscilloscopeOn) phase = (2 * PI) * waveSource.frequency * ((waveSource.timer - (i * timebaseScale / wavePointsNum)) - Vector2Distance(waveSource.position, ship.position) / C);

        wavePoints[i].x = oscilloscopeDisplay.x + i;
        wavePoints[i].y = oscilloscopeDisplay.y + oscilloscopeDisplay.height/2.0f - sinf(phase) * (oscilloscopeDisplay.height/2.0f * amplitudeScale);

        dispatchWavePoints[i].x = oscilloscopeDisplay.x + i;
        dispatchWavePoints[i].y = oscilloscopeDisplay.y + oscilloscopeDisplay.height/2.0f - sinf(2 * PI * dispatchFrequency * i * timebaseScale / wavePointsNum) * (oscilloscopeDisplay.height/2.0f * amplitudeScale);

    }
}


void drawOscilloscope(Font dejavu8, Font dejavu12) {

    DrawRectangleRec(oscilloscopeUI, LIGHTERGRAY);
    DrawRectangleLinesEx(oscilloscopeUI, 1.0f, GRAY);
    // DrawTextEx(dejavu8, "Oscilloscope", (Vector2){oscilloscopeUI.x + 12.0f, oscilloscopeUI.y + 6.0f}, 8.0f, 0.7f, SPACEBLUETEXT);

    DrawRectangleRec(oscilloscopeDisplay, Fade(GRAY, 0.2f));

    // on-off flip switch
    DrawRectangleRec(powerSwitch, LIGHTGRAY);
    DrawRectangleLinesEx(powerSwitch, 0.5f, DARKGRAY);
    if (oscilloscopeOn) DrawRectangleV((Vector2){powerSwitch.x + 2.0f, powerSwitch.y + 2.0f}, (Vector2){powerSwitch.width - 4.0f, powerSwitch.height/2 - 2.0f}, Fade(GRAY, 0.5f));
    else DrawRectangleV((Vector2){powerSwitch.x + 2.0f, powerSwitch.y + powerSwitch.height/2}, (Vector2){powerSwitch.width - 4.0f, powerSwitch.height/2 - 2.0f}, Fade(GRAY, 0.5f));
    DrawTextEx(dejavu12, "PWR", (Vector2){powerSwitch.x + powerSwitch.width + 2.0f, powerSwitch.y + 1.5f}, 12.0f, 0.5f, DARKGRAY);

    // pulse configuration switch
    DrawRectangleRec(pulseSwitch, LIGHTGRAY);
    DrawRectangleLinesEx(pulseSwitch, 0.5f, DARKGRAY);
    if (pulseConfigOn) DrawRectangleV((Vector2){pulseSwitch.x + 2.0f, pulseSwitch.y + 2.0f}, (Vector2){pulseSwitch.width - 4.0f, pulseSwitch.height/2 - 2.0f}, Fade(GRAY, 0.5f));
    else DrawRectangleV((Vector2){pulseSwitch.x + 2.0f, pulseSwitch.y + pulseSwitch.height/2}, (Vector2){pulseSwitch.width - 4.0f, pulseSwitch.height/2 - 2.0f}, Fade(GRAY, 0.5f));
    DrawTextEx(dejavu12, "CFG", (Vector2){pulseSwitch.x + pulseSwitch.width + 2.0f, pulseSwitch.y + 1.5f}, 12.0f, 0.5f, DARKGRAY);

    // timebase and amplitude dials
    DrawCircleV(timebaseDial, dialRadius, LIGHTGRAY);
    DrawCircleLinesV(timebaseDial, dialRadius, GRAY);
    DrawLineEx(timebaseDial, (Vector2){timebaseDial.x, timebaseDial.y - dialRadius}, 0.8f, GRAY);
    DrawTextEx(dejavu12, "TB", (Vector2){timebaseDial.x - dialRadius - 15.0f, timebaseDial.y - 4.5f}, 12.0f, 0.5f, DARKGRAY);


        // temporarily pausing on this i seriously can't deal with these ticks right now
        // DrawLineEx(Vector2Add(amplitudeDial, (Vector2){(dialRadius - 4.0f) * cosf(i * PI/30), (dialRadius - 4.0f) * sinf(i * PI/30)}), Vector2Add(amplitudeDial, (Vector2){dialRadius * cosf(i * PI/30), dialRadius * sinf(i * PI/30)}), 0.5f, GRAY);
        // DrawLineEx(Vector2Add(timebaseDial, (Vector2){(dialRadius - 1.0f) * cosf(i * PI/30), (dialRadius - 1.0f) * sinf(i * PI/30)}), Vector2Add(timebaseDial, (Vector2){dialRadius * cosf(i * PI/30), dialRadius * sinf(i * PI/30)}), 0.25f, GRAY);
        // DrawLineEx(Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius - 4.0f) * cosf(i * PI/30), (stopwatchRadius - 4.0f) * sinf(i * PI/30)}), Vector2Add(stopwatchCentre, (Vector2){(stopwatchRadius) * cosf(i * PI/30), (stopwatchRadius) * sinf(i * PI/30)}), 1.0f, GRAY);

    DrawCircleV(amplitudeDial, dialRadius, LIGHTGRAY);
    DrawCircleLinesV(amplitudeDial, dialRadius, GRAY);
    DrawLineEx(amplitudeDial, (Vector2){amplitudeDial.x, amplitudeDial.y - dialRadius}, 0.8f, GRAY);
    DrawTextEx(dejavu12, "AMP", (Vector2){amplitudeDial.x - dialRadius - 22.0f, amplitudeDial.y - 4.5f}, 12.0f, 0.5f, DARKGRAY);

    DrawLineEx((Vector2){oscilloscopeUI.x + 8.0f, amplitudeDial.y + dialRadius + (((dispatchPulseButton.y - buttonRadius) - (amplitudeDial.y + dialRadius)) / 2)}, (Vector2){oscilloscopeDisplay.x - 8.0f, amplitudeDial.y + dialRadius + (((dispatchPulseButton.y - buttonRadius) - (amplitudeDial.y + dialRadius)) / 2)}, 0.3f, BLACK);

    // dispatch pulse button
    DrawCircleV(dispatchPulseButton, buttonRadius, LIGHTGRAY);
    DrawCircleLinesV(dispatchPulseButton, buttonRadius, GRAY);
    DrawTextEx(dejavu12, "DSP", (Vector2){dispatchPulseButton.x + 8.0f, dispatchPulseButton.y - 5.0f}, 12.0f, 0.5f, DARKGRAY);

    // dispatch pulse status light (i'm just doing an if loop for now i'll sort it later)
    if (pulseStatus == PULSE_IDLE) {
        DrawCircleV(pulseStatusLight, buttonRadius, Fade(SPACEBLUE, 0.6f));
        DrawTextEx(dejavu12, TextFormat("IDLE"), (Vector2){pulseStatusLight.x + 8.0f, pulseStatusLight.y - 5.0f}, 12.0f, 0.5f, DARKGRAY);
    } else if (pulseStatus == PULSE_SENT) {
        DrawCircleV(pulseStatusLight, buttonRadius, Fade(RED, 0.6f));
        DrawTextEx(dejavu12, TextFormat("SENT"), (Vector2){pulseStatusLight.x + 8.0f, pulseStatusLight.y - 5.0f}, 12.0f, 0.5f, DARKGRAY);
    } else if (pulseStatus == SOURCE_RECIEVED) {
        DrawCircleV(pulseStatusLight, buttonRadius, Fade(ORANGE, 0.6f));
        DrawTextEx(dejavu12, TextFormat("RESP"), (Vector2){pulseStatusLight.x + 8.0f, pulseStatusLight.y - 5.0f}, 12.0f, 0.5f, DARKGRAY);
    } else if (pulseStatus == RESPONSE_RECIEVED) {
        DrawCircleV(pulseStatusLight, buttonRadius, Fade(DARKGREEN, 0.3f));
        DrawTextEx(dejavu12, TextFormat("RCVD"), (Vector2){pulseStatusLight.x + 8.0f, pulseStatusLight.y - 5.0f}, 12.0f, 0.5f, DARKGRAY);
    }
    DrawCircleLinesV(pulseStatusLight, buttonRadius, Fade(GRAY, 0.6f));

    // dispatch frequency
    DrawTextEx(dejavu12, TextFormat("%0.1fHz", dispatchFrequency), (Vector2){dispatchPulseButton.x - 6.0f, dispatchPulseButton.y + 10.0f}, 12.0f, 0.5, DARKGRAY);

    // dispatch countdown
    DrawTextEx(dejavu12, TextFormat("%0.1fs", sourceCountdown), (Vector2){pulseStatusLight.x - 4.0f, pulseStatusLight.y + 10.0f}, 12.0f, 0.5, DARKGRAY);

    for (int i = 1; i < 10; i++) {
        DrawLineEx((Vector2){oscilloscopeDisplay.x + i * (oscilloscopeDisplay.width / 10), oscilloscopeDisplay.y}, (Vector2){oscilloscopeDisplay.x + i * (oscilloscopeDisplay.width / 10), oscilloscopeDisplay.y + oscilloscopeDisplay.height}, 0.8f, Fade(GRAY, 0.2f));
        DrawLineEx((Vector2){oscilloscopeDisplay.x, oscilloscopeDisplay.y + i * (oscilloscopeDisplay.height / 10)}, (Vector2){oscilloscopeDisplay.x + oscilloscopeDisplay.width, oscilloscopeDisplay.y + i * (oscilloscopeDisplay.height / 10)}, 0.8f, Fade(GRAY, 0.2f));
    }

    if (pulseConfigOn && oscilloscopeOn) DrawSplineLinear(dispatchWavePoints, wavePointsNum, 1.5f, Fade(SPACEBLUE, 0.4f));
    DrawSplineLinear(wavePoints, wavePointsNum, 1.5f, RAYWHITE);

    DrawTextEx(dejavu8, TextFormat("%0.2fHz", waveSource.frequency), (Vector2){oscilloscopeDisplay.x + 4.0f, oscilloscopeDisplay.y + 3.0f}, 8.0f, 0.5f, DARKGRAY);

    DrawRectangleLinesEx(oscilloscopeDisplay, 1.0f, GRAY);

}

bool focusOscilloscope() {
    return CheckCollisionPointRec(GetMousePosition(), oscilloscopeUI) || (timebaseChanging || amplitudeChanging);
}

void createSource() {
    waveSource.position = ship.position;
    waveSource.velocity = ship.velocity;
    waveSource.timer = ship.timer;
}

void updateSource(float dt) {
    waveSource.position.x += waveSource.velocity.x * dt;
    waveSource.position.y += waveSource.velocity.y * dt;
}

void drawSource() {
    DrawCircleV(waveSource.position, 5.0f, GRAY);
}

void dispatchPulse() {
    pulseSendTime = waveSource.timer;
    timeToSource = Vector2Distance(ship.position, waveSource.position) / C;
    pulseStatus = PULSE_SENT;
}

void updatePulse() {

    if (pulseStatus == PULSE_SENT) {
        sourceCountdown = timeToSource - (waveSource.timer - pulseSendTime);

        if (sourceCountdown <= 0) {
            pulseStatus = SOURCE_RECIEVED;
            printf("source recieved");
            pulseSendTime = waveSource.timer;

        }
    }

    else if (pulseStatus == SOURCE_RECIEVED) {
        timeToSource = Vector2Distance(ship.position, waveSource.position) / C;
        sourceCountdown = timeToSource - (waveSource.timer - pulseSendTime);

        if (sourceCountdown <= 0) {
            pulseStatus = RESPONSE_RECIEVED;
            waveSource.frequency = dispatchFrequency;
            sourceCountdown = 0.0f;
        }

    }
}