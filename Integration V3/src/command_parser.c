#include "command_parser.h"
#include "serial.h"
#include "timer_module.h"
#include "led_module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

CommandType get_command_type(const uint8_t *input) {
    char command[16] = {0};
    sscanf((const char *)input, "%15s", command);

    if (strcmp(command, "led") == 0) return CMD_LED;
    if (strcmp(command, "serial") == 0) return CMD_SERIAL;
    if (strcmp(command, "oneshot") == 0) return CMD_ONESHOT;
    if (strcmp(command, "timer") == 0) return CMD_TIMER;

    return CMD_UNKNOWN;
}

uint32_t get_command(const uint8_t *input) {
    char command[16] = {0};
    char operand[32] = {0};

    int parsed = sscanf((const char *)input, "%15s %31s", command, operand);

    if (parsed == 2) {
        // Detect base depending on command type
        if (strcmp(command, "led") == 0) {
            // Convert binary string (e.g. "10101010") to number
            return (uint32_t)strtol(operand, NULL, 2);  // base 2
        } else {
            return (uint32_t)atoi(operand);  // base 10
        }
    } else {
        SerialOutputString((uint8_t *)"Failed to parse operand\n", &USART1_PORT);
        return 0;
    }
}

const char* get_string_operand(const uint8_t *input) {
    const char *input_str = (const char *)input;

    // Find the first space after the command word
    const char *space = strchr(input_str, ' ');
    if (space != NULL) {
        return space + 1;  // Return the pointer to the start of the message
    } else {
        return NULL;  // No operand found
    }
}

void handle_command(const uint8_t *input, SerialPort *port) {
    CommandType commandType = get_command_type(input);

    switch (commandType) {
        case CMD_LED:
            SerialOutputString((uint8_t *)"LED command detected\n", port);
            uint8_t pattern = get_command(input);  			// Extract LED pattern from user command
            dio_set_leds(pattern);
            break;
        case CMD_SERIAL:
            SerialOutputString((uint8_t *)"Serial command detected\n", port);
            const char *msg = get_string_operand(input);
                if (msg != NULL) {
                    SerialOutputString((uint8_t *)msg, port);  // Echo message back
                } else {
                    SerialOutputString((uint8_t *)"No message provided\n", port);
                }
            break;
        case CMD_ONESHOT:
            SerialOutputString((uint8_t *)"Oneshot command detected\n", port);
            uint32_t duration = get_command(input);  			// Extract "1000" from "oneshot 1000"
            timer_oneshot(duration, led_flash);              	// Call your timer function
            break;
        case CMD_TIMER:
            SerialOutputString((uint8_t *)"Timer command detected\n", port);
            uint32_t duration2 = get_command(input);  			// Extract "1000" from "oneshot 1000"
            timer_init(duration2, led_flash);              		// Call your timer function
            break;
        default:
            SerialOutputString((uint8_t *)"Unknown command\n", port);
            break;
    }
}
