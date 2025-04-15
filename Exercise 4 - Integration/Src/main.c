#include "stm32f303xc.h"
#include "serial.h"
#include "timer_module.h"
#include "led_module.h"
#include "user_command.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_LENGTH 100  // Maximum number of characters allowed in the input buffer

// Global variables for input handling
volatile bool input_received_flag = false;               // Flag set when a full input line is received
volatile uint8_t received_string[MAX_BUFFER_LENGTH];     // Input buffer for storing incoming characters
volatile uint32_t string_index = 0;                      // Current index in the input buffer

// Default baud rate (can be set to: BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, BAUD_115200)
volatile uint8_t baud_rate = BAUD_115200;

/* Adds a visible delay after a message is sent, used as a visual indicator. */
void finishedTransmission(uint32_t bytes_sent) {
    for (volatile uint32_t i = 0; i < 0x3FFFF; i++) { }
}

/* Clears the stored string, resets the index, and clears the input received flag. */
void resetInputBuffer(void) {
    string_index = 0;
    input_received_flag = false;
    memset((void *)received_string, 0, MAX_BUFFER_LENGTH);
}

int main(void) {
    // Initialise hardware modules
    initialiseTimers();
    initialiseLeds();
    SerialInitialise(baud_rate, &USART1_PORT, &finishedTransmission);

    while (1) {
        // Check for incoming serial data
        while (SerialInputAvailable(&USART1_PORT)) {
            uint8_t c = SerialReadChar(&USART1_PORT);  // Read a character from the serial port

            // Ensure buffer doesn't overflow (reserve space for null terminator)
            if (string_index < MAX_BUFFER_LENGTH - 1) {
                received_string[string_index++] = c;

                // End of input line detected (newline or carriage return)
                if (c == '\n' || c == '\r') {
                    received_string[string_index - 1] = '\0';  	// Replace with null terminator
                    input_received_flag = true;               	// Mark that a complete input line has been received
                }
            } else {
                string_index = 0;	// Input too long, reset buffer to avoid overflow
            }
        }

        // Once a complete input line is available
        if (input_received_flag) {
            handleCommand((const uint8_t *)received_string, &USART1_PORT);  // Process command
            resetInputBuffer();                                             // Prepare buffer for next input
        }
    }
}
