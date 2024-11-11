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

#define FSK_MAX_SAMPLES		1000
#define FSK_DEMOD_DELAY		0.446 // Delay in milliseconds
#define FSK_MAX_STREAM_LEN	100

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef unsigned char byte_t;

typedef enum {
	FSK_0,
	FSK_1,

	FSK_CANT_IDS
} fsk_id_t;

typedef enum {
	FSK_MOD,
	FSK_DEMOD
} fsk_type_t;

typedef enum {
	FSK_FILTER_FIR,
	FSK_FILTER_IIR
} fsk_filter_t;

typedef struct {
	float			mark;		// '1' frequency
	float			space;		// '0' frequency
	float			br;			// Baud rate
	float			sr;			// Sample rate
	float			amplitude;
	float			offset;
} fsk_mod_cfg_t;

typedef struct {
	size_t			delay;		// Delay in samples
	fsk_filter_t	filter;		// Filter type
	float			threshold;	// Histeresis for the comparator
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
 * @brief Deinitialize a FSK module
 * @param id FSK ID
 */
void FSK_Delete(const fsk_id_t id);

/**
 * @brief Modulate a bitstream
 * @param id FSK ID
 * @param config Modulation configuration
 * @return Number of samples in the signal
 */
void FSK_Mod(const fsk_id_t id, byte_t* const data, const size_t len);

size_t FSK_ModStatus(const fsk_id_t id);

/**
 * @brief Demodulate a signal
 * @param id FSK ID
 * @param nsamples Number of samples to demodulate
 */
void FSK_Demod(const fsk_id_t id, const size_t nsamples);

size_t FSK_DemodStatus(const fsk_id_t id);

/**
 * @brief Get the data from the demodulated signal
 * @param id FSK ID
 * @return Data
 */
byte_t FSK_GetData(const fsk_id_t id);

/**
 * @brief Check if there is data available
 * @param id FSK ID
 * @return True if there is data available
 */
bool FSK_DataStatus(const fsk_id_t id);

/*******************************************************************************
 ******************************************************************************/

#endif // _FSK_H_
