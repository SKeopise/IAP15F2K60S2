#include "system.h"

#define TEMP    0
#define TIME    1
#define EEPROM  2
#define ADC     3
#define DAC     4
#define SONIC   5
u8 display = TEMP;

//串口接受到标志
bit sendflag = 0;
char cmd[12];
//系统初始化
void SystemProcess()
{
	DeviceCtrl(0x80,0xff);//led
	DeviceCtrl(0xa0,0x00);//buzz and relay
	
}
//SONIC测距
u8 sonicnt = 0;
u16 distance =0 ;
char disbuf[10];
void SonicProcess()
{
	if(sonicnt>200){
		sonicnt = 0;
		distance = GetDistance();
	}
}
//DAC输出
u8 dacnt = 0;
u8 dacval = 220;
u16 dacdis = 0;
void DacProcess()
{
	if(dacnt>180){
		dacnt = 0;
		Dacout(dacval);
		dacdis = (u16)(100*((float)dacval/51));
	}
}
//获取ADC采样
u8 adcnt = 0 ;
u16 ch1 = 0;//光敏
u16 ch3 = 0;//电压
bit adcmode = 0;
void AdcProcess()
{
	if(adcnt>100){
		adcnt = 0;
		ReadAdc(0x43);													//空读一次 ，转换是上一次的么
		ch3  = (((float)ReadAdc(0x43)/51)*100);	//电位器
		ReadAdc(0x41);													//空读一次 ，转换是上一次的么
		ch1  = (((float)ReadAdc(0x41)/51)*100);	//光敏电阻
	}
}
//EEPROM记录开机次数
u8 opennum = 0;
//获取时间函数
u8 timecnt =0 ;
u8 hour,min,sec;
void TimeProcess()
{
	if(timecnt>100){
		timecnt = 0;
		hour = BcdToDec(Read_Ds1302_Byte(0x85));
		min  = BcdToDec(Read_Ds1302_Byte(0x83));
		sec  = BcdToDec(Read_Ds1302_Byte(0x81));
	}
}
//温度读取
u8 tempcnt = 0;
u16 temp = 0;
void TempProcess()
{
	if(tempcnt>100){
		tempcnt = 0;
		temp = (u16)(ReadTemp()*10);//一位小数
		if(temp==850){
			temp =230;
		}
	}
}
// 独立按键获取程序
u8 keycnt = 0 ;
void BtnProcess()
{
	if(keycnt>15){
		keycnt = 0;
		BtnThree();
		if(Btrg==0x08){
			display = (display+1)%6;
		}
		if(Btrg==0x01){
			memset(disbuf,'\0',10);
			sprintf(disbuf,"距离是:%d",distance);
			SendString(disbuf);
			SendString("\r\n");
		}
		if(Bcont==0x04){//s5 长按
			if(display==ADC){
				adcmode = 1;
			}
		}
		if(Btrg==0x00&&Bcont==0x00){//弹起检测
			if(display==ADC){
					adcmode = 0;
				}
		}
		if(display==DAC){
			if(Btrg==0x02){
				dacval = dacval + 20; //因为借助按键发送Sonic数据暂时不开
			}
			if(Btrg==0x01){
			//	dacval = dacval - 20;	//因为借助按键发送Sonic数据暂时不开
			}
		}
	}	
}
//数码管缓存函数
void SmgProcess()
{
	if(display==TEMP){
		smgbuf[0] = smgcode[1];
		smgbuf[1] = smgcode[10];
		smgbuf[2] = smgcode[10];
		smgbuf[3] = smgcode[10];
		smgbuf[4] = smgcode[10];
		smgbuf[5] = smgcode[temp/100%10];
		smgbuf[6] = smgcode[temp/10%10]|0x80;
		smgbuf[7] = smgcode[temp%10];
	}else if(display==TIME){
		smgbuf[0] = smgcode[hour/10%10];
		smgbuf[1] = smgcode[hour%10];
		smgbuf[2] = 0x40;
		smgbuf[3] = smgcode[min/10%10];
		smgbuf[4] = smgcode[min%10];
		smgbuf[5] = 0x40;
		smgbuf[6] = smgcode[sec/10%10];
		smgbuf[7] = smgcode[sec%10];
	}else if(display==EEPROM){
		smgbuf[0] = smgcode[3];
		smgbuf[1] = smgcode[10];
		smgbuf[2] = smgcode[10];
		smgbuf[3] = smgcode[10];
		smgbuf[4] = smgcode[10];
		smgbuf[5] = smgcode[opennum/100%10];
		smgbuf[6] = smgcode[opennum/10%10];
		smgbuf[7] = smgcode[opennum%10];
	}else if(display==ADC){
		if(adcmode==0){
				smgbuf[0] = smgcode[4];
				smgbuf[1] = smgcode[10];
				smgbuf[2] = smgcode[10];
				smgbuf[3] = smgcode[10];
				smgbuf[4] = smgcode[10];
				smgbuf[5] = smgcode[ch1/100%10]|0x80;
				smgbuf[6] = smgcode[ch1/10%10];
				smgbuf[7] = smgcode[ch1%10];
			}else{
				smgbuf[0] = smgcode[4];
				smgbuf[1] = smgcode[10];
				smgbuf[2] = smgcode[10];
				smgbuf[3] = smgcode[10];
				smgbuf[4] = smgcode[10];
				smgbuf[5] = smgcode[ch3/100%10]|0x80;
				smgbuf[6] = smgcode[ch3/10%10];
				smgbuf[7] = smgcode[ch3%10];
			}
	}else if(display==DAC){
		smgbuf[0] = smgcode[5];
		smgbuf[1] = smgcode[10];
		smgbuf[2] = smgcode[10];
		smgbuf[3] = smgcode[10];
		smgbuf[4] = smgcode[10];
		smgbuf[5] = smgcode[dacdis/100%10]|0x80;
		smgbuf[6] = smgcode[dacdis/10%10];
		smgbuf[7] = smgcode[dacdis%10];
	}else if(display==SONIC){
		smgbuf[0] = smgcode[6];
		smgbuf[1] = smgcode[10];
		smgbuf[2] = smgcode[10];
		smgbuf[3] = smgcode[10];
		smgbuf[4] = smgcode[10];
		smgbuf[5] = smgcode[distance/100%10];
		smgbuf[6] = smgcode[distance/10%10];
		smgbuf[7] = smgcode[distance%10];
	}
}
//主函数
void main()
{	
	SystemProcess();// 额 这个放在 while1 之前啊 	
	Timer2Init();
	SetClock(23,59,53);
	opennum = ReadEeprom(0x00);
	WriteEeprom(0x00,++opennum);
	Timer0Init();
	UartInit();
	while(1)
	{
		TimeProcess();
		SmgProcess();
		BtnProcess();
		TempProcess();
		AdcProcess();
		DacProcess();
		SonicProcess();
		if(sendflag==1){ //如果有数据接收到，才发送
			sendflag = 0;
			SendString("recive data:");
			SendString(cmd);
			SendString("\r\n");
			memset(cmd,'\0',12);
		}
	}
}
//定时器2中断服务函数
void Timer2Handler() interrupt 12
{	
	ScanfSmg();	
	keycnt++;
	tempcnt++;
	timecnt++;
	adcnt++;
	dacnt++;
	sonicnt++;	
}
//串口一中断服务函数
void UartHandler() interrupt 4
{
//	char cmd[12]; //这里千万别初始化为0 否则 strcmp 永远不相等
	char tmp ;	
	static u8 i =0;

	if(TI){
		TI =0 ;
	}
	if(RI){
		RI = 0;
		tmp = SBUF;
		if(tmp=='o'){
			i = 0;
		}
		cmd[i++] = tmp;		
		if(strcmp(cmd,"openacd")==0){
			i = 0;
			sendflag = 1;
		}	
		if(i>=12){i=0;memset(cmd,'\0',12);}
	}
}


