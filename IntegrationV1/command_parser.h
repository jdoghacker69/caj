// command_parser.h
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

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

#endif // COMMAND_PARSER_H
