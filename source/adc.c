/***************************************************************************//**
  @file     adc.c
  @brief    Analog to Digital Converter (ADC) driver for the K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdlib.h>

#include "adc.h"
#include "hardware.h"
#include "macros.h"
#include "pdb.h"
#include "pit.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ADC_REG(id, reg)	(ADC_Ptrs[id]->reg)

#define TWO_POW_NUM_OF_CAL	(1 << 4)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	adc_id_t		id;
	adc_cfg_t		cfg;
	adc_cfg_ch_t	ch_cfg;
	callback_t		cb[ADC_CANT_MUXS];
	adc_data_t		data[ADC_CANT_MUXS];
	bool			data_ready[ADC_CANT_MUXS];
	bool			init;
} adc_t;

typedef struct {
	uint32_t*	clk;
	uint32_t	mask;
} sim_clk_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief ADC interrupt handler
 * @param id ADC peripheral to handle
 */
static void handler (adc_id_t id);

/**
 * @brief Calibrate the ADC peripheral
 * @param id ADC peripheral to calibrate
 * @param mux Mux to calibrate
 * @return True if calibration was successful, false otherwise
 */
static bool ADC_Calibrate (adc_id_t id, adc_mux_t mux);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ADC_Type*	const ADC_Ptrs[]		=	  ADC_BASE_PTRS;
static sim_clk_t	const ADC_Clks[]		= { { &SIM->SCGC6, SIM_SCGC6_ADC0_MASK } ,
												{ &SIM->SCGC3, SIM_SCGC3_ADC1_MASK } };
static uint8_t		const ADC_IRQn[]		=	  ADC_IRQS;

static uint32_t		const ADC_TriggMask[]	=	{ SIM_SOPT7_ADC0ALTTRGEN_MASK,	SIM_SOPT7_ADC0PRETRGSEL_MASK,	SIM_SOPT7_ADC0TRGSEL_MASK,
												  SIM_SOPT7_ADC1ALTTRGEN_MASK,	SIM_SOPT7_ADC1PRETRGSEL_MASK,	SIM_SOPT7_ADC1TRGSEL_MASK };
static uint32_t		const ADC_TriggShift[]	=	{ SIM_SOPT7_ADC0ALTTRGEN_SHIFT,	SIM_SOPT7_ADC0PRETRGSEL_SHIFT,	SIM_SOPT7_ADC0TRGSEL_SHIFT,
												  SIM_SOPT7_ADC1ALTTRGEN_SHIFT,	SIM_SOPT7_ADC1PRETRGSEL_SHIFT,	SIM_SOPT7_ADC1TRGSEL_SHIFT };

static pdb_id_t		const PDB_Channels[]	=	{ PDB_CH0, PDB_CH1 };			// Triggers for ADCx

static adc_t adc[ADC_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool ADC_Init (adc_id_t id, adc_cfg_t cfg)
{
	if (id < ADC_CANT_IDS && adc[id].init == false)
	{
		*ADC_Clks[id].clk |= ADC_Clks[id].mask;
		NVIC_EnableIRQ(ADC_IRQn[id]);

		SIM->SOPT7			|= REG_WRITE(uint32_t, ADC_TriggShift[id * 3],		ADC_TriggMask[id * 3],		cfg.trigg != ADC_TRIGG_PDB)	// Defalut trigger is PDB
							|  REG_WRITE(uint32_t, ADC_TriggShift[id * 3 + 1],	ADC_TriggMask[id * 3 + 1],	cfg.trigg);

		ADC_REG(id, CFG1)	|= ADC_CFG1_ADLPC	(cfg.pwr)						// Low-Power Configuration
							|  ADC_CFG1_ADIV	(cfg.ps)						// Divide ratio used to generate input clock
							|  ADC_CFG1_ADLSMP	(cfg.cycles != ADC_CYCLES_SHORT)// Conversion mode selection
							|  ADC_CFG1_MODE	(cfg.res);						// Number of bits
		ADC_REG(id, CFG2)	|= ADC_CFG2_ADHSC	(cfg.speed)						// High-Speed Configuration
							|  ADC_CFG2_ADLSTS	(cfg.cycles);					// Long Sample Time Select
		ADC_REG(id, SC2)	|= ADC_SC2_ADTRG	(cfg.trigg != ADC_TRIGG_SW);	// Software or hardware trigger
		ADC_REG(id, SC3)	|= ADC_SC3_AVGE		(cfg.hw_avg != ADC_TAPS_1)		// Number of conversions to be averaged
							|  ADC_SC3_AVGS		(cfg.hw_avg);

		// for(uint8_t id = 0; id < ADC_CANT_IDS; id++)
		// 	for(uint8_t mux = 0; mux < ADC_CANT_MUXS; mux++)
		//  		ADC_Calibrate(id, mux);

		if (cfg.trigg == ADC_TRIGG_PDB)
		{
			PDB_Init(PDB0_ID, (pdb_cfg_t) { PDB_PS_1, PDB_MULT_10, PDB_TRIGG_SW, PDB_CONTINUOUS, false, 0 });
			PDB_SetChannelDelay(PDB0_ID, (pdb_cfg_delay_t){ PDB_Channels[id], PDB_CH_DELAY_0, 2000 });
			PDB_SetChannelDelay(PDB0_ID, (pdb_cfg_delay_t){ PDB_Channels[id], PDB_CH_DELAY_1, 4000 });

			// ADC_Start(ADC0_ID, ADC_MUX_A, true, false, 0, NULL);

			PDB_SetChannelMux(PDB0_ID, (pdb_cfg_mux_t){ PDB_CH0, PDB_PRETRIGG_0 });
			PDB_Start(PDB0_ID);
		}
		else if (cfg.trigg == ADC_TRIGG_PIT0 || cfg.trigg == ADC_TRIGG_PIT1 || cfg.trigg == ADC_TRIGG_PIT2 || cfg.trigg == ADC_TRIGG_PIT3)
		{
			PIT_Init((pit_id_t)(cfg.trigg - ADC_TRIGG_PIT0), NULL, PIT_HZ2TICKS(1000));
			// PIT_Start((pit_id_t)(cfg.trigg - ADC_TRIGG_PIT0));
		}

		adc[id].cfg	= cfg;
		adc[id].init = true;
	}

	return adc[id].init;
}

adc_cfg_t* ADC_GetConfig (adc_id_t id)
{
	static adc_cfg_t cfg = { 0, 0, 0, 0, 0};

	if (id < ADC_CANT_IDS)
	{
		cfg.trigg	= (ADC_REG(id, SC2)					& ADC_SC2_ADTRG_MASK)
					?  ADC_TRIGG_SW		: ((SIM->SOPT7	& ADC_TriggMask[id * 3])
					?  ADC_TRIGG_PDB	:  (SIM->SOPT7	& ADC_TriggMask[id * 3 + 2]));
		cfg.ps		= (ADC_REG(id, CFG1)				& ADC_CFG1_ADIV_MASK) >> ADC_CFG1_ADIV_SHIFT;
		cfg.res		=  ADC_REG(id, CFG1)				& ADC_CFG1_MODE_MASK;
		cfg.cycles	= (ADC_REG(id, CFG1)				& ADC_CFG1_ADLSMP_MASK)	? ADC_CYCLES_SHORT	: ADC_REG(id, CFG2)	& ADC_CFG2_ADLSTS_MASK;
		cfg.hw_avg	= (ADC_REG(id, SC3)					& ADC_SC3_AVGE_MASK)	? ADC_TAPS_1		: ADC_REG(id, SC3)	& ADC_SC3_AVGS_MASK;
	}

	return &cfg;
}

bool ADC_Start (adc_id_t id, adc_cfg_ch_t cfg)
{
	bool status = (id < ADC_CANT_IDS) && (cfg.mux < ADC_CANT_MUXS);

	if (status)
	{
		SIM->SOPT7 |= REG_WRITE(uint32_t, ADC_TriggMask[id * 3 + 2], ADC_TriggShift[id * 3 + 2], cfg.mux);
		ADC_REG(id, CFG2) = (ADC_REG(id, CFG2) & ~ADC_CFG2_MUXSEL_MASK) | ADC_CFG2_MUXSEL(cfg.mux);
		ADC_REG(id, SC1[cfg.mux]) = ADC_SC1_AIEN(cfg.ie) | ADC_SC1_DIFF(cfg.diff) | ADC_SC1_ADCH(cfg.ch);

		PDB_SetChannelMux(PDB0_ID, (pdb_cfg_mux_t){ PDB_Channels[id], cfg.mux });

		if (cfg.cb != NULL) adc[id].cb[cfg.mux] = cfg.cb;
		adc[id].ch_cfg = cfg;
	}

	return status;
}

bool ADC_IsReady (adc_id_t id, adc_mux_t mux)
{
	bool status = false;

	if ((id < ADC_CANT_IDS) && (mux < ADC_CANT_MUXS))
		status = adc[id].data_ready[mux];

	return status;
}

adc_data_t ADC_GetData (adc_id_t id, adc_mux_t mux)
{
	adc_data_t data = 0;

	if ((id < ADC_CANT_IDS) && (mux < ADC_CANT_MUXS))
	{
		data = adc[id].data[mux];
		adc[id].data_ready[mux] = false;
	}

	return data;
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// ISR Functions ///////////////////////////////////////////////////////////////

__ISR__ ADC0_IRQHandler (void) { handler(ADC0_ID); }
__ISR__ ADC1_IRQHandler (void) { handler(ADC1_ID); }

static void handler (adc_id_t id)
{
#if DEBUG_ADC
P_DEBUG_TP_SET
#endif
//	adc_mux_t mux = adc[id].ch_cfg.mux;

	for (adc_mux_t mux = ADC_MUX_A; mux < ADC_CANT_MUXS; mux++)
		if (ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK) // Check which of the conversions just triggered
		{
			adc[id].data[mux] = (adc_data_t)ADC_REG(id, R[mux]); // This will clear the COCO bit that is also the interrupt flag
			adc[id].data_ready[mux] = true;

			if (adc[id].cb[mux] != NULL)
				adc[id].cb[mux]();

			// PDB_SetChannelMux(PDB0_ID, (pdb_cfg_mux_t){ PDB_Channels[id], mux });
		}
#if DEBUG_ADC
P_DEBUG_TP_CLR
#endif
}

////////////////////////////////////////////////////////////////////////////////

static bool ADC_Calibrate (adc_id_t id, adc_mux_t mux)
{
	int32_t offset = 0;
	uint32_t minus[7] = { 0, 0, 0, 0, 0, 0, 0 }, plus[7] = { 0, 0, 0, 0, 0, 0, 0 }, scr3;
	uint32_t temp[] = { ADC_REG(id, CLP0), ADC_REG(id, CLP1), ADC_REG(id, CLP2), ADC_REG(id, CLP3), ADC_REG(id, CLP4), ADC_REG(id, CLPS),
						ADC_REG(id, CLM0), ADC_REG(id, CLM1), ADC_REG(id, CLM2), ADC_REG(id, CLM3), ADC_REG(id, CLM4), ADC_REG(id, CLMS) };

	/* Setup */
	ADC_REG(id, SC1[mux]) = 0x1F;
	scr3 = ADC_REG(id, SC3);
	ADC_REG(id, SC3) &= (ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK);

	/* Initial calibration */
	ADC_REG(id, SC3) &= ~ADC_SC3_CAL_MASK;
	ADC_REG(id, SC3) |=  ADC_SC3_CAL_MASK;

	while (!(ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK));
	if (ADC_REG(id, SC3) & ADC_SC3_CALF_MASK)
	{
		ADC_REG(id, SC3) |= ADC_SC3_CALF_MASK;
		return false;
	}

	ADC_REG(id, PG) = (0x8000 | ((ADC_REG(id, CLP0) + ADC_REG(id, CLP1) + ADC_REG(id, CLP2) + ADC_REG(id, CLP3) + ADC_REG(id, CLP4) + ADC_REG(id, CLPS)) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC_REG(id, MG) = (0x8000 | ((ADC_REG(id, CLM0) + ADC_REG(id, CLM1) + ADC_REG(id, CLM2) + ADC_REG(id, CLM3) + ADC_REG(id, CLM4) + ADC_REG(id, CLMS)) >> (1 + TWO_POW_NUM_OF_CAL)));

	/* Extra calibrations */
	for (uint8_t i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		ADC_REG(id, SC3) &= ~ADC_SC3_CAL_MASK;
		ADC_REG(id, SC3) |=  ADC_SC3_CAL_MASK;

		while (!(ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK));
		if (ADC_REG(id, SC3) & ADC_SC3_CALF_MASK)
		{
			ADC_REG(id, SC3) |= ADC_SC3_CALF_MASK;
			return 1;
		}

		offset += (short)ADC_REG(id, OFS);
		for (uint8_t j = 0; j < 7; j++)
		{
			plus[j]  += (unsigned long)temp[j];
			minus[j] += (unsigned long)temp[j+7];
		}
	}

	ADC_REG(id, PG)   = (0x8000 | ((plus[0]  + plus[1]  + plus[2]  + plus[3]  + plus[4]  + plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC_REG(id, MG)   = (0x8000 | ((minus[0] + minus[1] + minus[2] + minus[3] + minus[4] + minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));

	ADC_REG(id, OFS)  = (offset   >> TWO_POW_NUM_OF_CAL);
	for (uint8_t i = 0; i < 7; i++)
	{
		plus[i]  = (plus[i]  >> TWO_POW_NUM_OF_CAL);
		minus[i] = (minus[i] >> TWO_POW_NUM_OF_CAL);
	}

	/* Clear setup */
	ADC_REG(id, SC3) = scr3;

	return true;
}

/******************************************************************************/
