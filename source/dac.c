/***************************************************************************//**
  @file     dac.c
  @brief    Digital to Analog Converter (DAC) Driver for K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "dac.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DAC_DATL_DATA0_WIDTH 8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief DAC Periodic Interrupt Service Routine
 */
void DAC_PISR (void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void DAC_Init (void)
{
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	SIM->SCGC2 |= SIM_SCGC2_DAC1_MASK;

	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK | DAC_C0_DACTRGSEL_MASK;
	DAC1->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK | DAC_C0_DACTRGSEL_MASK;


	DAC0->C1 &= ~(DAC_C1_DMAEN_MASK  | DAC_C1_DACBFMD_MASK | DAC_C1_DACBFEN_MASK );
	DAC0->C1 |= (DAC_C1_DMAEN(0) |		// DMA disabled
				DAC_C1_DACBFMD(0) |		// Normal Mode (circular buffer)
				DAC_C1_DACBFEN(0));		// Buffer Disabled


	DAC1->C1 &= ~(DAC_C1_DMAEN_MASK  | DAC_C1_DACBFMD_MASK | DAC_C1_DACBFEN_MASK );
	DAC1->C1 |= (DAC_C1_DMAEN(0) |		// DMA disabled
				DAC_C1_DACBFMD(0) |		// Normal Mode (circular buffer)
				DAC_C1_DACBFEN(0));		// Buffer Disabled

}

void DAC_SetData (DAC_t dac, DACData_t data)
{
	dac->DAT[0].DATL = DAC_DATL_DATA0(data);
	dac->DAT[0].DATH = DAC_DATH_DATA1(data >> DAC_DATL_DATA0_WIDTH);
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// ISR Functions ///////////////////////////////////////////////////////////////

void DAC_PISR (void)
{
	static uint16_t k = 0;

	DAC_SetData(DAC0, k++);

	k %= 0x0FFF;
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
