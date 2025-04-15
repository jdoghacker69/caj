#include "user_command.h"
#include "serial.h"
#include "timer_module.h"
#include "led_module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

CommandType getCommandType(const uint8_t *input)
{
    char command[16] = {0};
    sscanf((const char *)input, "%15s", command);

    if (strcmp(command, "led") == 0) return CMD_LED;
    if (strcmp(command, "serial") == 0) return CMD_SERIAL;
    if (strcmp(command, "oneshot") == 0) return CMD_ONESHOT;
    if (strcmp(command, "timer") == 0) return CMD_TIMER;

    return CMD_UNKNOWN;
}

uint32_t getCommand(const uint8_t *input)
{
    char command[16] = {0};
    char operand[32] = {0};

    int parsed = sscanf((const char *)input, "%15s %31s", command, operand);

    // If both a command and value were given
    if (parsed == 2)
    {
        // If command led, convert binary string to a number
        if (strcmp(command, "led") == 0)
        {
            return (uint32_t)strtol(operand, NULL, 2);  // base 2
        } else {
            return (uint32_t)atoi(operand);  			// base 10
        }
    } else {
        SerialOutputString((uint8_t *)"No message provided\n", &USART1_PORT);
        return 0;
    }
}

void getStringOperand(const uint8_t *input)
{
    const char *input_str = (const char *)input;

    // Find the first space after the command keyword
    const char *space = strchr(input_str, ' ');
    if (space != NULL && *(space + 1) != '\0')
    {
        // Move past the space to get the operand/message
        const char *operand = space + 1;
        SerialOutputString((uint8_t *)operand, &USART1_PORT);
        SerialOutputChar('\n', &USART1_PORT);
    } else {
        SerialOutputString((uint8_t *)"No message provided\n", &USART1_PORT);
    }
}

void handleCommand(const uint8_t *input, SerialPort *port)
{
    CommandType commandType = getCommandType(input);

    switch (commandType) {
        case CMD_LED:
            SerialOutputString((uint8_t *)"LED command detected\n", port);
            uint8_t pattern = getCommand(input);  	// Extract LED pattern
            dio_set_leds(pattern);
            break;

        case CMD_SERIAL:
            SerialOutputString((uint8_t *)"Serial command detected\n", port);
            getStringOperand(input);  				// Extract and echo string message
            break;

        case CMD_ONESHOT:
            SerialOutputString((uint8_t *)"Oneshot command detected\n", port);
            uint32_t duration = getCommand(input);  // Extract delay in ms
            timerOneshot(duration, led_flash);  	// Start one-shot timer
            break;

        case CMD_TIMER:
            SerialOutputString((uint8_t *)"Timer command detected\n", port);
            uint32_t duration2 = getCommand(input); // Extract interval in ms
            timerPeriodic(duration2, led_flash);  	// Start periodic timer
            break;

        default:
            SerialOutputString((uint8_t *)"Unknown command\n", port);
            break;
    }
}
