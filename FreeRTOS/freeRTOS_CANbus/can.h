
#ifndef SRC_CAN_H_
#define SRC_CAN_H_

#include "board.h"

/**
 * Physical CAN parameters:
 */
#define CAN_CTRL_NO         0
#define LPC_CAN             (LPC_CAN1)

#define FULL_CAN_AF_USED    1
#define CAN_TX_MSG_STD_ID (0x80)
#define CAN_TX_MSG_REMOTE_STD_ID (0x300)
#define CAN_TX_MSG_EXT_ID (0x10000200)
#define CAN_RX_MSG_ID (0x100)

//	GPIO Interrupt:
#define GPIO_IRQ_HANDLER  			GPIO_IRQHandler		/* GPIO interrupt IRQ function name */

CAN_BUFFER_ID_T TxBuf;			//	Buffer
typedef struct
{
	unsigned int ID;
	unsigned char ID_ext;
	unsigned char DLC;
	unsigned data[8];
} can_frame;

/** Sets up system hardware */
void prvSetupHardware(void);

//	CAN Prototypes:
void can_init();									//	Initialize can controller 1 and 2
void PrintCANErrorInfo(uint32_t Status);		//	Print error
void PrintCANMsg(CAN_MSG_T *pMsg);					//	Print CAN message


#endif
