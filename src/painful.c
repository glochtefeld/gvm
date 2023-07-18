#include <string.h>
#include <stdio.h>
#include "painful.h"

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == '.') {
        linenoiseAddCompletion(lc, ".execute");
        linenoiseAddCompletion(lc, ".program");
        linenoiseAddCompletion(lc, ".undo");
        linenoiseAddCompletion(lc, ".clear");
        linenoiseAddCompletion(lc, ".history");
        linenoiseAddCompletion(lc, ".binary");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf, "hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}

void print_history(const char *fname) {
    return;
}

void start_painful_repl(VM *vm) {
    char *line;

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
    linenoiseSetMultiLine(1);
    linenoiseHistorySetMaxLen(1000);
    linenoiseHistoryLoad("painful_history.txt");

    while ((line = linenoise(">>> ")) != NULL) {
        if (line[0] != '\0' && line[0] != '.') {
            printf("You wrote: %s\n", line);
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("painful_history.txt");
        } else if (!strncmp(line, ".history", 8)) {
            print_history("painful_history.txt");
        } else if (!strncmp(line, ".program", 8)) {
            // List Program Instructions
        } else if (!strncmp(line, ".execute", 8)) {
            // Run Program
        } else if (!strncmp(line, ".clear", 6)) {
            // Clear current program
        } else if (!strncmp(line, ".undo", 5)) {
            // Undo last instruction
        }
        linenoiseFree(line);
    }
    printf("Goodbye!\n");
    return;
}

