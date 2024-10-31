/***************************************************************************//**
  @file     pdb.c
  @brief    Programmable Delay Block (PDB) driver for K64F
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

#define PDB0_BASE_ADDR		0x40036000
#define PDB_REG(id, reg)	(PDB_Ptrs[id]->reg)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint32_t SC;
	uint32_t MOD;
	uint32_t IDLY;
} pdb_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static PDB_Type* const	PDB_Ptrs[] =   PDB_BASE_PTRS;
static uint32_t const	PDB_Clks[] = { SIM_SCGC6_PDB_MASK,
									   SIM_SCGC6_PDB_MASK };
static uint8_t const	PDB_IRQn[] =   PDB_IRQS;

static pdb_t pdb[PDB_CANT_IDS];

static bool init[PDB_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool PDB_Init(pdb_id_t id)
{
	bool status = false;

    if (id < PDB_CANT_IDS)
	{
		// Enable clock for PDB
		SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;

		// Configure PDB
		PDB_REG(id, SC) = PDB_SC_PDBEN_MASK | PDB_SC_CONT_MASK | PDB_SC_PRESCALER(0) | PDB_SC_MULT(0);
		PDB_REG(id, MOD) = 0xFFFF; // Set maximum period

		status = true;
	}

    return status;
}

void PDB_Start(pdb_id_t id)
{
    if (id < PDB_CANT_IDS)
	{
	    // Start PDB
	    PDB_REG(id, SC) |= PDB_SC_SWTRIG_MASK;
    }
}

void PDB_Stop(pdb_id_t id)
{
    if (id < PDB_CANT_IDS)
	{
	    // Stop PDB
	    PDB_REG(id, SC) &= ~PDB_SC_PDBEN_MASK;
    }
}

void PDB_SetDelay(pdb_id_t id, uint32_t delay)
{
    if (id < PDB_CANT_IDS)
	{
	    // Set delay value
	    PDB_REG(id, IDLY) = delay;
    }
}

/******************************************************************************/
