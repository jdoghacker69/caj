/*
 * diode.h
 *
 *  Created on: Apr 7, 2025
 *      Author: aurora
 */

#ifndef DIODE_H_
#define DIODE_H_

#include <stdint.h>

/*
 * Initialises GPIO for LEDs and button.
 * - Registers the provided button callback function
 * - Configures PE8–PE15 as output pins
 */
void dio_init(void (*button_callback)(void));

/*
 * Updates the output state of LEDs on PE8–PE15.
 * Takes an 8-bit pattern, where bit 0 corresponds to PE8.
 */
void dio_set_leds(uint8_t pattern);

/*
 * Returns the current 8-bit LED pattern (PE8–PE15)
 */
uint8_t dio_get_leds(void);

/*
 * Shifts the active LED one position to the left but
 * only allows updates when the timer grants permission.
 * Wraps around when it overflows.
 */
void dio_chase_led_limited(void);

/*
 * EXTI0 interrupt handler.
 * Called when button on PA0 is pressed.
 */
void EXTI0_IRQHandler(void);

#endif
