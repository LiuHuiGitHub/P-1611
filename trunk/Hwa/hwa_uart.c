#include <STC15F2K60S2.H>
#include "app_brush.h"
#include "hwa_uart.h"
#include "sys_uart.h"
#include "string.h"
#include "buzzer.h"
#include "led.h"
#include "app_config.h"
#include "mifare.h"
#include "hwa_mifare.h"


#define TX_BUFF_LEN     RX_BUFF_LEN
BOOL comRxFalg = FALSE;
BOOL comTxFlag = FALSE;
data UINT8 comRxLen = 0;
//UINT8 comRxBuff[RX_BUFF_LEN] = {0xAA,0x7,0x20,0x01,0x00,0x30,0x00,0x00,0x16,0xCC};    //添加时间
//UINT8 comRxBuff[RX_BUFF_LEN] = {0xAA,0x7,0x20,0x01,0x00,0x00,0x00,0x00,0x26,0xCC};    //清除时间
data UINT8 comRxBuff[RX_BUFF_LEN] = {0xAA,0x2,0x23,0x21,0xCC};      //读版本
//UINT8 comRxBuff[RX_BUFF_LEN] = {0xAA,0x7,0x20,0x01,0x30,0x00,0x00,0x00,0x16,0xCC};
data UINT8 comTxBuff[TX_BUFF_LEN];

#define MAX_DATA_LEN		10

typedef struct
{
    UINT8 head;
    UINT8 len;
    UINT8 cmd;
    UINT8 dat[MAX_DATA_LEN];
    UINT8 check;
    UINT8 end;
}COM_STD_STRUCT;

typedef struct
{
    UINT8 head;
    UINT8 cmd;
    UINT8 value;
    UINT8 end;
    COM_STD_STRUCT std_dat;
    UINT8 expand;
}COM_TX_STRUCT;

data COM_STD_STRUCT s_comRxBuff;
data COM_TX_STRUCT s_comTxBuff;
code COM_TX_STRUCT c_s_comTxBuff = {
									0xBB,0x00,0x00,0xCC,
									0xAA,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,
									0x00,0xCC,0x00
									};

void hwa_uartInit(void)
{
    sys_uartInit();
}

void hwa_uartHandler1ms(void)
{
	UINT8 i;
    UINT8* p = comRxBuff;
	if(sys_uartOverTime1ms()!=FALSE)
	{
		comRxLen = sys_uartReadData(comRxBuff);
        s_comRxBuff.head = *p++;
        s_comRxBuff.len = *p++;
        s_comRxBuff.cmd = *p++;
        for(i=0; i<s_comRxBuff.len-2; i++)
        {
            if(i < MAX_DATA_LEN)
            {
                s_comRxBuff.dat[i] = *p++;
            }
        }
        for(; i<MAX_DATA_LEN; i++)
        {
            s_comRxBuff.dat[i] = 0x00;
        }
        s_comRxBuff.check = comRxBuff[s_comRxBuff.len+1];
        s_comRxBuff.end = comRxBuff[s_comRxBuff.len+2];
        comRxFalg = TRUE;
	}
}

#define CE_OK                   0
#define CE_ERROR                1
#define CE_CHECK_ERROR          2
#define CE_BAD_COMMAND          3
#define CE_BAD_PARAM            4
#define CE_BAD_PADATA           5

UINT8 comfun_0x00(void)
{
    pMoney->money = 0;
    if(hwa_mifareWriteSector(gBuff, s_System.Sector))
    {
        s_comTxBuff.expand = TRUE;
        s_comTxBuff.std_dat.dat[0] = pMoney->money/100;
        s_comTxBuff.std_dat.dat[1] = pMoney->money%100;
        s_comTxBuff.std_dat.dat[2] = gCard_UID[0];
        s_comTxBuff.std_dat.dat[3] = gCard_UID[1];
        s_comTxBuff.std_dat.dat[4] = gCard_UID[2];
        s_comTxBuff.std_dat.dat[5] = gCard_UID[3];
        s_comTxBuff.std_dat.dat[6] = gCard_UID[4];
        s_comTxBuff.std_dat.len = 9;
        buzzer_SoundNumber(1);
        led_ShowNumber(pMoney->money/100, pMoney->money%100, 1<<3);
        return CE_OK;
    }
    buzzer_SoundNumber(2);
    return CE_ERROR;
}

UINT8 comfun_0x01(void)
{
	s_comTxBuff.expand = TRUE;
    s_comTxBuff.std_dat.dat[0] = pMoney->money/100;
    s_comTxBuff.std_dat.dat[1] = pMoney->money%100;
    s_comTxBuff.std_dat.dat[2] = gCard_UID[0];
    s_comTxBuff.std_dat.dat[3] = gCard_UID[1];
    s_comTxBuff.std_dat.dat[4] = gCard_UID[2];
    s_comTxBuff.std_dat.dat[5] = gCard_UID[3];
    s_comTxBuff.std_dat.dat[6] = gCard_UID[4];
    s_comTxBuff.std_dat.len = 9;
    buzzer_SoundNumber(1);
    led_ShowNumber(pMoney->money/100, pMoney->money%100, 1<<3);
    return CE_OK;
}

UINT8 comfun_0x02(void)
{
    pMoney->money += s_comRxBuff.dat[0]*100;
    if(pMoney->money > 20000)
    {
        buzzer_SoundNumber(2);
        return CE_BAD_PADATA;
    }
    if(hwa_mifareWriteSector(gBuff, s_System.Sector))
    {
        s_comTxBuff.expand = TRUE;
        s_comTxBuff.std_dat.dat[0] = pMoney->money/100;
        s_comTxBuff.std_dat.dat[1] = pMoney->money%100;
        s_comTxBuff.std_dat.dat[2] = gCard_UID[0];
        s_comTxBuff.std_dat.dat[3] = gCard_UID[1];
        s_comTxBuff.std_dat.dat[4] = gCard_UID[2];
        s_comTxBuff.std_dat.dat[5] = gCard_UID[3];
        s_comTxBuff.std_dat.dat[6] = gCard_UID[4];
        s_comTxBuff.std_dat.len = 9;
        buzzer_SoundNumber(1);
        led_ShowNumber(pMoney->money/100, pMoney->money%100, 1<<3);
        return CE_OK;
    }
    else
    {
        buzzer_SoundNumber(2);
        return CE_ERROR;
    }
}


UINT8 comCheck(UINT8* dat, UINT8 len)
{
	UINT8 i, check = 0;
	for(i=0; i<=len; i++)
	{
		check ^= dat[i];
	}
	return check;
}

void hwa_uartHandler10ms(void)
{
	UINT8 i;
    UINT8 error = CE_OK;
    if(comRxFalg)
    {
        comRxFalg = FALSE;
        if(s_comRxBuff.head != 0xAA)
        {
            return;
        }
        comTxFlag = TRUE;
        memcpy((UINT8*)&s_comTxBuff, (UINT8*)&c_s_comTxBuff, sizeof(s_comTxBuff));
		if(s_comRxBuff.len > MAX_DATA_LEN
			|| s_comRxBuff.len < 2)
		{
            error = CE_BAD_PARAM;
		}
        if(s_comRxBuff.head == 0xAA && s_comRxBuff.end == 0xCC)
        {
        	if(comCheck(&s_comRxBuff.len, s_comRxBuff.len) == s_comRxBuff.check)
        	{
        		s_comTxBuff.cmd = s_comRxBuff.cmd;
                
                if(app_brushCard() == USER_CARD)
                {
                    if(s_comRxBuff.cmd == 0x00)
                    {
                        error = comfun_0x00();
                    }
                    else if(hwa_mifareReadSector(gBuff, s_System.Sector))
                    {
                        switch(s_comRxBuff.cmd)
                        {
                            case 0x01:      //Read Money
                                error = comfun_0x01();
                                break;
                            case 0x02:      //Add Money
                                error = comfun_0x02();
                                break;
                            default:        //Cmd Error
                                error = CE_BAD_COMMAND;
                                break;
                        }
                    }
                    else 
                    {
                        error = CE_ERROR;
                    }
                }
                else 
                {
                    error = CE_ERROR;
                }
        	}
            else
            {
                error = CE_CHECK_ERROR;
            }
        }
        else
        {
            error = CE_BAD_PARAM;
        }
        s_comTxBuff.value = error;
    }
    
    if(comTxFlag)
    {
        comTxFlag = FALSE;
        comTxBuff[0] = s_comTxBuff.head;
        comTxBuff[1] = s_comTxBuff.cmd;
        comTxBuff[2] = s_comTxBuff.value;
        comTxBuff[3] = s_comTxBuff.end;
        if(s_comTxBuff.expand)
        {
        	s_comTxBuff.std_dat.cmd = s_comTxBuff.cmd;
	        comTxBuff[4] = s_comTxBuff.std_dat.head;
	        comTxBuff[5] = s_comTxBuff.std_dat.len;
	        comTxBuff[6] = s_comTxBuff.std_dat.cmd;
	        for(i=0; i<s_comTxBuff.std_dat.len-2; i++)
	        {
	        	comTxBuff[7+i] = s_comTxBuff.std_dat.dat[i];
	        }
	        i += 7;
        	s_comTxBuff.std_dat.check = comCheck((UINT8*)&s_comTxBuff.std_dat.len, s_comTxBuff.std_dat.len);
	        comTxBuff[i++] = s_comTxBuff.std_dat.check;
	        comTxBuff[i++] = s_comTxBuff.std_dat.end;
	        
            sys_uartSendData(comTxBuff, 4+2+s_comTxBuff.std_dat.len+1);
        }
        else
        {
            sys_uartSendData(comTxBuff, 4);
        }
    }
}

