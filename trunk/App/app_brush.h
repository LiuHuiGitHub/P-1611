#ifndef __APP_BRUSH_H__
#define __APP_BRUSH_H__

#include "typedef.h"

/*ËÑË÷¿¨£¬·µ»Ø¿¨Æ¬ÀàÐÍ£¬
	0->ÎÞ¿¨
	1->¹ÜÀí¿¨
	2->ÃÜÂë¿¨
	3->ÓÃ»§¿¨
	4->´íÎó¿¨
	*/
#define NONE_CARD       0
#define MEM_CARD        1
#define USER_CARD       2
#define PWD_CARD        3
UINT8 app_brushCard(void);
void app_Show(void);
void app_brushInit(void);
void app_brushCycle1s(void);

#endif

