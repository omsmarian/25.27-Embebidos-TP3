
#ifndef _FTM_H_
#define _FTM_H_

#include "hardware.h"

typedef enum {
	FTM_mInputCapture,
	FTM_mOutputCompare,
	FTM_mPulseWidthModulation,
} FTMMode_t;

typedef enum {
	FTM_eRising 		= 0x01,
	FTM_eFalling 		= 0x02,
	FTM_eEither 		= 0x03,
} FTMEdge_t;

typedef enum {
	FTM_eToggle 		= 0x01,
	FTM_eClear 			= 0x02,
	FTM_eSet 			= 0x03,
} FTMEffect_t;

typedef enum {
	FTM_lAssertedHigh	= 0x02,
	FTM_lAssertedLow 	= 0x03,
} FTMLogic_t;

typedef enum {
	FTM_PSC_x1			= 0x00,
	FTM_PSC_x2			= 0x01,
	FTM_PSC_x4			= 0x02,
	FTM_PSC_x8			= 0x03,
	FTM_PSC_x16			= 0x04,
	FTM_PSC_x32			= 0x05,
	FTM_PSC_x64			= 0x06,
	FTM_PSC_x128		= 0x07,
} FTM_Prescal_t;

typedef enum {
	FTM_CH_0,
	FTM_CH_1,
	FTM_CH_2,
	FTM_CH_3,
	FTM_CH_4,
	FTM_CH_5,
	FTM_CH_6,
	FTM_CH_7
} FTM_Channel_t;

typedef FTM_Type* FTM_t;
typedef uint16_t FTMData_t;
typedef uint32_t FTMChannel_t; /* FTM0/FTM3: Channel 1-8; FTM1/FTM2: Channel 1-2 */

void 		FTM_Init 						 (void);

void        FTM_SetPrescaler 				 (FTM_t, FTM_Prescal_t);
void     	FTM_SetModulus 					 (FTM_t, FTMData_t);
FTMData_t 	FTM_GetModulus 					 (FTM_t);

void 		FTM_StartClock					 (FTM_t);
void 		FTM_StopClock					 (FTM_t);

void 		FTM_SetOverflowMode   			 (FTM_t, bool);
bool 		FTM_IsOverflowPending 			 (FTM_t);
void 		FTM_ClearOverflowFlag 			 (FTM_t);

void        FTM_SetWorkingMode				 (FTM_t, FTMChannel_t, FTMMode_t);
FTMMode_t   FTM_GetWorkingMode				 (FTM_t, FTMChannel_t);
void        FTM_SetInputCaptureEdge 		 (FTM_t, FTMChannel_t, FTMEdge_t);
FTMEdge_t   FTM_GetInputCaptureEdge 		 (FTM_t, FTMChannel_t);
void        FTM_SetOutputCompareEffect 	 	 (FTM_t, FTMChannel_t, FTMEffect_t);
FTMEffect_t FTM_GetOutputCompareEffect 		 (FTM_t, FTMChannel_t);
void        FTM_SetPulseWidthModulationLogic (FTM_t, FTMChannel_t, FTMLogic_t);
FTMLogic_t  FTM_GetPulseWidthModulationLogic (FTM_t, FTMChannel_t);

void        FTM_SetCounter 					 (FTM_t, FTMChannel_t, FTMData_t);
FTMData_t   FTM_GetCounter 					 (FTM_t, FTMChannel_t);

void 		FTM_SetInterruptMode   			 (FTM_t, FTMChannel_t, bool);
bool 		FTM_IsInterruptPending 			 (FTM_t, FTMChannel_t);
void 		FTM_ClearInterruptFlag 			 (FTM_t, FTMChannel_t);

#endif // _FTM_H_
