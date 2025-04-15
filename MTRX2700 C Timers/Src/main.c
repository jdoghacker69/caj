#include "stm32f303xc.h"
#include "timer_module.h"
#include "led_module.h"

void enable_core_clocks(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
}

int main(void)
{
    enable_core_clocks();
    enable_led_clock();

    enable_periodic_clock();
    enable_oneshot_clock();

    initialise_leds();

    // testing.
    timer_init(1000, led_blink_sequence);
    timer_oneshot(4000, led_flash);

    while (1) {
    	// idle loop
    }
}
