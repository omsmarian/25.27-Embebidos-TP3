/***************************************************************************//**
  @file     converter.h
  @brief    AD and DA converter driver for K64F, using ADC and DAC peripherals
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
 ******************************************************************************/

#ifndef _CONVERTER_H_
#define _CONVERTER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DEVELOPMENT_MODE			1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef unsigned char uchar_t;

typedef enum {
	ADC,
	DAC,
	ADC_DAC // Both
} converter_type_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the converter(s) module(s)
 * @return Initialization succeed
 */
bool converterInit (converter_type_t type);

bool converterStart (void);

bool converterStatus (void);

bool converterGetData (void);

/*******************************************************************************
 ******************************************************************************/

#endif // _SERIAL_H_
