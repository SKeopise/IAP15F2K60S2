#include "kbd.h"

u8 GetioThree()
{
	u8 io = 0xff;

	P3=0xf0;P4=0xff;//先列
	if(P44==0){io = 0x70;}
	if(P42==0){io = 0xb0;}
	if(P35==0){io = 0xd0;}
	if(P34==0){io = 0xe0;}
	P3=0x0f;P4=0x00;//再行
	if(P33==0){io |= 0x07;}
	if(P32==0){io |= 0x0b;}
	if(P31==0){io |= 0x0d;}
	if(P30==0){io |= 0x0e;}
	
	return io;
	
}
u8 Trg = 0;
u8 Cont = 0;
void KbdThree()
{
	u8 readdata = GetioThree()^0xff;
	Trg = readdata&(readdata^Cont);
	Cont = readdata;
	
}

u8 GetIoState()
{
	u8 io = 0;

	P3=0xf0;P4=0xff;//先列
	if(P44==0){io = 4;}
	if(P42==0){io = 8;}
	if(P35==0){io = 12;}
	if(P34==0){io = 16;}
	P3=0x0f;P4=0x00;//再行
	if(P33==0){io = io+0;}
	if(P32==0){io = io+1;}
	if(P31==0){io = io+2;}
	if(P30==0){io = io+3;}
	
	return io;
}

u8 KbdState()
{
	static u8 state = 0;
	u8 keyreturn = 0;
	u8 keypress  = GetIoState();
	
	switch(state)
	{
		case 0 :if(keypress!=0){state = 1;}break;
		case 1 :if(keypress!=0){
							keyreturn  = keypress;
							state  = 2;
						}else{
							state  = 0;
						}
						break;
		case 2:if(keypress==0){
							state  = 0;
						}
					break;
					
	}	
	return keyreturn;
}

	