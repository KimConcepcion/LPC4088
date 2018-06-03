
#include <stdio.h>
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "can.h"

xQueueHandle tx_queue;			//	Declare queuehandler for transmitting
xQueueHandle rx_queue;			//	Declare queuehandler for receiving
xSemaphoreHandle binary_sem;	//	Declare gatekeeper

/**
 * GPIO Interrupt handler/ISR
 */
void GPIO_IRQ_HANDLER(void){
	long task_woken = 0;
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 2, 1 << 10);
	xSemaphoreGiveFromISR(binary_sem, &task_woken);
}

/**
 * CAN Interrupt handler
 */
void CAN_IRQHandler(void)
{
	long task_woken = 0;
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);
	PrintCANErrorInfo(IntStatus);

	if(IntStatus & CAN_ICR_RI)	//	React on new message
	{
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		xQueueSendFromISR(rx_queue, &RcvMsgBuf, &task_woken);
	}
}

/**
 * @name TX_task
 * @param pvParameters
 */
static void TX_task(void *pvParameters) {

	CAN_MSG_T test;
	test.ID = 14;
	test.Type = 0;
	test.Data[0] = 'K';
	test.Data[1] = 'i';
	test.Data[2] = 'm';
	test.DLC = sizeof(test.Data);

	while (1)
	{
		if(xSemaphoreTake(binary_sem, 9999999)){
			printf("Button was pressed!\n");
			long ok = xQueueSend(tx_queue, &test, 500);
			puts(ok ? "OK" : "Queue is full!");
		}
	}
}

/**
 * @name RX_task
 * @param pvParameters
 */
static void RX_task(void *pvParameters) {

	CAN_MSG_T test;

	while (1){
		if(xQueueReceive(rx_queue, &test, 500))
		{
			printf("Received data:\n");
			PrintCANMsg(&test);
		}

		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/**
 * @name can_task
 * @param pvParameters
 */
static void can_task(void *pvParameters){
	CAN_MSG_T test;

	while(1)
	{
		if(xQueueReceive(tx_queue, &test, 500))
		{
			DEBUGOUT("Message Sent!!!\r\n");
			PrintCANMsg(&test);
			TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);		//	Find free buffer
			Chip_CAN_Send(LPC_CAN, TxBuf, &test);
		}
	}
}

static char WelcomeMenu[] = "\n\rHello NXP Semiconductors \r\n"
							"CAN DEMO : Use CAN to transmit and receive Message from CAN Analyzer\r\n"
							"CAN bit rate : 125kBit/s\r\n";
/**
 * @name main
 * @return
 */
int main(void){

	prvSetupHardware();
	can_init();

	tx_queue = xQueueCreate(8, sizeof(CAN_MSG_T));		//	Install the tx queue
	rx_queue = xQueueCreate(8, sizeof(CAN_MSG_T));		//	Install the rx queue
	vSemaphoreCreateBinary(binary_sem);					//	Install binary semaphore to use sync mechanisms

	DEBUGOUT(WelcomeMenu);

	/** tx task:*/
	xTaskCreate(TX_task, (signed char *) "TX_task", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);
	/** rx task:*/
	xTaskCreate(RX_task, (signed char *) "RX_task", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);
	/** can task:*/
	xTaskCreate(can_task, (signed char *) "can_task", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), (xTaskHandle *) NULL);

	/** Start the scheduler */
	vTaskStartScheduler();

	return 1;	//	Should never arrive here.
}
