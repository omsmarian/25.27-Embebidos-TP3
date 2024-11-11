#include <dma.h>
#include <fsk_dmod.h>
#include <ftm.h>
#include <pit.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include "MK64F12.h"
#include "uart.h"

// Define constants for maximum count and frequency thresholds
#define MAX_COUNT 50000
#define F0_THRESHHOLD 14000
#define F1_THRESHHOLD 19000

// Enumeration for demodulator states
typedef enum {
	START_DETECT, INIT_WAIT, READ_STATE, RESET,
} demodulatorStates_t;

// Enumeration for port multiplexing options
typedef enum {
	PORT_mAnalog,
	PORT_mGPIO,
	PORT_mAlt2,
	PORT_mAlt3,
	PORT_mAlt4,
	PORT_mAlt5,
	PORT_mAlt6,
	PORT_mAlt7,
} PORTMux_t;

// Enumeration for port event types
typedef enum {
	PORT_eDisabled = 0x00,
	PORT_eDMARising = 0x01,
	PORT_eDMAFalling = 0x02,
	PORT_eDMAEither = 0x03,
	PORT_eInterruptDisasserted = 0x08,
	PORT_eInterruptRising = 0x09,
	PORT_eInterruptFalling = 0x0A,
	PORT_eInterruptEither = 0x0B,
	PORT_eInterruptAsserted = 0x0C,
} PORTEvent_t;

// Function prototypes
void FSKDemCallback(void);
void FSKDemCallback2(void);

// Static variables for internal state and data
static uint8_t timerID;
static uint16_t InputCaptureOut = 0;
static uint16_t lastCapture = 0;
static uint32_t timeDiff = 0;
static queue_t outputQueue = { { 0 }, 0, 0, false, true };
static demodulatorStates_t stateDEM = START_DETECT;
static bool errorFlag = false;
static uint32_t counter = 0;

// Function to calculate odd parity
static bool oddParity(uint8_t word, uint8_t parity);

// Function to initialize the FSK demodulator
void initFSKDemodulator() {
	FTM_Init();
	DMA_Init();

	timerID = createTimer(833.0f, FSKDemCallback2);

	// Configure Comparator 0
	SIM->SCGC4 |= SIM_SCGC4_CMP_MASK; // Enable clock gating

	PORTC->PCR[3] = PORT_PCR_MUX(PORT_mAnalog) | PORT_PCR_IRQC(PORT_eDisabled);
	PORTC->PCR[4] = PORT_PCR_MUX(
			PORT_mAlt6) | PORT_PCR_DSE(true) | PORT_PCR_IRQC(PORT_eDisabled);

	CMP1->CR0 = CMP_CR0_FILTER_CNT(7) | CMP_CR0_HYSTCTR(3); // Set filter and hysteresis
	CMP1->CR1 = CMP_CR1_EN(true) | CMP_CR1_OPE(1) | CMP_CR1_PMODE(1); // Enable comparator
	CMP1->FPR = 0x20;                                       // Set filter period
	CMP1->DACCR = CMP_DACCR_DACEN(1) | CMP_DACCR_VOSEL(31) | CMP_DACCR_VRSEL(1); // Enable DAC
	CMP1->MUXCR = CMP_MUXCR_PSEL(1) | CMP_MUXCR_MSEL(7);

	// Set FTM configuration
	FTM_SetPrescaler(FTM1, FTM_PSC_x1);
	FTM_SetInterruptMode(FTM1, FTM_CH_0, false); // Disable interrupts
	FTM_SetMod(FTM1, MAX_COUNT);

	// Set FTM as InputCapture mode
	FTM_SetWorkingMode(FTM1, FTM_CH_0, FTM_mInputCapture);
	FTM_SetInputCaptureEdge(FTM1, FTM_CH_0, FTM_eEither);
	FTM_SetInputCaptureChannelSource(FTM1, CMP1_OUTPUT);

	// Configure DMA
	FTM_DMAMode(FTM1, FTM_CH_0, true);
	FTM_SetInterruptMode(FTM1, FTM_CH_0, true);

	DMA_SetSourceModulo(DMA_CH1, 0);
	DMA_SetDestModulo(DMA_CH1, 0);

	DMA_SetSourceAddr(DMA_CH1, (uint32_t) &(FTM1->CONTROLS[FTM_CH_0].CnV));
	DMA_SetDestAddr(DMA_CH1, (uint32_t) (&InputCaptureOut));

	DMA_SetSourceAddrOffset(DMA_CH1, 0);
	DMA_SetDestAddrOffset(DMA_CH1, 0);

	DMA_SetSourceLastAddrOffset(DMA_CH1, 0);
	DMA_SetDestLastAddrOffset(DMA_CH1, 0);

	DMA_SetSourceTransfSize(DMA_CH1, DMA_TransSize_16Bit);
	DMA_SetDestTransfSize(DMA_CH1, DMA_TransSize_16Bit);

	DMA_SetMinorLoopTransCount(DMA_CH1, 2);

	DMA_SetCurrMajorLoopCount(DMA_CH1, 1);
	DMA_SetStartMajorLoopCount(DMA_CH1, 1);

	DMA_SetEnableRequest(DMA_CH1, true);

	DMAMUX_ConfigChannel(DMA_CH1, true, false, kDmaRequestMux0FTM1Channel0);
	DMA_SetChannelInterrupt(DMA_CH1, true, &FSKDemCallback);
	FTM_ClearInterruptFlag(FTM1, FTM_CH_0);
	FTM_ClearOverflowFlag(FTM1);
	FTM_StartClock(FTM1);
}

// Callback function for FSK demodulation
void FSKDemCallback(void) {
//	gpioToggle(PORTNUM2PIN(PB, 9));

	static uint32_t semiPeriodcounter = 0;

	// Calculate time difference between captures
	timeDiff =
			InputCaptureOut >= lastCapture ?
					InputCaptureOut - lastCapture :
					MAX_COUNT - lastCapture + InputCaptureOut;

	switch (stateDEM) {
	case START_DETECT:
		if (timeDiff < F0_THRESHHOLD + 3000) {
			stateDEM = INIT_WAIT;
		}
		break;
	case INIT_WAIT:
		if (semiPeriodcounter == 1) {
			stateDEM = READ_STATE;
			startTimer(timerID);
			semiPeriodcounter = 0;
			break;
		}
		semiPeriodcounter++;
		break;
	case RESET:
	case READ_STATE:
		break;
	default:
		stateDEM = START_DETECT;
		break;
	}

	lastCapture = InputCaptureOut;
//	gpioToggle(PORTNUM2PIN(PB, 9));

}

// Second callback function for FSK demodulation
void FSKDemCallback2(void) {
//	gpioToggle(PORTNUM2PIN(PB, 9));

	static uint8_t output = 0;
	static uint32_t bitCounter = 0;
	static bool parity = 0;
	static char msg[7] = { 0 };
	static char msg2 = '\n';

	switch (stateDEM) {
	case READ_STATE:
		if (bitCounter == 8) {
//			gpioToggle(PORTNUM2PIN(PB, 9));
			parity |= timeDiff > F0_THRESHHOLD ? 1 : 0;
			bitCounter = 0;
			stateDEM = RESET;
			break;
		}
		output |= timeDiff > F0_THRESHHOLD ? 1 << (7 - bitCounter) : 0;
		bitCounter++;
		break;
	case RESET:
		stopTimer(timerID);
		if (oddParity(output, parity))
		{
			gpioToggle(PORTNUM2PIN(PB, 9));
			put(&outputQueue, output);
		} else {
			errorFlag = 1;
		}
		output = 0;
		parity = 0;
		stateDEM = START_DETECT;
		break;
	case INIT_WAIT:
	case START_DETECT:
	default:
		stopTimer(timerID);
		stateDEM = START_DETECT;
		output = 0;
		parity = 0;
		break;
	}
//	gpioToggle(PORTNUM2PIN(PB, 9));

}

// Function to check if data is ready
bool isDataReadyHART() {
	return getFillLevel(&outputQueue) != 0;
}

// Function to get the next value from the queue
char getNextValueHART(void) {
	return getNext(&outputQueue);
}

// Function to calculate odd parity
static bool oddParity(uint8_t word, uint8_t parity) {
	uint8_t i = 0;
	uint8_t parityCalc = 0;
	for (i = 0; i < 8; i++) {
		if (word >> i & 1) {
			parityCalc ^= 1;
		}
	}
	return (parityCalc != parity);
}
