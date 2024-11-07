/***************************************************************************//**
  @file     fsk_test.c
  @brief    FSK Testbench
  @author   Group 4: - Oms, Mariano
					 - Solari Raigoso, Agustín
					 - Wickham, Tomás
					 - Vieira, Valentin Ulises
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>

#include "fsk.h"

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

int main (void)
{
	char* input_data = "111001011";
	size_t signalLength;
    float mod_data[FSK_MAX_SAMPLES], demod_data[FSK_MAX_SAMPLES], sample_rate = 12000.0;
	float coeffs[15] = { 0.00928296, 0.01514932, 0.03156164, 0.05600867, 0.08389277,
						 0.10954621, 0.12754891, 0.13401904, 0.12754891, 0.10954621,
						 0.08389277, 0.05600867, 0.03156164, 0.01514932, 0.00928296 };
	fsk_cfg_t cfg = {
		.mod_cfg = {
			.bitstream = input_data,
			.len = sizeof(input_data) - 1,
			.signal = mod_data,
			.mark = 1200,
			.space = 2200,
			.br = 1200,
			.sr = sample_rate,
			.amplitude = 2,
			.offset = 0
		},
		.demod_cfg = {
			.delay = (FSK_DEMOD_DELAY * sample_rate) / 1000,
			.coeffs = coeffs,
			.filter = FSK_FILTER_FIR,
			.taps = 23
		}
	};
	fsk_id_t id = FSK_Init(&cfg);

	signalLength = FSK_Mod(id);
	for (size_t i = 0; i < signalLength; ++i)
		demod_data[i] = FSK_Demod(id, mod_data[i]);

	printf("x = [");
	for (size_t i = 0; i < signalLength; ++i)
		printf(", %f", mod_data[i]);
	printf("]\n");

	printf("y = [");
	for (size_t i = 0; i < signalLength; ++i)
		printf(", %f", demod_data[i]);
	printf("]\n");

	return 0;
}

/******************************************************************************/
