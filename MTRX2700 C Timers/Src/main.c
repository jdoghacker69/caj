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

    timer_init(200, led_flash);

    uint32_t a = 1;

    while (1) {
    	// idle loop
    	if (a == 1) {
    		set_timer_period(200); // 5x a second.
    		a = 0;
    	} else if (a == 2) {
    		set_timer_period(1000); // 1x a second.
    		a = 0;
    	}
    }
}
