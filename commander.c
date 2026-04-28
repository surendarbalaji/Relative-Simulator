#include "commander.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "grid.h"
#include "oscilloscope.h"
#include "raymath.h"
#include "terminal.h"
#include "ship.h"

// converts string to lowercase
void lowerString(char *string) {
    for (int i = 0; string[i]; i++) {
        string[i] = tolower(string[i]);
    }
}

// splits input into the defined command structure
inputCommand parseCommand(char *input) {

    inputCommand cmd = { 0 };

    char *token = strtok(input, " ");
    while (token != NULL && cmd.count < MAX_ARGS) {
        lowerString(token);
        cmd.args[cmd.count] = token;
        cmd.count++;
        token = strtok(NULL, " ");
    }

    return cmd;
}

// list of commands
Command commands[] = {
    {"set", "speed", setSpeed},
    {"set", "acceleration", setAcceleration},
    {"set", "thrust", setThrust},
    {"toggle", "info", toggleInfo},
    {"toggle", "grid", toggleGrid},
    {"toggle", "oscilliscope", toggleOscilliscope},
    {"show", NULL, showInfo},
    {"hide", NULL, hideInfo},
    {"move", NULL, moveShip},
    {"deploy", "source", deploySource},
    {NULL, NULL, NULL}
};

// this might be dialolical logic but i finally got it working so that's that
void executeCommand(inputCommand cmd) {

    for (int i = 0; commands[i].action != NULL; i++) {
        if (!strcmp(cmd.args[0], commands[i].action) && (commands[i].target == NULL) || (!strcmp(cmd.args[0], commands[i].action) && !strcmp(cmd.args[1], commands[i].target))) {
            commands[i].handler(cmd.args);
            return;
        }
    }

    terminalOutput("unknown command");

}

void setSpeed(char **args) {

    float newSpeed;

    if (args[2][strlen(args[2])-1] == 'c') {

        args[2][strlen(args[2])-1] = '\0';

        if (atof(args[2]) >= 0.0f && atof(args[2]) < 1.0f) newSpeed = atof(args[2]) * C;
        else {
            terminalOutput("unsuitable speed");
            return;
        }

    } else newSpeed = atof(args[2]);

    if (ship.speed == 0) {
        ship.velocity.x = newSpeed * cosf(ship.angle);
        ship.velocity.y = newSpeed * sinf(ship.angle);
    }

    else {
        float scale = newSpeed / ship.speed;
        ship.velocity.x *= scale;
        ship.velocity.y *= scale;
    }

    char output[STRING_CHARACTERS_MAX];
    snprintf(output, sizeof(output), "new velocity is (%0.2fi + %0.2fj)", ship.velocity.x, ship.velocity.y);
    terminalOutput(output);

}

void setAcceleration(char **args) {

    if (ship.acceleration.x * ship.acceleration.y == 0) { //checking if accelration is 0;
        ship.acceleration.x = atof(args[2]) * cosf(ship.angle);
        ship.acceleration.y = atof(args[2]) * sinf(ship.angle);
    }

    else {
        float scale = atof(args[2]) / sqrtf(ship.acceleration.x * ship.acceleration.x + ship.acceleration.y * ship.acceleration.y);
        ship.acceleration.x *= scale;
        ship.acceleration.y *= scale;
    }

    ship.constantThrust = true;
    char output[STRING_CHARACTERS_MAX];
    snprintf(output, sizeof(output), "new acceleration is (%0.2fi + %0.2fj)", ship.acceleration.x, ship.acceleration.y);
    terminalOutput(output);

}

void setThrust(char **args) {

    ship.thrust = atof(args[2]);

    char output[STRING_CHARACTERS_MAX];
    snprintf(output, sizeof(output), "new thrust is %0.2f", ship.thrust);
    terminalOutput(output);

}

// list of properties that can be displayed on the display panel
InfoItem infoItems[] = {
    {"position", true, infoVector2, &ship.position},
    {"velocity", true, infoVector2, &ship.velocity},
    {"speed", true, infoFloat, &ship.speed},
    {"acceleration", true, infoVector2, &ship.acceleration},
    {"angle", true, infoFloat, &ship.angle},
    {NULL, false, infoFloat, NULL}
};

// show property on the display panel
void showInfo(char **args) {
    for (int i = 0; infoItems[i].name != NULL; i++) {
        if (!strcmp(args[1], infoItems[i].name)) {
            infoItems[i].visible = true;
            terminalOutput(TextFormat("displaying %s", infoItems[i].name)); // i don't know why this is underlined yellow but i don't care (i'm so glad i don't have to use sprintf)
            return;
        }
    }

    terminalOutput("no such property to display");
}

// hide property on the display panel
void hideInfo(char **args) {
    for (int i = 0; infoItems[i].name != NULL; i++) {
        if (!strcmp(args[1], infoItems[i].name)) {
            infoItems[i].visible = false;
            terminalOutput(TextFormat("%s hidden", infoItems[i].name));
            return;
        }
    }

    terminalOutput("no such property to hide");
}

void moveShip(char **args) {

    if (ship.speed != 0) {
        terminalOutput("you are not stationary");
        return;
    }

    ship.target = (Vector2){atof(args[1]) + ship.position.x, atof(args[2]) + ship.position.y};
    ship.distanceTarget = Vector2Length(Vector2Subtract(ship.position, ship.target));
    ship.targetSet = true;

    terminalOutput(TextFormat("moving to (%.0f, %.0f)", ship.target.x, ship.target.y));

}

void deploySource() {
    createSource(); // just where the ship is for now
    terminalOutput(TextFormat("wave source deployed at (%.0f, %.0f)", ship.position.x, ship.position.y));
}