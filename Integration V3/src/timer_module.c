#include <stdint.h>
#include "stm32f303xc.h"

// defines the maximum number of queued events.
#define ONESHOT_QUEUE_SIZE 8

// structure containing a single one-shot event to be cloned.
typedef struct {
	uint32_t delay_ms;
	void (*callback)(void);
} OneShotEvent;

// one-shot event queue variables.
static OneShotEvent oneshot_queue[ONESHOT_QUEUE_SIZE];
static uint8_t queue_head = 0;
static uint8_t queue_tail = 0;
static uint8_t oneshot_active = 0;

// timer variables.
volatile uint32_t interval_ms = 0;
static void (*periodic_callback)(void) = 0;
static void (*oneshot_callback)(void) = 0;


// force updates the PSC and ARR settings.
static void trigger_prescaler(TIM_TypeDef *TIMx) {
    TIMx->EGR |= TIM_EGR_UG;	// update the Update Generation bit.
}

// enqueues a one-shot event if buffer is not full.
static uint32_t enqueue_oneshot(uint32_t delay_ms, void(*callback)(void)) {
	uint8_t next = (queue_tail + 1) % ONESHOT_QUEUE_SIZE;

	// buffer is full.
	if (next == queue_head) {
		return -1;
	}

	// store event data at end of list and shift tail.
	oneshot_queue[queue_tail].delay_ms = delay_ms;
	oneshot_queue[queue_tail].callback = callback;
	queue_tail = next;

	return 0;
}

// pushes to the next one-shot event if buffer is not empty.
static uint32_t dequeue_oneshot(OneShotEvent *event) {
	// buffer is empty.
	if (queue_head == queue_tail) {
		return -1;
	}

	// push event to run and advance queue.
	*event = oneshot_queue[queue_head];
	queue_head = (queue_head + 1) % ONESHOT_QUEUE_SIZE;

	return 0;
}

void enable_periodic_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

void enable_oneshot_clock(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}

void set_timer_period(uint32_t new_ms) {
	interval_ms = new_ms;
	TIM2->ARR = interval_ms;	// updates time delay value.
	trigger_prescaler(TIM2);	// force updates PSC and ARR.
}

uint32_t get_timer_period(void) {
	return interval_ms;
}

void timer_init(uint32_t ms, void (*callback)(void)) {
    periodic_callback = callback;

    TIM2->CR1 &= ~TIM_CR1_CEN;	// disables the counter.

    // timing.
    TIM2->PSC = 7999;			// PSC = 8MHz / (7999+1) = 1kHz.
    TIM2->ARR = ms - 1;			// set time delay value.
    TIM2->CNT = 0;				// reset timer counter value.
    trigger_prescaler(TIM2);	// force updates PSC and ARR.

    // interrupts.
    TIM2->SR &= ~TIM_SR_UIF;	// clear active update interrupt flag.
    TIM2->DIER |= TIM_DIER_UIE;	// enable update interrupts.
    NVIC_EnableIRQ(TIM2_IRQn);	// enable TIM2 interrupts in NVIC.

    TIM2->CR1 |= TIM_CR1_CEN;	// re-enable the counter.
}

void timer_oneshot(uint32_t ms, void(*callback)(void)) {
    // if one-shot event already active, queue as new item.
	if (oneshot_active) {
        enqueue_oneshot(ms, callback);
        return;
    }

    oneshot_callback = callback;
    oneshot_active = 1;

    TIM3->CR1 &= ~TIM_CR1_CEN;	// disable the counter.

    // timing.
    TIM3->PSC = 7999;			// PSC = 1kHz.
    TIM3->ARR = ms - 1;			// set time delay value.
    TIM3->CNT = 0;				// reset timer counter value.
    trigger_prescaler(TIM3);	// force updates PSC and ARR.

    // interrupts.
    TIM3->SR &= ~TIM_SR_UIF;	// clear active update interrupt flags.
    TIM3->DIER |= TIM_DIER_UIE;	// enable update interrupts.
    NVIC_EnableIRQ(TIM3_IRQn);	// enable TIM3 interrupts in NVIC.

    TIM3->CR1 |= TIM_CR1_CEN;	// re-enable the counter.
}


// TIM2 interrupt handler. handles periodic timer completion.
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {	// check if update interrupt occurred.
        TIM2->SR &= ~TIM_SR_UIF;	// clear active update interrupt flags.

        if (periodic_callback) {
            periodic_callback();	// call user-defined function.
        }
    }
}

// TIM3 interrupt handler. handles one-shot timer completion.
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {	// check if update interrupt occurred.
        TIM3->SR &= ~TIM_SR_UIF;	// clear active update interrupt flags.

        TIM3->CR1 &= ~TIM_CR1_CEN;	// disable the counter.
		oneshot_active = 0;			// note one-shot event completed.

        if (oneshot_callback) {
            oneshot_callback();		// call user-defined function.
            oneshot_callback = 0;	// clear the function pointer.
        }

        // triggers next one-shot in queue if it exists.
        OneShotEvent next;
        if (dequeue_oneshot(&next) == 0) {
            timer_oneshot(next.delay_ms, next.callback);
        }
    }
}

