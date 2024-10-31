/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED				PORTNUM2PIN(PB, 22)	// PTB22
#define PIN_LED_GREEN			PORTNUM2PIN(PE, 26)	// PTE26
#define PIN_LED_BLUE			PORTNUM2PIN(PB, 21)	// PTB21

#define LED_ACTIVE      		LOW

// On Board User Switches
#define PIN_SW2					PORTNUM2PIN(PC, 6)	// PTC6
#define PIN_SW3					PORTNUM2PIN(PA, 4)	// PTA4

#define SW_ACTIVE				LOW
#define SW_INPUT_TYPE			INPUT_PULLUP

// UART
#define UART0_RX_PIN			PORTNUM2PIN(PB, 16)	// PTA1
#define UART0_TX_PIN			PORTNUM2PIN(PB, 17)	// PTA2
#define UART1_RX_PIN			PORTNUM2PIN(PE, 1)	// PTC3
#define UART1_TX_PIN			PORTNUM2PIN(PE, 0)	// PTC4
#define UART2_RX_PIN			PORTNUM2PIN(PD, 2)	// PTD2
#define UART2_TX_PIN			PORTNUM2PIN(PD, 3)	// PTD3
#define UART3_RX_PIN			PORTNUM2PIN(PE, 5)	// PTC16
#define UART3_TX_PIN			PORTNUM2PIN(PE, 4)	// PTC17
#define UART4_RX_PIN			PORTNUM2PIN(PE, 25)	// PTC14
#define UART4_TX_PIN			PORTNUM2PIN(PE, 24)	// PTC15
#define UART5_RX_PIN			PORTNUM2PIN(PE, 9)	// PTD8
#define UART5_TX_PIN			PORTNUM2PIN(PE, 8)	// PTC9

// ADC
#define ADC0_SE8_PIN			PORTNUM2PIN(PC, 0)	// PTC0
#define ADC0_SE14_PIN			PORTNUM2PIN(PC, 1)	// PTC1
#define ADC0_SE15_PIN			PORTNUM2PIN(PC, 2)	// PTC2
#define ADC0_SE4b_PIN			PORTNUM2PIN(PD, 1)	// PTD1
#define ADC0_SE5b_PIN			PORTNUM2PIN(PD, 5)	// PTD5
#define ADC0_SE6b_PIN			PORTNUM2PIN(PD, 6)	// PTD6
#define ADC0_SE7b_PIN			PORTNUM2PIN(PD, 7)	// PTD7
#define ADC0_SE0_PIN			PORTNUM2PIN(PD, 0)	// PTD0
#define ADC0_SE1_PIN			PORTNUM2PIN(PC, 16)	// PTC16
#define ADC0_SE2_PIN			PORTNUM2PIN(PC, 17)	// PTC17
#define ADC0_SE3_PIN			PORTNUM2PIN(PC, 18)	// PTC18
#define ADC0_SE4a_PIN			PORTNUM2PIN(PC, 19)	// PTC19
#define ADC0_SE5a_PIN			PORTNUM2PIN(PD, 8)	// PTD8
#define ADC0_SE6a_PIN			PORTNUM2PIN(PD, 9)	// PTD9
#define ADC0_SE7a_PIN			PORTNUM2PIN(PD, 10)	// PTD10
#define ADC0_SE8_PIN			PORTNUM2PIN(PD, 11)	// PTD11
#define ADC0_SE9_PIN			PORTNUM2PIN(PC, 9)	// PTC9

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
