#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "typedef.h"

typedef struct
{
    UINT8 MGM_Card[6];
    UINT8 Sector;       //Ǯ��������
    UINT8 PulseWidth;
    UINT8 Reserved[2];
    UINT8 USER_Card[6];
    UINT8 RecoveryOldCard;          //1->���ݾɿ�
	UINT16 Money;                  //ˢ���۳�����λ��
}SYSTEM_STRUCT;
extern BOOL b_FactorySystem;
extern SYSTEM_STRUCT s_System;
extern code SYSTEM_STRUCT s_FactorySystem;

extern UINT32 MoneySum;               //Ӫҵ����������

#define SYSTEM_SETTING_SECTOR               0
#define MONEY_SECTOR                        1


void app_configInit(void);
BOOL app_configRead(UINT8 Sector);
void app_configWrite(UINT8 Sector);
void app_confingHandler1s(void);

#endif

