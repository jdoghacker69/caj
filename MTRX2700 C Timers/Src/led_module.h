#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <stdint.h>

void enable_led_clock(void);
void initialise_leds(void);

void led_flash(void);
void led_blink_sequence(void);
void led_flash_left(void);

#endif
