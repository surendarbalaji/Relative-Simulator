#include "commander.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "terminal.h"

// converts string to lowercase
void lowerString(char *string) {
    for (int i = 0; string[i]; i++) {
        string[i] = tolower(string[i]);
    }
}

// splits input into the defined command structure
Command parseCommand(char *input) {

    Command cmd = { 0 };

    char *token = strtok(input, " ");
    while (cmd.count < MAX_ARGS) {
        if (token != NULL) lowerString(token);
        cmd.args[cmd.count] = token;
        cmd.count++;
        token = strtok(NULL, " ");
    }

    return cmd;
}

void executeCommand(Command cmd) {

    if (!strcmp(cmd.args[0], "set")) {

        if (!strcmp(cmd.args[1], "speed")) {

            if (cmd.args[2] > 0) {

                if (ship.speed == 0) {
                    ship.velocity.x = atof(cmd.args[2]) * cosf(ship.angle);
                    ship.velocity.y = atof(cmd.args[2]) * sinf(ship.angle);
                }

                else {
                    float scale = atof(cmd.args[2]) / ship.speed;
                    ship.velocity.x *= scale;
                    ship.velocity.y *= scale;
                }

                char output[STRING_CHARACTERS_MAX];
                snprintf(output, sizeof(output), "new velocity is (%0.2fi + %0.2fj)", ship.velocity.x, ship.velocity.y);
                terminalOutput(output);
            }
        }
    }

    if (!strcmp(cmd.args[0], "increase")) {
        printf("we are increasing a value\n");
    }

    if (!strcmp(cmd.args[0], "decrease")) {
        printf("we are decreasing a value\n");
    }

    if (!strcmp(cmd.args[0], "configure")) {
        printf("we are configuring a value\n");
    }

}

