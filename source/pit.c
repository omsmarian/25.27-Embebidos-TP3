/*
 * pit.c
 *
 *  Created on: 6 nov 2024
 *      Author: asolari
 */

#include "pit.h"

// Global variables
static TIMER_t timer[MAXTIMERS]; // Array of timers

void initPIT() {
	// Enable clock for PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Enable PIT module
	PIT->MCR = 0;

	// Enable interrupts for PIT channels 0 to 3
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_EnableIRQ(PIT3_IRQn);

	// Uncomment to configure GPIO pins for debugging
	// gpioMode(PORTNUM2PIN(PC, 7), OUTPUT);
	// gpioMode(PORTNUM2PIN(PC, 0), OUTPUT);
}

uint8_t createTimer(float time, void (*funcallback)(void)) {
	uint8_t i = 0;
	bool found = false;

	// Find a free timer slot
	while (i < MAXTIMERS && !found) {
		if (timer[i].state == FREE) {
			// Initialize the timer
			timer[i].state = IDLE;
			timer[i].callback = funcallback;
			// Set the timer load value based on the specified time
			PIT->CHANNEL[i].LDVAL = (int) (time / REFERENCE);
			// Enable timer interrupt
			PIT->CHANNEL[i].TCTRL |= PIT_TCTRL_TIE_MASK;
			found = true;
		} else {
			i++;
		}
	}

	// Return the timer ID
	return i;
}

void configTimerTime(uint8_t id, float time) {
	// Disable the timer
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	// Set the new load value based on the specified time
	PIT->CHANNEL[id].LDVAL = (int) (time / REFERENCE);
	// Enable the timer
	PIT->CHANNEL[id].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void destroyTimer(uint8_t id) {
	// Disable the timer
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	// Mark the timer as free
	timer[id].state = FREE;
}

void startTimer(uint8_t id) {
	// Enable the timer
	PIT->CHANNEL[id].TCTRL |= PIT_TCTRL_TEN_MASK;
	// If the timer is not free, mark it as running
	if (timer[id].state != FREE) {
		timer[id].state = RUNNING;
	}
}

void stopTimer(uint8_t id) {
	// Disable the timer
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	// If the timer is not free, mark it as idle
	if (timer[id].state != FREE) {
		timer[id].state = IDLE;
	}
}

uint32_t getTime(uint8_t id) {
	// Return the current value of the timer
	return PIT->CHANNEL[id].CVAL;
}

uint8_t getTimerState(uint8_t id) {
	// Return the state of the timer
	return timer[id].state;
}

void PIT0_IRQHandler(void) {
	// Set GPIO pin for debugging
	gpioWrite(PORTNUM2PIN(PC, 0), true);
	// Clear the interrupt flag
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	// Call the timer callback function
	timer[0].callback();
	// Clear the pending interrupt
	NVIC_ClearPendingIRQ(PIT0_IRQn);
	// Reset GPIO pin for debugging
	gpioWrite(PORTNUM2PIN(PC, 0), false);
}

void PIT1_IRQHandler(void) {
	// Set GPIO pin for debugging
	gpioWrite(PORTNUM2PIN(PC, 7), true);
	// Clear the interrupt flag
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
	// Call the timer callback function
	timer[1].callback();
	// Clear the pending interrupt
	NVIC_ClearPendingIRQ(PIT1_IRQn);
	// Reset GPIO pin for debugging
	gpioWrite(PORTNUM2PIN(PC, 7), false);
}

void PIT2_IRQHandler(void) {
	// Clear the interrupt flag
	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF(1);
	// Call the timer callback function
	timer[2].callback();
	// Clear the pending interrupt
	NVIC_ClearPendingIRQ(PIT2_IRQn);
}

void PIT3_IRQHandler(void) {
	// Clear the interrupt flag
	PIT->CHANNEL[3].TFLG = PIT_TFLG_TIF(1);
	// Call the timer callback function
	timer[3].callback();
	// Clear the pending interrupt
	NVIC_ClearPendingIRQ(PIT3_IRQn);
}
