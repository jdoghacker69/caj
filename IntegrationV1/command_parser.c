
// command_parser.c
#include "command_parser.h"
#include <string.h>
#include <stdlib.h>

CommandType get_command_type(const uint8_t *input) {
    if (input == NULL) return CMD_UNKNOWN;

    // Copy the input string into a buffer to avoid modifying the original
    char buffer[100];
    strncpy(buffer, (const char *)input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Get the first word
    char *command = strtok(buffer, " \n\r");
    if (!command) return CMD_UNKNOWN;

    if (strcmp(command, "led") == 0) return CMD_LED;
    if (strcmp(command, "serial") == 0) return CMD_SERIAL;
    if (strcmp(command, "oneshot") == 0) return CMD_ONESHOT;
    if (strcmp(command, "timer") == 0) return CMD_TIMER;

    return CMD_UNKNOWN;
}
