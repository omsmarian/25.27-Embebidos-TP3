/***************************************************************************//**
  @file     adc.h
  @brief    Analog to Digital Converter (ADC) driver for K64F
  @author   Group 4: - Oms, Mariano
					 - Solari Raigoso, Agustín
					 - Wickham, Tomás
					 - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

#ifndef _ADC_H_
#define _ADC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ADC0_DP0_CHANNEL	0x00

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	ADC0_ID,
	ADC1_ID,

	ADC_CANT_IDS
} adc_id_t;

typedef enum {
	ADC_PSC_x1,
	ADC_PSC_x2,
	ADC_PSC_x4,
	ADC_PSC_x8
} adc_prescaler_t;

typedef enum {
	ADC_BITS_8,
	ADC_BITS_12,
	ADC_BITS_10,
	ADC_BITS_16,
} adc_bits_t;

typedef enum {
	ADC_CYCLES_24,
	ADC_CYCLES_16,
	ADC_CYCLES_10,
	ADC_CYCLES_6,
	ADC_CYCLES_4,
} adc_cycles_t;

typedef enum {
	ADC_TAPS_4,
	ADC_TAPS_8,
	ADC_TAPS_16,
	ADC_TAPS_32,
	ADC_TAPS_1 // Hardware average disabled
} adc_taps_t;

typedef enum {
	ADC_MUX_A,
	ADC_MUX_B,

	ADC_MUX_CANT
} adc_mux_t;

typedef struct {
	adc_prescaler_t	prescaler;
	adc_bits_t		resolution;
	adc_cycles_t	cycles;
	adc_taps_t		hw_avg;
} adc_cfg_t;

typedef uint8_t adc_channel_t; // Channel 0-23
typedef uint16_t adc_data_t; // Maximum resolution is 16 bits

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the ADC peripheral
 * @param id: ADC peripheral to be initialized
 * @param config: Configuration for the ADC peripheral
 */
void adcInit (adc_id_t id, adc_cfg_t config);

/**
 * @brief Get the configuration of the ADC peripheral
 * @param id: ADC peripheral to get the configuration from
 * @return Configuration of the ADC peripheral
 */
adc_cfg_t* adcGetConfig (adc_id_t id);

/**
 * @brief Check if an interrupt is pending
 * @param id: ADC peripheral to check
 * @param mux: Mux to check
 * @return True if an interrupt is pending, false otherwise
 */
bool adcIsInterruptPending (adc_id_t id, adc_mux_t mux);

/**
 * @brief Clear the interrupt flag
 * @param id: ADC peripheral to clear the interrupt flag
 * @param mux: Mux to clear the interrupt flag
 */
void adcClearInterruptFlag (adc_id_t id, adc_mux_t mux);

/**
 * @brief Calibrate the ADC peripheral
 * @param id: ADC peripheral to calibrate
 * @param mux: Mux to calibrate
 * @return True if calibration was successful, false otherwise
 */
bool adcCalibrate (adc_id_t id, adc_mux_t mux);

/**
 * @brief Start an ADC conversion
 * @param id: ADC peripheral to start the conversion
 * @param ch: Channel to convert
 * @param mux: Mux to convert
 * @param ie: Enable interrupt
 */
void adcStart (adc_id_t id, adc_channel_t ch, adc_mux_t mux, bool ie);

/**
 * @brief Check if the ADC is ready
 * @param id: ADC peripheral to check
 * @param mux: Mux to check
 * @return True if the ADC is ready, false otherwise
 */
bool adcIsReady (adc_id_t id, adc_mux_t mux);

/**
 * @brief Get the data from the ADC
 * @param id: ADC peripheral to get the data from
 * @param mux: Mux to get the data from
 * @return Data from the ADC
 */
adc_data_t adcGetData (adc_id_t id, adc_mux_t mux);

/*******************************************************************************
 ******************************************************************************/

#endif // _ADC_H_
