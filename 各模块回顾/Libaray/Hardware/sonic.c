#include "sonic.h"

sbit TX = P1^0;
sbit RX = P1^1;

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0暂时不开
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