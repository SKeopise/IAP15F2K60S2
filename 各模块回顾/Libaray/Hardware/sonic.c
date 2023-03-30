#include "sonic.h"

sbit TX = P1^0;
sbit RX = P1^1;

void Timer0Init(void)		//1����@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x00;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0��ʱ����
}

void sendwave()
{
	u8 i=0;
	for(i=0;i<8;i++){
		TX =1;
		Delay13us();
		TX =0;
		Delay13us();
	}
}

u16 GetDistance()
{
	u16 distance = 0;
	
	TH0 = 0 ;TL0 = 0;TF0 = 0;
	sendwave();
	TR0 = 1;
	while(RX==1 && TF0==0);
	TR0 = 0;
	TF0 = 0;
	distance = (((TH0<<8)|TL0)*0.017); //CM
	
	return distance;
	
}