/*
 * pwm.h
 *
 *  Created on: 5 nov 2024
 *      Author: asolari
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"
#include "gpio.h"

#include "ftm.h"
#include "dma.h"


/**
 * @brief Initialize the PWM module
 */
void PWM_Init(void);

/**
 * @brief Set the duty cycle of the PWM signal
 *
 * @param DC Duty cycle as a float value
 */
void PWM_SetDC(float DC);

/**
 * @brief Get the current duty cycle of the PWM signal
 *
 * @return Duty cycle as a float value
 */
float PWM_GetDC(void);

/**
 * @brief Set the number of ticks per period for the PWM signal
 *
 * @param ticks Number of ticks per period
 */
void PWM_SetTickPerPeriod(uint16_t ticks);

/**
 * @brief Get the number of ticks per period for the PWM signal
 *
 * @return Number of ticks per period
 */
uint16_t PWM_GetTickPerPeriod(void);

/**
 * @brief Generate a PWM waveform
 *
 * @param waveform Pointer to the waveform data
 * @param wave_length Length of the waveform data
 * @param waveTable_offset Offset in the waveform table
 */
void PWM_GenWaveform(uint16_t *waveform, uint32_t wave_length, uint32_t waveTable_offset);

/**
 * @brief Set the offset in the waveform table
 *
 * @param waveTable_offset Offset in the waveform table
 */
void PWM_SetWaveformOffset(uint32_t waveTable_offset);

/**
 * @brief Get the offset in the waveform table
 *
 * @return Offset in the waveform table
 */
uint32_t PWM_GetWaveformOffset(void);
#endif /* PWM_H_ */