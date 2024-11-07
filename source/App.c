/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Nicolás Magliola
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <gpio.h>
#include "board.h"
#include "uart.h"
#include "FSKMod.h"
#include "PIT.h"
#include "FSKDem.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
uart_cfg_t config = {
    true,
    true,
    1200,
    TXRX};

void delay(uint32_t a)
{
  while (a)
  {
    a--;
  }
}
/* Función que se llama 1 vez, al comienzo del programa */
void App_Init(void)
{
  initPIT();

  initFSKMod();
  initFSKDemodulator();

  uartInit(0, config);
//  uartInit(3, config);

  gpioMode(PORTNUM2PIN(PB, 9), OUTPUT);
  gpioWrite(PORTNUM2PIN(PB, 9), LOW);

}



uint8_t c1 = 0;
uint8_t c2 = 0;



/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void)
{
	if (uartIsRxMsg(0)) {
		uartReadMsg(0, &c1, 1);
		gpioToggle(PORTNUM2PIN(PB, 9));
		putArrayFSKMod(&c1, 1);
	}

	if(isDataReadyHART())
	{
		gpioToggle(PORTNUM2PIN(PB, 9));
		c2 = getNextValueHART();
		uartWriteMsg(0, &c2, 1);
	}

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
