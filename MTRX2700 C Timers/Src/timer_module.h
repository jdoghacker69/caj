#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include <stdint.h>

void timer_init(uint32_t ms, void (*callback)(void));
void timer_oneshot(uint32_t ms, void (*callback)(void));

void enable_periodic_clock(void);
void enable_oneshot_clock(void);

void set_timer_period(uint32_t new_ms);
uint32_t get_timer_period(void);

#endif
