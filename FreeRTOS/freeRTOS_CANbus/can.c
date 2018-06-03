#include "can.h"

void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	DigitalIn(2, 10);	//	Set pindirection as input
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, 2, 1 << 10);

	NVIC_ClearPendingIRQ(GPIO_IRQn);
	NVIC_EnableIRQ(GPIO_IRQn);
	NVIC_SetPriority(GPIO_IRQn, 5);	//	Since FreeRTOS only can handle priorities between 0-5, GPIO interrupt is given the highest
}

/**
 * @name can_init
 */
void can_init() {

	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(LPC_CAN, 500000);
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

	Board_CAN_Init(LPC_CAN);						//	Initialize CAN channel 1

	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(LPC_CAN, 125000);			//	125 kbit/s on channel 1
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);
	NVIC_EnableIRQ(CAN_IRQn);
}

/**
 * @name PrintCANErrorInfo
 * @param Status
 */
void PrintCANErrorInfo(uint32_t Status)
{
	if (Status & CAN_ICR_EI) {
		DEBUGOUT("Error Warning!\r\n");
	}
	if (Status & CAN_ICR_DOI) {
		DEBUGOUT("Data Overrun!\r\n");
	}
	if (Status & CAN_ICR_EPI) {
		DEBUGOUT("Error Passive!\r\n");
	}
	if (Status & CAN_ICR_ALI) {
		DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n", CAN_ICR_ALCBIT_VAL(Status));
	}
	if (Status & CAN_ICR_BEI) {

		DEBUGOUT("Bus error !!!\r\n");

		if (Status & CAN_ICR_ERRDIR_RECEIVE) {
			DEBUGOUT("\t Error Direction: Transmiting\r\n");
		}
		else {
			DEBUGOUT("\t Error Direction: Receiving\r\n");
		}

		DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(Status));
		DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(Status));
	}
}

/** Print CAN Message */
void PrintCANMsg(CAN_MSG_T *pMsg)
{
	uint8_t i;
	DEBUGOUT("\t**************************\r\n");
	DEBUGOUT("\tMessage Information: \r\n");
	DEBUGOUT("\tMessage Type: ");
	if (pMsg->ID & CAN_EXTEND_ID_USAGE) {
		DEBUGOUT(" Extend ID Message");
	}
	else {
		DEBUGOUT(" Standard ID Message");
	}
	if (pMsg->Type & CAN_REMOTE_MSG) {
		DEBUGOUT(", Remote Message");
	}
	DEBUGOUT("\r\n");
	DEBUGOUT("\tMessage ID :0x%x\r\n", (pMsg->ID & (~CAN_EXTEND_ID_USAGE)));
	DEBUGOUT("\tMessage Data :");
	for (i = 0; i < pMsg->DLC; i++)
		DEBUGOUT("%x ", pMsg->Data[i]);
	DEBUGOUT("Characters:\n");
	for(i = 0; i < pMsg->DLC; i++)
	{
		if((pMsg->Data[i] >= 'a' && pMsg->Data[i] <= 'z') || (pMsg->Data[i] >= 'A' && pMsg->Data[i] <= 'Z'))
			DEBUGOUT("[%c] ", pMsg->Data[i]);
	}

	DEBUGOUT("\r\n\t**************************\r\n");
}
