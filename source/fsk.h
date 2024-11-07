/***************************************************************************//**
  @file     fsk.h
  @brief    FSK Modulation and Demodulation functions
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
 ******************************************************************************/

#ifndef _FSK_H_
#define _FSK_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FSK_MAX_SAMPLES	1000
#define FSK_DEMOD_DELAY	0.446 // Delay in milliseconds
#define FSK_CANT_IDS	sizeof(fsk_id_t)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint8_t fsk_id_t;

typedef enum {
	FSK_FILTER_FIR,
	FSK_FILTER_IIR
} fsk_filter_t;

typedef struct {
	unsigned char*	bitstream;
	size_t			len;		// Length of the bitstream
	float*			signal;		// Pointer to store the modulated signal
	float			mark;		// '1' frequency
	float			space;		// '0' frequency
	float			br;			// Baud rate
	float			sr;			// Sample rate
	float			amplitude;
	float			offset;
} fsk_mod_cfg_t;

typedef struct {
	size_t			delay;		// Delay in samples
	float*			coeffs;		// FIR filter coefficients
	fsk_filter_t	filter;		// Filter type
	size_t			taps;		// Number of taps
} fsk_demod_cfg_t;

typedef struct {
	fsk_mod_cfg_t	mod_cfg;
	fsk_demod_cfg_t	demod_cfg;
} fsk_cfg_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Request a FSK module (initialize)
 * @param cfg Configuration of the modulation and demodulation
 * @return ID of the module
 */
fsk_id_t FSK_Init(const fsk_cfg_t* cfg);

/**
 * @brief Modulate a bitstream
 * @param id FSK ID
 * @param config Modulation configuration
 * @return Number of samples in the signal
 */
size_t FSK_Mod(const fsk_id_t id);

/**
 * @brief Demodulate a signal
 * @param id FSK ID
 * @param config Demodulation configuration
 * @return Demodulated data
 */
float FSK_Demod(const fsk_id_t id, float data);

/**
 * @brief Deinitialize a FSK module
 * @param id FSK ID
 */
void FSK_Deinit(const fsk_id_t id);

/*******************************************************************************
 ******************************************************************************/

#endif // _FSK_H_
