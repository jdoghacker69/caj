#include <stdint.h>
#include "stm32f303xc.h"

#define ONESHOT_QUEUE_SIZE 8

typedef struct {
	uint32_t delay_ms;
	void (*callback)(void);
} OneShotEvent;

/*-------------------------------------------------*/

// variables.

static OneShotEvent oneshot_queue[ONESHOT_QUEUE_SIZE];
static uint8_t queue_head = 0;
static uint8_t queue_tail = 0;
static uint8_t oneshot_active = 0;

volatile uint32_t interval_ms = 0;
static void (*periodic_callback)(void) = 0;
static void (*oneshot_callback)(void) = 0;

/*--------------------------------------------------*/

// pre-scaler adjusts clock frequency.

static void trigger_prescaler(TIM_TypeDef *TIMx) {
    TIMx->EGR |= TIM_EGR_UG;
}

/*--------------------------------------------------*/

// enables clock timers

void enable_periodic_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

void enable_oneshot_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}

/*--------------------------------------------------*/

// creates a dynamically linked list to queue one-shots.

uint32_t enqueue_oneshot(uint32_t delay_ms, void(*callback)(void)) {
	uint8_t next = (queue_tail + 1) % ONESHOT_QUEUE_SIZE;

	if (next == queue_head) {
		return -1;
	}

	oneshot_queue[queue_tail].delay_ms = delay_ms;
	oneshot_queue[queue_tail].callback = callback;
	queue_tail = next;

	return 0;
}

uint32_t dequeue_oneshot(OneShotEvent *event) {
	if (queue_head == queue_tail) {
		return -1;
	}

	*event = oneshot_queue[queue_head];
	queue_head = (queue_head + 1) % ONESHOT_QUEUE_SIZE;

	return 0;
}

/*--------------------------------------------------*/

// setter and getter for modularity.

void set_timer_period(uint32_t new_ms) {
	interval_ms = new_ms;
	TIM2->ARR = interval_ms * 1;
	trigger_prescaler(TIM2);
}

uint32_t get_timer_period(void) {
	return interval_ms;
}

/*--------------------------------------------------*/

// a timer thats calls a function periodically on a set delay.
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
    if (oneshot_active) {
        enqueue_oneshot(ms, callback);
        return;
    }

    oneshot_callback = callback;
    oneshot_active = 1;

    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->PSC = 7999;
    TIM3->ARR = ms - 1;
    TIM3->CNT = 0;
    trigger_prescaler(TIM3);

    TIM3->SR &= ~TIM_SR_UIF;
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}

/*--------------------------------------------------*/

// handles the interrupt calls of each timer.

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (periodic_callback) {
            periodic_callback();
        }
    }
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;

        TIM3->CR1 &= ~TIM_CR1_CEN;
		oneshot_active = 0;

        if (oneshot_callback) {
            oneshot_callback();
            oneshot_callback = 0;
        }

        // triggers the next one-shot in queue.
        OneShotEvent next;
        if (dequeue_oneshot(&next) == 0) {
            timer_oneshot(next.delay_ms, next.callback);
        }
    }
}


