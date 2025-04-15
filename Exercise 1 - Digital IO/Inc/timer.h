/*
 * timer.h
 *
 * Timer Module Header - STM32F3
 * --------------------------------
 * This module provides functionality for initialising and handling
 * timer-based interrupts on the STM32F3 Discovery board.
 *
 * Features:
 * - Sets up external interrupt (EXTI0) on PA0 for rising edge detection.
 * - Configures TIM2 as a periodic timer for regular updates (10kHz base, 200ms interval).
 * - Allows user-defined callback registration for TIM2 update events.
 * - Declares a global flag (`led_update_allowed`) used to trigger LED updates.
 *
 * Created on: Apr 9, 2025
 * Author: aurora
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
