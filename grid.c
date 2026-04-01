#include "grid.h"
#include "config.h"
#include "raylib.h"
#include "terminal.h"

//simple grid for now

static bool showGrid = true;

void drawGrid() {

    if (showGrid) {
        // grid lines (i could include the horizontal lines in the below grid dots loop but why would i do that)
        for (int x = -2000000; x <=2000000; x+= UNIT) DrawLine(x, -2000000, x, 2000000, GRAY);
        for (int y = -2000000; y <=2000000; y+= UNIT) DrawLine(-2000000, y, 2000000, y, GRAY);

        // grid dots (except drawing circles is expensive so its a small area with dots for now)
        for (int x = -2000; x <=2000; x+=UNIT) {
            for (int y = -2000; y <=2000; y += UNIT) {
                DrawCircle(x, y, 2.0f, GRAY);
            }
        }
    }

}

void toggleGrid() {
    showGrid = !showGrid;
    terminalOutput("grid toggled");
}