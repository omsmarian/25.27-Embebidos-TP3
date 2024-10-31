/***************************************************************************//**
  @file     dac.h
  @brief    Digital to Analog Converter (DAC) Driver for K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

#ifndef DAC_H_
#define DAC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef DAC_Type* DAC_t;
typedef uint16_t DACData_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the DAC module
 */
void DAC_Init (void);

/**
 * @brief Sets the data to be output by the DAC
 * @param dac DAC to set the data
 * @param data Data to be output
 */
void DAC_SetData (DAC_t, DACData_t);

/*******************************************************************************
 ******************************************************************************/

#endif // _DAC_H_
