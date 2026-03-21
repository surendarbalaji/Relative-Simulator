#include "commander.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "grid.h"
#include "terminal.h"

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
    {"toggle", "info", toggleInfo},
    {"toggle", "grid", toggleGrid},
    {"show", NULL, showInfo},
    {"hide", NULL, hideInfo},
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

        if (ship.speed == 0) {
            ship.velocity.x = atof(args[2]) * cosf(ship.angle);
            ship.velocity.y = atof(args[2]) * sinf(ship.angle);
        }

        else {
            float scale = atof(args[2]) / ship.speed;
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