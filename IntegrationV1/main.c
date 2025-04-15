#include <stdint.h>
#include <stdbool.h>
#include "stm32f303xc.h"
#include "serial.h"
#include <string.h>  // for strcmp
#include "command_parser.h" // Include the module

#define MAX_BUFFER_LENGTH 100

volatile bool input_received_flag = false;
volatile uint8_t received_string[MAX_BUFFER_LENGTH];
volatile uint32_t string_index = 0;

// Optional visual delay after transmission
void finished_transmission(uint32_t bytes_sent) {
    for (volatile uint32_t i = 0; i < 0x3ffff; i++) { }
}

// 🆕 Function to check if input is the command "start"
bool is_start_command(const uint8_t *input) {
    return strcmp((const char *)input, "start") == 0;
}

int main(void) {
    SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

    while (1) {
        // Keep reading while data is available
        while (SerialInputAvailable(&USART1_PORT)) {
            uint8_t c = SerialReadChar(&USART1_PORT);

            if (string_index < MAX_BUFFER_LENGTH - 1) {
                received_string[string_index++] = c;

                if (c == '\n' || c == '\r') {
                    received_string[string_index - 1] = '\0'; // null-terminate
                    input_received_flag = true;
                }
            } else {
                string_index = 0;
            }
        }

        if (input_received_flag) {
            SerialOutputString((uint8_t *)received_string, &USART1_PORT);

            CommandType cmd = get_command_type(received_string);
            switch (cmd) {
                case CMD_LED:
                    SerialOutputString((uint8_t *)"LED command detected\n", &USART1_PORT);
                    break;
                case CMD_SERIAL:
                    SerialOutputString((uint8_t *)"Serial command detected\n", &USART1_PORT);
                    break;
                case CMD_ONESHOT:
                    SerialOutputString((uint8_t *)"Oneshot command detected\n", &USART1_PORT);
                    break;
                case CMD_TIMER:
                    SerialOutputString((uint8_t *)"Timer command detected\n", &USART1_PORT);
                    break;
                default:
                    SerialOutputString((uint8_t *)"Unknown command\n", &USART1_PORT);
                    break;
            }

            // Reset buffer and flags
            string_index = 0;
            input_received_flag = false;
            for (uint32_t i = 0; i < MAX_BUFFER_LENGTH; i++) {
                received_string[i] = 0;
            }
        }
/*	CODE TO CHECK FOR START COMMAND FOR EXERCISE 2
        if (input_received_flag) {
            SerialOutputString((uint8_t *)received_string, &USART1_PORT);

            // 🆕 Check if the command is "start"
            if (is_start_command(received_string)) {
                uint8_t response[] = "Command START received.\n";
                SerialOutputString(response, &USART1_PORT);
            }

            // Reset
            string_index = 0;
            input_received_flag = false;

            for (uint32_t i = 0; i < MAX_BUFFER_LENGTH; i++) {
                received_string[i] = 0;
            }
        } */
    }
}
