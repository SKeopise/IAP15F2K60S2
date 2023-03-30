#ifndef __UART_H 
#define __UART_H 
#include "system.h"

void UartInit(void);		//115200bps@12.000MHz
void SendString(char *str);
//void SendByte(u8 dat);

#endif 
