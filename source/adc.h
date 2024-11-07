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

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef void (*callback_t)(void);
typedef uint8_t adc_channel_t; // Channel 0-23
typedef uint16_t adc_data_t; // Maximum resolution is 16 bits

typedef enum {
	ADC0_ID,
	ADC1_ID,

	ADC_CANT_IDS
} adc_id_t;

typedef enum {
	ADC_TRIGG_PDB_EXT,
	ADC_TRIGG_CMP0,
	ADC_TRIGG_CMP1,
	ADC_TRIGG_CMP2,
	ADC_TRIGG_PIT0,
	ADC_TRIGG_PIT1,
	ADC_TRIGG_PIT2,
	ADC_TRIGG_PIT3,
	ADC_TRIGG_FTM0,
	ADC_TRIGG_FTM1,
	ADC_TRIGG_FTM2,
	ADC_TRIGG_FTM3,
	ADC_TRIGG_RTC_ALARM,
	ADC_TRIGG_RTC_SECONDS,
	ADC_TRIGG_LPTMR,
	ADC_TRIGG_PDB,
	ADC_TRIGG_SW
} adc_trigger_t;

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
	ADC_BITS_16
} adc_bits_t;

typedef enum {
	ADC_CYCLES_24,
	ADC_CYCLES_16,
	ADC_CYCLES_10,
	ADC_CYCLES_6,
	ADC_CYCLES_SHORT
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

	ADC_CANT_MUXS
} adc_mux_t;

typedef struct {
	adc_trigger_t	trigg;
	adc_prescaler_t	ps;
	adc_bits_t		res; // Resolution
	bool			speed; // Higher speed conversion clocks with 2 additional ADCK
	adc_cycles_t	cycles;
	adc_taps_t		hw_avg;
	bool			pwr; // Power is reduced at the expense of maximum clock speed
} adc_cfg_t;

typedef struct {
	adc_mux_t		mux;
	bool			ie; // Interrupt enable
	bool			diff; // Differential mode
	adc_channel_t	ch;
	callback_t 		cb;
} adc_cfg_ch_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the ADC peripheral
 * @param id ADC peripheral to be initialized
 * @param cfg Configuration for the ADC peripheral
 */
bool ADC_Init (adc_id_t id, adc_cfg_t cfg);

/**
 * @brief Get the configuration of the ADC peripheral
 * @param id ADC peripheral to get the configuration from
 * @return Configuration of the ADC peripheral
 */
adc_cfg_t* ADC_GetConfig (adc_id_t id);

/**
 * @brief Start an ADC conversion
 * @param id ADC peripheral to start the conversion
 * @param mux SC1n and Rn register to use
 * @param ie Enable interrupt
 * @param ch Channel to convert
 */
bool ADC_Start (adc_id_t id, adc_cfg_ch_t cfg);

/**
 * @brief Check if the ADC is ready
 * @param id ADC peripheral to check
 * @param mux Mux to check
 * @return True if the ADC is ready, false otherwise
 */
bool ADC_IsReady (adc_id_t id, adc_mux_t mux);

/**
 * @brief Get the data from the ADC
 * @param id ADC peripheral to get the data from
 * @param mux Mux to get the data from
 * @return Data from the ADC
 */
adc_data_t ADC_GetData (adc_id_t id, adc_mux_t mux);

/*******************************************************************************
 ******************************************************************************/

#endif // _ADC_H_
