/***************************************************************************/
/**
 @file     fsk_mod.h
 @brief    Header file for FSK modulation functions
 @details  This file contains the function prototypes and necessary includes
 for implementing FSK modulation.
 @author   asolari
 ******************************************************************************/

#ifndef _FSKMOD_H_
#define _FSKMOD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <queue.h>
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the FSK modulation module.
 */
void initFSKMod(void);

/**
 * @brief Puts an array of data into the FSK modulation queue.
 * @param data Pointer to the data array to be queued.
 * @param dataAmount The amount of data to be queued.
 */
void putArrayFSKMod(queuedata_t *data, uint32_t dataAmount);

/*******************************************************************************
 ******************************************************************************/

#endif // _FSKMOD_H_
