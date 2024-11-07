/***************************************************************************//**
  @file     pdb.h
  @brief    Programmable Delay Block (PDB) driver for K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
 ******************************************************************************/

#ifndef _PDB_H_
#define _PDB_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PDB_FREQUENCY_HZ	1000
#define PDB_HZ2TICKS(f)		(PDB_FREQUENCY_HZ / (f))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef void (*callback_t)(void);

typedef enum {
    PDB0_ID,

    PDB_CANT_IDS
} pdb_id_t;

typedef enum {
    PDB_CH0,
	PDB_CH1,

    PDB_CANT_CHS
} pdb_channel_t;

typedef enum {
    PDB_PRETRIGG_0,
	PDB_PRETRIGG_1,

    PDB_CANT_PRETRIGGS
} pdb_pretrigg_t;

typedef enum {
    PDB_CH_DELAY_0,
	PDB_CH_DELAY_1,

    PDB_CANT_DELAYS
} pdb_ch_delay_t;

typedef enum {
	PDB_PS_1,
	PDB_PS_2,
	PDB_PS_4,
	PDB_PS_8,
	PDB_PS_16,
	PDB_PS_32,
	PDB_PS_64,
	PDB_PS_128
} pdb_prescaler_t;

typedef enum {
	PDB_MULT_1,
	PDB_MULT_10,
	PDB_MULT_20,
	PDB_MULT_40
} pdb_mult_t;

typedef enum {
	PDB_TRIGG_EXT,
	PDB_TRIGG_CMP0,
	PDB_TRIGG_CMP1,
	PDB_TRIGG_CMP2,
	PDB_TRIGG_PIT0,
	PDB_TRIGG_PIT1,
	PDB_TRIGG_PIT2,
	PDB_TRIGG_PIT3,
	PDB_TRIGG_FTM0,
	PDB_TRIGG_FTM1,
	PDB_TRIGG_FTM2,
	PDB_TRIGG_FTM3,
	PDB_TRIGG_RTC_ALARM,
	PDB_TRIGG_RTC_SECONDS,
	PDB_TRIGG_LPTMR,
	PDB_TRIGG_SW
} pdb_trigg_t;

typedef enum {
	PDB_ONE_SHOT,
	PDB_CONTINUOUS
} pdb_mode_t;

typedef struct {
	pdb_prescaler_t	ps;
	pdb_mult_t		mult;
	pdb_trigg_t		trigg;
	pdb_mode_t		mode;
	bool			bb; // Back to back operation (ADC)
	uint16_t		dly;
} pdb_cfg_t;

typedef struct {
	pdb_channel_t	ch;
	pdb_ch_delay_t	dly;
	uint32_t		val;
} pdb_cfg_delay_t;

typedef struct {
	pdb_channel_t	ch;
	pdb_pretrigg_t	mux;
} pdb_cfg_mux_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the PDB module
 * @param id PDB module ID
 * @param config PDB configuration
 * @return true if initialization was successful
 */
bool PDB_Init(pdb_id_t id, pdb_cfg_t cfg);

/**
 * @brief Set the delay value for the PDB module
 * @param id PDB module ID
 * @param delay Delay value in PDB ticks
 */
bool PDB_SetDelay(pdb_id_t id, uint32_t delay);

/**
 * @brief Set one delay value of a channel in the PDB module
 * @param id PDB module ID
 * @param config Delay configuration
 * @return true if the configuration was successful
 */
bool PDB_SetChannelDelay(pdb_id_t id, pdb_cfg_delay_t cfg);

/**
 * @brief Set the channel multiplexer for the PDB module
 * @param id PDB module ID
 * @param cfg Mux configuration
 */
bool PDB_SetChannelMux(pdb_id_t id, pdb_cfg_mux_t cfg);

/**
 * @brief Start the PDB module
 * @param id PDB module ID
 */
bool PDB_Start(pdb_id_t id);

/**
 * @brief Stop the PDB module
 * @param id PDB module ID
 */
bool PDB_Stop(pdb_id_t id);

/*******************************************************************************
 ******************************************************************************/

#endif // _PDB_H_
