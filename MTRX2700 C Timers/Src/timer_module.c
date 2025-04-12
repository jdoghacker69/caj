#include <stdint.h>
#include "stm32f303xc.h"

// variables.
static uint8_t oneshot_mode = 0;
volatile uint32_t interval_ms = 0;
void (*callback_function)(void) = 0;

static void trigger_prescaler(void) {
    TIM2->EGR |= TIM_EGR_UG;
}

void enable_timer_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

// setter to keep variables contained.
void set_timer_period(uint32_t new_ms) {
	interval_ms = new_ms;
	TIM2->ARR = interval_ms * 1000;
	trigger_prescaler();
}

// getter to keep variables contained.
uint32_t get_timer_period(void) {
	return interval_ms;
}

// a timer that calls a function repeatedly on a set interval.
void timer_init(uint32_t ms, void (*callback)(void)) {
    interval_ms = ms;
    callback_function = callback;

    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = 0x07;
    TIM2->ARR = interval_ms * 1000;
    trigger_prescaler();
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

// a timer that calls a function once after a set delay.
void timer_oneshot(uint32_t ms, void(*callback)(void)) {
	interval_ms = ms;
	callback_function = callback;
	oneshot_mode = 1;

	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->PSC = 0x07;
	TIM2->ARR = interval_ms * 1000;
	trigger_prescaler();
	TIM2->CNT = 0;

	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->CR1 |= TIM_CR1_CEN;
}

// handles the interrupt calls of the timer functions.
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        if (callback_function) {
            callback_function();
        }

        if (oneshot_mode) {
        	TIM2->CR1 &= ~TIM_CR1_CEN;
        	callback_function = 0;
        	oneshot_mode = 0;
        }
    }
}
