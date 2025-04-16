#include "serial.h"
#include "stm32f303xc.h"
#include "timer_module.h"
#include "led_module.h"
#include "user_command.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_LENGTH 100  // Maximum number of characters allowed in the input buffer

// Default baud rate (can be set to: BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, BAUD_115200)
volatile uint8_t baud_rate = BAUD_115200;

/* Adds a visible delay after a message is sent, used as a visual indicator. */
void finishedTransmission(uint32_t bytes_sent) {
    for (volatile uint32_t i = 0; i < 0x3FFFF; i++) { }
}

int main(void) {
    // Initialise hardware modules
    initialiseTimers();
    initialiseLeds();
    SerialInitialise(baud_rate, &USART1_PORT, &finishedTransmission);
	setupNVIC();

    while (1) {
    }
}
