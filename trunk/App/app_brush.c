#include "app_brush.h"
#include "app_config.h"
#include "app_pulse.h"
#include "sys_eeprom.h"
#include "sys_uart.h"
#include "sys_delay.h"
#include "system.h"
#include "hwa_eeprom.h"
#include "hwa_uart.h"
#include "LED.h"
#include "mifare.h"
#include "buzzer.h"
#include "string.h"
#include "hwa_mifare.h"

code UINT8 PWD_Card[] = { 0xAC, 0x1E, 0x57, 0xAF, 0x19, 0x4E };	//密码卡密码
UINT8 LastCardId[5] = {0};
UINT16 u8_BrushNum = 0;

void app_Show(void)
{
    if(!b_FactorySystem)
    {
        sys_delayms(1000);
        led_ShowNumber(MoneySum/100, MoneySum%100, 0);
        sys_delayms(1000);
        gLedBuf[0] = 12;
        gLedBuf[1] = 15;
        gLedBuf[2] = 15;
        gLedBuf[3] = 15;
        gLedBuf[4] = 15;
        gLedBuf[5] = 12;
        gShowDot = 0;
        sys_delayms(1000);
    }
}

void app_brushInit(void)
{
	Init_FM1702();
}

void app_brushHandler100ms(void)
{
}


UINT8 app_brushCard(void)
{
	UINT8 Sector;
	UINT8 CardIndex;
    UINT8 i;
	for (CardIndex = MEM_CARD; CardIndex <= PWD_CARD; CardIndex++)
	{
		if (b_FactorySystem)
		{
			CardIndex = PWD_CARD;
		}
		if (CardIndex == MEM_CARD)
		{
			Load_Key(&s_System.MGM_Card);
		}
		else if (CardIndex == USER_CARD)
		{
			Load_Key(&s_System.USER_Card);
		}
		else if (CardIndex == PWD_CARD)
		{
			Load_Key(PWD_Card);
		}
		MIF_Halt();
		if (Request(RF_CMD_REQUEST_STD) != FM1702_OK)
		{
			continue;
		}
        for(i=0; i<2; i++)
        {
            if (AntiColl() == FM1702_OK && SelectCard() == FM1702_OK)
            {
                if (CardIndex == USER_CARD)     //用户卡验证钱所在扇区
                {
                    Sector = s_System.Sector;
                }
                else                            //管理和密码卡验证1扇区
                {
                    Sector = 1;
                }
                if (Authentication(gCard_UID, Sector, 0x60) == FM1702_OK)
                {
                    return CardIndex;
                }
            }
        }
	}
	return NONE_CARD;
}

UINT8 LastCard = NONE_CARD;

#define u8_First_Brush_Card_Dly     3
UINT8 u8_FirstBrushCardDly = 0;
BOOL b_SettingChange = FALSE;

void app_brushCycle1s(void)
{
    switch (app_brushCard())
    {
        case MEM_CARD:
            break;
//            if(hwa_mifareReadBlock(gBuff,4))
//            {
//                if(u8_FirstBrushCardDly)
//                {
//                    if(gBuff[0]==0x01 && gBuff[1]==0x0A)
//                    {
//                        s_System.Money += 1000;
//						if (s_System.Money > 20000)
//						{
//							s_System.Money = 0;
//						}
//                        s_System.Money -= s_System.Money%1000;
//                    }
//                    else if(gBuff[0]==0xFA && gBuff[1]==0x01)
//                    {
//                        if(s_System.Money<20000)
//                        {
//                            s_System.Money += 100;
//                            if (s_System.Money % 1000 == 0 && s_System.Money/1000>0)
//                            {
//                                s_System.Money -= 1000;
//                            }
//                        }
//                    }
//                }
//                led_ShowNumber1(s_System.Money/100, u8_BrushNum, 0);
//                u8_FirstBrushCardDly = u8_First_Brush_Card_Dly;
//                buzzer_SoundNumber(1);
//                b_SettingChange = TRUE;
//            }
//            break;
            
        case PWD_CARD:                                 //从初始卡中读取管理卡密码，并储存至E2
            if(hwa_mifareReadBlock(gBuff,4))       //读取管理卡密码
            {
                memcpy(&s_System, gBuff, 16);
                s_System.Money = 0;

	            if (hwa_mifareReadBlock(gBuff, 5))			//读取管理卡和用户卡密码以及扇区
	            {
	                if(gBuff[0] == 0x01)
	                {
	                    s_System.RecoveryOldCard = 1;
	                }
	                else
	                {
	                    s_System.RecoveryOldCard = 0;
	                }
	            }
	            else
	            {
	                break;
	            }
                app_configWrite(SYSTEM_SETTING_SECTOR);
                buzzer_SoundNumber(1);
                b_FactorySystem = FALSE;
            }
            break;
            
        case USER_CARD:
            
            if(hwa_mifareReadSector(gBuff, s_System.Sector))
            {
                led_ShowNumber(pMoney->money/100, pMoney->money%100, 1<<3);
                u8_FirstBrushCardDly = 3;
            }
            break;
//            pMoney->money = 1000;
//            hwa_mifareWriteSector(gBuff, s_System.Sector);
//            break;
//            if(hwa_mifareReadSector(gBuff, s_System.Sector))
//            {
//                if(b_SettingChange == FALSE)
//                {
//                    led_ShowNumber(pMoney->money/100, pMoney->money%100, 1<<3);
//                    u8_FirstBrushCardDly = u8_First_Brush_Card_Dly;
//                    buzzer_SoundNumber(1);
//                }
//                else
//                {
//                    if ((pMoney->money+s_System.Money <= 20000
//                        && memcmp(LastCardId, gCard_UID, 5))
//                        || s_System.Money == 0)
//                    {
//                        if(s_System.Money == 0)
//                        {
//                            pMoney->money = 0;
//                            if(hwa_mifareWriteSector(gBuff, s_System.Sector))
//                            {
//                                buzzer_SoundNumber(1);
//                                led_ShowNumber1(pMoney->money/100, u8_BrushNum, 0);
//                                u8_FirstBrushCardDly = u8_First_Brush_Card_Dly;
//                                break;
//                            }
//                            else
//                            {
//                                buzzer_SoundNumber(2);
//                            }
//                        }
//                        else
//                        {
//                            pMoney->money += s_System.Money;
//                            if(hwa_mifareWriteSector(gBuff, s_System.Sector))
//                            {
//                                MoneySum += s_System.Money/100;
//                                u8_BrushNum++;
//                                app_configWrite(MONEY_SECTOR);
//                                led_ShowNumber1(pMoney->money/100, u8_BrushNum, 0);
//                                u8_FirstBrushCardDly = u8_First_Brush_Card_Dly;
//                                buzzer_SoundNumber(1);
//                            }
//                        }
//                    }
//                    else if(pMoney->money+s_System.Money > 20000)
//                    {
//                        buzzer_SoundNumber(2);
//                    }
//                }
//                memcpy(LastCardId, gCard_UID, 5);
//            }
            break;
            
        default:
            break;
    }
    
    if(u8_FirstBrushCardDly)
    {
        u8_FirstBrushCardDly--;
    }
    else if(!b_FactorySystem)
    {
        if(b_SettingChange)
        {
            led_ShowNumber1(s_System.Money/100, u8_BrushNum, 0);
        }
        else
        {
            gLedBuf[0] = 12;
            gLedBuf[1] = 15;
            gLedBuf[2] = 15;
            gLedBuf[3] = 15;
            gLedBuf[4] = 15;
            gLedBuf[5] = 12;
            gShowDot = 0;
        }
    }
}

