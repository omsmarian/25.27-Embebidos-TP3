/***************************************************************************//**
  @file     pit.c
  @brief    Periodic Interrupt Timer (PIT) driver for K64F
  @author   Group 4: - Oms, Mariano
					 - Solari Raigoso, Agustín
					 - Wickham, Tomás
					 - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"
#include "pit.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DEVELOPMENT_MODE					1

#define PIT_CLOCK							(__CORE_CLOCK__ / 2)
#define PIT_HZ_TO_TICKS(freq)				(PIT_CLOCK / (freq))
#define PIT_REG(id, reg)					(PIT_Ptrs[id]->reg)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	pit_callback_t callback;
	uint32_t period;
	uint32_t counter;
} pit_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static PIT_Type* const PIT_Ptrs[] = PIT_BASE_PTRS;
static uint8_t const PIT_IRQn[] = PIT_IRQn;

static bool init_flags[PIT_CANT_IDS];
static pit_t pit_callbacks[PIT_MAX_CALLBACKS][PIT_CANT];
static uint8_t pit_count = 0;

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool pitInit (pit_id_t id, pit_callback_t callback, uint32_t freq)
{
	bool status = false;

	if(!init_flags[id])
	{
		SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Clock Gating for PIT

		NVIC_EnableIRQ(PIT_IRQn[id]); // Enable PIT IRQ

		PIT_REG(id, MCR) &= ~PIT_MCR_MDIS_MASK & ~PIT_MCR_FRZ_MASK; // PIT Module enable
		PIT_REG(id, CHANNEL[id].LDVAL) = PIT_HZ_TO_TICKS(PIT_FREQUENCY_HZ) - 1;
		// PIT_REG(id, CHANNEL[id].TCTRL) |= PIT_TCTRL_TIE_MASK; // PIT interrupt enable is not used
		PIT_REG(id, CHANNEL[id].TCTRL) |= PIT_TCTRL_TEN_MASK;

		init_flags[id] = true;
	}

	if((pit_callbacks[pit_count][id].callback != NULL) && callback && (pit_count < PIT_CANT))
	{
        pit_callbacks[pit_count][id].callback = callback;
        pit_callbacks[pit_count][id].period = pit_callbacks[pit_count++][id].counter = PIT_HZ_TO_TICKS(freq);
        status = true;
	}

	return !(init_flags[id] && status);
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// ISR Functions ///////////////////////////////////////////////////////////////

__ISR__ PIT0_IRQHandler (void) { pit(PIT_0); }
__ISR__ PIT1_IRQHandler (void) { pit(PIT_1); }
__ISR__ PIT2_IRQHandler (void) { pit(PIT_2); }
__ISR__ PIT3_IRQHandler (void) { pit(PIT_3); }

void pit (pit_id_t id)
{
#if DEBUG_PIT
P_DEBUG_TP_SET
#endif
	PIT_REG(id, CHANNEL[id].TFLG) = PIT_TFLG_TIF_MASK; // Clear interrupt flag

	if (id < PIT_CANT_IDS)
	{
		for(uint8_t i = 0; i < pit_count; i++)
		{
			if(pit_callbacks[i][id].counter == 0)
			{
				pit_callbacks[i][id].counter = pit_callbacks[i][id].period;
				pit_callbacks[i][id].callback();
			}
			else
				pit_callbacks[i][id].counter--;
		}
	}
#if DEBUG_PIT
P_DEBUG_TP_CLR
#endif
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
