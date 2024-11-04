/***************************************************************************//**
  @file     pdb.c
  @brief    Programmable Delay Block (PDB) driver for the K64F
  @author   Group 4: - Oms, Mariano
					 - Solari Raigoso, Agustín
					 - Wickham, Tomás
					 - Vieira, Valentin Ulises
 ******************************************************************************/

#include "hardware.h"
#include "pdb.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PDB_REG(id, reg)	(PDB_Ptrs[id]->reg)
#define BUS_CLK				(__CORE_CLOCK__ / 2)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	pdb_id_t		id;
	pdb_cfg_t		cfg;
	pdb_cfg_delay_t	ch_delay[PDB_CANT_CHS][PDB_CANT_DELAYS];
	callback_t		cb;
	bool			init;
} pdb_t; 

typedef struct {
	uint32_t*	clk;
	uint32_t	mask;
} sim_clk_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static PDB_Type*	const PDB_Ptrs[] 	=     PDB_BASE_PTRS;
static sim_clk_t	const PDB_Clks[] 	= { { &SIM->SCGC6, SIM_SCGC6_PDB_MASK },
											{ &SIM->SCGC6, SIM_SCGC6_PDB_MASK } };
static uint8_t		const PDB_IRQn[]	=     PDB_IRQS;

static uint8_t		const PDB_Ps[]		=	{ 1, 2, 4, 8, 16, 32, 64, 128 };
static uint8_t		const PDB_Mult[]	=	{ 1, 10, 20, 40 };

static pdb_t pdb[PDB_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool PDB_Init(pdb_id_t id, pdb_cfg_t cfg)
{
	uint32_t clock = BUS_CLK / (PDB_Ps[cfg.ps] * PDB_Mult[cfg.mult]);

	if (id < PDB_CANT_IDS && pdb[id].init == false)
	{
		*PDB_Clks[id].clk |= PDB_Clks[id].mask;
		
		/* Only for debugging purposes */
		NVIC_EnableIRQ(PDB_IRQn[id]);
		/* Only for debugging purposes */

		// ps or mult should be calculated internally in case clock / PDB_FREQUENCY_HZ > 0xFFFF

		PDB_REG(id, MOD)			= PDB_MOD_MOD(clock / PDB_FREQUENCY_HZ);	// Counter period
		PDB_REG(id, IDLY)			= PDB_IDLY_IDLY		(cfg.dly);				// Trigger interrupt every counter reset
		PDB_REG(id, SC)				= PDB_SC_PDBEIE_MASK						// PDB sequence error interrupt enable
									| PDB_SC_PRESCALER	(cfg.ps)				// PRESCALER = BUSCLK / (MULT x PDBCLK)
									| PDB_SC_TRGSEL		(cfg.trigg)				// Trigger input source select
									| PDB_SC_PDBEN_MASK							// Enable PDB
									| PDB_SC_PDBIE_MASK							// Interrupt enable
									| PDB_SC_MULT		(cfg.mult)				// MULT = BUSCLK / (PRESCALER x PDBCLK)
									| PDB_SC_CONT		(cfg.mode)				// Continuous or one-shot mode
									| PDB_SC_LDOK_MASK;							// Update buffered registers

		for (uint8_t i = 0; i < PDB_CANT_CHS; i++)
			PDB_REG(id, CH[i].C1)	= PDB_C1_BB			(cfg.bb ? 0x3 : 0x0)	// Back-to-back operation
									| PDB_C1_TOS		(0x3);					// No bypass

		pdb[id].cfg = cfg;														// Not necessary
		pdb[id].init = true;
	}

	return pdb[id].init;
}

bool PDB_SetDelay(pdb_id_t id, uint32_t delay)
{
	bool status = (id < PDB_CANT_IDS) && pdb[id].init;

	if (status)
	{
		PDB_REG(id, IDLY) = delay;
		PDB_REG(id, SC) |= PDB_SC_LDOK_MASK;
	}

	return status;
}

bool PDB_SetChannelDelay(pdb_id_t id, pdb_cfg_delay_t cfg)
{
	bool status = (id < PDB_CANT_IDS) && (cfg.ch < PDB_CANT_CHS) && (cfg.dly < PDB_CANT_DELAYS) && pdb[id].init;

	if (status)
	{
		PDB_REG(id, CH[cfg.ch].DLY[cfg.dly]) = cfg.val;
		PDB_REG(id, SC) |= PDB_SC_LDOK_MASK;

		pdb[id].ch_delay[cfg.ch][cfg.dly] = cfg;								// Not necessary
	}

	return status;
}

bool PDB_SetChannelMux(pdb_id_t id, pdb_cfg_mux_t cfg)
{
	bool status = (id < PDB_CANT_IDS) && (cfg.ch < PDB_CANT_CHS) && (cfg.mux < PDB_CANT_PRETRIGGS) && pdb[id].init;

	if (status)
	{
		PDB_REG(id, CH[cfg.ch].C1) &= ~PDB_C1_EN_MASK;
		if (cfg.mux != PDB_CANT_PRETRIGGS)
			PDB_REG(id, CH[cfg.ch].C1) |= PDB_C1_EN(1 << cfg.mux);				// Only one pre-trigger enabled per channel
	}

	return status;
}

bool PDB_Start(pdb_id_t id)
{
	bool status = (id < PDB_CANT_IDS) && pdb[id].init;

	if (status)
		PDB_REG(id, SC) |= PDB_SC_SWTRIG_MASK;

	return status;
}

bool PDB_Stop(pdb_id_t id)
{
	bool status = (id < PDB_CANT_IDS) && pdb[id].init;

	if (status)
		PDB_REG(id, SC) &= ~PDB_SC_PDBEN_MASK;

	return status;
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// ISR Functions ///////////////////////////////////////////////////////////////

__ISR__ PDB0_IRQHandler(void)
{
#if DEBUG_PDB
P_DEBUG_TP_SET
#endif
	/* Only for debugging purposes */
	if (PDB_REG(PDB0_ID, SC) & PDB_SC_PDBIF_MASK)
		PDB_REG(PDB0_ID, SC) &= ~PDB_SC_PDBIF_MASK;
	for (uint8_t i = 0; i < PDB_CANT_CHS; i++)
		if (PDB_REG(PDB0_ID, CH[i].S) & PDB_S_ERR_MASK)
			PDB_REG(PDB0_ID, CH[i].S) &= ~PDB_S_CF_MASK & ~PDB_S_ERR_MASK;
	/* Only for debugging purposes */
#if DEBUG_PDB
P_DEBUG_TP_CLR
#endif
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
