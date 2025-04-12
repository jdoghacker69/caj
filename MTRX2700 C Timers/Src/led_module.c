#include <stdint.h>
#include "stm32f303xc.h"


static uint8_t led_mask_pattern = 0b11111111;
static uint8_t *led_output_register;

void enable_led_clock(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
}

void initialise_leds(void) {
    uint16_t *moder_upper = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *moder_upper = 0x5555;
    led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
}

void led_flash(void) {
    *led_output_register = led_mask_pattern;
    for (volatile int i = 0; i < 100000; ++i);
    *led_output_register = 0x00;
}
