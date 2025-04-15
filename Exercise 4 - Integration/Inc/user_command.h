/**
 * @file user_command.h
 * Command parsing and handling interface for user input over serial communication.
 *
 * This module provides functionality to interpret textual commands received over
 * the serial interface and trigger appropriate actions on the STM32 board, such as
 * controlling LEDs or starting timers.
 *
 * Supported commands:
 * - "led <binary_pattern>"    : Sets LEDs PE8–PE15 based on the binary pattern (e.g., "led 10101010").
 * - "serial <message>"        : Echoes back the message sent after the command.
 * - "oneshot <duration_ms>"   : Starts a one-shot timer for the specified duration in milliseconds.
 * - "timer <interval_ms>"     : Starts a periodic timer with the specified interval in milliseconds.
 */

#ifndef USER_COMMAND_H
#define USER_COMMAND_H

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

/* Determines the command type based on the first word of the input string.
 * Given input, the full user input string
 * Returns a value from the CommandType enum representing the detected command.
 */
CommandType getCommandType(const uint8_t *input);

/* Extracts and converts the operand (e.g., number or pattern) from the input string.
 * For the "led" command, it converts a binary string to a number.
 * For all other known commands, it treats the operand as a base-10 integer.
 * Given input, the full user input string
 * Returns the parsed operand as a uint32_t. Returns 0 if no operand is provided.
 */
void handleCommand(const uint8_t *input, SerialPort *port);

/* Extracts and prints the string operand after the first word of input.
 * Used with the "serial" command to echo a message.
 * Given input, the full user input string.
 */
uint32_t getCommand(const uint8_t *input);

/* Main command handler. Parses input and dispatches to the appropriate handler.
 * Given input, The full user input string.
 * Given port, The serial port to output responses to.
 */
void getStringOperand(const uint8_t *input);

#endif // USER_COMMAND_H
