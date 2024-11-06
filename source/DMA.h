/*
 * dma.h
 *
 *  Created on: 5 nov 2024
 *      Author: asolari
 */

#ifndef DMA_H_
#define DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//Struct for DMA channel configuration
// Size of the bits transferred in a brust
typedef enum
{
  DMA_TransSize_8Bit = 0x0,
  DMA_TransSize_16Bit = 0x01,
  DMA_TransSize_32Bit = 0x02,
  DMA_TransSize_16BitBurst = 0x04,
  DMA_TransSize_32BitBurst = 0x05
} DMATranfSize_t;

typedef enum
{
  DMA_CH0,
  DMA_CH1,
  DMA_CH2,
  DMA_CH3,
  DMA_CH4,
  DMA_CH5,
  DMA_CH6,
  DMA_CH7,
  DMA_CH8,
  DMA_CH9,
  DMA_CH10,
  DMA_CH11,
  DMA_CH12,
  DMA_CH13,
  DMA_CH14,
  DMA_CH15,
} DMAChannel_t;

typedef void (*callback_t)(void);


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the DMA peripheral
 */
void DMA_Init(void);

/**
 * @brief Start a DMA transfer on the specified channel
 *
 * @param chn DMA channel to start the transfer on
 */
void DMA_StartTransfer(DMAChannel_t chn);

/**
 * @brief Configure the DMAMUX for a specific channel
 *
 * @param chn DMA channel to configure
 * @param enable Enable or disable the channel
 * @param trigger Enable or disable the trigger
 * @param source DMA request source
 */
void DMAMUX_ConfigChannel(DMAChannel_t chn, bool enable, bool trigger, dma_request_source_t source);

/**
 * @brief Set the interrupt mode and callback for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param mode Interrupt mode
 * @param cb Callback function to be called on interrupt
 */
void DMA_SetChannelInterrupt(DMAChannel_t chn, bool mode, callback_t cb);

/**
 * @brief Enable or disable DMA request for a channel
 *
 * @param chn DMA channel to configure
 * @param enable Enable or disable the request
 */
void DMA_SetEnableRequest(DMAChannel_t chn, bool enable);

/**
 * @brief Get the enable request status of a DMA channel
 *
 * @param chn DMA channel to query
 * @return true if the request is enabled, false otherwise
 */
bool DMA_GetEnableRequest(DMAChannel_t chn);

/**
 * @brief Set the source address modulo for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param mod Modulo value
 */
void DMA_SetSourceModulo(DMAChannel_t chn, uint16_t mod);

/**
 * @brief Get the source address modulo for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Modulo value
 */
uint16_t DMA_GetSourceModulo(DMAChannel_t chn);

/**
 * @brief Set the source address for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param addr Source address
 */
void DMA_SetSourceAddr(DMAChannel_t chn, uint32_t addr);

/**
 * @brief Get the source address for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Source address
 */
uint32_t DMA_GetSourceAddr(DMAChannel_t chn);

/**
 * @brief Set the source address offset for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param offset Source address offset
 */
void DMA_SetSourceAddrOffset(DMAChannel_t chn, int32_t offset);

/**
 * @brief Get the source address offset for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Source address offset
 */
int32_t DMA_GetSourceAddrOffset(DMAChannel_t chn);

/**
 * @brief Set the source transfer size for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param txsize Transfer size
 */
void DMA_SetSourceTransfSize(DMAChannel_t chn, DMATranfSize_t txsize);

/**
 * @brief Get the source transfer size for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Transfer size
 */
DMATranfSize_t DMA_GetSourceTransfSize(DMAChannel_t chn);

/**
 * @brief Set the source last address offset for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param offset Last address offset
 */
void DMA_SetSourceLastAddrOffset(DMAChannel_t chn, int32_t offset);

/**
 * @brief Get the source last address offset for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Last address offset
 */
int32_t DMA_GetSourceLastAddrOffset(DMAChannel_t chn);

/**
 * @brief Set the destination address modulo for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param mod Modulo value
 */
void DMA_SetDestModulo(DMAChannel_t chn, uint16_t mod);

/**
 * @brief Get the destination address modulo for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Modulo value
 */
uint16_t DMA_GetDestModulo(DMAChannel_t chn);

/**
 * @brief Set the destination address for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param addr Destination address
 */
void DMA_SetDestAddr(DMAChannel_t chn, uint32_t addr);

/**
 * @brief Get the destination address for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Destination address
 */
uint32_t DMA_GetDestAddr(DMAChannel_t chn);

/**
 * @brief Set the destination address offset for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param offset Destination address offset
 */
void DMA_SetDestAddrOffset(DMAChannel_t chn, int32_t offset);

/**
 * @brief Get the destination address offset for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Destination address offset
 */
int32_t DMA_GetDestAddrOffset(DMAChannel_t chn);

/**
 * @brief Set the destination transfer size for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param txsize Transfer size
 */
void DMA_SetDestTransfSize(DMAChannel_t chn, DMATranfSize_t txsize);

/**
 * @brief Get the destination transfer size for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Transfer size
 */
DMATranfSize_t DMA_GetDestTransfSize(DMAChannel_t chn);

/**
 * @brief Set the destination last address offset for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param offset Last address offset
 */
void DMA_SetDestLastAddrOffset(DMAChannel_t chn, int32_t offset);

/**
 * @brief Get the destination last address offset for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Last address offset
 */
int32_t DMA_GetDetLastAddrOffset(DMAChannel_t chn);

/**
 * @brief Set the minor loop transfer count for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param MinorLoopSize Minor loop size
 */
void DMA_SetMinorLoopTransCount(DMAChannel_t chn, uint32_t MinorLoopSize);

/**
 * @brief Get the minor loop transfer count for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Minor loop size
 */
uint32_t DMA_GetMinorLoopTransCount(DMAChannel_t chn);

/**
 * @brief Set the current major loop count for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param count Major loop count
 */
void DMA_SetCurrMajorLoopCount(DMAChannel_t chn, uint16_t count);

/**
 * @brief Get the current major loop count for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Major loop count
 */
uint16_t DMA_GetCurrMajorLoopCount(DMAChannel_t chn);

/**
 * @brief Set the start major loop count for a DMA channel
 *
 * @param chn DMA channel to configure
 * @param count Major loop count
 */
void DMA_SetStartMajorLoopCount(DMAChannel_t chn, uint16_t count);

/**
 * @brief Get the start major loop count for a DMA channel
 *
 * @param chn DMA channel to query
 * @return Major loop count
 */
uint16_t DMA_GetStartMajorLoopCount(DMAChannel_t chn);

#endif /* DMA_H_ */
