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

#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "adc.h"
#include "fsk.h"
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

static fsk_id_t id;
static ticks_t tim_adc;

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
	// ADC_Init(ADC0_ID, (adc_cfg_t){ ADC_TRIGG_PDB, ADC_PSC_x1, ADC_BITS_12, true, ADC_CYCLES_24, ADC_TAPS_8, false });
	// ADC_Start(ADC0_ID, (adc_cfg_ch_t){ ADC_MUX_A, true, false, 0, NULL });


	fsk_cfg_t cfg = {
		.mod_cfg = {
			.mark = 1200,
			.space = 2200,
			.br = 1200,
			.sr = 12000,
			.amplitude = 3.3f,
			.offset = 1.65f
		},
		.demod_cfg = {
			.delay = (FSK_DEMOD_DELAY * 12000) / 1000,
			.filter = FSK_FILTER_FIR,
			.threshold = 0.15f
		}
	};
	id = FSK_Init(&cfg);
	serialInit();

//	debugInit();

	timerInit();
	tim_adc = timerStart(TIMER_MS2TICKS(1000));
}

//void ADC_PISR (void);

/**
 * @brief Run the application
 * @note This function is called constantly in an infinite loop
 */
void App_Run (void)
{
	static adc_mux_t mux = ADC_MUX_A;
	char raw[30];
	// adc_data_t data = 0;
	uint8_t len = 0;
	size_t status = 0;
	bool data = false;
	// uint16_t data;

	// ADC_Start(ADC0_ID, ADC_MUX_A, true, false, 0, NULL);

	// if(timerExpired(tim_adc) && ADC_IsReady(ADC0_ID, mux))
	if(timerExpired(tim_adc) && (status = FSK_DemodStatus(id)))
	{
		// data = ADC_GetData(ADC0_ID, mux);
		FSK_Demod(id, status);
		// len = sprintf(raw, "%d\n", data);

		if (FSK_DataStatus(id))
		{
			data = FSK_GetData(id);
			serialWriteData((uchar_t*)&data, 1);

		}
//			serialWriteDataBlocking((uchar_t*)"Data received\n", 14);
		// serialWriteData((uchar_t*)&raw, sizeof(raw));
		// serialWriteDataBlocking((uchar_t*)raw, len);

		// tim_adc = timerStart(TIMER_MS2TICKS(125));

//		mux = (mux + 1) % ADC_CANT_MUXS;
//		ADC_Start(ADC0_ID, (adc_cfg_ch_t){ mux, true, false, 0, NULL });
	}
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
