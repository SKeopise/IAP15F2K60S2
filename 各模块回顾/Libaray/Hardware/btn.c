#include "btn.h"

u8 Btrg = 0;
u8 Bcont = 0;
void BtnThree()
{
	u8 readdata = P3^0xff;
	Btrg = readdata&(readdata^Bcont);
	Bcont = readdata;	
}


u8 BtnState()
{
	static u8 state = 0;
	u8 keyreturn = 0; 
	u8 keypress = 0;
	
	keypress = P3 & 0x0F;
	
	switch(state){
		case 0:if(keypress!=0x0f){state = 1;} break;
		case 1:if(keypress!=0x0f){
							if(keypress==0x07){keyreturn=4;}
							if(keypress==0x0b){keyreturn=5;}
							if(keypress==0x0d){keyreturn=6;}
							if(keypress==0x0e){keyreturn=7;}
							state = 2;
						}else{
							state = 0;
						}
					break;
		case 2:if(keypress==0x0f){
							state = 0;
						}
					break;
	}
	
	return keyreturn;
	
}