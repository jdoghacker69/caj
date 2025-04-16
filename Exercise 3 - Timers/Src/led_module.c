#include <stdint.h>
#include "stm32f303xc.h"

// variables.
static uint8_t led_mask_pattern = 0b11111111;
static uint8_t led_mask_pattern_left = 0b11110000;
static uint8_t *led_output_register;

void enable_led_clock(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
}

// enables the leds.
void initialise_leds(void) {
    uint16_t *moder_upper = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *moder_upper = 0x5555;
    led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
}

// flashes a bit-mask of the leds.
void led_flash(void) {
    *led_output_register = led_mask_pattern;
    for (volatile int i = 0; i < 100000; ++i);
    *led_output_register = 0x00;
}

void led_blink_sequence(void) {
    static uint8_t pattern = 0x01;
    *led_output_register = pattern;
    pattern <<= 1;
    if (pattern == 0) pattern = 0x01;
}

// flashes half the leds. for testing mostly.
void led_flash_left(void) {
    *led_output_register = led_mask_pattern_left;
    for (volatile int i = 0; i < 100000; ++i);
    *led_output_register = 0x00;
}
