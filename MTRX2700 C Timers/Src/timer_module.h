/**
 * @file    timer.h
 * @author  530508812
 * @brief   Timer module interface for delay and periodic callbacks
 * @version 1.0
 * @date    2025-04-15
 *
 * @copyright Copyright (c) 2025
 */

#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include <stdint.h>

/**
 * Initializes the periodic timer, sets its callback function, and calls the function after every interval.
 *
 * @param ms        Period in milliseconds.
 * @param callback  Function to call every period.
 */
void timer_init(uint32_t ms, void (*callback)(void));

/**
 * Initializes the one-shot timer and sets its callback function. Calls the function once after a set delay.
 *
 * @param ms        Period in milliseconds.
 * @param callback  Function to call after delay.
 */
void timer_oneshot(uint32_t ms, void (*callback)(void));

/**
 * Enables the peripheral clock for TIM2 (the periodic timer).
 */
void enable_periodic_clock(void);

/**
 * Enables the peripheral clock for TIM3 (the one-shot timer).
 */
void enable_oneshot_clock(void);

/**
 * Updates the interval delay for the periodic timer.
 *
 * @param new_ms        new period in milliseconds.
 */
void set_timer_period(uint32_t new_ms);

/**
 * Returns the current periodic timer's period value.
 *
 * @return uint32_t Current timer interval delay that is active.
 */
uint32_t get_timer_period(void);

#endif
