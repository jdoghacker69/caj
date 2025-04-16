#include "user_command.h"
#include "timer_module.h"
#include "led_module.h"
#include "serial.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


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

static int isValidOperand(const char *command, const char *operand)
{
    if (strcmp(command, "led") == 0) {
        // Check that it's exactly 8 characters of 0s and 1s
        if (strlen(operand) != 8) return 0;
        for (int i = 0; i < 8; i++) {
            if (operand[i] != '0' && operand[i] != '1') return 0;
        }
        return 1;
    }

    if (strcmp(command, "timer") == 0 || strcmp(command, "oneshot") == 0) {
        // Check all digits
        for (int i = 0; operand[i] != '\0'; i++) {
            if (!isdigit((unsigned char)operand[i])) return 0;
        }
        int value = atoi(operand);
        if (strcmp(command, "timer") == 0 && value >= 2) return 1;
        if (strcmp(command, "oneshot") == 0 && value >= 1) return 1;
        return 0;
    }

    // Default fallback (accept digits)
    for (int i = 0; operand[i] != '\0'; i++) {
        if (!isdigit((unsigned char)operand[i])) return 0;
    }

    return 1;
}

uint32_t getCommand(const uint8_t *input)
{
    char command[16] = {0};
    char operand[32] = {0};

    int parsed = sscanf((const char *)input, "%15s %31s", command, operand);

    if (parsed == 2)
    {
        if (!isValidOperand(command, operand)) {
            SerialOutputString((uint8_t *)"Invalid operand for command\n", &USART1_PORT);
            return 0;
        }

        if (strcmp(command, "led") == 0) {
            return (uint32_t)strtol(operand, NULL, 2);  // base 2
        } else {
            return (uint32_t)atoi(operand);  			// base 10
        }
    }
    else
    {
        SerialOutputString((uint8_t *)"Please structure your instruction '[command] [value]'\n", &USART1_PORT);
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
        SerialOutputString((uint8_t *)"No message provided, please add a message after the command\n", &USART1_PORT);
    }
}

void handleCommand(const uint8_t *input, SerialPort *port)
{
    CommandType commandType = getCommandType(input);

    switch (commandType) {
        case CMD_LED:
            uint8_t pattern = getCommand(input);  	// Extract LED pattern
            dio_set_leds(pattern);
            break;

        case CMD_SERIAL:
            getStringOperand(input);  				// Extract and echo string message
            break;

        case CMD_ONESHOT:
            uint32_t duration = getCommand(input);  // Extract delay in ms
            timerOneshot(duration, led_flash);
            break;

        case CMD_TIMER:
            uint32_t duration2 = getCommand(input); // Extract interval in ms
            timerPeriodic(duration2, led_flash);
            break;

        default:
            SerialOutputString((uint8_t *)"Unknown command\n", port);
            break;
    }
}
