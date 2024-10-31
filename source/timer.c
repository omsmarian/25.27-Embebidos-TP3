/***************************************************************************//**
  @file     timer.c
  @brief    Timer driver. Simple implementation, support multiple timers
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "debug.h"
#include "pisr.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DEVELOPMENT_MODE		1

#define TIMER_FREQUENCY_HZ		(1000000/TIMER_TICK_US)
#define TIMER_TICKS2MS(ticks)	((ticks)*TIMER_TICK_US/1000)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void timer_isr(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static volatile ticks_t timer_main_counter, timer_mark;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timerInit(void)
{
    static bool yaInit = false;
    if (yaInit)
        return;
    
    pisrRegister(timer_isr, PISR_FREQUENCY_HZ / TIMER_FREQUENCY_HZ); // init peripheral
    
    yaInit = true;
}

ticks_t timerStart(ticks_t ticks)
{
    ticks_t now_copy;
    
    if (ticks < 0)
        ticks = 0; // truncate min wait time
    
    //disable_interrupts();
    now_copy = timer_main_counter; // esta copia debe ser atomic!!
    //enable_interrupts();

    now_copy += ticks;

    return now_copy;
}

bool timerExpired(ticks_t timeout)
{
    ticks_t now_copy;

    //disable_interrupts();
    now_copy = timer_main_counter; // esta copia debe ser atomic!!
    //enable_interrupts();

    now_copy -= timeout;
    return (now_copy >= 0);
}

void timerDelay(ticks_t ticks)
{
    ticks_t tim;
    
    tim = timerStart(ticks);
    while (!timerExpired(tim))
    {
        // wait...
    }
}

uint32_t timerCounter(void)
{
	ticks_t diff = timer_main_counter-timer_mark;

	timer_mark = timer_main_counter;

	return TIMER_TICKS2MS(diff);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void timer_isr(void)
{
#if DEBUG_TIMER
P_DEBUG_TP_SET
#endif
    ++timer_main_counter; // update main counter
#if DEBUG_TIMER
P_DEBUG_TP_CLR
#endif
}

/******************************************************************************/
