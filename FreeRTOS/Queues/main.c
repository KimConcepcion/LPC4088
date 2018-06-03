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
#include "queue.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

typedef struct
{
	unsigned int ID;
	unsigned char ID_ext;
	unsigned char DLC;
	unsigned data[8];
} can_frame;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

xQueueHandle queue_handle;

/** Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
}

/** tx toggle thread */
static void tx(void *pvParameters) {

	can_frame test;
	test.ID = 14;
	test.ID_ext = 0;
	test.data[0] = 'K';
	test.data[1] = 'i';
	test.data[2] = 'm';
	test.DLC = sizeof(test.data);

	while (1) {
		printf("Send telegram with ID 14 to receiver task\n");
		long ok = xQueueSend(queue_handle, &test, 500);
		puts(ok ? "OK" : "FAILED");

		vTaskDelay(200);
	}
}

/** rx toggle thread */
static void rx(void *pvParameters) {

	can_frame test;

	while (1) {
		if(xQueueReceive(queue_handle, &test, 500))
		{
			printf("Received CAN telegram with %d bytes of data:\n", test.DLC);
			printf("ID		:%d\n", test.ID);
			printf("Data [0] : %c\n", test.data[0]);
			printf("Data [1] : %c\n", test.data[1]);
			printf("Data [2] : %c\n", test.data[2]);
		}
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
	printf("Welcome to this freertos example with queues!\n");

	prvSetupHardware();
	queue_handle = xQueueCreate(2, sizeof(can_frame));	//	Install the queue

	/* tx thread */
	xTaskCreate(tx, (signed char *) "tx", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);

	/* rx thread */
	xTaskCreate(rx, (signed char *) "rx", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
