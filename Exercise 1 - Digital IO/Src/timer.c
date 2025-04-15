/*
 * timer.c
 *
 *  Created on: Apr 9, 2025
 *      Author: aurora
 */

#include "stm32f303xc.h"
#include "timer.h"

// Initially set the global flag to 1 to allow LED update
volatile uint8_t led_update_allowed = 1;

static void (*timer2_callback)(void) = 0;

/*
 * Timer callback function.
 * Called on every timer tick to enable the next LED update.
 */
static void timer_tick_handler(void)
{
    led_update_allowed = 1;
}

void timer_init(void)
{
    // Configure EXTI line 0 for PA0 (button input)
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;  // Route EXTI0 to PA0
    EXTI->RTSR |= EXTI_RTSR_TR0;                 // Rising edge trigger
    EXTI->IMR  |= EXTI_IMR_MR0;                  // Unmask interrupt
    NVIC_SetPriority(EXTI0_IRQn, 1);             // Set interrupt priority
    NVIC_EnableIRQ(EXTI0_IRQn);                  // Enable EXTI0 interrupt

    // Initialise timer and register the timer callback
    timer2_init();
    timer2_set_callback(timer_tick_handler);
}

void timer2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7199;
    TIM2->ARR = 2000;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void timer2_set_callback(void (*callback)(void))
{
    timer2_callback = callback;
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;

    if (timer2_callback != 0) {
        timer2_callback();
    }
}
