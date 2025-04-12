#include <stdint.h>
#include "stm32f303xc.h"

// global variables.
volatile uint32_t interval_ms = 0;
void (*callback_function)(void) = 0;

static void trigger_prescaler(void) {
    TIM2->EGR |= TIM_EGR_UG;
}


void enable_timer_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

void set_timer_period(uint32_t new_ms) {
	interval_ms = new_ms;
	TIM2->ARR = interval_ms * 1000;
	trigger_prescaler();
}

uint32_t get_timer_period(void) {
	return interval_ms;
}

void timer_init(uint32_t ms, void (*callback)(void)) {
    interval_ms = ms;
    callback_function = callback;

    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = 7;
    TIM2->ARR = interval_ms * 1000;
    trigger_prescaler();
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (callback_function) {
            callback_function();
        }
    }
}
