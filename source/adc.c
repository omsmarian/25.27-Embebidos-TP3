/***************************************************************************//**
  @file     adc.c
  @brief    Analog to Digital Converter (ADC) driver for K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "adc.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ADC_REG(id, reg)	(ADC_Ptrs[id]->reg)

#define TWO_POW_NUM_OF_CAL	(1 << 4)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ADC_Type* ptr;
	adc_cfg_t config;
	bool init;
} adc_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ADC_Type* const	ADC_Ptrs[] =   ADC_BASE_PTRS;
static uint32_t const	ADC_Clks[] = { SIM_SCGC6_ADC0_MASK,
									   SIM_SCGC3_ADC1_MASK };
static uint8_t const	ADC_IRQn[] =   ADC_IRQS;

static adc_t adc[ADC_CANT_IDS];

static bool init[ADC_CANT_IDS];
static adc_mux_t mux[ADC_MUX_CANT];

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void adcInit (adc_id_t id, adc_cfg_t config)
{
	/* Enable the clock for ADCx and its PORT */
	(id == ADC0_ID) ? (SIM->SCGC6 |= ADC_Clks[id]) : (SIM->SCGC3 |= ADC_Clks[id]);
	// SIM->SCGC5 |= PORT_Clks[pinRx.port] | PORT_Clks[pinTx.port];

	/* Enable IRQs for ADCx */
	NVIC_EnableIRQ(ADC_IRQn[id]);

	/* Select divide ratio used to generate input clock */
	ADC_REG(id, CFG1) = ADC_CFG1_ADIV(config.prescaler);

	/* Select the ADC resolution mode */
	ADC_REG(id, CFG1) = (ADC_REG(id, CFG1) & ~ADC_CFG1_MODE_MASK) | ADC_CFG1_MODE(config.resolution);

	/* Select sample time */
	if (config.cycles & ~ADC_CFG2_ADLSTS_MASK)
		ADC_REG(id, CFG1) &= ~ADC_CFG1_ADLSMP_MASK;
	else
	{
		ADC_REG(id, CFG1) |= ADC_CFG1_ADLSMP_MASK;
		ADC_REG(id, CFG2) = (ADC_REG(id, CFG2) & ~ADC_CFG2_ADLSTS_MASK) | ADC_CFG2_ADLSTS(config.cycles);
	}

	/* Select number of conversions to be averaged */
	if (config.hw_avg & ~ADC_SC3_AVGS_MASK)
		ADC_REG(id, SC3) &= ~ADC_SC3_AVGE_MASK;
	else
	{
		ADC_REG(id, SC3) |= ADC_SC3_AVGE_MASK;
		ADC_REG(id, SC3) = (ADC_REG(id, SC3) & ~ADC_SC3_AVGS_MASK) | ADC_SC3_AVGS(config.hardware_avg);
	}

	// ADC_SetResolution(ADC0, ADC_b12);
	// ADC_SetCycles(ADC0, ADC_c4);
	// ADC_Calibrate(ADC0);
}

bool adcIsInterruptPending (adc_id_t id, adc_mux_t mux)
{
	return ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK;
}

void adcClearInterruptFlag (adc_id_t id, adc_mux_t mux)
{
	ADC_REG(id, SC1[mux]) = 0x00;
}

adc_cfg_t* adcGetConfig (adc_id_t id)
{
	static adc_cfg_t config;

	// config = { (ADC_REG(id, CFG1)	& ADC_CFG1_ADIV_MASK) >> ADC_CFG1_ADIV_SHIFT,
	// 			ADC_REG(id, CFG1)	& ADC_CFG1_MODE_MASK,
	// 			ADC_REG(id, CFG2)	& ADC_CFG2_ADLSTS_MASK,
	// 			ADC_REG(id, SC3)	& ADC_SC3_AVGS_MASK };

	config.prescaler	= (ADC_REG(id, CFG1)	& ADC_CFG1_ADIV_MASK) >> ADC_CFG1_ADIV_SHIFT;
	config.resolution	=  ADC_REG(id, CFG1)	& ADC_CFG1_MODE_MASK;
	config.cycles		= (ADC_REG(id, CFG1)	& ADC_CFG1_ADLSMP_MASK) ? ADC_CYCLES_4 : ADC_REG(id, CFG2) & ADC_CFG2_ADLSTS_MASK;
	config.hw_avg		= (ADC_REG(id, SC3)		& ADC_SC3_AVGE_MASK) ? ADC_TAPS_1 : ADC_REG(id, SC3) & ADC_SC3_AVGS_MASK;

	return &config;
}

bool adcCalibrate (adc_id_t id, adc_mux_t mux)
{
	int32_t offset = 0;
	uint32_t minus[7] = { 0, 0, 0, 0, 0, 0, 0 }, plus[7] = { 0, 0, 0, 0, 0, 0, 0 }, scr3;
	uint8_t i;

	/* Setup */
	ADC_REG(id, SC1[mux]) = 0x1F;
	scr3 = ADC_REG(id, SC3);
	ADC_REG(id, SC3) &= (ADC_SC3_AVGS(ADC_) | ADC_SC3_AVGE_MASK);

	/* Initial calibration */
	ADC_REG(id, SC3) &= ~ADC_SC3_CAL_MASK;
	ADC_REG(id, SC3) |=  ADC_SC3_CAL_MASK;

	while (!(ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK));
	if (ADC_REG(id, SC3) & ADC_SC3_CALF_MASK)
	{
		ADC_REG(id, SC3) |= ADC_SC3_CALF_MASK;
		return false;
	}

	ADC_REG(id, PG) = (0x8000 | ((ADC_REG(id, CLP0)+ADC_REG(id, CLP1)+ADC_REG(id, CLP2)+ADC_REG(id, CLP3)+ADC_REG(id, CLP4)+ADC_REG(id, CLPS)) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC_REG(id, MG) = (0x8000 | ((ADC_REG(id, CLM0)+ADC_REG(id, CLM1)+ADC_REG(id, CLM2)+ADC_REG(id, CLM3)+ADC_REG(id, CLM4)+ADC_REG(id, CLMS)) >> (1 + TWO_POW_NUM_OF_CAL)));

	/* Extra calibrations */
	for (i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		ADC_REG(id, SC3) &= ~ADC_SC3_CAL_MASK;
		ADC_REG(id, SC3) |=  ADC_SC3_CAL_MASK;

		while (!(ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK));
		if (ADC_REG(id, SC3) & ADC_SC3_CALF_MASK)
		{
			ADC_REG(id, SC3) |= ADC_SC3_CALF_MASK;
			return 1;
		}

		offset   += (short)ADC_REG(id, OFS);
		plus[0]  += (unsigned long)ADC_REG(id, CLP0);
		plus[1]  += (unsigned long)ADC_REG(id, CLP1);
		plus[2]  += (unsigned long)ADC_REG(id, CLP2);
		plus[3]  += (unsigned long)ADC_REG(id, CLP3);
		plus[4]  += (unsigned long)ADC_REG(id, CLP4);
		plus[5]  += (unsigned long)ADC_REG(id, CLPS);
		plus[6]  += (unsigned long)ADC_REG(id, CLPD);
		minus[0] += (unsigned long)ADC_REG(id, CLM0);
		minus[1] += (unsigned long)ADC_REG(id, CLM1);
		minus[2] += (unsigned long)ADC_REG(id, CLM2);
		minus[3] += (unsigned long)ADC_REG(id, CLM3);
		minus[4] += (unsigned long)ADC_REG(id, CLM4);
		minus[5] += (unsigned long)ADC_REG(id, CLMS);
		minus[6] += (unsigned long)ADC_REG(id, CLMD);
	}

	ADC_REG(id, PG)   = (0x8000 | ((plus[0]  + plus[1]  + plus[2]  + plus[3]  + plus[4]  + plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC_REG(id, MG)   = (0x8000 | ((minus[0] + minus[1] + minus[2] + minus[3] + minus[4] + minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC_REG(id, OFS)  = (offset   >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLP0) = (plus[0]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLP1) = (plus[1]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLP2) = (plus[2]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLP3) = (plus[3]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLP4) = (plus[4]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLPS) = (plus[5]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLPD) = (plus[6]  >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLM0) = (minus[0] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLM1) = (minus[1] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLM2) = (minus[2] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLM3) = (minus[3] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLM4) = (minus[4] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLMS) = (minus[5] >> TWO_POW_NUM_OF_CAL);
	ADC_REG(id, CLMD) = (minus[6] >> TWO_POW_NUM_OF_CAL);

	/* Clear setup */
	ADC_REG(id, SC3) = scr3;

	return true;
}

void adcStart (adc_id_t id, adc_channel_t ch, adc_mux_t mux, bool ie)
{
	ADC_REG(id, CFG2) = (ADC_REG(id, CFG2) & ~ADC_CFG2_MUXSEL_MASK) | ADC_CFG2_MUXSEL(mux);
	ADC_REG(id, SC1[mux]) = ADC_SC1_AIEN(ie) | ADC_SC1_ADCH(ch);
}

bool adcIsReady (adc_id_t id, adc_mux_t mux)
{
	return ADC_REG(id, SC1[mux]) & ADC_SC1_COCO_MASK;
}

adc_data_t adcGetData (adc_id_t id, adc_mux_t mux)
{
	return ADC_REG(id, R[mux]);
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// ISR Functions ///////////////////////////////////////////////////////////////

__ISR__ ADC0_IRQHandler (void)
{
	adc_data_t data = adcGetData(ADC0_ID, mux[ADC0_ID]);
	adcClearInterruptFlag(ADC0_ID, mux[ADC0_ID]);
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
