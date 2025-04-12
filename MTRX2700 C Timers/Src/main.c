#include "stm32f303xc.h"
#include "timer_module.h"
#include "led_module.h"

void enable_core_clocks(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
}

int main(void)
{
    enable_core_clocks();
    enable_led_clock();
    enable_timer_clock();
    initialise_leds();

    // testing.
    timer_init(1000, led_flash);
    //timer_oneshot(10000, led_flash_left);

    while (1) {
    	// idle loop
    }
}
