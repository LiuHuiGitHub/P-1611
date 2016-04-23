#include <intrins.h>
#include "normal.h"
#include "reg52.h"
#include "fm1702sl.h"

void Delay_10us(uchar _10us)
{
    while(_10us--)
	{
	    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();		
//		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
//		_nop_();	  //6.79MHz
    }
}

///////////////////////////////////////////////////////////////////////
// Delay 1ms
///////////////////////////////////////////////////////////////////////
void Delay_1ms(uchar _1ms)
{
    while (_1ms--)
    {
	    Delay_10us(100);
    }
}

///////////////////////////////////////////////////////////////////////
// ������ ��һ��
///////////////////////////////////////////////////////////////////////
void Ding(void)
{
	SPEAKER=1;
	Delay_1ms(150);
	SPEAKER=0;
	Delay_1ms(80);
}

/* �ر�IAP���ܣ�����������Ĵ��� */
void IAP_Disable(void)
{
	ISP_CONTR=0;
	ISP_CMD=0;
	ISP_TRIG=0;
	ISP_ADDRH = 0X80;
	ISP_ADDRL = 0X00;
}
/* �������� */
void SectorErase(uint unDataAddr)
{
	uint unAddrTmp;
	unAddrTmp = unDataAddr + DATA_FLASH_START_ADDRESS;

	ISP_ADDRH=(uchar)(unAddrTmp>>8);
	ISP_ADDRL=0;
	ISP_CMD = ISP_IAP_BYTE_ERASE;
	ISP_CONTR = ENABLE_ISP;
	EA=0;
	ISP_TRIG=0x5A;
	ISP_TRIG=0xA5;
	EA=1;
}

/* ��һ�ֽ�*/
uchar ByteRead(uint unDataAddr)
{
	uint unAddrTmp;
	unAddrTmp = unDataAddr + DATA_FLASH_START_ADDRESS;
	
	ISP_ADDRH=(uchar)(unAddrTmp>>8);
	ISP_ADDRL=(uchar)(unAddrTmp&0x00ff);
	ISP_CMD = ISP_IAP_BYTE_READ;
	ISP_CONTR = ENABLE_ISP;
	EA=0;
	ISP_TRIG=0X5A;
	ISP_TRIG=0XA5;
	EA=1;
	return ISP_DATA;
}

/* ���һ���ֽ� */
void ByteProgram(uint unAddr, uchar ucData)
{
	uint unAddrTmp;
	unAddrTmp = unAddr + DATA_FLASH_START_ADDRESS;

	ISP_ADDRH=(uchar)(unAddrTmp>>8);
	ISP_ADDRL=(uchar)(unAddrTmp&0x00ff);
	ISP_CMD = ISP_IAP_BYTE_PROGRAM;
	ISP_CONTR = ENABLE_ISP;
	ISP_DATA = ucData;
	EA=0;
	ISP_TRIG=0x5A;
	ISP_TRIG=0xA5;
	EA=1;
}

void Write_E2PROM8_CardID(void)
{
	SectorErase(0x800);
	ByteProgram(0x800,USER_Card_ID[0][0]);	   //��0·����0λCardID
	ByteProgram(0x801,USER_Card_ID[0][1]);	   //��0·����1λCardID
	ByteProgram(0x802,USER_Card_ID[0][2]);	   //��0·����2λCardID
	ByteProgram(0x803,USER_Card_ID[0][3]);	   //��0·����3λCardID
	ByteProgram(0x804,USER_Card_ID[0][4]);	   //��0·����4λCardID

	ByteProgram(0x805,USER_Card_ID[1][0]);	   //��1·����0λCardID
	ByteProgram(0x806,USER_Card_ID[1][1]);	   //��1·����1λCardID
	ByteProgram(0x807,USER_Card_ID[1][2]);	   //��1·����2λCardID
	ByteProgram(0x808,USER_Card_ID[1][3]);	   //��1·����3λCardID
	ByteProgram(0x809,USER_Card_ID[1][4]);	   //��1·����4λCardID

	ByteProgram(0x80a,USER_Card_ID[2][0]);	   //��2·����0λCardID
	ByteProgram(0x80b,USER_Card_ID[2][1]);	   //��2·����1λCardID
	ByteProgram(0x80c,USER_Card_ID[2][2]);	   //��2·����2λCardID
	ByteProgram(0x80d,USER_Card_ID[2][3]);	   //��2·����3λCardID
	ByteProgram(0x80e,USER_Card_ID[2][4]);	   //��2·����4λCardID

	ByteProgram(0x80f,USER_Card_ID[3][0]);	   //��3·����0λCardID
	ByteProgram(0x810,USER_Card_ID[3][1]);	   //��3·����1λCardID
	ByteProgram(0x811,USER_Card_ID[3][2]);	   //��3·����2λCardID
	ByteProgram(0x812,USER_Card_ID[3][3]);	   //��3·����3λCardID
	ByteProgram(0x813,USER_Card_ID[3][4]);	   //��3·����4λCardID

	ByteProgram(0x814,USER_Card_ID[4][0]);	   //��4·����0λCardID
	ByteProgram(0x815,USER_Card_ID[4][1]);	   //��4·����1λCardID
	ByteProgram(0x816,USER_Card_ID[4][2]);	   //��4·����2λCardID
	ByteProgram(0x817,USER_Card_ID[4][3]);	   //��4·����3λCardID
	ByteProgram(0x818,USER_Card_ID[4][4]);	   //��4·����4λCardID

	ByteProgram(0x819,USER_Card_ID[5][0]);	   //��5·����0λCardID
	ByteProgram(0x81a,USER_Card_ID[5][1]);	   //��5·����1λCardID
	ByteProgram(0x81b,USER_Card_ID[5][2]);	   //��5·����2λCardID
	ByteProgram(0x81c,USER_Card_ID[5][3]);	   //��5·����3λCardID
	ByteProgram(0x81d,USER_Card_ID[5][4]);	   //��5·����4λCardID

	ByteProgram(0x81e,USER_Card_ID[6][0]);	   //��6·����0λCardID
	ByteProgram(0x81f,USER_Card_ID[6][1]);	   //��6·����1λCardID
	ByteProgram(0x820,USER_Card_ID[6][2]);	   //��6·����2λCardID
	ByteProgram(0x821,USER_Card_ID[6][3]);	   //��6·����3λCardID
	ByteProgram(0x822,USER_Card_ID[6][4]);	   //��6·����4λCardID

	ByteProgram(0x823,USER_Card_ID[7][0]);	   //��7·����0λCardID
	ByteProgram(0x824,USER_Card_ID[7][1]);	   //��7·����1λCardID
	ByteProgram(0x825,USER_Card_ID[7][2]);	   //��7·����2λCardID
	ByteProgram(0x826,USER_Card_ID[7][3]);	   //��7·����3λCardID
	ByteProgram(0x827,USER_Card_ID[7][4]);	   //��7·����4λCardID

	ByteProgram(0x828,USER_Card_ID[8][0]);	   //��8·����0λCardID
	ByteProgram(0x829,USER_Card_ID[8][1]);	   //��8·����1λCardID
	ByteProgram(0x82a,USER_Card_ID[8][2]);	   //��8·����2λCardID
	ByteProgram(0x82b,USER_Card_ID[8][3]);	   //��8·����3λCardID
	ByteProgram(0x82c,USER_Card_ID[8][4]);	   //��8·����4λCardID

	ByteProgram(0x82d,USER_Card_ID[9][0]);	   //��9·����0λCardID
	ByteProgram(0x82e,USER_Card_ID[9][1]);	   //��9·����1λCardID
	ByteProgram(0x82f,USER_Card_ID[9][2]);	   //��9·����2λCardID
	ByteProgram(0x830,USER_Card_ID[9][3]);	   //��9·����3λCardID
	ByteProgram(0x831,USER_Card_ID[9][4]);	   //��9·����4λCardID
	IAP_Disable();
}

void Read_E2PROM8_CardID(void)
{
    if(ByteRead(0x800) == 0xff) USER_Card_ID[0][0]=0; else USER_Card_ID[0][0]=ByteRead(0x800);    //��0·����0λCardID
	if(ByteRead(0x801) == 0xff) USER_Card_ID[0][1]=0; else USER_Card_ID[0][1]=ByteRead(0x801);    //��0·����1λCardID
	if(ByteRead(0x802) == 0xff) USER_Card_ID[0][2]=0; else USER_Card_ID[0][2]=ByteRead(0x802);    //��0·����2λCardID
	if(ByteRead(0x803) == 0xff) USER_Card_ID[0][3]=0; else USER_Card_ID[0][3]=ByteRead(0x803);    //��0·����3λCardID
	if(ByteRead(0x804) == 0xff) USER_Card_ID[0][4]=0; else USER_Card_ID[0][4]=ByteRead(0x804);    //��0·����4λCardID

	if(ByteRead(0x805) == 0xff) USER_Card_ID[1][0]=0; else USER_Card_ID[1][0]=ByteRead(0x805);    //��1·����0λCardID
	if(ByteRead(0x806) == 0xff) USER_Card_ID[1][1]=0; else USER_Card_ID[1][1]=ByteRead(0x806);    //��1·����1λCardID
	if(ByteRead(0x807) == 0xff) USER_Card_ID[1][2]=0; else USER_Card_ID[1][2]=ByteRead(0x807);    //��1·����2λCardID
	if(ByteRead(0x808) == 0xff) USER_Card_ID[1][3]=0; else USER_Card_ID[1][3]=ByteRead(0x808);    //��1·����3λCardID
	if(ByteRead(0x809) == 0xff) USER_Card_ID[1][4]=0; else USER_Card_ID[1][4]=ByteRead(0x809);    //��1·����4λCardID

	if(ByteRead(0x80a) == 0xff) USER_Card_ID[2][0]=0; else USER_Card_ID[2][0]=ByteRead(0x80a);    //��2·����0λCardID
	if(ByteRead(0x80b) == 0xff) USER_Card_ID[2][1]=0; else USER_Card_ID[2][1]=ByteRead(0x80b);    //��2·����1λCardID
	if(ByteRead(0x80c) == 0xff) USER_Card_ID[2][2]=0; else USER_Card_ID[2][2]=ByteRead(0x80c);    //��2·����2λCardID
	if(ByteRead(0x80d) == 0xff) USER_Card_ID[2][3]=0; else USER_Card_ID[2][3]=ByteRead(0x80d);    //��2·����3λCardID
	if(ByteRead(0x80e) == 0xff) USER_Card_ID[2][4]=0; else USER_Card_ID[2][4]=ByteRead(0x80e);    //��2·����4λCardID

	if(ByteRead(0x80f) == 0xff) USER_Card_ID[3][0]=0; else USER_Card_ID[3][0]=ByteRead(0x80f);    //��3·����0λCardID
	if(ByteRead(0x810) == 0xff) USER_Card_ID[3][1]=0; else USER_Card_ID[3][1]=ByteRead(0x810);    //��3·����1λCardID
	if(ByteRead(0x811) == 0xff) USER_Card_ID[3][2]=0; else USER_Card_ID[3][2]=ByteRead(0x811);    //��3·����2λCardID
	if(ByteRead(0x812) == 0xff) USER_Card_ID[3][3]=0; else USER_Card_ID[3][3]=ByteRead(0x812);    //��3·����3λCardID
	if(ByteRead(0x813) == 0xff) USER_Card_ID[3][4]=0; else USER_Card_ID[3][4]=ByteRead(0x813);    //��3·����4λCardID

	if(ByteRead(0x814) == 0xff) USER_Card_ID[4][0]=0; else USER_Card_ID[4][0]=ByteRead(0x814);    //��4·����0λCardID
	if(ByteRead(0x815) == 0xff) USER_Card_ID[4][1]=0; else USER_Card_ID[4][1]=ByteRead(0x815);    //��4·����1λCardID
	if(ByteRead(0x816) == 0xff) USER_Card_ID[4][2]=0; else USER_Card_ID[4][2]=ByteRead(0x816);    //��4·����2λCardID
	if(ByteRead(0x817) == 0xff) USER_Card_ID[4][3]=0; else USER_Card_ID[4][3]=ByteRead(0x817);    //��4·����3λCardID
	if(ByteRead(0x818) == 0xff) USER_Card_ID[4][4]=0; else USER_Card_ID[4][4]=ByteRead(0x818);    //��4·����4λCardID

	if(ByteRead(0x819) == 0xff) USER_Card_ID[5][0]=0; else USER_Card_ID[5][0]=ByteRead(0x819);    //��5·����0λCardID
	if(ByteRead(0x81a) == 0xff) USER_Card_ID[5][1]=0; else USER_Card_ID[5][1]=ByteRead(0x81a);    //��5·����1λCardID
	if(ByteRead(0x81b) == 0xff) USER_Card_ID[5][2]=0; else USER_Card_ID[5][2]=ByteRead(0x81b);    //��5·����2λCardID
	if(ByteRead(0x81c) == 0xff) USER_Card_ID[5][3]=0; else USER_Card_ID[5][3]=ByteRead(0x81c);    //��5·����3λCardID
	if(ByteRead(0x81d) == 0xff) USER_Card_ID[5][4]=0; else USER_Card_ID[5][4]=ByteRead(0x81d);    //��5·����4λCardID

	if(ByteRead(0x81e) == 0xff) USER_Card_ID[6][0]=0; else USER_Card_ID[6][0]=ByteRead(0x81e);    //��6·����0λCardID
	if(ByteRead(0x81f) == 0xff) USER_Card_ID[6][1]=0; else USER_Card_ID[6][1]=ByteRead(0x81f);    //��6·����1λCardID
	if(ByteRead(0x820) == 0xff) USER_Card_ID[6][2]=0; else USER_Card_ID[6][2]=ByteRead(0x820);    //��6·����2λCardID
	if(ByteRead(0x821) == 0xff) USER_Card_ID[6][3]=0; else USER_Card_ID[6][3]=ByteRead(0x821);    //��6·����3λCardID
	if(ByteRead(0x822) == 0xff) USER_Card_ID[6][4]=0; else USER_Card_ID[6][4]=ByteRead(0x822);    //��6·����4λCardID

	if(ByteRead(0x823) == 0xff) USER_Card_ID[7][0]=0; else USER_Card_ID[7][0]=ByteRead(0x823);    //��7·����0λCardID
	if(ByteRead(0x824) == 0xff) USER_Card_ID[7][1]=0; else USER_Card_ID[7][1]=ByteRead(0x824);    //��7·����1λCardID
	if(ByteRead(0x825) == 0xff) USER_Card_ID[7][2]=0; else USER_Card_ID[7][2]=ByteRead(0x825);    //��7·����2λCardID
	if(ByteRead(0x826) == 0xff) USER_Card_ID[7][3]=0; else USER_Card_ID[7][3]=ByteRead(0x826);    //��7·����3λCardID
	if(ByteRead(0x827) == 0xff) USER_Card_ID[7][4]=0; else USER_Card_ID[7][4]=ByteRead(0x827);    //��7·����4λCardID

	if(ByteRead(0x828) == 0xff) USER_Card_ID[8][0]=0; else USER_Card_ID[8][0]=ByteRead(0x828);    //��8·����0λCardID
	if(ByteRead(0x829) == 0xff) USER_Card_ID[8][1]=0; else USER_Card_ID[8][1]=ByteRead(0x829);    //��8·����1λCardID
	if(ByteRead(0x82a) == 0xff) USER_Card_ID[8][2]=0; else USER_Card_ID[8][2]=ByteRead(0x82a);    //��8·����2λCardID
	if(ByteRead(0x82b) == 0xff) USER_Card_ID[8][3]=0; else USER_Card_ID[8][3]=ByteRead(0x82b);    //��8·����3λCardID
	if(ByteRead(0x82c) == 0xff) USER_Card_ID[8][4]=0; else USER_Card_ID[8][4]=ByteRead(0x82c);    //��8·����4λCardID

	if(ByteRead(0x82d) == 0xff) USER_Card_ID[9][0]=0; else USER_Card_ID[9][0]=ByteRead(0x82d);    //��9·����0λCardID
	if(ByteRead(0x82e) == 0xff) USER_Card_ID[9][1]=0; else USER_Card_ID[9][1]=ByteRead(0x82e);    //��9·����1λCardID
	if(ByteRead(0x82f) == 0xff) USER_Card_ID[9][2]=0; else USER_Card_ID[9][2]=ByteRead(0x82f);    //��9·����2λCardID
	if(ByteRead(0x830) == 0xff) USER_Card_ID[9][3]=0; else USER_Card_ID[9][3]=ByteRead(0x830);    //��9·����3λCardID
	if(ByteRead(0x831) == 0xff) USER_Card_ID[9][4]=0; else USER_Card_ID[9][4]=ByteRead(0x831);    //��9·����4λCardID
}























