
//=============================2014-08-21========================================

//U3.1�汾Ϊ����խˢ���壬����ֱ�����Ͷ������װ����ˢ������¼�������Ѿ����˷���ˢ�������ݿ����Զ��ָ���

#include "reg52.h"
#include "normal.h"
#include "mifare.h"
#include "LED.h"
#include "fm1702sl.h"
#include "string.h"
#include "UART.h"

//bit AutoBrush;
bit Brushed;	 //�����жϵ�ǰ�Ƿ���м۸��С��֡�������
uchar gPrice[2];   //gPrice[1]��λΪԪ��gPrice[0]��λΪ�֣�0~99��
//uchar gPlusWidth;
unsigned int Timer_Buff;  //�ݴ��������ص�ͨ��ʱ��
unsigned int SK_Timer;	  //ˢ���ĵ�λʱ�䣬�����˿�
unsigned char Channel = 0x0f;
unsigned char USER_Card_ID[10][5];
unsigned char RevBuffer[40];  

/*
*	id: 	������RC500 �е�����
*	ucType:	����ִ�����ͣ�
*			0   ��gBuff[0]~gBuff[5]�ڵ��������
*			1   ��gBuff[0]~gBuff[5]��gBuff[10]~gBuff[15]�����������
*/
uchar ChangePWD(uchar id,uchar ucType)
{
	uchar mm0[6];
	uchar mm1[6];
    //1CB533FBAFC6
	mm0[0]=gBuff[0];mm0[1]=gBuff[1];mm0[2]=gBuff[2];mm0[3]=gBuff[3];mm0[4]=gBuff[4];mm0[5]=gBuff[5];
	mm1[0]=gBuff[10];mm1[1]=gBuff[11];mm1[2]=gBuff[12];mm1[3]=gBuff[13];mm1[4]=gBuff[14];mm1[5]=gBuff[15];
	if(ucType==0)
		return Load_Key(id,mm0);
	else if(Load_Key(id,mm0)||Load_Key(id+1,mm1))
		  return 1;
	return 0;
}


	//�ȴ�ˢ��	
void WaitingBrush(void)
{
	unsigned char i,j;
	do
	{
		WDT_CONTR = 0x3E;
		MIF_Halt();
		Request(RF_CMD_REQUEST_STD);
		AntiColl();
		if(SelectCard()==FM1702_OK)
			return;
		else if(Brushed ==1)
		{
			Brushed = 0;
			Show(0,0);
		}
		else
		{
			if(RxD_Buffer == 0x55)	 //0x55��ʾָ�����
			{
				RxD_Buffer = 0;
				SendData(0x00);		 //��û��ˢ����״̬�£����͸�����0x00��
			}
			else if((RxD_Buffer>>4) == 6) //RxD_Buffer��4λΪ6����4λ��ͨ����,���俨��ID��
			{
				if((RxD_Buffer!=0x6f)&&(RxD_Buffer!=0x6e)) Channel = RxD_Buffer&0x0f;
				else
				{
					if(RxD_Buffer==0x6f)
					{
						for(i=0;i<5;i++){USER_Card_ID[Channel][i]=gCard_UID[i];}//Last_gCard_UID[i]=0;
						Write_E2PROM8_CardID();	 //ÿ��ͨĳ��ͨ������дE2PROMһ�Σ���¼�¸ÿ��ŵ�id
						Delay_1ms(200);
						Delay_1ms(200);
						Delay_1ms(200);
						Delay_1ms(200);
					}
					Channel = 0x0f;
				}
			}
			else if(RxD_Buffer == 0x77)		 //0x77���ܴ����ʾ��ˢ����λʱ��ķ���
			{
				Delay_1ms(100);
				SendData(0x33);	//��Чָ��
				Delay_1ms(100);
				SK_Timer = RxD_Buffer*100;
				RxD_Buffer = 0;
				SendData(0x33);	//��Чָ��
				Delay_1ms(100);
				SK_Timer += RxD_Buffer;
				RxD_Buffer = 0;
				SendData(0x33);	//��Чָ��
				Delay_1ms(100);

				//Show(SK_Timer,0);
				SectorErase(0x600);
				ByteProgram(0x600,(SK_Timer/100));
				ByteProgram(0x601,(SK_Timer%100));
				IAP_Disable();
			}
			else if((RxD_Buffer>>4) == 8)
			{
				Delay_1ms(100);
				SendData(0x33);	//��Чָ��
				for(i=0;i<5;i++){USER_Card_ID[RxD_Buffer&0x0f][i]=0;} 
				Write_E2PROM8_CardID();	 
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
			}
			else if(RxD_Buffer == 0x99)
			{
				for(j=0;j<10;j++){for(i=0;i<5;i++){USER_Card_ID[j][i]=0;}}
				Write_E2PROM8_CardID();	 
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
			}
		}
	}while(1);
}

/*
*	Ѱ����ʹ��A��B�뽨��ͨѶ
*	ucField:	Ҫ���ж�д��������
*	ucBlock:	Ҫ���ж�д�����ݿ�
*	ucKey:		��Ҫ��֤������ ����λ�ж�  
*	�ɹ������ؿ������ͣ�0 ����   1 �û���    2 ���뿨   3 ���п�
*	ʧ�ܣ�����0XFF
*/
uchar InitCard(uchar ucField,uchar ucBlock,uchar ucKey)
{
	uchar i;
	
	/*ʹ��A�������Ѱ����֤*/
	for(i=0;i<4;i++)
	{
		if((ucKey>>i) & 0x01)
		{
			WaitingBrush();
			Load_Key_EE(i);
			if(Authentication(gCard_UID,ucField,0x60) ==FM1702_OK) //��֤ͨ�������ж�����
			{
				if(Read_Block(gBuff,ucField*4+ucBlock)==FM1702_OK)//������ͨ�����򷵻أ����ٽ���B������֤
					return i;
				break;
			}
		}
	}
	/*ʹ��B�������Ѱ����֤*/
	for(i=0;i<4;i++)
	{
		if((ucKey>>i) & 0x01)
		{
			WaitingBrush();
			Load_Key_EE(i);
			if(Authentication(gCard_UID,ucField,0x61) ==FM1702_OK) //��֤ͨ�������ж�����
			{
				if(Read_Block(gBuff,ucField*4+ucBlock)==FM1702_OK)//������ͨ�����򷵻أ����ٽ���B������֤
					return i;
				break;
			}
		}
	}
	return 0xFF;
}

/*****************************************************�ж��û��������*******************************************************/
unsigned char USER_Card_Check(void)
{
	unsigned char i;
	for(i = 0;i < 10;i++)
	{
		if((USER_Card_ID[i][0]==gCard_UID[0])&&(USER_Card_ID[i][1]==gCard_UID[1])&&(USER_Card_ID[i][2]==gCard_UID[2])&&(USER_Card_ID[i][3]==gCard_UID[3])&&
		   (USER_Card_ID[i][4]==gCard_UID[4])) 
		{
			return i;
		}
	}
	return 0x55;
}


void Init(void)
{
	P1M1 = 0;
	P1M0 = 0xFF;
	P3M1 = 0;
	P3M0 = 0x20;
	UART_INIT();          //���ڳ�ʼ��
	Timer0_initialize();
	//gPlusWidth = 0;
	while(Init_FM1702()==false);
	SPEAKER=0;           //�رշ�����
//	Ding();
}
//=======================================================
//	����: main
//	˵��: ��������
//FLASH��0�����׵�ַΪ0��0��1���ݴ洢ucPrice��Ԫ���֣�,��ʾ�������ѽ��
//		1�����׵�ַΪ0x200 ��0���ݴ洢ucWorkState����ʾ����״̬��Ϊ0xFFʱִ��������³���
//		2�����׵�ַΪ0x400:0,1,2,3���ݴ洢��ʷ���Ѽ�¼��ucTotal[4]
//		  ���Ѽ�¼ = ucTotal[2]*10000+ucTotal[1]*100+ucTotal[0]+ucTotal[4]/100 (Ԫ)
//=======================================================
void main(void)
{
	bit Back_Menoy_Flag = FALSE;
	uchar i;
	uchar Channel_Value;
	uchar ucTmp,ucPrice[2];
	uchar ucWorkState;
    unsigned int Total_Money[2]={0}; //Total_Money[0]:��λԪ(�洢���ݷ�Χ9999~0)��Total_Money[1]:��λ��(�洢���ݷ�Χ9999~0)��
	uint unMoney;
	Init();		  //�˿ڳ�ʼ��
	
	/***********************************E2PROM��ȡ**********************************/
	ucPrice[0] = ByteRead(0); if(ucPrice[0] == 0xff) ucPrice[0] = 0;
	ucPrice[1] = ByteRead(1); if(ucPrice[1] == 0xff) ucPrice[1] = 0;
	ucWorkState= ByteRead(0x200);
	Total_Money[0] = ByteRead(0x400)*100+ByteRead(0x401);if(ByteRead(0x400) == 0xff)	Total_Money[0] = 0;
	Total_Money[1] = ByteRead(0x402)*100+ByteRead(0x403);if(ByteRead(0x402) == 0xff)	Total_Money[1] = 0;
	SK_Timer = ByteRead(0x600)*100+ByteRead(0x601);
	if(ByteRead(0x600) == 0xff)	SK_Timer = 200;
	Read_E2PROM8_CardID();
	IAP_Disable();
	/*******************************************************************************/
	switch(ucWorkState)
	{
	case 0xFF://200905052154(������ʼ�����뿨����)
		Show(ucWorkState,0);
		gBuff[0]=0x20;gBuff[1]=0x09;gBuff[2]=0x05;gBuff[3]=0x05;gBuff[4]=0x21;gBuff[5]=0x54;
		if(ChangePWD(2,0))
		{
			Ding();
			Ding();
		}
		while(1)//�ȴ�ˢ��ʼ���뿨д��Ĭ�Ϲ�����Ĭ���û������û����뿨ͨѶ����
		{
			if(InitCard(1,0,0x04)==2)
			{
				if(Read_Block(gBuff,0x4)!=FM1702_OK || ChangePWD(0,1))
				{
					Ding();
					Ding();
				}
				else if(Read_Block(gBuff,0x5)!=FM1702_OK)
				{
					Ding();
					Ding();
				}
				else
				{
					ucWorkState = 1;	 //Ĭ��Ǯ��ʹ�÷���1
					if(ChangePWD(2,0))
					{
						Ding();
						Ding();
					}
					else
					{
						Ding();
						SectorErase(0x200);
						ByteProgram(0x200,ucWorkState);
						IAP_Disable();
						Delay_1ms(200);
						Delay_1ms(200);
						break;
					}
				}
			}
		}
		break;
	default:
		break;	
	}
	//��ʾ�汾��
	gShowDot=0;
	gLedBuf[0]=12;
	gLedBuf[1]=12;
	gLedBuf[2]=12;
	gLedBuf[3]=10;
	gLedBuf[4]=13;
	gLedBuf[5]=2;
	gShowDot=0;
	Delay_1ms(200);
	Delay_1ms(200);

//	Delay_1ms(200);
//	Delay_1ms(200);
//	Delay_1ms(200);
//	Delay_1ms(200);

	//��̨���������ѽ��
	Show(Total_Money[0],Total_Money[1]/100);
	Delay_1ms(200);
	Delay_1ms(200);
//	Delay_1ms(200);
//	Delay_1ms(200);
//	Delay_1ms(200);
//	Delay_1ms(200);
	Ding();
	Show(0,0);
	gShowDot=1;
	Brushed = 0;
	while (1)
	{
		switch (InitCard(1,0,0x04))
		{
		case 2:	  //�û����뿨
			Brushed = 0;
			if(Read_Block(gBuff,0x4)!=FM1702_OK)
			{
				Ding();
				Ding();
			}
			else
			{ 
				if(gBuff[6] == 0x10) //˵�������뿨����������Ǯ�������ģ�������Ϣ��gBuff[7]�洢
				{
					ucWorkState = gBuff[7];
					SectorErase(0x200);
					ByteProgram(0x200,ucWorkState);
					IAP_Disable();
					Delay_1ms(2);
					Show(ucWorkState,0);
					Ding();
					Delay_1ms(200);
					Delay_1ms(200);
					Delay_1ms(200);
					Delay_1ms(200);
					Show(0,0);
				}
				else if(ChangePWD(0,1))
				{
					Ding();
					Ding();
				}
				else
					Ding();
			}
			break;
		case 0xff: //�����û����뿨�����ж��Ƿ�������û���
			switch (InitCard(ucWorkState,0,0x03))
			{
			case 0:	  //����
				ucTmp=Read_Block(gBuff,ucWorkState*4);
				if(gBuff[0] == 0xF0)  //�趨������
				{
					Brushed = 0;
					gShowDot=0;
					Show(Total_Money[0],Total_Money[1]/100);
				}
				else
				{
					Brushed = 0;
					
					if(gBuff[0] == 0xfa)	 //��λ����
					{
						ucPrice[1]+=10;

						if(ucPrice[1] >= 100)
						{
							ucPrice[1] = 0;
						}
					}
					if(gBuff[0] == 0x01)	 //��λ��Ԫ
					{
						ucPrice[0]++;
						if(ucPrice[0] >= 6)  //����10Ԫ����ߵ��ۣ���������
						{
						    ucPrice[0] = 0;
						}
					}
					SectorErase(0);
					ByteProgram(0,ucPrice[0]);
					ByteProgram(1,ucPrice[1]);
					//ByteProgram(2,gPlusWidth);
					IAP_Disable();
					Show(ucPrice[0],ucPrice[1]);
				}
				Ding();
				Delay_1ms(250);
				Delay_1ms(250);
				if(	Brushed == 0)
				{	
					Delay_1ms(250);
					Delay_1ms(250);
					Delay_1ms(250);
					Delay_1ms(250);
					Delay_1ms(250);
					Delay_1ms(250);
					Show(0,0);
					gShowDot=1;
				}
				break;
			case 1:	  //�û���
				Brushed = 0;
				ucTmp=Read_Block(gBuff,ucWorkState*4+1);
				if( gBuff[1] == 0xFF) gBuff[1] = 0;
				if (ucTmp!=FM1702_OK || ucPrice[0]==0xff || gBuff[0]*100+gBuff[1]<ucPrice[0]*100+ucPrice[1])
				{
					Ding();
					Ding();
					break;
				}
                if(gBuff[0]>200)
                {
				    ucTmp=Read_Block(gBuff,ucWorkState*4+2);
					if( gBuff[1] == 0xFF) gBuff[1] = 0;
				    if (ucTmp!=FM1702_OK || ucPrice[0]==0xff || gBuff[0]*100+gBuff[1]<ucPrice[0]*100+ucPrice[1]||gBuff[0]>200)
				    {
					    Ding();
					    Ding();
					    break;
				    }
               	}
				/************************************����**********************************/
				Channel_Value = USER_Card_Check(); //
				if(Channel_Value != 0x55) 
				{
					while(RxD_Buffer != 0x55);     //�ȴ�����ָ��
				    SendData(0x20+Channel_Value);  //�������ܵ�
					Delay_1ms(50);
					Timer_Buff = RxD_Buffer*100;
					SendData(0x33);	//��Чָ��
					Delay_1ms(50);
					Timer_Buff += RxD_Buffer;
					SendData(0x33);	//��Чָ��
					unMoney = gBuff[0]*100+gBuff[1] + ((ucPrice[0]*10+ucPrice[1]/10)*Timer_Buff)/(SK_Timer/10)-1;//�����SK_Timer����ʱ�䣬��λ����
					if((((ucPrice[0]*10+ucPrice[1]/10)*Timer_Buff)/(SK_Timer/10)-1)<Total_Money[1])
						Total_Money[1] -= ((ucPrice[0]*10+ucPrice[1]/10)*Timer_Buff)/(SK_Timer/10)-1;
					Back_Menoy_Flag = TURE; 
//					if((Channel>=0)&&(Channel<=9));
//					else	
					Channel = 0x0f;
				}
				/**************************************************************************/
				else 
				{	
					Back_Menoy_Flag = FALSE;
					if((Channel>=0)&&(Channel<=9))unMoney = gBuff[0]*100+gBuff[1] - ucPrice[0]*100-ucPrice[1];
					else unMoney = gBuff[0]*100+gBuff[1];
				}
				gBuff[1] = unMoney%100;
				gBuff[0] = unMoney/100;
				
				ucTmp=Write_Block(gBuff,ucWorkState*4+2);
				ucTmp=Write_Block(gBuff,ucWorkState*4+1);
				if (ucTmp!=FM1702_OK)
				{
					if(ucTmp == 1)
					{
						Ding();
					    Ding();
					    break;
					}						
				}
				
				Show(gBuff[0],gBuff[1]);
				if(!Back_Menoy_Flag) 
				{
					if((Channel>=0)&&(Channel<=9))
					{
						Total_Money[1] += (ucPrice[0]*100+ucPrice[1]);	      //��λ����
						if(Total_Money[1]>9999){Total_Money[1] -= 9999;Total_Money[0]++;}	
					}
				}
				else 
				{
					for(i=0;i<5;i++)USER_Card_ID[Channel_Value][i]=0;
					SendData(0x33);	         //����䷢�͸������������˿�ɹ������԰�ʱ��������
					Write_E2PROM8_CardID();	 //ÿ�����ĳ��ͨ����ID���ţ���дE2PROMһ��
				}
				if((Total_Money[0] >= 9999)&&((Total_Money[1]/100) >= 99))	 //��߼�¼999999��,���ﵽ999999ʱ���������㣬��ΪҪ���ǵ��˿�����
				{
					Total_Money[0] = 0;
					Total_Money[1] = 0;
				}
				SectorErase(0x400);
				ByteProgram(0x400,Total_Money[0]/100);
				ByteProgram(0x401,Total_Money[0]%100);
				ByteProgram(0x402,Total_Money[1]/100);
				ByteProgram(0x403,Total_Money[1]%100);
				IAP_Disable();
				if(!Back_Menoy_Flag)
				{
					if((Channel>=0)&&(Channel<=9))
					{
						while(RxD_Buffer != 0x55);//�ȴ�����ָ��
						RxD_Buffer = 0;
						SendData(0x11);	 //�������ܵ�0x11���ۼ�һ��ʱ��
						//for(i=0;i<5;i++)Last_gCard_UID[i]=gCard_UID[i];
					}
				}
				Ding();
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Delay_1ms(200);
				Show(0,0);
				//��������ucPrice��ֵ�����ȷ�����巢����ˢ�����ϵ磬����Բ�ִ��
				ucPrice[0] = ByteRead(0);
				ucPrice[1] = ByteRead(1);
				IAP_Disable();
				break;
			case 0xff:
				Ding();
				Ding();
				break;
			default:
				break;
			}
				break;
		default:
			break;
		}
	}	  
}