/*
 * partA.c
 *
 *  Created on: Apr 7, 2025
 *      Author: aurora
 */

#include "timer.h"
#include "diode.h"

/* Button interrupt handler.
 * This function is called when the button is pressed.
 * It calls dio_chase_led_limited() to shift the LED if the timer allows.
 */
void my_button_handler(void)
{
	//dio_chase_led();  // Move the lit LED left

	// Attempt to move the lit LED to the next position
    dio_chase_led_limited();
}

int main(void)
{
	// Initialise the diode module and register the button handler as the callback
	dio_init(my_button_handler);    // Set up LEDs
	timer_init();     				// Set up EXTI0 + timer

    // Main loop: the program runs here forever after initialisation
    while (1)
    {
    	// No polling or blocking required; behaviour is interrupt-driven
    }
}
