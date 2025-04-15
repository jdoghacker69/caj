/*
 * diode.c
 *
 *  Created on: Apr 7, 2025
 *      Author: aurora
 */

#include "stm32f303xc.h"
#include "diode.h"
#include "timer.h"

// Function pointer for external button press callback
static void (*btn_callback)(void) = 0x00;

// Current state of the LEDs (bitmask for PE8–PE15)
static uint8_t led_state = 0x02;


void dio_init(void (*button_callback)(void))
{
    btn_callback = button_callback;

    // Enable clocks for GPIOA, GPIOC, GPIOE, and SYSCFG
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Configure PE8–PE15 as general-purpose outputs (MODER = 01b per pin)
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;

    // Set initial LED pattern
    dio_set_leds(led_state);
}


void dio_set_leds(uint8_t pattern)
{
	led_state = pattern;  // Set the current LED state to the new pattern (8-bit bitmask for the LEDs)

	uint8_t *led_output = ((uint8_t *)&(GPIOE->ODR)) + 1;  // Get the address of the GPIOE output data register (ODR), and offset by 1 to access the appropriate bits for PE8-PE15

	*led_output = led_state;  // Write the current LED state (pattern) to the GPIOE output register, updating the LEDs
}


uint8_t dio_get_leds(void)
{
    return led_state;
}


void dio_chase_led(void)
{
	if (led_update_allowed) {  // Check if the timer has granted permission to update the LED pattern
	    led_update_allowed = 0;  // Consume the permission (set flag to 0 so that next update must wait for the timer)

	    led_state <<= 1;  // Shift the current LED pattern one position to the left (move the "lit" LED to the next position)

	    if (led_state == 0) {  // Check if the shifted LED pattern has reached zero (i.e., all LEDs are off)
	        led_state = 0x01;  // If all LEDs are off, restart from the first LED (PE8) by setting the first bit
	    }

	    dio_set_leds(led_state);  // Update the actual LEDs with the new pattern (display the new "lit" LED)
	}
}


void EXTI0_IRQHandler(void)
{
	// Check if a user-defined button handler has been set (callback is not null)
    if (btn_callback != 0x00) {
        btn_callback();  // Call user-defined button handler
    }

    EXTI->PR |= EXTI_PR_PR0;  // Clear EXTI line 0 pending bit
}
