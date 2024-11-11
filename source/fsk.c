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

//#include <math.h>
#include <stdlib.h>

#include "adc.h"
#include "dac.h"
#include "fsk.h"
#include "macros.h"
#include "cqueue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PI			3.14159265358979323846
#define BUFFER_SIZE	1000

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	float		samples[FSK_MAX_SAMPLES], demod_samples[FSK_MAX_SAMPLES], filtered_data[FSK_MAX_SAMPLES];
	bool 		cmp_samples[FSK_MAX_SAMPLES], dac_bits[FSK_MAX_SAMPLES];
	fsk_cfg_t	cfg;
	queue_id_t	adc_samples, adc_bits, adc_bytes, dac_bytes;
	bool		init, dac_start, stop, dac_stop;
	uint8_t		start;
	size_t		bit_samples;
} fsk_t;

typedef enum {
	IDLE,
	START,
	DATA,
	STOP
} fsk_bus_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Sine function
 * @param phase Phase in degrees
 * @return Sine value
 */
static float __sin__(const float phase);

/**
 * @brief Get a sample from the ADC
 * @param id ADC ID
 */
static void __getSample__(const adc_id_t id);

/**
 * @brief Set a sample to the DAC
 * @param id DAC ID
 */
static void __setSample__(const dac_id_t id);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const float sine_table[] = {
	0.000000f, 0.024541f, 0.049068f, 0.073565f, 0.098017f, 0.122411f, 0.146730f, 0.170962f,
	0.195090f, 0.219101f, 0.242980f, 0.266713f, 0.290285f, 0.313682f, 0.336890f, 0.359895f,
	0.382683f, 0.405241f, 0.427555f, 0.449611f, 0.471397f, 0.492898f, 0.514103f, 0.534998f,
	0.555570f, 0.575808f, 0.595699f, 0.615232f, 0.634393f, 0.653173f, 0.671559f, 0.689541f,
	0.707107f, 0.724247f, 0.740951f, 0.757209f, 0.773010f, 0.788346f, 0.803208f, 0.817585f,
	0.831470f, 0.844854f, 0.857729f, 0.870087f, 0.881921f, 0.893224f, 0.903989f, 0.914210f,
	0.923880f, 0.932993f, 0.941544f, 0.949528f, 0.956940f, 0.963776f, 0.970031f, 0.975702f,
	0.980785f, 0.985278f, 0.989177f, 0.992480f, 0.995185f, 0.997290f, 0.998795f, 0.999699f
};

static float coeffs[] = {
		 -0.002086994263159,-0.006003054525357,-0.008633170414909,-0.001790251586214,
		    0.02460882704212,   0.0756853298976,   0.1448247875714,   0.2127701317527,
		     0.2551960820215,   0.2551960820215,   0.2127701317527,   0.1448247875714,
		     0.0756853298976,  0.02460882704212,-0.001790251586214,-0.008633170414909,
		  -0.006003054525357,-0.002086994263159
		};

static const adc_id_t ADC_Channels[FSK_CANT_IDS] = { ADC0_ID, ADC1_ID };
static const dac_id_t DAC_Channels[FSK_CANT_IDS] = { DAC0_ID, DAC1_ID };

// static bool ids[FSK_CANT_IDS];
static fsk_t fsk[FSK_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

fsk_id_t FSK_Init(const fsk_cfg_t* cfg)
{
	fsk_id_t id = FSK_CANT_IDS;

	for (uint8_t i = 0; (i < FSK_CANT_IDS) && (id == FSK_CANT_IDS); i++)
	{
		if (!fsk[i].init)
		{
//			fsk[i].adc_samples	= queueInit(FSK_MAX_STREAM_LEN, sizeof(adc_data_t));
			fsk[i].adc_samples	= queueInit();
//			fsk[i].adc_bytes	= queueInit(FSK_MAX_STREAM_LEN, sizeof(byte_t));
			fsk[i].adc_bits		= queueInit();
			fsk[i].adc_bytes	= queueInit();
//			fsk[i].dac_bytes	= queueInit(FSK_MAX_STREAM_LEN, sizeof(byte_t));
			fsk[i].dac_bytes	= queueInit();
			fsk[i].cfg = *cfg;
			fsk[i].bit_samples = (size_t)(fsk[i].cfg.mod_cfg.sr / fsk[i].cfg.mod_cfg.br);
			fsk[i].start = fsk[i].stop = false;

			for (size_t j = 0; j < FSK_MAX_SAMPLES; j++)
			{
				fsk[i].samples[j] = fsk[i].demod_samples[j] = fsk[i].cmp_samples[j] = fsk[i].filtered_data[j] = 0;
			}

			ADC_Init(ADC_Channels[i],	(adc_cfg_t)		{ .trigg	= ADC_TRIGG_PDB,
														  .ps		= ADC_PSC_x1,
														  .res		= ADC_BITS_12,
														  .speed	= true,
														  .cycles	= ADC_CYCLES_24,
														  .hw_avg	= ADC_TAPS_8,
														  .pwr		= false });
			ADC_Start(ADC_Channels[i],	(adc_cfg_ch_t)	{ .mux		= ADC_MUX_A,
														  .ie		= true,
														  .diff		= false,
														  .ch		= ADC_MUX_A,
														  .cb		= __getSample__ });

			fsk[i].init = true;
			id = i;
		}
	}


	return id;
}

void FSK_Delete(const fsk_id_t id)
{
	if ((id < FSK_CANT_IDS) && fsk[id].init)
	{
		ADC_Delete(ADC_Channels[id]);
		// DAC_Delete(DAC_Channels[id]);

		fsk[id].init = false;
	}
}

void FSK_Mod(const fsk_id_t id, byte_t* const data, const size_t len)
{
	if ((id < FSK_CANT_IDS) && fsk[id].init)
		for (size_t i = 0; i < len; i++)
			queuePush(fsk[id].dac_bytes, data[i]);
}

size_t FSK_ModStatus(const fsk_id_t id)
{
	size_t status = 0;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
		status = queueSize(fsk[id].adc_bits);

	return status;
}

void FSK_Demod(const fsk_id_t id, const size_t nsamples)
{
	static uint16_t index = 0, delayed_index, filter_index, cmp_index, start_index, a;						// Current index in the buffer
	static bool cmp = false, parity = false, cmp_prev = false;
	bool bit = false, start = true;
	uint8_t bit_check = 0;
	static fsk_bus_t state = IDLE;
	byte_t data;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
	{
		for (size_t i = 0; i < nsamples; i++)
		{
//			float a = ((queuePop(fsk[id].adc_samples) | queuePop(fsk[id].adc_samples) << 8) - 2048) * 3.3f / 4095;
			fsk[id].samples[index] = ((queuePop(fsk[id].adc_samples) | queuePop(fsk[id].adc_samples) << 8) - 2048) * 3.3f / 4095;

			while (fsk[id].cfg.demod_cfg.delay >= BUFFER_SIZE) { fsk[id].cfg.demod_cfg.delay -= BUFFER_SIZE; }
			delayed_index = (index + BUFFER_SIZE) - fsk[id].cfg.demod_cfg.delay;
			if (delayed_index >= BUFFER_SIZE) { delayed_index -= BUFFER_SIZE; }

			fsk[id].demod_samples[index] = fsk[id].samples[index] * fsk[id].samples[delayed_index]; // Demodulate by multiplying with a delayed copy

			fsk[id].filtered_data[index] = 0;
			if (fsk[id].cfg.demod_cfg.filter == FSK_FILTER_FIR)
				for (uint8_t i = 0; i < sizeof(coeffs) / sizeof(coeffs[0]); i++)
				{
					filter_index = (index + BUFFER_SIZE) - i;
					if (filter_index >= BUFFER_SIZE) { filter_index -= BUFFER_SIZE; }
					fsk[id].filtered_data[index] += coeffs[i] * fsk[id].demod_samples[filter_index];
				}
			// else if (cfg->>filter == FSK_FILTER_IIR) {}

			if (cmp != cmp_prev) { cmp_prev = cmp; }
			if (!cmp && (fsk[id].filtered_data[index] > fsk[id].cfg.demod_cfg.threshold))		{ cmp = true; }
			else if (cmp && (fsk[id].filtered_data[index] < -fsk[id].cfg.demod_cfg.threshold)) { cmp = false; }
			fsk[id].cmp_samples[cmp_index] = cmp;

			if (++index >= FSK_MAX_SAMPLES) { index = 0; }

			switch (state)
			{
				case IDLE:
					if (cmp && !cmp_prev)
					{
						for (uint8_t i = 0; i < 3; i++)
						{
							start_index = (cmp_index + BUFFER_SIZE) - 3;
							if (start_index >= BUFFER_SIZE) { start_index -= BUFFER_SIZE; }
							start &= !fsk[id].cmp_samples[start_index + i];
						}

						if (start)
						{
							fsk[id].cmp_samples[0] = true;
							cmp_index = 0;
							parity = false;
							queueClear(fsk[id].adc_bits);
							state = START;
							a = index;
						}
					}
					break;

				case START:
					if (cmp_index == fsk[id].bit_samples)
					{
						bit_check = fsk[id].cmp_samples[(size_t)(0.2f * fsk[id].bit_samples)]
								  + fsk[id].cmp_samples[(size_t)(0.5f * fsk[id].bit_samples)]
								  + fsk[id].cmp_samples[(size_t)(0.8f * fsk[id].bit_samples)];
						if (bit_check > 1)
							state = DATA;
						else
							state = IDLE;
					}

					break;

				case DATA:
					if (cmp_index == fsk[id].bit_samples)
					{
						bit_check = fsk[id].cmp_samples[cmp_index / 2]
								  + fsk[id].cmp_samples[cmp_index / 2 + 1]
								  + fsk[id].cmp_samples[cmp_index / 2 - 1];
						bit = bit_check > 1;
						parity ^= bit;
						queuePush(fsk[id].adc_bits, bit);

						if (queueSize(fsk[id].adc_bits) >= 8 + 2)
						{
							queueSize(fsk[id].adc_bits);
							if (!bit)	{ state = STOP; }
							else		{ state = IDLE; }
//							state = STOP;
						}
					}
					break;

				case STOP:
//					if (queueSize(fsk[id].adc_bits) == 8 + 3)
					{
//						if (!parity)
						{
							data = 0;
							for (uint8_t i = 0; i < 8; i++)
								data |= queuePop(fsk[id].adc_bits) << i;
								// data |= (fsk[id].cmp_samples[8 + 1 - i] << i);
							queuePush(fsk[id].adc_bytes, data);

						}
					
						state = IDLE;
					}
					break;

				default:
					state = IDLE;
					break;
			}

			if (++cmp_index > fsk[id].bit_samples) { cmp_index = 0; }
			if (index == 100)
				index = 100;
		}
	}
}

size_t FSK_DemodStatus(const fsk_id_t id)
{
	size_t status = 0;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
		status = (size_t)(queueSize(fsk[id].adc_samples) / sizeof(adc_data_t));

	return status;
}

byte_t FSK_GetData(const fsk_id_t id)
{
	byte_t data = false;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
		data = queuePop(fsk[id].adc_bytes);

	return data;
}

bool FSK_DataStatus(const fsk_id_t id)
{
	bool status = false;

	if ((id < FSK_CANT_IDS) && fsk[id].init)
		status = !queueIsEmpty(fsk[id].adc_bytes);

	return status;
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void __getSample__(const adc_id_t id)
{
	adc_data_t data;

	if ((id < ADC_CANT_IDS) && (fsk[id].init))
	{
		data = ADC_GetData(ADC_Channels[id], ADC_MUX_A);
		queuePush(fsk[id].adc_samples, data & 0xFF);
		queuePush(fsk[id].adc_samples, data >> 8);
	}
}

static void __setSample__(const dac_id_t id)
{
	byte_t data = 1;
    static size_t index = 0;
    static float phase = 0, freq, delta_phase;

	if ((id < DAC_CANT_IDS) && fsk[id].init)
	{
		if (!index)
		{
			if (!queueIsEmpty(fsk[id].dac_bytes))
				data = queuePop(fsk[id].dac_bytes);

			fsk[id].dac_bits[0] = 0;											// Start bit
			for (uint8_t i = 1; i < 8 + 1; i++)
				fsk[id].dac_bits[i] = BITGET(data, i);							// Data bits
			// fsk[id].dac_bits[sizeof(byte_t) + 1] = Parity...;				// Parity bit
			fsk[id].dac_bits[8 + 2] = 0;										// Stop bit

			freq = fsk[id].dac_bits[index] ? fsk[id].cfg.mod_cfg.mark : fsk[id].cfg.mod_cfg.space;
			delta_phase = 2 * PI * freq / fsk[id].cfg.mod_cfg.sr;
		}

		if ((phase += delta_phase) > 2 * PI) { phase -= 2 * PI; }
//		DAC_SetData(DAC_Channels[id], (__sin__(phase) * fsk[id].cfg.mod_cfg.amplitude) / 2 + fsk[id].cfg.mod_cfg.offset);

		if (++index >= 8 + 3) { index = 0; }
	}
}

static float __sin__(const float phase)
{
	uint8_t table_index;
	// float delta_index = TABLE_SIZE / 360.0, val;
	size_t table_size = sizeof(sine_table) / sizeof(float);
	float delta_index = 2 * table_size / PI, val, ph = phase;

	while (ph < 0) { ph += 2 * PI; }
	while (ph >= 2 * PI) { ph -= 2 * PI; }

	if (2 * ph < PI)
		table_index = (uint8_t)(ph * delta_index);
	else if (ph <= PI)
		table_index = (uint8_t)((PI - ph) * delta_index);
	else if (2 * ph < 3 * PI)
		table_index = (uint8_t)((ph - PI) * delta_index);
	else if (ph < 2 * PI)
		table_index = (uint8_t)((2 * PI - ph) * delta_index);

	val = sine_table[table_index] * ((ph > PI) ? -1 : 1);

	return val;
}

/******************************************************************************/
