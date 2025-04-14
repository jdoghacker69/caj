#include <stdint.h>
#include "stm32f303xc.h"

// variables.
volatile uint32_t interval_ms = 0;
static void (*periodic_callback)(void) = 0;
static void (*oneshot_callback)(void) = 0;

static void trigger_prescaler(TIM_TypeDef *TIMx) {
    TIMx->EGR |= TIM_EGR_UG;
}

// TIMER2
void enable_periodic_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

// TIMER3
void enable_oneshot_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}

// setter to keep variables contained.
void set_timer_period(uint32_t new_ms) {
	interval_ms = new_ms;
	TIM2->ARR = interval_ms * 1;
	trigger_prescaler(TIM2);
}

// getter to keep variables contained.
uint32_t get_timer_period(void) {
	return interval_ms;
}

// a timer that calls a function repeatedly on a set interval.
void timer_init(uint32_t ms, void (*callback)(void)) {
    periodic_callback = callback;

    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = 7999;
    TIM2->ARR = ms * 1;
    TIM2->CNT = 0;
    trigger_prescaler(TIM2);

    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

// a timer that calls a function once after a set delay.
void timer_oneshot(uint32_t ms, void(*callback)(void)) {
	oneshot_callback = callback;

	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->PSC = 7999;
	TIM3->ARR = ms * 1;
	TIM3->CNT = 0;
	trigger_prescaler(TIM3);

	TIM3->SR &= ~TIM_SR_UIF;
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn);

	TIM3->CR1 |= TIM_CR1_CEN;
}

// handles the interrupt calls of the timer2.
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (periodic_callback) {
            periodic_callback();
        }
    }
}

// handles the interrupt calls of the timer3.
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        if (oneshot_callback) {
            oneshot_callback();
            oneshot_callback = 0;
        }

        TIM3->CR1 &= ~TIM_CR1_CEN;
    }
}
