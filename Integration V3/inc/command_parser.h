#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "serial.h"
#include "timer_module.h"
#include "led_module.h"
#include <stdint.h>
#include <stdbool.h>

// Enum to represent command types
typedef enum {
    CMD_UNKNOWN,
    CMD_LED,
    CMD_SERIAL,
    CMD_ONESHOT,
    CMD_TIMER
} CommandType;

// Function to extract command type from input string
CommandType get_command_type(const uint8_t *input);

// Function to handle command switch logic
void handle_command(const uint8_t *input, SerialPort *port);

uint32_t get_command(const uint8_t *input);

const char* get_string_operand(const uint8_t *input);

#endif // COMMAND_PARSER_H
