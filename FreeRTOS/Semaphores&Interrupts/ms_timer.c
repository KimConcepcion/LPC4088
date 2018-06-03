/*
 * ms_timer.c
 *
 *  Created on: Sep 21, 2015
 *      Author: mortenopprudjakobsen
 */
#include <stdio.h>
#include "board.h"

/*Do GPIO debug ? */
#define DEBUG_GPIO
/* GPIO PIN if doing debug*/
#define DEBUG_PORT	1
#define DEBUG_PIN		12

/* Module Global variables */
static volatile unsigned int msCount;

/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
void TIMER0_IRQHandler (void)
{
	/* do we have a match Interrupt ?*/
	if (Chip_TIMER_MatchPending (LPC_TIMER0, 1))
	{
		/* Yep, acknowledge and clear periphereal*/
		Chip_TIMER_ClearMatch (LPC_TIMER0, 1);
		/* increment mS counter */
		msCount++;
		/* include only below if #define DEBUG_GPIO is stated*/
#ifdef DEBUG_GPIO
		Chip_GPIO_SetPinToggle (LPC_GPIO, DEBUG_PORT, DEBUG_PIN);
#endif
	}
}

/**
 * @brief Initializes timer ? to generate interrupts every 1ms
 * @return	Nothing
 */
void ms_timer_init (void)
{
	unsigned int timerFreq;

	/* Enable timer 1 clock */
	Chip_TIMER_Init (LPC_TIMER0);

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate ();

	/* Timer setup for match and interrupt at 1KHz */
	Chip_TIMER_Reset (LPC_TIMER0);
	Chip_TIMER_MatchEnableInt (LPC_TIMER0, 1);
	//Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE_HZ1));
	/* timer seems to be runninf at timerfreq/2*/
	Chip_TIMER_SetMatch (LPC_TIMER0, 1, (timerFreq / 2000));
	Chip_TIMER_ResetOnMatchEnable (LPC_TIMER0, 1);
	Chip_TIMER_Enable (LPC_TIMER0);

	/* DEBUG ? set up pins*/
#ifdef DEBUG_GPIO
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DEBUG_PORT, DEBUG_PIN);
#endif

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ (TIMER0_IRQn);
	NVIC_EnableIRQ (TIMER0_IRQn);
}

/**
 * @brief resets 1 ms timer, and restarts timer
 * @return	Nothing
 */
void ms_timer_reset (void)
{
	/* clear HW timer*/
	Chip_TIMER_Reset(LPC_TIMER0);
	/* clear ms counter */
	msCount=0;
	/* (re)-start timer periphereal*/
	Chip_TIMER_Enable(LPC_TIMER0);
}

/**
 * @brief Stops timer, but maintains the current count
 * @return	Nothing
 */
void ms_timer_stop (void)
{
	/* stop HW from counting */
	Chip_TIMER_Disable(LPC_TIMER0);
}
/**
 * @brief returns ellapsed milliseconds, can be called while timer is running
 * @return	Nothing
 */
unsigned int ms_timer_read (void)
{
	/* return current ms count*/
	return msCount;
}
/**
 * @brief starts timer, NOTE does NOT reset the counter value
 * @return	Nothing
 */
void ms_timer_start (void)
{
	/* tell HW to start counting*/
	Chip_TIMER_Enable(LPC_TIMER0);
}
