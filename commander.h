#pragma once

#define MAX_ARGS 16

typedef struct {
    char *args[MAX_ARGS];
    int count;
} Command;

Command parseCommand(char *input);
void executeCommand(Command cmd);