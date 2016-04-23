#ifndef __HWA_UART_H__
#define __HWA_UART_H__

#include "typedef.h"


#define NONE_BRUSH_CHANNEL      0xFF

extern UINT8 BrushMoneyH;
extern UINT8 BrushMoneyL;
extern UINT8 u8_BrushChannel;
extern UINT16 u16_Channel;
extern BOOL b_PauseBrush;

/*
extern UINT8 u8_BrushCount;
*/
void hwa_uartInit(void);
void hwa_uartHandler1ms(void);
void hwa_uartHandler10ms(void);


#endif

