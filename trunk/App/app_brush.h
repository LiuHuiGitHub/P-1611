#ifndef __APP_BRUSH_H__
#define __APP_BRUSH_H__

#include "typedef.h"

/*�����������ؿ�Ƭ���ͣ�
	0->�޿�
	1->����
	2->���뿨
	3->�û���
	4->����
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

