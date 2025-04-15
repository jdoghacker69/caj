/* @file timer_module.h
 * @brief Timer control module for STM32F3 Discovery board using TIM2 and TIM3.
 *
 * This module provides an interface for configuring and managing two types of timers:
 * - TIM2: a periodic timer that repeatedly triggers a callback function at a specified interval.
 * - TIM3: a one-shot timer that triggers a callback function once after a set delay.
 *
 * The module includes functions to initialise the timers, set or retrieve the periodic timer interval,
 * and assign callback functions to be executed on timer events. It is designed for use with
 * STM32F303xC micro-controllers.
 */

#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include <stdint.h>

void print_oneshot_queue(void);

/* Enables the peripheral clock for TIM2 (the periodic timer) and TIM3 (the one-shot timer). */
void initialiseTimers(void);

/* Updates the interval delay for the periodic timer.
 * Given new_ms, new period in milliseconds. */
void setTimerPeriod(uint32_t new_ms);

/* Returns the current periodic timer's period value. */
uint32_t getTimerPeriod(void);

/* Initialises the periodic timer, sets its callback function, and calls the function after every interval.
 * Given ms, period in milliseconds.
 * Given callback, function to call every period. */
void timerPeriodic(uint32_t ms, void (*callback)(void));

/* Initialises the one-shot timer and sets its callback function. Calls the function once after a set delay.
 * Given ms, period in milliseconds.
 * Given callback, function to call after delay. */
void timerOneshot(uint32_t ms, void (*callback)(void));

#endif
