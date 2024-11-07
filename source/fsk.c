/***************************************************************************//**
  @file     fsk.c
  @brief    FSK Modulation and Demodulation functions
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES

 ******************************************************************************/

#include <math.h>
#include <stdio.h>

// #include "cqueue.h"
#include "fsk.h"
#include "macros.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SAMPLE_RATE	44100
#define FREQ_MARK	1200
#define FREQ_SPACE	2200
#define BAUD_RATE	300
#define PI			3.14159265358979323846
// #define TABLE_SIZE	256
#define BUFFER_SIZE	1000

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	// fsk_id_t		id;
	// queue_id_t		queue[2];
	float			samples[FSK_MAX_SAMPLES];
	float			demod_samples[FSK_MAX_SAMPLES];
	fsk_cfg_t		cfg;
	bool			init;
} fsk_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Sine function
 * @param phase Phase in degrees
 * @return Sine value
 */
static float sine(float phase);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const float sine_table[] = {
	0.000000, 0.024541, 0.049068, 0.073565, 0.098017, 0.122411, 0.146730, 0.170962,
	0.195090, 0.219101, 0.242980, 0.266713, 0.290285, 0.313682, 0.336890, 0.359895,
	0.382683, 0.405241, 0.427555, 0.449611, 0.471397, 0.492898, 0.514103, 0.534998,
	0.555570, 0.575808, 0.595699, 0.615232, 0.634393, 0.653173, 0.671559, 0.689541,
	0.707107, 0.724247, 0.740951, 0.757209, 0.773010, 0.788346, 0.803208, 0.817585,
	0.831470, 0.844854, 0.857729, 0.870087, 0.881921, 0.893224, 0.903989, 0.914210,
	0.923880, 0.932993, 0.941544, 0.949528, 0.956940, 0.963776, 0.970031, 0.975702,
	0.980785, 0.985278, 0.989177, 0.992480, 0.995185, 0.997290, 0.998795, 0.999699
};

// static bool ids[FSK_CANT_IDS];
static fsk_t fsk[FSK_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

fsk_id_t FSK_Init (const fsk_cfg_t* cfg)
{
	fsk_id_t id = FSK_CANT_IDS;

	for (uint8_t i = 0; i < FSK_CANT_IDS; i++)
	{
		if (!fsk[i].init)
		{
			// fsk[i].queue[0] = queueInit(FSK_MAX_SAMPLES, sizeof(float));
			// fsk[i].queue[1] = queueInit(FSK_MAX_SAMPLES, sizeof(float));

			fsk[i].cfg = *cfg;

			for (size_t j = 0; j < FSK_MAX_SAMPLES; j++)
				fsk[i].samples[j] = fsk[i].demod_samples[j] = 0;

			fsk[i].init = true;
			id = i;
			i = FSK_CANT_IDS;
		}
	}

	return id;
}

void FSK_Deinit(const fsk_id_t id)
{
	if ((id < FSK_CANT_IDS) && fsk[id].init)
	{
		// queueDelete(fsk[id].queue[0]);
		// queueDelete(fsk[id].queue[1]);

		fsk[id].init = false;
	}
}

size_t FSK_Mod(const fsk_id_t id)
{
    size_t index = 0, bit_samples = (size_t)(fsk[id].cfg.mod_cfg.sr / fsk[id].cfg.mod_cfg.br); // Number of samples per bit
    float phase = 0, freq, delta_phase;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
	{
		for (size_t i = 0; i < fsk[id].cfg.mod_cfg.len; i++)
		{
			freq = (ASCII2NUM(fsk[id].cfg.mod_cfg.bitstream[i])) ? fsk[id].cfg.mod_cfg.mark : fsk[id].cfg.mod_cfg.space;
			delta_phase = 2 * PI * freq / fsk[id].cfg.mod_cfg.sr;

			for (size_t j = 0; j < bit_samples && index < FSK_MAX_SAMPLES; j++)
			{
				if ((phase += delta_phase) > 2 * PI) { phase -= 2 * PI; }
				printf("Phase: %f, Sin: %f, Sine: %f\n", phase, sin(phase), sine(phase));
				fsk[id].cfg.mod_cfg.signal[index++] = (sine(phase) * fsk[id].cfg.mod_cfg.amplitude) / 2 + fsk[id].cfg.mod_cfg.offset;
			}
		}
	}

	return index;
}

float FSK_Demod(const fsk_id_t id, const float data)
{
	static uint16_t index = 0, delayed_index;									// Current index in the buffer
	float filtered_data = 0;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
	{
		fsk[id].samples[index] = data;
		delayed_index = ((index + BUFFER_SIZE) - (fsk[id].cfg.demod_cfg.delay % BUFFER_SIZE)) % BUFFER_SIZE;
		fsk[id].demod_samples[index] = data * fsk[id].samples[delayed_index]; // Demodulate by multiplying with a delayed copy

		filtered_data = 0;
		if (fsk[id].cfg.demod_cfg.filter == FSK_FILTER_FIR)
			for (uint8_t i = 0; i < fsk[id].cfg.demod_cfg.taps; i++)
				filtered_data += fsk[id].cfg.demod_cfg.coeffs[i] * fsk[id].demod_samples[((index + BUFFER_SIZE) - i) % BUFFER_SIZE];
		// else if (cfg->>filter == FSK_FILTER_IIR) {}

		if (index++ >= BUFFER_SIZE) { index = 0; }
	}

	return filtered_data;
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

float sine(float phase)
{
	uint8_t table_index;
	// float delta_index = TABLE_SIZE / 360.0, val;
	size_t table_size = sizeof(sine_table) / sizeof(float);
	float delta_index = 2 * table_size / PI, val;

	printf("Delta: %f\n", delta_index);
	while (phase < 0) { phase += 2 * PI; }
	while (phase >= 2 * PI) { phase -= 2 * PI; }

	if (2 * phase < PI)
		table_index = (uint8_t)(phase * delta_index);
	else if (phase <= PI)
		table_index = (uint8_t)((PI - phase) * delta_index);
	else if (2 * phase < 3 * PI)
		table_index = (uint8_t)((phase - PI) * delta_index);
	else if (phase < 2 * PI)
		table_index = (uint8_t)((2 * PI - phase) * delta_index);


	val = sine_table[table_index] * ((phase > PI) ? -1 : 1);

	return val;
}

// void demodulate_fsk_signal(const float *signal, int signal_len, char *data, int *data_len) {
// 	int samples_per_bit = SAMPLE_RATE / BAUD_RATE;
// 	*data_len = signal_len / samples_per_bit;
// 	for (int i = 0; i < *data_len; i++) {
// 		float mark_power = 0.0;
// 		float space_power = 0.0;
// 		for (int j = 0; j < samples_per_bit; j++) {
// 			float sample = signal[i * samples_per_bit + j];
// 			mark_power += sample * sin(2 * PI * FREQ_MARK * j / SAMPLE_RATE);
// 			space_power += sample * sin(2 * PI * FREQ_SPACE * j / SAMPLE_RATE);
// 		}
// 		data[i] = (mark_power > space_power) ? '1' : '0';
// 	}
// }

// float FSK_Demod(fsk_id_t id, fsk_demod_cfg_t* cfg)
// {
// 	static float samples[BUFFER_SIZE] = {0}, demod_samples[BUFFER_SIZE] = {0};	// Buffer to store past samples
// 	static uint16_t index = 0, delayed_index;									// Current index in the buffer
// 	float filtered_data = 0;

// 	if (id < ids)
// 	{
// 		cfg->>delay %= BUFFER_SIZE;
// 		samples[index] = cfg->data;

// 		delayed_index = ((index + BUFFER_SIZE) - cfg->delay) % BUFFER_SIZE;
// 		demod_samples[index] = cfg->data * samples[delayed_index];; // Demodulate by multiplying with a delayed copy

// 		if (index++ >= BUFFER_SIZE) { index = 0; }

// 		if (cfg->>filter == FSK_FILTER_FIR)
// 			for (uint8_t i = 0; i < cfg->taps; i++)
// 				filtered_data += cfg->coeffs[i] * demod_samples[((index + BUFFER_SIZE) - i) % BUFFER_SIZE];
// 		// else if (cfg->>filter == FSK_FILTER_IIR) {}
// 	}

// 	return filtered_data;
// }

/******************************************************************************/
