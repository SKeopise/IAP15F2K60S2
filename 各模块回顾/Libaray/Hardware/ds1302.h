#ifndef __DS1302_H 
#define __DS1302_H 
#include <system.h>


#define DecToBcd(dec)  ((dec/10)*16+dec%10)
#define BcdToDec(bcd)  ((bcd/16)*10+bcd%16)

unsigned char Read_Ds1302_Byte ( unsigned char address );

void SetClock(u8 hour,u8 min,u8 sec);

#endif 