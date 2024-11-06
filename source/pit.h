/*
 * pit.h
 *
 *  Created on: 6 nov 2024
 *      Author: asolari
 */

#ifndef PIT_H_
#define PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "MK64F12.h"
#include "hardware.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAXTIMERS (4) /**< Maximum number of timers */
#define MAXCOUNT ((float)(4294967296/__CORE_CLOCK__/2)) /**< Maximum count value */
#define REFERENCE ((float)(0.02)) /**< Reference value */

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
/**
 * @brief Enumeration for timer states
 */
enum states {
	FREE, /**< Timer is free */
	IDLE, /**< Timer is idle */
	RUNNING /**< Timer is running */
};

/**
 * @brief Structure for timer configuration
 */
typedef struct {
	uint8_t state; /**< State of the timer */
	void (*callback)(void); /**< Callback function for the timer */
} TIMER_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the PIT module
 */
void initPIT();

/**
 * @brief Create a timer with a specified time and callback function
 *
 * @param time Time duration for the timer
 * @param funcallback Callback function to be called when the timer expires
 * @return Timer ID
 */
uint8_t createTimer(float time, void (*funcallback)(void));

/**
 * @brief Configure the time for a specific timer
 *
 * @param id Timer ID
 * @param time New time duration for the timer
 */
void configTimerTime(uint8_t id, float time);

/**
 * @brief Destroy a specific timer
 *
 * @param id Timer ID
 */
void destroyTimer(uint8_t id);

/**
 * @brief Reset a specific timer
 *
 * @param id Timer ID
 */
void resetTimer(uint8_t id);

/**
 * @brief Start a specific timer
 *
 * @param id Timer ID
 */
void startTimer(uint8_t id);

/**
 * @brief Stop a specific timer
 *
 * @param id Timer ID
 */
void stopTimer(uint8_t id);

/**
 * @brief Get the current time of a specific timer
 *
 * @param id Timer ID
 * @return Current time of the timer
 */
uint32_t getTime(uint8_t id);

/**
 * @brief Get the state of a specific timer
 *
 * @param id Timer ID
 * @return State of the timer
 */
uint8_t getTimerState(uint8_t id);

#endif /* PIT_H_ */
