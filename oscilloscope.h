#pragma once

#include "raylib.h"

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float timer;
    float frequency;
    float amplitude;
} WaveSource;

// just one source that can be moved for now;
extern WaveSource waveSource;

// status of configuration pulse
typedef enum {
    PULSE_IDLE,
    PULSE_SENT,
    SOURCE_RECIEVED,
    RESPONSE_RECIEVED
} PulseStatus;

void initialiseOscilloscope(float x, float y, float width, float height);
void updateOscilloscope();
void drawOscilloscope(Font dejavu8, Font dejavu12);

bool focusOscilloscope();

void createSource();
void updateSource(float dt);
void drawSource();

void dispatchPulse();
void updatePulse();
