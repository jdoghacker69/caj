#include <stdint.h>
#include <stdbool.h>
#include "stm32f303xc.h"
#include "serial.h"

#define MAX_BUFFER_LENGTH 100

volatile bool input_received_flag = false;
volatile uint8_t received_string[MAX_BUFFER_LENGTH];
volatile uint32_t string_index = 0;

void finished_transmission(uint32_t bytes_sent) {
    for (volatile uint32_t i = 0; i < 0x8ffff; i++) { }
}

int main(void) {
    SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

    while (1) {
        // Read all characters in RX buffer
        while (SerialInputAvailable(&USART1_PORT)) {
            uint8_t c = SerialReadChar(&USART1_PORT);

            if (string_index < MAX_BUFFER_LENGTH - 1) {
                received_string[string_index++] = c;

                // Treat newline or carriage return as end of string
                if (c == '\n' || c == '\r') {
                    received_string[string_index - 1] = '\0'; // Overwrite newline
                    input_received_flag = true;
                    break; // process it
                }
            } else {
                // Buffer overflow, reset to avoid issues
                string_index = 0;
            }
        }

        if (input_received_flag) {
            SerialOutputString((uint8_t *)received_string, &USART1_PORT);
            string_index = 0;
            input_received_flag = false;
        }
    }
}
