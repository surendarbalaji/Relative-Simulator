#pragma once

#include <stdbool.h>
#include <stddef.h>

#define MAX_ARGS 16

typedef struct {
    char *args[MAX_ARGS];
    int count;
} inputCommand;

typedef struct {
    char *action;
    char *target;
    void (*handler)(char **args);
} Command;

typedef enum {
    infoFloat,
    infoVector2
} infoType;

typedef struct {
    char *name;
    bool visible;
    infoType type;
    void *dataPtr;
} InfoItem;

extern InfoItem infoItems[];

inputCommand parseCommand(char *input);
void executeCommand(inputCommand cmd);

inputCommand divideCommand(inputCommand cmd);

void setSpeed(char **args);
void setAcceleration(char **args);
void setThrust(char **args);
void moveShip(char **args);

void showInfo(char **args);
void hideInfo(char **args);

void deploySource();