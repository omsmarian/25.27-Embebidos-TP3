/*
 * ftm.c
 *
 *  Created on: 4 nov 2024
 *      Author: asolari
 */


#include "FTM.h"


void FTM_Init(void)
{
	//Enable clocks
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;

	//Enable the FTM module interrupts
	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM1_IRQn);
	NVIC_EnableIRQ(FTM2_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);
}

void FTM_SetPrescaler(FTM_Type *base, FTM_Prescal_t prescaler)
{
	base->SC = (base->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(prescaler);
}

void FTM_SetMod(FTM_Type *base, uint16_t mod)
{
	//Reset the counter CNTIN and the actual counter CNT
	base->CNTIN = 0;
	base->CNT = 0;

	//Set the mod value (value to compare with the counter)
	base->MOD = FTM_MOD_MOD(mod);
}

uint16_t FTM_GetMod(FTM_Type *base)
{
	//Return the mod value
	return base->MOD;
}

void FTM_StartClock(FTM_Type *base)
{
	//Set the clock source to the system clock
	base->SC |= FTM_SC_CLKS(FTM_SC_CLKS_SYSTEM_CLOCK);
}

void FTM_StopClock(FTM_Type *base)
{
	//Set the clock source to no clock for stopping the counter
	base->SC  = FTM_SC_CLKS(FTM_SC_CLKS_DISABLED);
	//Cleear the counter
	base->CNT = 0;
}


void FTM_SetOverflowMode(FTM_Type *base, bool enable)
{
	//Enable or disable the overflow interrupt
	// TOIE = 1: Enable overflow interrupt
	// TOIE = 0: Disable overflow interrupt
	base->SC = (base->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(enable);
}

bool FTM_IsOverflowPending(FTM_Type *base)
{
	//Return the overflow flag
	return base->SC & FTM_SC_TOF_MASK;
}

void FTM_ClearOverflowFlag(FTM_Type *base)
{
	//Clear the overflow flag
	base->SC &= ~FTM_SC_TOF_MASK;
}

void FTM_SetWorkingMode(FTM_Type *base, FTMChannel_t channel, FTMMode_t mode)
{
	//Set the mode
	// MSB - MSA
	// 0b00 : Input Capture
	// 0b01 : Output Compare
	// 0b1x : PWM
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
								  (FTM_CnSC_MSB((mode >> 1) & 0X01) | FTM_CnSC_MSA((mode >> 0) & 0X01));

	//If the mode is PWM, set the PWMLOAD register
	if (mode == FTM_mPulseWidthModulation)
	{
		base->PWMLOAD = (FTM_PWMLOAD_LDOK_MASK | (0x01 << channel) | 0x200);
	}
}


FTMMode_t FTM_GetWorkingMode(FTM_Type *base, FTMChannel_t channel)
{
	//Return the mode
	return (base->CONTROLS[channel].CnSC & (FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) >> FTM_CnSC_MSA_SHIFT;
}

//Input Capture
void FTM_SetInputCaptureEdge(FTM_Type *base, FTMChannel_t channel, FTMEdge_t edge)
{
	//Set the edge
	// ELSB - ELSA
	// 0b00 : Rising edge
	// 0b01 : Falling edge
	// 0b10 : Rising and falling edge
	// 0b11 : None
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((edge >> 1) & 0X01) | FTM_CnSC_ELSA((edge >> 0) & 0X01));
}
FTMEdge_t FTM_GetInputCaptureEdge(FTM_Type *base, FTMChannel_t channel)
{
	//Return the edge
	return (base->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

//Output Compare
void FTM_SetOutputCompareEffect(FTM_Type *base, FTMChannel_t channel, FTMEffect_t effect)
{
	//Set the effect
	// MSB - MSA
	// 0b00 : Do nothing
	// 0b01 : Toggle output
	// 0b10 : Clear output
	// 0b11 : Set output
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((effect >> 1) & 0X01) | FTM_CnSC_ELSA((effect >> 0) & 0X01));
}
FTMEffect_t FTM_GetOutputCompareEffect(FTM_Type *base, FTMChannel_t channel)
{
	//Return the effect
	return (base->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

//Pulse Width Modulation
void FTM_SetPulseWidthModulationLogic(FTM_Type *base, FTMChannel_t channel, FTMLogic_t logic)
{
	//Set the logic
	// ELSB - ELSA
	// 0b00 : High true pulses
	// 0b01 : Low true pulses
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((logic >> 1) & 0X01) | FTM_CnSC_ELSA((logic >> 0) & 0X01));
}
FTMLogic_t FTM_GetPulseWidthModulationLogic(FTM_Type *base, FTMChannel_t channel)
{
	//Return the logic
	return (base->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}


void FTM_SetCounter(FTM_Type *base, FTMChannel_t channel, uint16_t value)
{
	//Set the counter value of CnV which is the value to compare with the counter
	base->CONTROLS[channel].CnV = FTM_CnV_VAL(value);
}
uint16_t FTM_GetCounter(FTM_Type *base, FTMChannel_t channel)
{
	//Return the counter value
	return base->CONTROLS[channel].CnV;
}

void FTM_SetInterruptMode(FTM_Type *base, FTMChannel_t channel, bool enable)
{
	//Enable or disable the interrupt
	// CHIE = 1: Enable interrupt
	// CHIE = 0: Disable interrupt
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(enable);
}
bool FTM_IsInterruptPending(FTM_Type *base, FTMChannel_t channel)
{
	//Return the interrupt flag
	return base->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK;
}
void FTM_ClearInterruptFlag(FTM_Type *base, FTMChannel_t channel)
{
	//Clear the interrupt flag
	base->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

void FTM_DMAMode(FTM_Type *base, FTMChannel_t channel , bool enable)
{
	//Enable or disable the DMA mode
	// DMA = 1: Enable DMA mode
	// DMA = 0: Disable DMA mode
	base->CONTROLS[channel].CnSC = (base->CONTROLS[channel].CnSC & ~(FTM_CnSC_DMA_MASK)) |
								  (FTM_CnSC_DMA(enable));
}

void FTM_SetInputCaptureSource(FTM_Type *base, FTM_InputCaptureSource_t source)
{
	//Set the input capture source
	// CLKS - PS
	// 0b00 : FTM_CLKIN0
	// 0b01 : FTM_CLKIN1
	// 0b10 : FTM_CLKIN2
	// 0b11 : FTM_CLKIN3

	switch ((uint32_t)base)
	{
	case (uint32_t)FTM1:
		SIM->SOPT4 = (SIM->SOPT4 & ~SIM_SOPT4_FTM1CH0SRC_MASK) | SIM_SOPT4_FTM1CH0SRC(source);
		break;

	case (uint32_t)FTM2:
		SIM->SOPT4 = (SIM->SOPT4 & ~SIM_SOPT4_FTM2CH0SRC_MASK) | SIM_SOPT4_FTM2CH0SRC(source);
		break;
	}
}

FTM_InputCaptureSource_t FTM_GetInputCaptureSource(FTM_Type *base)
{
	switch ((uint32_t)base)
	{
	case (uint32_t)FTM1:
		return (SIM->SOPT4 & SIM_SOPT4_FTM1CH0SRC_MASK) >> SIM_SOPT4_FTM1CH0SRC_SHIFT;
		break;

	case (uint32_t)FTM2:
		return (SIM->SOPT4 & SIM_SOPT4_FTM2CH0SRC_MASK) >> SIM_SOPT4_FTM2CH0SRC_SHIFT;
		break;
	}
	return 0;
}

