#include "command_parser.h"
#include "serial.h"
#include <string.h>

CommandType get_command_type(const uint8_t *input) {
    char command[16] = {0};
    sscanf((const char *)input, "%15s", command);

    if (strcmp(command, "led") == 0) return CMD_LED;
    if (strcmp(command, "serial") == 0) return CMD_SERIAL;
    if (strcmp(command, "oneshot") == 0) return CMD_ONESHOT;
    if (strcmp(command, "timer") == 0) return CMD_TIMER;

    return CMD_UNKNOWN;
}

void handle_command(const uint8_t *input, SerialPort *port) {
    CommandType cmd = get_command_type(input);

    switch (cmd) {
        case CMD_LED:
            SerialOutputString((uint8_t *)"LED command detected\n", port);
            break;
        case CMD_SERIAL:
            SerialOutputString((uint8_t *)"Serial command detected\n", port);
            break;
        case CMD_ONESHOT:
            SerialOutputString((uint8_t *)"Oneshot command detected\n", port);
            break;
        case CMD_TIMER:
            SerialOutputString((uint8_t *)"Timer command detected\n", port);

            break;
        default:
            SerialOutputString((uint8_t *)"Unknown command\n", port);
            break;
    }
}
