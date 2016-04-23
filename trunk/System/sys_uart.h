#ifndef __SYS_UART_H__
#define __SYS_UART_H__

#include "typedef.h"

#define BOUD_OVER_TIME		(10)		//10ms
#define RX_BUFF_LEN         (15)


void sys_uartInit(void);
void sys_uartSendString(UINT8 *s);
void sys_uartSendData(UINT8 *pData, UINT8 len);
UINT8 sys_uartOverTime1ms(void);
UINT8 sys_uartReadData(UINT8 *pData);

#endif
