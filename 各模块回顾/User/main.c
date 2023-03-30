#include "system.h"

#define TEMP    0
#define TIME    1
#define EEPROM  2
#define ADC     3
#define DAC     4
#define SONIC   5
u8 display = TEMP;

//���ڽ��ܵ���־
bit sendflag = 0;
char cmd[12];
//ϵͳ��ʼ��
void SystemProcess()
{
	DeviceCtrl(0x80,0xff);//led
	DeviceCtrl(0xa0,0x00);//buzz and relay
	
}
//SONIC���
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
//DAC���
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
//��ȡADC����
u8 adcnt = 0 ;
u16 ch1 = 0;//����
u16 ch3 = 0;//��ѹ
bit adcmode = 0;
void AdcProcess()
{
	if(adcnt>100){
		adcnt = 0;
		ReadAdc(0x43);													//�ն�һ�� ��ת������һ�ε�ô
		ch3  = (((float)ReadAdc(0x43)/51)*100);	//��λ��
		ReadAdc(0x41);													//�ն�һ�� ��ת������һ�ε�ô
		ch1  = (((float)ReadAdc(0x41)/51)*100);	//��������
	}
}
//EEPROM��¼��������
u8 opennum = 0;
//��ȡʱ�亯��
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
//�¶ȶ�ȡ
u8 tempcnt = 0;
u16 temp = 0;
void TempProcess()
{
	if(tempcnt>100){
		tempcnt = 0;
		temp = (u16)(ReadTemp()*10);//һλС��
		if(temp==850){
			temp =230;
		}
	}
}
// ����������ȡ����
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
			sprintf(disbuf,"������:%d",distance);
			SendString(disbuf);
			SendString("\r\n");
		}
		if(Bcont==0x04){//s5 ����
			if(display==ADC){
				adcmode = 1;
			}
		}
		if(Btrg==0x00&&Bcont==0x00){//������
			if(display==ADC){
					adcmode = 0;
				}
		}
		if(display==DAC){
			if(Btrg==0x02){
				dacval = dacval + 20; //��Ϊ������������Sonic������ʱ����
			}
			if(Btrg==0x01){
			//	dacval = dacval - 20;	//��Ϊ������������Sonic������ʱ����
			}
		}
	}	
}
//����ܻ��溯��
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
//������
void main()
{	
	SystemProcess();// �� ������� while1 ֮ǰ�� 	
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
		if(sendflag==1){ //��������ݽ��յ����ŷ���
			sendflag = 0;
			SendString("recive data:");
			SendString(cmd);
			SendString("\r\n");
			memset(cmd,'\0',12);
		}
	}
}
//��ʱ��2�жϷ�����
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
//����һ�жϷ�����
void UartHandler() interrupt 4
{
//	char cmd[12]; //����ǧ����ʼ��Ϊ0 ���� strcmp ��Զ�����
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


