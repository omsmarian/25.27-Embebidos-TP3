/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "adc.h"
#include "board.h"
#include "debug.h"
#include "macros.h"
#include "serial.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Initialize the application
 * @note This function is called once at the beginning of the main program
 */
void App_Init (void)
{
	adcInit(ADC0_ID, (adc_cfg_t){ ADC_PSC_x8, ADC_BITS_12, ADC_CYCLES_24, ADC_TAPS_1 });
	serialInit();
//	debugInit();

	timerInit();
}

/**
 * @brief Run the application
 * @note This function is called constantly in an infinite loop
 */
void App_Run (void)
{
	adcStart(ADC0_ID, ADC0_DP0_CHANNEL, ADC_MUX_A, true);

	while (!adcIsReady(ADC0_ID, ADC_MUX_A));

	adc_data_t data = adcGetData(ADC0_ID, ADC_MUX_A);
	adcClearInterruptFlag(ADC0_ID, ADC_MUX_A);

	serialWriteData((uchar_t*)&data, sizeof(adc_data_t));
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void updateOutgoing (void)														// Send data to the serial port
{
}

void updateIncoming (void)														// Receive data from the serial port
{
}

/******************************************************************************/
