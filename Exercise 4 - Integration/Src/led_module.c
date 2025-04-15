#include <stdint.h>
#include "stm32f303xc.h"

// Mask used to control which LEDs are turned on (PE8–PE15)
static uint8_t led_mask_pattern = 0b11111111;

// Pointer to the upper byte of GPIOE's output data register (ODR),
// corresponding to PE8–PE15, which are used as LEDs on the STM32F3 Discovery board
static uint8_t *led_output_register;


void initialiseLeds(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;							// Enable GPIOE clock
    uint16_t *moder_upper = ((uint16_t *)&(GPIOE->MODER)) + 1; 	// Pointer to upper 16 bits of GPIOE->MODER
    *moder_upper = 0x5555;										// Set PE8–PE15 to output mode
    led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;		// Set pointer to the upper byte of ODR to control PE8–PE15
}


void led_flash(void)
{
    *led_output_register = led_mask_pattern;  	// Turn on LEDs as per mask
    for (volatile int i = 0; i < 100000; ++i);  // Delay loop
    *led_output_register = 0x00;  				// Turn off all LEDs
}


void led_blink_sequence(void)
{
    static uint8_t pattern = 0x01;  	// Start with first LED (PE8)
    *led_output_register = pattern;  	// Display current pattern
    pattern <<= 1;  					// Shift to the next LED on the left
    if (pattern == 0) pattern = 0x01;  	// Wrap around to first LED after last
}


void dio_set_leds(uint8_t pattern)
{
    uint8_t *led_output = ((uint8_t *)&(GPIOE->ODR)) + 1;   // Access upper byte of ODR
    *led_output = pattern;  								// Set LEDs according to pattern
}
