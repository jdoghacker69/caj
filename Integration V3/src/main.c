#include "stm32f303xc.h"
#include "command_parser.h"
#include "serial.h"
#include "timer_module.h"
#include "led_module.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_LENGTH 100

// Global variables for input buffer and flags
volatile bool input_received_flag = false;
volatile uint8_t received_string[MAX_BUFFER_LENGTH];
volatile uint32_t string_index = 0;

// Simple delay used after transmission as a visual indicator (optional)
void finished_transmission(uint32_t bytes_sent)
{
    for (volatile uint32_t i = 0; i < 0x3ffff; i++){ }
}

// Function to reset the input buffer and related variables
void reset_input_buffer(void) {
    string_index = 0;
    input_received_flag = false;
    memset((void *)received_string, 0, MAX_BUFFER_LENGTH);
}

void enable_core_clocks(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
}

int main(void)
{
	enable_core_clocks();
	enable_led_clock();
	enable_periodic_clock();
	enable_oneshot_clock();
	initialise_leds();

    // Initialise the serial port with desired baud rate and optional completion callback
    SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

    while (1) {
        // Continuously check for incoming serial data
        while (SerialInputAvailable(&USART1_PORT)){
            uint8_t c = SerialReadChar(&USART1_PORT);

            // If buffer length + space for termination character not reached
            if (string_index < MAX_BUFFER_LENGTH - 1) {
                received_string[string_index++] = c; 			// Store characters into buffer

                // If newline or carriage return received
                if (c == '\n' || c == '\r') {
                    received_string[string_index - 1] = '\0'; 	// Null-terminate string
                    input_received_flag = true;					// Mark input flag as complete
                }
            } else {
                string_index = 0;
            }
        }

        // Handle input once a full line has been received
        if (input_received_flag)
        {
        	// Print string received for debugging stage
            SerialOutputString((uint8_t *)received_string, &USART1_PORT);

            // Parse command to choose which module to run
            handle_command((const uint8_t *)received_string, &USART1_PORT);

            // Reset buffer and flags for next input
            reset_input_buffer();
        }
    }
}
