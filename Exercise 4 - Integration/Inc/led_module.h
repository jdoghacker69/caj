/* @file led_module.h
 * @brief LED control module for STM32F3 Discovery board using GPIOE pins 8 to 15.
 *
 * This module provides functions to initialise and control the on-board LEDs
 * connected to GPIOE pins 8 through 15. It includes utilities for flashing all LEDs,
 * running a simple left-shifting blink sequence, and setting the LED states using a bitmask.
 */

#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <stdint.h>

/* Initialises GPIOE pins 8 to 15 as outputs for LED control. */
void initialiseLeds(void);

/* Briefly turns all LEDs on using the pattern in `led_mask_pattern`, then off. */
void led_flash(void);

/* Displays a blinking sequence where a single LED turns on and shifts left each call. */
void led_blink_sequence(void);

/* Directly sets LED states using the provided pattern on PE8–PE15.
 * Given 'pattern', a byte where each bit corresponds to an LED
 */
void dio_set_leds(uint8_t pattern);

#endif
