#include "device.h"


void DeviceCtrl(u8 p2data,u8 p0data)
{
	P0 = p0data; 
	P2 = (P2&0x1f)|p2data;
	P2 &= 0x1f;
}