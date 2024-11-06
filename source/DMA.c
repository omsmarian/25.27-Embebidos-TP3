/*
 * dma.c
 *
 *  Created on: 5 nov 2024
 *      Author: asolari
 */

#include "DMA.h"

static callback_t callback[16] = {0};


// Initializes the DMA by enabling the clock for the eDMA and DMAMUX, clearing pending events, and enabling interrupts
void DMA_Init()
{
	/* Enable the clock for the eDMA and the DMAMUX. */
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;  // Enable clock for eDMA
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  // Enable clock for DMAMUX

	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn);  // Clear pending IRQ for DMA0
	NVIC_ClearPendingIRQ(DMA1_IRQn);  // Clear pending IRQ for DMA1
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA0_IRQn);  // Enable IRQ for DMA0
	NVIC_EnableIRQ(DMA1_IRQn);  // Enable IRQ for DMA1
//	gpioMode(PORTNUM2PIN(PC, 8), OUTPUT);  // Configure pin as output to monitor in the interrupt
}

// Starts the transfer on the specified DMA channel
void DMA_StartTransfer(DMAChannel_t chn)
{
	DMA0->SSRT = chn;  // Set the Start bit for the DMA transfer on channel
}

// Configures the DMAMUX for the specified DMA channel, enabling it, setting trigger mode, and selecting the data source
void DMAMUX_ConfigChannel(DMAChannel_t chn, bool enable, bool trigger, dma_request_source_t source)
{
	DMA0->TCD[chn].CSR = 0;  // Clear the TCD Control and Status register
	DMAMUX0->CHCFG[chn] = DMAMUX_CHCFG_ENBL(enable) + DMAMUX_CHCFG_SOURCE(source) + DMAMUX_CHCFG_TRIG(trigger);  // Configure DMAMUX settings
}

// Sets an interrupt on the specified DMA channel, with the option to associate a callback function that is triggered upon completion
void DMA_SetChannelInterrupt(DMAChannel_t chn, bool mode, callback_t cb)
{
	callback[chn] = cb;  // Assign the callback function for the interrupt
	DMA0->TCD[chn].CSR = (DMA0->TCD[chn].CSR & ~DMA_CSR_INTMAJOR_MASK) + DMA_CSR_INTMAJOR(mode);  // Enable the major loop interrupt if mode is true
}

// Enables or disables the DMA request for the specified channel
void DMA_SetEnableRequest(DMAChannel_t chn, bool state)
{
	DMA0->ERQ = (DMA0->ERQ & ~(1 << chn)) + (state << chn);  // Set or clear the enable request bit
}

// Gets the enable status of the DMA request for the specified channel
bool DMA_GetEnableRequest(DMAChannel_t chn)
{
	return (DMA0->ERQ >> chn) & 1;  // Return the enable status for the request
}

// Sets the source address modulo, defining the circular buffer size for the source address
void DMA_SetSourceModulo(DMAChannel_t chn, uint16_t mod)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_SMOD_MASK) + DMA_ATTR_SMOD(mod);  // Set source address modulo
}

// Gets the source address modulo setting
uint16_t DMA_GetSourceModulo(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_SMOD_MASK) >> DMA_ATTR_SMOD_SHIFT;  // Return the source address modulo
}

// Sets the destination address modulo, defining the circular buffer size for the destination address
void DMA_SetDestModulo(DMAChannel_t chn, uint16_t mod)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_DMOD_MASK) + DMA_ATTR_DMOD(mod);  // Set destination address modulo
}

// Gets the destination address modulo setting
uint16_t DMA_GetDestModulo(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_DMOD_MASK) >> DMA_ATTR_DMOD_SHIFT;  // Return the destination address modulo
}

// Sets the source address for the specified DMA channel
void DMA_SetSourceAddr(DMAChannel_t chn, uint32_t addr)
{
	DMA0->TCD[chn].SADDR = addr;  // Set the source address
}

// Gets the source address for the specified DMA channel
uint32_t DMA_GetSourceAddr(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SADDR;  // Return the source address
}

// Sets the source address offset, added to the address after each minor loop transfer
void DMA_SetSourceAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].SOFF = offset;  // Set source address offset
}

// Gets the source address offset
int32_t DMA_GetSourceAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SOFF;  // Return source address offset
}

// Sets the source transfer size for each minor loop
void DMA_SetSourceTransfSize(DMAChannel_t chn, DMATranfSize_t txsize)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_SSIZE_MASK) + DMA_ATTR_SSIZE(txsize);  // Set source transfer size
}

// Gets the source transfer size for each minor loop
DMATranfSize_t DMA_GetSourceTransfSize(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_SSIZE_MASK) >> DMA_ATTR_SSIZE_SHIFT;  // Return source transfer size
}

// Sets the final source address offset after a major loop transfer
void DMA_SetSourceLastAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].SLAST = offset;  // Set source last address offset
}

// Gets the final source address offset after a major loop transfer
int32_t DMA_GetSourceLastAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SLAST;  // Return source last address offset
}

// Sets the destination address for the specified DMA channel
void DMA_SetDestAddr(DMAChannel_t chn, uint32_t addr)
{
	DMA0->TCD[chn].DADDR = addr;  // Set destination address
}

// Gets the destination address for the specified DMA channel
uint32_t DMA_GetDestAddr(DMAChannel_t chn)
{
	return DMA0->TCD[chn].DADDR;  // Return destination address
}

// Sets the destination address offset, added after each minor loop transfer
void DMA_SetDestAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].DOFF = offset;  // Set destination address offset
}

// Gets the destination address offset
int32_t DMA_GetDestAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].DOFF;  // Return destination address offset
}

// Sets the destination transfer size for each minor loop
void DMA_SetDestTransfSize(DMAChannel_t chn, DMATranfSize_t txsize)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_DSIZE_MASK) + DMA_ATTR_DSIZE(txsize);  // Set destination transfer size
}

// Gets the destination transfer size for each minor loop
DMATranfSize_t DMA_GetDestTransfSize(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_DSIZE_MASK) >> DMA_ATTR_DSIZE_SHIFT;  // Return destination transfer size
}

void DMA_SetDestLastAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].DLAST_SGA = offset;  // Set destination last address offset
}


// Sets the minor loop transfer count, determining the data amount per minor loop cycle
void DMA_SetMinorLoopTransCount(DMAChannel_t chn, uint32_t MinorLoopSize)
{
	DMA0->TCD[chn].NBYTES_MLOFFNO = MinorLoopSize & DMA_NBYTES_MLOFFNO_NBYTES_MASK;  // Set minor loop size
}

// Gets the minor loop transfer count
uint32_t DMA_GetMinorLoopTransCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].NBYTES_MLOFFNO;  // Return minor loop transfer count
}

// Sets the current iteration count for the major loop (count of minor loops for each major loop)
void DMA_SetCurrMajorLoopCount(DMAChannel_t chn, uint16_t count)
{
	DMA0->TCD[chn].CITER_ELINKNO = count & DMA_CITER_ELINKNO_CITER_MASK;  // Set current major loop count
}

// Gets the current major loop count
uint16_t DMA_GetCurrMajorLoopCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK;  // Return current major loop count
}

// Sets the initial count for the major loop, starting the iteration counter
void DMA_SetStartMajorLoopCount(DMAChannel_t chn, uint16_t count)
{
	DMA0->TCD[chn].BITER_ELINKNO = count & DMA_BITER_ELINKNO_BITER_MASK;  // Set initial major loop count
}

// Gets the initial major loop count
uint16_t DMA_GetStartMajorLoopCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].BITER_ELINKNO & DMA_BITER_ELINKNO_BITER_MASK;  // Return initial major loop count
}

// Interrupt handler for DMA0, clears interrupt flag and calls the associated callback if defined
__ISR__ DMA0_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMA0->CINT = 0;  // Clear DMA0 interrupt flag

	if (callback[0] != 0)  // If callback is defined, execute it
	{
		callback[0]();
	}
}

// Interrupt handler for DMA1, clears interrupt flag, toggles a GPIO pin, and calls the associated callback if defined
__ISR__ DMA1_IRQHandler()
{
//	gpioWrite(PORTNUM2PIN(PC, 8), true);  // Set GPIO high for timing the interrupt
	/* Clear the interrupt flag. */
	DMA0->CINT = 1;  // Clear DMA1 interrupt flag

	if (callback[1] != 0)  // If callback is defined, execute it
	{
		callback[1]();
	}
//	gpioWrite(PORTNUM2PIN(PC, 8), false);  // Set GPIO low after interrupt handling
}
