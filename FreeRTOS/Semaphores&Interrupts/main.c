/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <stdio.h>
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ms_timer.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
}

xSemaphoreHandle binary_sem;	//	Declare gatekeeper

/* thread 1 */
static void thr_1(void *pvParameters) {

	long task_woken = 0;

	while (1)
	{
		while (ms_timer_read () < 1000);	//	Vent 1s før et interrupts genereres!
		xSemaphoreGiveFromISR(binary_sem, &task_woken);
		if(task_woken)
		{
			taskYIELD();
		}
		ms_timer_reset ();					//	Reset Counter
	}
}

/**
 * Task1 ---->
 * 	ISR --> Give Sem
 * 						--> Back to Task1
 */

/* thread 2 */
static void thr_2(void *pvParameters) {

	bool LedState = false;

	while (1)
	{
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;	//	Toggle LED on board

		if(xSemaphoreTake(binary_sem, 9999999)){
			puts("Thread 2 says: Tick!");
		}

		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();
	ms_timer_init ();

	vSemaphoreCreateBinary(binary_sem);	// Install binary semaphore

	/* thread 1 */
	xTaskCreate(thr_1, (signed char *) "thr_1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);

	/* thread 2 */
	xTaskCreate(thr_2, (signed char *) "thr_2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
