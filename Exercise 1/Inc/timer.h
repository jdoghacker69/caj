/*
 * timer.h
 *
 *  Created on: Apr 9, 2025
 *      Author: aurora
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f303xc.h"

// Flag set by the timer to allow one LED update
extern volatile uint8_t led_update_allowed;

/*
 * Sets up interrupts and timer.
 * - Sets up EXTI0 for rising edge interrupts on PA0
 * - Initialises TIM2 for periodic updates
 */
void timer_init(void);

/* The timer2 function initialises TIM2 with preset values (10kHz base, 200ms interval) */
void timer2_init(void);

/* The timer2_set_callback function registers a callback to be called on every TIM2 update event */
void timer2_set_callback(void (*callback)(void));

#endif
