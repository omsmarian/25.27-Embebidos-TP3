/*
 * ftm.h
 *
 *  Created on: 4 nov 2024
 *      Author: asolari
 */

#ifndef FTM_H_
#define FTM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hardware.h"
#include "MK64F12.h"


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum
{
	FTM_CH_0 = 0,
	FTM_CH_1,
	FTM_CH_2,
	FTM_CH_3,
	FTM_CH_4,
	FTM_CH_5,
	FTM_CH_6,
	FTM_CH_7,
}FTM_Channel_t;

typedef enum
{
	FTM_SC_CLKS_DISABLED = 0b00,
	FTM_SC_CLKS_SYSTEM_CLOCK = 0b01,
	FTM_SC_CLKS_FIXED_FREQUENCY = 0b10,
	FTM_SC_CLKS_EXTERNAL_CLOCK = 0b11,
}FTM_SC_CLKS_t;

//Modes of operation, could be:
// - Input Capture
// - Output Compare
// - Pulse Width Modulation
typedef enum
{
  FTM_mInputCapture,
  FTM_mOutputCompare,
  FTM_mPulseWidthModulation,
} FTMMode_t;

//Edge modes for input capture
// - Rising
// - Falling
// - Either
typedef enum
{
  FTM_eRising = 0x01,
  FTM_eFalling = 0x02,
  FTM_eEither = 0x03,
} FTMEdge_t;

//Output modes for output compare
// - Toggle
// - Clear
// - Set
typedef enum
{
  FTM_eToggle = 0x01,
  FTM_eClear = 0x02,
  FTM_eSet = 0x03,
} FTMEffect_t;

//Polarity for output compare
// - High
// - Low
typedef enum
{
  FTM_lAssertedHigh = 0x02,
  FTM_lAssertedLow = 0x03,
} FTMLogic_t;

//Prescaler values
// - x1
// - x2
// - x4
// - x8
// - x16
// - x32
// - x64
// - x128
typedef enum
{
  FTM_PSC_x1 = 0x00,
  FTM_PSC_x2 = 0x01,
  FTM_PSC_x4 = 0x02,
  FTM_PSC_x8 = 0x03,
  FTM_PSC_x16 = 0x04,
  FTM_PSC_x32 = 0x05,
  FTM_PSC_x64 = 0x06,
  FTM_PSC_x128 = 0x07,

} FTM_Prescal_t;

//Output compare channels
// - 0: FTM_CH0
// - 1: CMP0_OUTPUT
// - 2: CMP1_OUTPUT
typedef enum
{
  FTMx_CH0_SIGNAL = 0x00,
  CMP0_OUTPUT = 0x01,
  CMP1_OUTPUT = 0x02,
} FTM_InputCaptureSource_t;


typedef uint16_t FTMData_t;
typedef uint32_t FTMChannel_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief Initialize the FTM module
 * @param void
 * @return void
 */
void FTM_Init(void);

/*
 * @brief Set prescaler for the FTM module
 * @param base: FTM module to configure
 * @param prescaler: Prescaler value
 * @return void
 */
void FTM_SetPrescaler(FTM_Type *base, FTM_Prescal_t prescaler);

/*
 * @brief Set the mode of operation for the FTM module
 * @param base: FTM module to configure
 * @param mod: Set the mod of the compare
 * @return void
 */
void FTM_SetMod(FTM_Type *base, uint16_t mod);
uint16_t FTM_GetMod(FTM_Type *base);


/*
 * @brief Start or stop the FTM Clock
 * @param base: FTM module to configure
 */
void FTM_StartClock(FTM_Type *base);
void FTM_StopClock(FTM_Type *base);


/*
 * @brief Start the Overflow interrupt
 * @param base: FTM module to configure
 * @param enable: Enable or disable the interrupt
 * @return void
 */
void FTM_SetOverflowMode(FTM_Type *base, bool enable);
bool FTM_IsOverflowPending(FTM_Type *base);
void FTM_ClearOverflowFlag(FTM_Type *base);



/*
 * @brief Set working mode for a channel
 * @param base: FTM module to configure
 * @param channel: Channel to configure
 * @param mode: Mode of operation
 * @return void
 */
void FTM_SetWorkingMode(FTM_Type *base, FTMChannel_t channel, FTMMode_t mode);
FTMMode_t FTM_GetWorkingMode(FTM_Type *base, FTMChannel_t channel);


//Working modes

//Input Capture
void FTM_SetInputCaptureEdge(FTM_Type *base, FTMChannel_t channel, FTMEdge_t edge);
FTMEdge_t FTM_GetInputCaptureEdge(FTM_Type *base, FTMChannel_t channel);

//Output Compare
void FTM_SetOutputCompareEffect(FTM_Type *base, FTMChannel_t channel, FTMEffect_t effect);
FTMEffect_t FTM_GetOutputCompareEffect(FTM_Type *base, FTMChannel_t channel);

//Pulse Width Modulation
void FTM_SetPulseWidthModulationLogic(FTM_Type *base, FTMChannel_t channel, FTMLogic_t logic);
FTMLogic_t FTM_GetPulseWidthModulationLogic(FTM_Type *base, FTMChannel_t channel);


/*
 * @brief Set counter value (CnV) for a channel
 * @param base: FTM module to configure
 * @param channel: Channel to configure
 * @param value: Value to set in CnV
 */
void FTM_SetCounter(FTM_Type *base, FTMChannel_t channel, uint16_t value);
uint16_t FTM_GetCounter(FTM_Type *base, FTMChannel_t channel);


/*
 * @brief Enable or disable the channel interrupt
 * @param base: FTM module to configure
 * @param channel: Channel to configure
 * @param enable: Enable or disable the interrupt
 */
void FTM_SetInterruptMode(FTM_Type *base, FTMChannel_t channel, bool enable);
bool FTM_IsInterruptPending(FTM_Type *base, FTMChannel_t channel);
void FTM_ClearInterruptFlag(FTM_Type *base, FTMChannel_t channel);


/*
 * @brief Enable or disable the DMA mode for a channel
 * @param base: FTM module to configure
 * @param channel: Channel to configure
 * @param enable: Enable or disable the DMA mode
 */
void FTM_DMAMode(FTM_Type *base, FTMChannel_t channel , bool enable);


/*
 * @brief Set the input capture source
 * @param base: FTM module to configure
 * @param source: Source to set
 */
void FTM_SetInputCaptureSource(FTM_Type *base, FTM_InputCaptureSource_t source);
FTM_InputCaptureSource_t FTM_GetInputCaptureSource(FTM_Type *base);

#endif /* FTM_H_ */
