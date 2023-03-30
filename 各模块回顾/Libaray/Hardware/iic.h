#ifndef __IIC_H 
#define __IIC_H 
#include <system.h>

void WriteEeprom(u8 add,u8 dat);
u8 ReadEeprom(u8 add);

u8 ReadAdc(u8 ch);
void Dacout(u8 val);

#endif 